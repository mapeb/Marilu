#ifndef SEGMENTO_H_
#define SEGMENTO_H_
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "commons/log.h"

typedef struct {
	uint32_t id;

	uint32_t inicioVirtual;	//Estos son los que cree el programa
	uint32_t finVirtual;

	uint32_t inicioReal;	//Estos son los offset reales respecto al memoria
	uint32_t finReal;
} Segmento;


Segmento * new_Segmento( uint32_t inicio, uint32_t fin ) ;

bool memCopy(Segmento *segmento, uint32_t offset, void *valor, uint32_t length);
bool memLeer(Segmento *segmento, void *destino, uint32_t offset, uint32_t length);


#endif /* SEGMENTO_H_ */

