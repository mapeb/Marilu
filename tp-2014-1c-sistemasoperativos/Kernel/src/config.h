#ifndef CONFIGS_H_
#define CONFIGS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "commons/config.h"
#include "commons/collections/dictionary.h"
#include "commons/collections/list.h"
#include "commons/collections/queue.h"

typedef struct {
	t_queue *cola;
	int32_t valor;
} semaforo_t;

bool cargar_config(char *configFile);

bool validar_configuracion();

void cargar_semaforos();
void cargar_variablesCompartidas();
void cargar_dispositivos();

void destruir_semaforos();
void destruir_config();

extern t_config *config;
extern t_dictionary *semaforos, *variablesCompartidas, *dispositivos;
#endif
