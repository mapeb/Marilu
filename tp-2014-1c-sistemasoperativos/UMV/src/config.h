#ifndef CONFIGS_H_
#define CONFIGS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "commons/log.h"
#include "commons/config.h"
#include "commons/collections/dictionary.h"
#include "commons/collections/list.h"
#include "commons/collections/queue.h"

#include "Consola.h"

bool cargar_config(char *config);
bool leerConfiguraciones( char * config);
bool validar_configuracion();
void destruir_config();

#define WORSTFIT 1
#define FIRSTFIT 0

extern t_config * umvConfig;

extern t_list * cpus;
extern t_list * programas;
extern t_list * tabla_segmentos;

extern void *memoria;
extern uint32_t memoria_size;
extern uint32_t retardoUMV;
extern uint32_t modoActualCreacionSegmentos;

#endif
