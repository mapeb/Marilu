#include "Kernel.h"

extern t_log *logger;
extern pthread_t threadConexiones;
extern uint32_t retardoUMV;

socket_pedirMemoria pedidoMemoria; //Ultimo pedido de memoria

void fnKernelConectado(int socketKernel)
{
	log_info(logger, "Se conecto el Kernel");

	while( recibirYProcesarPedidoKernel(socketKernel) != false );

	log_error(logger, "El Kernel se ha desconectado");
	pthread_cancel(threadConexiones);
}

bool recibirYProcesarPedidoKernel(int socketKernel)
{
	log_trace(logger, "Esperando pedido del Kernel");
	socket_header header;

	if( recv(socketKernel, &header, sizeof(socket_header), MSG_WAITALL | MSG_PEEK) != sizeof(socket_header) )
		return false;

	usleep(retardoUMV * 1000);

	switch(header.code)
	{
	case 'p':
		return pedirMemoria(socketKernel);
	case 'b':
		return borrarSegmentos(socketKernel);
	case 's':
		return recibirSegmentos(socketKernel);
	default:
		log_error(logger, "Pedido invalido del kernel");
		return false;
	}

	return true;
}

bool pedirMemoria(int socketKernel)
{
	log_trace(logger, "Procesando solicitud del Kernel de pedido de memoria para los segmentos");

	if( recv(socketKernel, &pedidoMemoria, sizeof(socket_pedirMemoria), MSG_WAITALL) != sizeof(socket_pedirMemoria) )
		return false;

	uint32_t memoriaDisponible = memoriaLibre();
	uint32_t tamanioTotalSegmentos = pedidoMemoria.codeSegmentSize + pedidoMemoria.etiquetasSegmentSize + pedidoMemoria.instruccionesSegmentSize + pedidoMemoria.stackSegmentSize;

	socket_respuesta respuestaSegmentos;

	respuestaSegmentos.header.size = sizeof(respuestaSegmentos);
	respuestaSegmentos.valor = true;

	if(tamanioTotalSegmentos > memoriaDisponible)
	{
		log_info(logger, "Memory Overload. No se ha podido reservar los segmentos solicitados por el Kernel");
		respuestaSegmentos.valor = false;
	}

	if( send(socketKernel, &respuestaSegmentos, sizeof(respuestaSegmentos), 0) < 0 ) {
		log_error(logger, "No se ha podido enviar respuesta de reservación de segmentos al Kernel");
		return false;
	}

	return true;
}

bool borrarSegmentos(int socketKernel)
{
	log_trace(logger, "Procesando solicitud del Kernel de borrado de programa");
	socket_borrarMemoria borradoMemoria;

	if( recv(socketKernel, &borradoMemoria, sizeof(socket_borrarMemoria), MSG_WAITALL) != sizeof( socket_borrarMemoria) )
		return false;

	Programa *programa = buscarPrograma(borradoMemoria.pid);

	if(programa == NULL) {
		log_error(logger, "El programa solicitado por el Kernel para su borrado no existe en memoria");
		return false;
	}

	destruirPrograma(programa);

	return true;
}

bool recibirSegmentos(int socketKernel)
{
	socket_header header;

	if( recv(socketKernel, &header, sizeof(socket_header), MSG_WAITALL) < 0 )
	{
		log_error(logger, "Error al recibir los segmentos del Kernel");
		return false;
	}

	uint32_t pid;
	void *script = malloc(pedidoMemoria.codeSegmentSize);
	void *etiquetas = malloc(pedidoMemoria.etiquetasSegmentSize);
	void *instrucciones	= malloc(pedidoMemoria.instruccionesSegmentSize);

	void liberar()
	{
		free(script);
		free(etiquetas);
		free(instrucciones);
	}

	if( recv(socketKernel, &pid, sizeof(uint32_t), MSG_WAITALL) < 0 ||
		recv(socketKernel, script, pedidoMemoria.codeSegmentSize, MSG_WAITALL) < 0 ||
		recv(socketKernel, etiquetas, pedidoMemoria.etiquetasSegmentSize, MSG_WAITALL) < 0 ||
		recv(socketKernel, instrucciones, pedidoMemoria.instruccionesSegmentSize, MSG_WAITALL) < 0)
	{
		log_error(logger, "Error al recibir los segmentos del Kernel");
		liberar();
		return false;
	}

	Programa *programa = crearPrograma(pid, script, etiquetas, instrucciones, &pedidoMemoria);



	socket_umvpcb datosSegmentos = crearEstructuraParaPCB(programa);

	if ( send(socketKernel, &datosSegmentos, sizeof(datosSegmentos), 0) < 0 ) {
		log_error( logger, "No se ha podido enviar los datos de los segmentos creados correctamente al Kernel" );
		liberar();
		return false;
	}

	log_info(logger, "Se ha enviado los datos de los segmentos creados correctamente al Kernel");

	liberar();

	return true;
}

