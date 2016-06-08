#ifndef CONFIGS_H_
#define CONFIGS_H_

#include <stdbool.h>
#include "commons/config.h"

#include "stack.h"
#include "primitivas.h"

bool cargar_config(char *configFile);
bool validar_configuracion();
void destruir_config();

extern t_config *config;
extern AnSISOP_funciones * ansisop_funciones;

#endif /* CONFIGS_H_ */
