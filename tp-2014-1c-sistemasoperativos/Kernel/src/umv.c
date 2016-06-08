#include "umv.h"
#include "config.h"

extern t_log *logplp;

extern sem_t semKernel;

int socketUMV;

bool conectarUMV()
{
	socketUMV = conectar(config_get_string_value(config, "IP_UMV"), config_get_int_value(config, "PUERTO_UMV"), logplp);

	if (socketUMV == -1) {
		log_error(logplp, "No se pudo establecer la conexion con la UMV");
		return false;
	}

	log_info(logplp, "Conectado con la UMV");

	socket_header handshake;

	handshake.size = sizeof(socket_header);
	handshake.code = 'k'; //Kernel

	if( send(socketUMV, &handshake, sizeof(socket_header), 0) <= 0 )
	{
		log_error(logplp, "No se puedo enviar Handshake a la UMV");
		sem_post(&semKernel);
		return false;
	}

	return true;
}

bool solicitarCreacionSegmentos(uint32_t scriptSize, t_metadata_program *scriptMetadata)
{
	log_info(logplp, "Pidiendole memoria a la UMV para que pueda correr el script ansisop");

	socket_pedirMemoria pedirMemoria;
	pedirMemoria.header.size = sizeof(pedirMemoria);
	pedirMemoria.header.code = 'p';

	pedirMemoria.codeSegmentSize = scriptSize + 1;
	pedirMemoria.stackSegmentSize = config_get_int_value(config, "STACK_SIZE");
	pedirMemoria.etiquetasSegmentSize = scriptMetadata->etiquetas_size;
	pedirMemoria.instruccionesSegmentSize = scriptMetadata->instrucciones_size * sizeof(t_intructions);

	if( send(socketUMV, &pedirMemoria, sizeof(socket_pedirMemoria), 0) <= 0 ){
		log_error(logplp, "No se puedo pedir memoria a la UMV. Desconectando");
		sem_post(&semKernel);
		return false;
	}

	return true;
}

bool respuestaCreacionSegmentos()
{
	socket_respuesta respuesta;

	if( recv(socketUMV, &respuesta, sizeof(socket_respuesta), MSG_WAITALL) != sizeof(socket_respuesta) ){
		log_error(logplp, "No se recibio respuesta de la UMV. Desconectando");
		sem_post(&semKernel);
		return false;
	}

	return respuesta.valor;
}

bool enviarSegmentos(uint32_t pid, char *script, uint32_t scriptSize, t_metadata_program *scriptMetadata)
{
	socket_header header;

	header.size = sizeof(socket_header)+sizeof(uint32_t)+(scriptSize+1)+scriptMetadata->etiquetas_size+scriptMetadata->instrucciones_size * sizeof(t_intructions);
	header.code = 's';

	void *buffer = malloc(header.size);
	uint32_t index = 0;

	memcpy(buffer+index, &header, sizeof(socket_header));
	index = sizeof(socket_header);
	memcpy(buffer+index, &pid, sizeof(uint32_t));
	index += sizeof(uint32_t);
	memcpy(buffer+index, script, scriptSize+1);
	index += scriptSize+1;
	memcpy(buffer+index, scriptMetadata->etiquetas, scriptMetadata->etiquetas_size);
	index += scriptMetadata->etiquetas_size;
	memcpy(buffer+index, scriptMetadata->instrucciones_serializado, scriptMetadata->instrucciones_size * sizeof(t_intructions));

	if( send(socketUMV, buffer, header.size, 0) < 0 ){
		log_error(logplp, "No se pudo enviar el contenido de los segmentos a la UMV. Desconectando");
		sem_post(&semKernel);
		return false;
	}

	free(buffer);

	return true;
}

bool respuestaSegmentos(socket_umvpcb *umvpcb)
{
	if( recv(socketUMV, umvpcb, sizeof(socket_umvpcb), MSG_WAITALL) != sizeof(socket_umvpcb) ){
		log_error(logplp, "No se recibio las direcciones de los segmentos de la UMV. Desconectando");
		sem_post(&semKernel);
		return false;
	}

	return true;
}

bool borrarSegmentos(uint32_t pid)
{
	log_trace(logplp, "Solicitando destruccion de segmentos a la UMV");

	socket_borrarMemoria borrarMemoria;
	borrarMemoria.header.size = sizeof(socket_borrarMemoria);
	borrarMemoria.header.code = 'b';
	borrarMemoria.pid = pid;

	if( send(socketUMV, &borrarMemoria, sizeof(socket_borrarMemoria), 0) <= 0 )
	{
		log_error(logplp, "No se pudo enviar la solicitud de destruccion de segmentos a la UMV. Desconectando");
		sem_post(&semKernel);
		return false;
	}

	return true;
}
