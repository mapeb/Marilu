#ifndef PROGRAMA_H_
#define PROGRAMA_H_
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "commons/sockets.h"
#include "commons/config.h"
#include "commons/collections/list.h"
#include "Segmento.h"

typedef struct {
	uint32_t pid;
	Segmento * stack;
	Segmento * script;
	Segmento * etiquetas;
	Segmento * instrucciones;
} Programa;


Programa *crearPrograma(uint32_t pid, void *script, void *etiquetas, void *instrucciones_serializado, socket_pedirMemoria *pedidoMemoria);

Segmento * crearDireccionesVirtuales(Segmento * segmento,
		uint32_t tamanioSegmento, uint32_t finVirtualDelAnterior);

socket_umvpcb crearEstructuraParaPCB( Programa * programa);

Programa * buscarPrograma( uint32_t pdi);

Segmento * buscarSegmentoEnProgramaPorReal(Programa * programa, uint32_t base);
Segmento * buscarSegmentoEnProgramaPorVirtual(Programa * programa, uint32_t base);

bool destruirPrograma( Programa * programa);
void destruirTodosLosProgramas();
#endif /*PROGRAMA_H_*/
