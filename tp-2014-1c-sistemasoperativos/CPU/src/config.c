#include "config.h"
#include "primitivas.h"
#include "kernel.h"

t_config *config;

AnSISOP_funciones *ansisop_funciones;
AnSISOP_kernel *ansisop_Kernelfunciones;
extern char *etiquetasCache;

bool cargar_config(char *configFile)
{
	config = config_create(configFile);

	if( !validar_configuracion() ) {
		return false;
	}

	ansisop_Kernelfunciones = crearAnSISOP_kernel();
	ansisop_funciones = crearAnSISOP_funciones();
	etiquetasCache = malloc(1);

	return true;
}

bool validar_configuracion()
{

	if (!config_has_property(config, "IPKERNEL")
			|| !config_has_property(config, "PUERTOKERNEL")
			|| !config_has_property(config, "IPUMV")
			|| !config_has_property(config, "PUERTOUMV"))
	{
		return false;
	}

	return true;
}

void destruir_config()
{
	free(etiquetasCache);
	free(ansisop_Kernelfunciones);
	free(ansisop_funciones);
	config_destroy(config);
}
