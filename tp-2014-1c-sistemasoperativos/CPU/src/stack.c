#include "stack.h"
#include "umv.h"

#include "commons/pcb.h"
#include "commons/log.h"
#include "commons/sockets.h"
#include "commons/collections/dictionary.h"

#include <stdbool.h>

extern pcb_t PCB_enEjecucion;
extern t_log * logger;
extern int socketUMV;
extern uint32_t quantumRestante;


t_diccionario_vabiable diccionarioVariables[37];


/* Apila una variable en el stack dado, y devuelve un "puntero" a esa variable */
uint32_t apilarVariable(char identificador)
{

	if(obtenerOffsetVarible(identificador) != 0){
		log_error(logger, "Se intento definir una variable ya existente");
		return 0;
	}

	if(!escribirStack(PCB_enEjecucion.stackCursor, 1, &identificador)){
		log_error(logger, "Error al apilar la variable");
		return 0;
	}

	t_diccionario_vabiable nuevaVariable;
	nuevaVariable.identificador = identificador;
	nuevaVariable.posicion = PCB_enEjecucion.stackCursor;

	diccionarioVariables[PCB_enEjecucion.contextSize] = nuevaVariable;
	PCB_enEjecucion.contextSize++;
	PCB_enEjecucion.stackCursor += 5;

	return PCB_enEjecucion.stackCursor - 4;

}


/*
 * Obtiene la posicion de memoria de una variable
 */
uint32_t obtenerOffsetVarible(char variable)
{
	int i = 0;
	for(;i<PCB_enEjecucion.contextSize; i++){
		if (diccionarioVariables[i].identificador == variable){
			return diccionarioVariables[i].posicion + 1;
		}
	}
	return 0;
}

uint32_t obtenerValor(uint32_t pos)
{
	if(!estaEnContexto(pos)){
		log_warning(logger, "Escribiendo fuera de contexto");
	}
	uint32_t* data = (uint32_t*) leerStack(pos, sizeof(uint32_t));
	if(data != NULL){
		uint32_t valor = *data;
		free(data);
		return valor;
	}else{
		log_error(logger, "Error al leer variable de la UMV");
		PCB_enEjecucion.lastErrorCode = 3;
		quantumRestante = 0;
		return 0;
	}
}

void modificarVariable(uint32_t pos, uint32_t valor)
{
	if(!estaEnContexto(pos)){
		log_warning(logger, "Escribiendo una variable fuera de contexto");
	}
	escribirStack(pos, sizeof(uint32_t), (void *) &valor);
}

bool estaEnContexto(uint32_t pos)
{
	return (pos>=(PCB_enEjecucion.stackCursor -  PCB_enEjecucion.contextSize * 5) && pos < (PCB_enEjecucion.stackCursor));
}


/*
 * Se apila
 * direccion base del contecto
 * programCounter
 * posicion de la variable de retorno
 */
bool apilarFuncionConRetorno(uint32_t variableRetorno)
{
	log_trace(logger, "Guardando el contexto: stackCursor = %d, contextSize = %d, ProgramCounter = %d, variableRetorno = %d", PCB_enEjecucion.stackCursor, PCB_enEjecucion.contextSize, PCB_enEjecucion.programCounter, variableRetorno);
	StackFuncionConRetorno llamada;
	llamada.lastContextInit = PCB_enEjecucion.stackCursor - PCB_enEjecucion.contextSize * 5;
	llamada.lastProgramCounter = PCB_enEjecucion.programCounter;
	llamada.variableRetorno = variableRetorno;

	if(!escribirStack(PCB_enEjecucion.stackCursor, sizeof(StackFuncionConRetorno), (void*)&llamada)){
		log_error(logger, "No se pudo apilar la funcion en el stack");
		return false;
	}else{
		PCB_enEjecucion.stackCursor += sizeof(StackFuncionConRetorno);
		PCB_enEjecucion.contextSize = 0;
		return generarDiccionarioVariables();
	}
}


bool apilarFuncionSinRetorno()
{
	log_trace(logger, "Guardando el contexto: stackCursor = %d, contextSize = %d, ProgramCounter = %d", PCB_enEjecucion.stackCursor, PCB_enEjecucion.contextSize, PCB_enEjecucion.programCounter);
	StackFuncion llamada;
	llamada.lastContextInit = PCB_enEjecucion.stackCursor - PCB_enEjecucion.contextSize*5;
	llamada.lastProgramCounter = PCB_enEjecucion.programCounter;

	if(!escribirStack(PCB_enEjecucion.stackCursor, sizeof(StackFuncion), (void*)&llamada)){
		log_error(logger, "No se pudo apilar la funcion en el stack");
		return false;
	}else{
		PCB_enEjecucion.stackCursor += sizeof(StackFuncion);
		PCB_enEjecucion.contextSize = 0;
		return generarDiccionarioVariables();
	}
}


bool generarDiccionarioVariables()
{
	log_trace(logger, "Regenerando diccionario de variables...");
	memset(&diccionarioVariables, 0, sizeof(diccionarioVariables));
	if(PCB_enEjecucion.contextSize == 0){
		return true;
	}

	void * data = leerStack(PCB_enEjecucion.stackCursor - PCB_enEjecucion.contextSize*5, PCB_enEjecucion.contextSize * 5);
	if(data == NULL) {
		log_error(logger, "Hubo un error al leer el stack e intentar regenerar el diccionario de variables");
		return false;
	}else{
		int i = 0;
		//printf("Se agrego al diccionario de variables: ");
		uint32_t contextInit = PCB_enEjecucion.stackCursor - PCB_enEjecucion.contextSize * 5;
		for(; i < PCB_enEjecucion.contextSize; i++ ) {
			t_diccionario_vabiable * variable = (t_diccionario_vabiable *) (data + i*5);
			diccionarioVariables[i] = *variable;
			diccionarioVariables[i].posicion = contextInit + 5 * i;
			//printf(" | %c - %d", variable->identificador, contextInit + 5 * i);
		}
		//printf("\n");
		log_debug(logger, "Se leyo correctamente el stack desde la UMV");
		free(data);
		return true;
	}
}


/*
 * Esto es para el caso que se llame al retorno de una funcion
 * Necesito conseguir todos los datos del contexto anterior, entonces
 * lo pido con esta funcion
 *
 */
bool retornarVoid()
{

	PCB_enEjecucion.stackCursor = PCB_enEjecucion.stackCursor - PCB_enEjecucion.contextSize*5 - sizeof(StackFuncion);

	void * respuesta = leerStack(PCB_enEjecucion.stackCursor, sizeof(StackFuncion));
	if(respuesta == NULL){
		log_error( logger, "Error al intentar retornar" );
		return false;
	}

	StackFuncion * respuestaStack	= (StackFuncion *) respuesta;
	PCB_enEjecucion.programCounter	= respuestaStack->lastProgramCounter;
	PCB_enEjecucion.contextSize		= (PCB_enEjecucion.stackCursor - respuestaStack->lastContextInit)/5;

	//printf("Recibi la respuesta con lastProgramCounter: %d, lastContextInit: %d", respuestaStack->lastProgramCounter, respuestaStack->lastContextInit);
	log_trace(logger, "Se recupero el contexto: stackCursor = %d, contextSize = %d, ProgramCounter = %d", PCB_enEjecucion.stackCursor, PCB_enEjecucion.contextSize, PCB_enEjecucion.programCounter);
	free(respuesta);

	return generarDiccionarioVariables();
}

bool retornarValor(t_valor_variable retorno)
{

	//printf("\nAhora el contexto es: stackCursor = %d, contextSize = %d, ProgramCounter = %d \n", PCB_enEjecucion.stackCursor, PCB_enEjecucion.contextSize, PCB_enEjecucion.programCounter);

	PCB_enEjecucion.stackCursor = PCB_enEjecucion.stackCursor - PCB_enEjecucion.contextSize*5 - sizeof(StackFuncionConRetorno);

	StackFuncionConRetorno * respuestaStack = (StackFuncionConRetorno *)leerStack(PCB_enEjecucion.stackCursor, sizeof(StackFuncionConRetorno));
	if(respuestaStack == NULL){
		log_error( logger, "Error al intentar retornar" );
		return false;
	}

	PCB_enEjecucion.programCounter	= respuestaStack->lastProgramCounter;
	PCB_enEjecucion.contextSize		= (PCB_enEjecucion.stackCursor - respuestaStack->lastContextInit)/5;

	//printf("Recibi la respuesta con lastProgramCounter: %d, lastContextInit: %d", respuestaStack->lastProgramCounter, respuestaStack->lastContextInit);
	log_trace(logger, "Se recupero el contexto: stackCursor = %d, contextSize = %d, ProgramCounter = %d", PCB_enEjecucion.stackCursor, PCB_enEjecucion.contextSize, PCB_enEjecucion.programCounter);

	if( !generarDiccionarioVariables() ){
		free(respuestaStack);
		return false;
	}

	log_debug(logger, "Guardando el valor en la variable de retorno, vale: %d, la guardo en la pos: %d", retorno, respuestaStack->variableRetorno);
	modificarVariable(respuestaStack->variableRetorno, retorno);

	log_trace(logger, "Recuperado el contextStack anterior");
	free(respuestaStack);

	return true;
}









