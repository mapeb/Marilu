#include "kernel.h"

#include "config.h"
#include "ejecucion.h"
#include "commons/pcb.h"
#include "commons/sockets.h"

#include <unistd.h>

extern t_log * logger;
extern pcb_t PCB_enEjecucion;
extern uint32_t quantumPorEjecucion;
extern uint32_t retardoQuantum;
extern uint32_t quantumRestante;
int socketKernel;

bool debeFinalizar = false;
bool ejecutando = false; //Usado para el manejo de la senial


bool crearConexionKernel() {

	log_debug(logger, "Conectando al Kernel en: %s:%d", config_get_string_value(config, "IPKERNEL"), config_get_int_value(config, "PUERTOKERNEL"));
	socketKernel = conectar(config_get_string_value(config, "IPKERNEL"), config_get_int_value(config, "PUERTOKERNEL"), logger);

	if (socketKernel < 0) {
		log_error( logger, "No se pudo conectar al Kernel" );
		close( socketKernel );
		return false;
	}

	return enviarHandshake();

}

bool enviarHandshake()
{
	socket_header cod;

	cod.code = 'h';
	cod.size = sizeof( socket_header );

	if ( send( socketKernel, &cod, cod.size, 0 ) < 0 ) {
		return false;
	}

	socket_cpucfg cpucfg;

	if( recv(socketKernel, &cpucfg, sizeof(socket_cpucfg), MSG_WAITALL) != sizeof(socket_cpucfg) )
		return false;

	quantumPorEjecucion = cpucfg.quantum;
	retardoQuantum = cpucfg.retardo;
	log_debug( logger, "El quantum que envio el kernel es: %d y el retardo: %d", quantumPorEjecucion, retardoQuantum);

	return true;
}

bool recibirYProcesarMensajesKernel()
{

	socket_pcb pcbNuevo;
	while(!debeFinalizar)
	{
		log_debug(logger, "Esperando nueva PCB del Kernel...");

		if( recv(socketKernel, &pcbNuevo, sizeof(socket_pcb), MSG_WAITALL) != sizeof(socket_pcb) && !debeFinalizar )
			return false;

		if(debeFinalizar){
			return true;
		}

		//Usado para el manejo de la senial
		ejecutando = true;

		log_trace(logger, "Se ha recibido una nueva PCB, seteo el quantum en %d", quantumPorEjecucion);

		PCB_enEjecucion = pcbNuevo.pcb;
		quantumRestante = quantumPorEjecucion;

		ejecutar();

		if( !enviarPCB() ) {
			log_error(logger, "No se puedo devolver el PCB al Kernel");
			return false;
		}else{
			log_trace( logger, "Se le devolvio el PCB al Kernel" );
		}

		//Lo hago aca porque si se envia la senial mas arriba, se cierra el socket con el kernel y nose le puede devolver el pcb
		ejecutando = false;
	}

	return true;
}

bool enviarPCB()
{
	socket_pcb spcb;

	spcb.terminoCpu = debeFinalizar;
	spcb.header.size = sizeof(socket_pcb);
	spcb.header.code = 'p';
	spcb.pcb = PCB_enEjecucion;
	log_debug( logger, "Enviando PCB id: %d", PCB_enEjecucion.id );
	if( send(socketKernel, &spcb, sizeof(socket_pcb), 0) < 0 )
		return false;

	return true;
}

void finalizarCpu(){
	debeFinalizar = true;
	if(!ejecutando){
		log_info(logger, "Se envio la señal de finalizar");
		close(socketKernel);
	}else{
		log_info(logger, "Se envio la señal de finalizar, ejecutando ultimo quantum");
	}
}

/****************** SYSCALLS ************************/

int32_t solcitarVariableCompartidaAKernel(t_nombre_compartida variable)
{
	socket_scObtenerValor sObtenerValor;

	sObtenerValor.header.code = 'o';
	sObtenerValor.header.size = sizeof(socket_scObtenerValor);
	strcpy(sObtenerValor.identificador, variable);

	if( send(socketKernel, &sObtenerValor, sizeof(socket_scObtenerValor), 0) < 0 )
		return false;

	if( recv(socketKernel, &sObtenerValor, sizeof(socket_scObtenerValor), MSG_WAITALL) != sizeof(socket_scObtenerValor) )
		return false;

	return sObtenerValor.valor;
}

bool enviarAKernelNuevoValorVariableCompartida(t_nombre_compartida variable, t_valor_variable valor)
{
	socket_scGrabarValor sGrabarValor;

	sGrabarValor.header.code = 'g';
	sGrabarValor.header.size = sizeof(socket_scGrabarValor);
	sGrabarValor.valor = valor;
	strcpy(sGrabarValor.identificador, variable);

	if( send(socketKernel, &sGrabarValor, sizeof(socket_scGrabarValor), 0) < 0 )
		return false;

	return true;
}

bool enviarAKernelImprimir( t_valor_variable valor )
{
	socket_imprimirTexto mensaje;
	mensaje.header.code = 'k';
	mensaje.header.size = sizeof( socket_imprimirTexto );
	mensaje.programaSocket = PCB_enEjecucion.programaSocket;
	mensaje.pid = PCB_enEjecucion.id;
	sprintf(mensaje.texto, "%d", valor);

	if( send(socketKernel, &mensaje, sizeof(socket_imprimirTexto), 0) < 0 )
		return false;

	return true;
}

bool enviarAKernelImprimirTexto( char * texto )
{
	socket_imprimirTexto mensaje;
	mensaje.header.code = 'k';
	mensaje.header.size = sizeof( socket_imprimirTexto );
	mensaje.programaSocket = PCB_enEjecucion.programaSocket;
	mensaje.pid = PCB_enEjecucion.id;
	strcpy( mensaje.texto, texto );

	if( send(socketKernel, &mensaje, sizeof(socket_imprimirTexto), 0) < 0 )
			return false;

	return true;
}

bool enviarAKernelEntradaSalida(t_nombre_dispositivo dispositivo, int tiempo)
{
	socket_scIO io;
	io.header.code = 'i';
	io.header.size = sizeof(socket_scIO);
	io.unidades = tiempo;
	strcpy(io.identificador, dispositivo);

	if( send(socketKernel, &io, sizeof(socket_scIO), 0) < 0 ){
		log_error( logger, "No se pudo enviar la Syscall de entrada/salida al kernel" );
		return false;
	}

	quantumRestante = 0;
	return true;
}

bool enviarAKernelSignal(t_nombre_semaforo identificador_semaforo)
{
	socket_scSignal sSignal;
	sSignal.header.code = 's';
	sSignal.header.size = sizeof(socket_scSignal);
	//identificador_semaforo[ strlen( identificador_semaforo ) - 1 ] = '\0';
	strcpy(sSignal.identificador, identificador_semaforo );

	if(send(socketKernel, &sSignal, sizeof(socket_scSignal), 0) < 0){
		log_error(logger, "No se pudo enviar al kernel el Signal");
		return false;
	}

	return true;
}

bool enviarAKernelWait(t_nombre_semaforo identificador_semaforo)
{
	socket_scWait sWait;
	sWait.header.code = 'w';
	sWait.header.size = sizeof(socket_scWait);
	//identificador_semaforo[ strlen( identificador_semaforo ) - 1 ] = '\0';
	strcpy(sWait.identificador, identificador_semaforo );

	if( send(socketKernel, &sWait, sizeof(socket_scWait), 0) < 0 ){
		log_error(logger, "No se puede enviar al kernel el Wait");
		return false;
	}


	socket_respuesta res;

	if(recv(socketKernel, &res, sizeof(socket_respuesta), MSG_WAITALL) != sizeof(socket_respuesta))
		return false;

	if(!res.valor)
	{
		//Enviar pcb y detener ejecucion
		quantumRestante = 0;
		//Detener ejecucion ???
	}

	return true;
}

