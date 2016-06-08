#include "config.h"
#include "io.h"

t_config *config;

t_dictionary *semaforos, *variablesCompartidas, *dispositivos;

const char cofig_properties[][25] = {
	"PUERTO_PROG", "PUERTO_CPU", "QUANTUM", "RETARDO",
	"MULTIPROGRAMACION", "SEMAFOROS", "VALOR_SEMAFORO",
	"HIO", "ID_HIO", "VARIABLES_COMPARTIDAS",
	"IP_UMV", "PUERTO_UMV", "STACK_SIZE"
};

bool cargar_config(char *configFile)
{
	config = config_create(configFile);

	if( !validar_configuracion() ) {
		return false;
	}

	cargar_semaforos();
	cargar_variablesCompartidas();
	cargar_dispositivos();

	return true;
}

bool validar_configuracion()
{
	bool ret = true;
	int elements = sizeof(cofig_properties)/sizeof(cofig_properties[0]);
	int i;

	for(i = 0; i < elements; i++) {
		if( !config_has_property(config, &cofig_properties[i]) ) {
			ret = false;
			break;
		}
	}

	return ret;
}

void cargar_semaforos()
{
	char** semaforosArray = config_get_array_value(config, "SEMAFOROS");
	char** valorSemaforosArray = config_get_array_value(config, "VALOR_SEMAFORO");

	semaforos = dictionary_create();

	int i;
	semaforo_t *semaforo;

	for(i = 0; semaforosArray[i] != NULL && valorSemaforosArray[i] != NULL; i++)
	{
		semaforo = malloc(sizeof(semaforo_t));
		semaforo->valor = atoi(valorSemaforosArray[i]);
		semaforo->cola = queue_create();

		dictionary_put(semaforos, semaforosArray[i], semaforo);
	}

	string_iterate_lines(semaforosArray, free);
	string_iterate_lines(valorSemaforosArray, free);
	free(semaforosArray);
	free(valorSemaforosArray);
}

void cargar_variablesCompartidas()
{
	char** variablesCompartidasArray = config_get_array_value(config, "VARIABLES_COMPARTIDAS");

	variablesCompartidas = dictionary_create();

	int i;

	for(i = 0; variablesCompartidasArray[i] != NULL; i++)
	{
		int32_t *valor = malloc(sizeof(int32_t));
		*valor = 0;

		dictionary_put(variablesCompartidas, variablesCompartidasArray[i], valor);
	}

	string_iterate_lines(variablesCompartidasArray, free);
	free(variablesCompartidasArray);
}

void cargar_dispositivos() {
	char** hioId = config_get_array_value(config, "ID_HIO");
	char** hioRetardo = config_get_array_value(config, "HIO");

	dispositivos = dictionary_create();

	int i;

	for(i = 0; hioId[i] != NULL; i++) {
		io_t *registro_hio = crear_registro(hioRetardo[i]);
		dictionary_put(dispositivos, hioId[i], registro_hio);
	}

	string_iterate_lines(hioId, free);
	string_iterate_lines(hioRetardo, free);
	free(hioId);
	free(hioRetardo);
}

void destruir_semaforos()
{
	void destruir_semaforo(semaforo_t *semaforo)
	{
		queue_destroy(semaforo->cola);
		free(semaforo);
	}
	dictionary_destroy_and_destroy_elements(semaforos, destruir_semaforo);
}

void destruir_config()
{
	destruir_semaforos();
	dictionary_destroy_and_destroy_elements(variablesCompartidas, free);
	destruir_dispositivos();

	config_destroy(config);
}
