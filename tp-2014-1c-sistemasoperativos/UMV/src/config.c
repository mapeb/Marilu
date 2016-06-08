#include "config.h"

extern t_log *logger;

t_list *cpus; //Lista en la que se van a guardar toda la info de cada cpu que se conecte
t_list *programas;
t_list *tabla_segmentos;

t_config *umvConfig;

void *memoria;
uint32_t memoria_size;
uint32_t retardoUMV;
uint32_t modoActualCreacionSegmentos;

const char cofig_properties[][25] = {
	"PUERTO", "MEMORIA", "RETARDOUMV", "MODOCREACIONSEGMENTOS"
};

bool cargar_config(char *config)
{
	umvConfig = config_create(config);

	if( !validar_configuracion() ) {
		return false;
	}

	programas			= list_create();
	tabla_segmentos		= list_create();

	memoria_size = config_get_int_value(umvConfig, "MEMORIA");
	retardoUMV = config_get_int_value(umvConfig, "RETARDOUMV");
	if( string_equals_ignore_case(config_get_string_value(umvConfig, "MODOCREACIONSEGMENTOS"), "WORSTFIT") ){
		modoActualCreacionSegmentos = WORSTFIT;
	}else {
		modoActualCreacionSegmentos = FIRSTFIT;
	}
	log_info( logger, "Reservando %d Bytes de memoria", memoria_size );

	memoria = malloc(memoria_size);
	if (memoria == 0) {
		log_error(logger, "No se pudo alocar la memoria, finalizando...");
		return false;
	}

	return true;
}

bool validar_configuracion()
{
	bool ret = true;
	int elements = sizeof(cofig_properties)/sizeof(cofig_properties[0]);
	int i;

	for(i = 0; i < elements; i++) {
		if( !config_has_property(umvConfig, &cofig_properties[i]) ) {
			ret = false;
			break;
		}
	}

	return ret;
}

void destruir_config()
{
	list_destroy_and_destroy_elements(tabla_segmentos, free);
	list_destroy_and_destroy_elements(programas, free);

	free(memoria);
	config_destroy(umvConfig);
}
