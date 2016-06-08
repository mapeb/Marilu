#ifndef PCB_H_
#define PCB_H_

#include <stdint.h>

typedef struct {
	uint32_t id;

	uint32_t codeSegment;
	uint32_t codeIndex;
	uint32_t etiquetaIndex;
	uint32_t etiquetasSize;

	uint32_t stackSegment;
	uint32_t stackCursor;
	uint32_t contextSize;

	uint32_t programCounter;

	int programaSocket;
	uint32_t prioridad;
	uint32_t lastErrorCode;	//Se guarda el codigo del error de la ultima instruccion ejecutada, 0 si no hay error
} pcb_t;

#endif /* PCB_H_ */
