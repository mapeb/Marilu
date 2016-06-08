#include "systemcalls.h"
#include "io.h"
#include "colas.h"
#include "config.h"

extern t_log *logpcp;

extern sem_t dispatcherReady, dispatcherCpu;

bool syscallIO(int socketCPU)
{
	socket_scIO io;
	socket_pcb spcb;

	if( recv(socketCPU, &io, sizeof(socket_scIO), MSG_WAITALL) != sizeof(socket_scIO) )
		return false;

	if( recv(socketCPU, &spcb, sizeof(socket_pcb), MSG_WAITALL) != sizeof(socket_pcb) )
		return false;

	log_debug(logpcp, "CPU: %d, mando nuevo trabajo de IO del dispositivo: %s", socketCPU, io.identificador);

	if(spcb.terminoCpu)
		free(sacarCpuDeExec(socketCPU));
	else{
		moverCpuAReady(sacarCpuDeExec(socketCPU));
		sem_post(&dispatcherCpu);
	}

	pcb_t *pcb = sacarDeExec(spcb.pcb.id);
	*pcb = spcb.pcb;

	//CHECK
	if(buscarProgramaConectado(pcb->id) == NULL){
		moverAExit(pcb);
		return true;
	}

	moverABlock(pcb);

	crear_pedido(io.identificador, pcb->id, io.unidades);

	return true;
}

bool syscallObtenerValor(int socketCPU)
{
	socket_scObtenerValor sObtenerValor;

	if( recv(socketCPU, &sObtenerValor, sizeof(socket_scObtenerValor), MSG_WAITALL) != sizeof(socket_scObtenerValor) )
		return false;

	log_debug(logpcp, "CPU: %d, pidio el valor de la variable: %s", socketCPU, sObtenerValor.identificador);
	log_trace(logpcp, "Obteniendo valor desde el diccionario de variables compartidas");

	if( !dictionary_has_key(variablesCompartidas, sObtenerValor.identificador))
		log_trace(logpcp, "La variable %s no se encuentra en el diccionario. Segmentation fault", sObtenerValor.identificador);

	int32_t *valor = dictionary_get(variablesCompartidas, sObtenerValor.identificador);
	sObtenerValor.valor = *valor;

	if( send(socketCPU, &sObtenerValor, sizeof(socket_scObtenerValor), 0) < 0 )
		return false;

	log_debug(logpcp, "Se envio el valor: %d, de la variable: %s al CPU: %d", sObtenerValor.valor, sObtenerValor.identificador, socketCPU);

	return true;
}

bool syscallGrabarValor(int socketCPU)
{
	socket_scGrabarValor sGrabarValor;

	if( recv(socketCPU, &sGrabarValor, sizeof(socket_scGrabarValor), MSG_WAITALL) != sizeof(socket_scGrabarValor) )
		return false;

	log_debug(logpcp, "CPU: %d, pidio grabar el valor: %d en la variable: %s", socketCPU, sGrabarValor.valor, sGrabarValor.identificador);
	log_trace(logpcp, "Grabando valor en el diccionario de variables compartidas");

	if( !dictionary_has_key(variablesCompartidas, sGrabarValor.identificador))
		log_trace(logpcp, "La variable %s no se encuentra en el diccionario. Segmentation fault", sGrabarValor.identificador);

	int32_t *valor = dictionary_get(variablesCompartidas, sGrabarValor.identificador);
	*valor = sGrabarValor.valor;

	return true;
}

bool syscallWait(int socketCPU)
{
	socket_scWait sWait;

	if( recv(socketCPU, &sWait, sizeof(socket_scWait), MSG_WAITALL) != sizeof(socket_scWait) )
			return false;

	log_debug(logpcp, "CPU: %d, hizo wait en el semaforo: %s", socketCPU, sWait.identificador);
	log_trace(logpcp, "Decrementando semaforo en el diccionario de semaforos");

	if( !dictionary_has_key(semaforos, sWait.identificador))
		log_trace(logpcp, "El semaforo %s no se encuentra en el diccionario. Segmentation fault", sWait.identificador);

	semaforo_t *semaforo = dictionary_get(semaforos, sWait.identificador);
	semaforo->valor -= 1;

	socket_respuesta res;
	res.header.size = sizeof(socket_respuesta);

	if (semaforo->valor < 0)
	{
		log_trace(logpcp, "El semaforo quedo con el valor negativo: %d, enviando respuesta y pidiendo PCB",semaforo->valor);

		socket_pcb spcb;
		res.valor = false;

		if( send(socketCPU, &res, sizeof(socket_respuesta), 0) < 0 )
			return false;

		if( recv(socketCPU, &spcb, sizeof(socket_pcb), MSG_WAITALL) != sizeof(socket_pcb) )
		    return false;

		log_trace(logpcp, "PCB recibida. Agregandola a la cola del semaforo");

		uint32_t *pid = malloc(sizeof(uint32_t));
		*pid = spcb.pcb.id;
		queue_push(semaforo->cola, pid);

		if(spcb.terminoCpu)
			free(sacarCpuDeExec(socketCPU));
		else{
			moverCpuAReady(sacarCpuDeExec(socketCPU));
			sem_post(&dispatcherCpu);
		}

		pcb_t *pcb = sacarDeExec(spcb.pcb.id);
		*pcb = spcb.pcb;
		moverABlock(pcb);
	}
	else
	{
		log_trace(logpcp, "El semaforo quedo con el valor positivo: %d, enviando respuesta para que el script continue",semaforo->valor);

		res.valor = true;

		if( send(socketCPU, &res, sizeof(socket_respuesta), 0) < 0 )
			return false;
	}

	return true;
}

bool syscallSignal(int socketCPU)
{
	socket_scSignal sSignal;

	if( recv(socketCPU, &sSignal, sizeof(socket_scSignal), MSG_WAITALL) != sizeof(socket_scSignal) )
		return false;

	log_debug(logpcp, "CPU: %d, hizo signal en el semaforo: %s", socketCPU, sSignal.identificador);
	log_trace(logpcp, "Incrementando semaforo en el diccionario de semaforos");

	if( !dictionary_has_key(semaforos, sSignal.identificador))
		log_trace(logpcp, "El semaforo %s no se encuentra en el diccionario. Segmentation fault", sSignal.identificador);

	semaforo_t *semaforo = dictionary_get(semaforos, sSignal.identificador);
	semaforo->valor += 1;

	if (semaforo->valor <= 0)
	{
		//Saco el pcb de la cola de bloqueados y lo pongo en la cola de ready
		log_trace(logpcp, "Habia PCB bloquedas, moviendo una a ready. Quedan: %d",abs(semaforo->valor));

		uint32_t *pid = queue_pop(semaforo->cola);

		moverAReady(sacarDeBlock(*pid));

		sem_post(&dispatcherReady);

		free(pid);
	}

	return true;
}

bool syscallImprimirTexto(int socketCPU)
{
	socket_imprimirTexto texto;

	if( recv(socketCPU, &texto, sizeof(socket_imprimirTexto), MSG_WAITALL) != sizeof(socket_imprimirTexto) )
			return false;

	log_debug(logpcp, "CPU: %d, envia un mensaje al Programa: %d", socketCPU, texto.programaSocket);

	//CHECK
	if( buscarProgramaConectado(texto.pid) == NULL )
		return true;

	socket_msg msg;
	msg.header.size = sizeof(socket_msg);
	msg.type = 0; //log_info

	strcpy(msg.msg, texto.texto);
	send(texto.programaSocket, &msg, sizeof(socket_msg), 0);

	log_trace(logpcp, "Mensaje enviado al Programa: %d", texto.programaSocket);

	return true;
}
