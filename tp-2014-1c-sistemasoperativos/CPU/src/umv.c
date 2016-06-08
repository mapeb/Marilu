#include "umv.h"
#include "config.h"

#include "commons/log.h"
#include "commons/sockets.h"
#include "commons/parser/parser.h"
#include "commons/parser/metadata_program.h"
#include "commons/string.h"

#include <unistd.h>
#include <sys/socket.h>

extern t_log * logger;
extern pcb_t PCB_enEjecucion;
extern uint32_t quantumRestante;
int socketUMV;
char * etiquetasCache;

bool crearConexionUMV() {

	log_debug(logger, "Conectando a la UMV en %s:%d", config_get_string_value(config, "IPUMV"), config_get_int_value(config, "PUERTOUMV"));
	socketUMV = conectar(config_get_string_value(config, "IPUMV"),	config_get_int_value(config, "PUERTOUMV"), logger);

	if (socketUMV < 0) {
		log_error(logger, "No se pudo conectar al UMV");
		close( socketUMV );
		return false;
	}

	socket_header handshake;
	handshake.code = 'c';
	handshake.size = sizeof(socket_header);
	if(send(socketUMV, &handshake, handshake.size, 0 ) > 0 ) {
		return true;
	}
	log_error(logger, "Error al enviar el handshake a la UMV");
	return false;
}



/*
 * Dado el program counter de un pcb, se le solicita a la UMV la linea de codigo a ejecutar
 *
 *
 */
char * solicitarLineaPrograma() {

	log_debug(logger, "Solicitando linea de programa a la UMV para el programCounter = %d, base = %d, offset = %d, length = %d", PCB_enEjecucion.programCounter, PCB_enEjecucion.codeIndex, sizeof(t_intructions) * PCB_enEjecucion.programCounter, sizeof(t_intructions));

	t_intructions * instruct = (t_intructions *) leerMemoria(PCB_enEjecucion.codeIndex, sizeof(t_intructions) * PCB_enEjecucion.programCounter, sizeof(t_intructions));
	if(instruct == NULL) {
		log_error( logger, "La UMV respondio con un error o Segmentation fault" );
		return (char *)-1;
	}

	log_debug( logger, "Se leyo el codeIndex, la proxima instruccion esta en %d - %d", instruct->offset, instruct->start );

	if(instruct->offset == 0){
		log_error( logger, "Llego un code index con length 0 | ( %s ) %s - %s",  __func__, __FILE__, __LINE__  );
		return (char *) -1;
	}

	char * respuesta = (char *) leerMemoria(PCB_enEjecucion.codeSegment, instruct->start, instruct->offset);
	if (respuesta == NULL) {
		return (char *)-1;
	}


	free(instruct);
	return eliminarSaltoLinea(respuesta);

}

char * eliminarSaltoLinea(char * linea){
	int i = 0;
	for(i = 0; i < strlen(linea); i++){
		if (linea[i] == '\n'){
			linea[i] = '\0';
		}
	}
	return linea;
}

bool obtenerEtiquetas(){

	if(PCB_enEjecucion.etiquetasSize == 0){
		etiquetasCache = realloc(etiquetasCache, 1);
		log_warning(logger, "No leyeron etiquetas, el programa tiene?");
		return true;
	}

	void * respuesta = leerMemoria(PCB_enEjecucion.etiquetaIndex, 0, PCB_enEjecucion.etiquetasSize);
	if(respuesta == NULL) {
		log_error( logger, "Error al leer las etiquiquetas | ( %s ) %s - %s  ",  __func__, __FILE__, __LINE__ );
		return false;
	}

	etiquetasCache = realloc(etiquetasCache, PCB_enEjecucion.etiquetasSize);
	memcpy(etiquetasCache, respuesta, PCB_enEjecucion.etiquetasSize);
	free(respuesta);
	return true;

}


//Rescrito respecto a lo de la catedra por un bug
uint32_t obtenerLineaDeLabel( t_nombre_etiqueta t_nombre_etiqueta ) {
	int i=0;
	int offset = 0;
	char* nombre;
	for(i=0;  offset < PCB_enEjecucion.etiquetasSize; i++){
	nombre = etiquetasCache + offset;
		if( string_equals_ignore_case(nombre, t_nombre_etiqueta) )
			return *(nombre + 1 + strlen(nombre));
		offset += strlen(nombre) + 1 + sizeof(t_puntero_instruccion);
	}
	return -1;//Mmmm, porque es negativo? TODO, verificar
}



bool escribirStack(uint32_t offset, uint32_t length, void * data){
	return escribirMemoria(PCB_enEjecucion.stackSegment, offset, length, data);
}

bool escribirMemoria(uint32_t base, uint32_t offset, uint32_t length, void * data){
	socket_guardarEnMemoria sGuardarEnMemoria;
	sGuardarEnMemoria.header.code = 'c';
	sGuardarEnMemoria.offset	= offset;
	sGuardarEnMemoria.pdi		= PCB_enEjecucion.id;
	sGuardarEnMemoria.length	= length;
	sGuardarEnMemoria.base		= base;

	uint32_t tam = sizeof(socket_guardarEnMemoria) + length;
	void *buffer = malloc(tam);

	memcpy(buffer, &sGuardarEnMemoria, sizeof(socket_guardarEnMemoria));
	memcpy(buffer+sizeof(socket_guardarEnMemoria), data, length);

	if( send(socketUMV, buffer, tam, 0) < 0){
		PCB_enEjecucion.lastErrorCode = 2;
		quantumRestante = 0;
		free(buffer);
		return false;
	}

	free(buffer);
	socket_RespuestaGuardarEnMemoria respuesta;

	if( recv(socketUMV, &respuesta, sizeof(socket_RespuestaGuardarEnMemoria), MSG_WAITALL) != sizeof(socket_RespuestaGuardarEnMemoria)){
		PCB_enEjecucion.lastErrorCode = 2;
		quantumRestante = 0;
		return false;
	}

	if(respuesta.status == false){
		PCB_enEjecucion.lastErrorCode = 2;
		quantumRestante = 0;
		return false;
	}

	return true;

}

void * leerMemoria(uint32_t base, uint32_t offset, uint32_t length){
	socket_leerMemoria sLeer;
	sLeer.header.code = 'b';
	sLeer.pdi = PCB_enEjecucion.id;
	sLeer.base = base;
	sLeer.offset = offset;
	sLeer.length = length;
	log_debug(logger, "Leyendo UMV base = %d, offset = %d, length = %d", base, offset, length);

	if( send(socketUMV, &sLeer, sizeof(socket_leerMemoria), 0) < 0 ){
		PCB_enEjecucion.lastErrorCode = 2;
		quantumRestante = 0;
		return NULL;
	}

	socket_RespuestaLeerMemoria respuesta;
	uint32_t tam = sizeof(socket_RespuestaLeerMemoria) + length;
	void *buffer = malloc(tam);

	if( recv(socketUMV, buffer, tam, MSG_WAITALL) != tam ){
		PCB_enEjecucion.lastErrorCode = 2;
		quantumRestante = 0;
		free(buffer);
		return NULL;
	}

	void *data = malloc(length);
	memcpy(&respuesta, buffer, sizeof(socket_RespuestaLeerMemoria));
	memcpy(data, buffer+sizeof(socket_RespuestaLeerMemoria), length);
	free(buffer);

	if(respuesta.status == false){
		PCB_enEjecucion.lastErrorCode = 2;
		quantumRestante = 0;
		return NULL;
	}

	return data;
}

void * leerStack(uint32_t offset, uint32_t length){
	return leerMemoria(PCB_enEjecucion.stackSegment, offset, length);
}
