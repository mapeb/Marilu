#ifndef STACK_H_
#define STACK_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "commons/parser/parser.h"
#include "commons/collections/dictionary.h"

/*
 * Esta estructura tiene una porcion del stack. Puede ser cualquier porcion.
 * Lo importante es que una vez que inicia el quantum, se solicita el stack
 *
 * desde: stackCursor - contextSize = offset
 * hasta:  stackCursor
 *
 * Al terminar el quantum se envia a la UMV para actualizar la informacion.
 * El único problema es si hay un return que probablemente se tenga que pedir
 * mas info de la que hay aca.
 *
 *
 */


typedef struct {
	uint32_t lastContextInit;
	uint32_t lastProgramCounter;
} __attribute__((packed)) StackFuncion;


typedef struct {
	uint32_t lastContextInit;
	uint32_t lastProgramCounter;
	uint32_t variableRetorno;
} __attribute__((packed)) StackFuncionConRetorno;



typedef struct {
	char identificador;
	uint32_t posicion;
} t_diccionario_vabiable;



uint32_t apilarVariable( char identificador );
uint32_t obtenerOffsetVarible( char variable );
uint32_t obtenerValor( uint32_t pos );
void modificarVariable( uint32_t pos, uint32_t valor );

bool apilarFuncionConRetorno( uint32_t variableRetorno );
bool apilarFuncionSinRetorno( );

bool estaEnContexto(uint32_t pos);

bool generarDiccionarioVariables();

bool retornarVoid();
bool retornarValor(t_valor_variable retorno);

#endif /* STACK_H_ */






