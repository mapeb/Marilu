#include "umc.h"
#include "funciones.h"
#include <config.h>
#include <stdio.h>
typedef struct nodo_t {
  char *dato;
  struct nodo_t *anterior;
  struct nodo_t *siguiente;
}nodo;
typedef struct lista_lru_t {
  nodo *inicio;
  nodo *fin;
  int tamanio;
}lista;
void inicializacion (lista *lista_lru){
	lista_lru->inicio = NULL;
	lista_lru->fin = NULL;
  lista_lru->tamanio = 0;
}
int insercion_en_lista_vacia (lista * lista, char *dato){
  nodo *nuevo_elemento;
  if ((nuevo_elemento = malloc (nuevo_elemento)) == NULL)
    return -1;
  strcpy (nuevo_elemento->dato, dato);
  nuevo_elemento->anterior = lista->inicio;
  nuevo_elemento->siguiente = lista->fin;
  lista->inicio = nuevo_elemento;
  lista->fin = nuevo_elemento;
  lista->tamanio++;
  return 0;
}
int push_lista(lista * lista, char *dato){
  nodo *nuevo_elemento;
  if ((nuevo_elemento = malloc (nuevo_elemento)) == NULL)
    return -1;
  strcpy (nuevo_elemento->dato, dato);
  nuevo_elemento->siguiente = NULL;
  nuevo_elemento->anterior = lista->fin;
  lista->fin->siguiente = nuevo_elemento;
  lista->fin = nuevo_elemento;
  lista->tamanio++;
  return 0;
}
int borrar(lista *lista, int pos){
  int i;
  int numTLB;
  nodo *sup_elemento,*actual;

  if(lista->tamanio == 0)
    return -1;

  if(pos == 1){ /* eliminación del 1er elemento */
    sup_elemento = lista->inicio;
    numTLB = lista->inicio->dato;
    lista->inicio = lista->inicio->siguiente;
    if(lista->inicio == NULL)
      lista->fin = NULL;
    else
      lista->inicio->anterior = NULL;
  }else if(pos == lista->tamanio){ /* eliminación del último elemento */
    sup_elemento = lista->fin;
    numTLB = lista->fin->dato;
    lista->fin->anterior->siguiente = NULL;
    lista->fin = lista->fin->anterior;
  }else { /* eliminación en otra parte */
    actual = lista->inicio;
      for(i=1;i<pos;++i)
        actual = actual->siguiente;
      numTLB = actual->dato;
    sup_elemento = actual;
    actual->anterior->siguiente = actual->siguiente;
    actual->siguiente->anterior = actual->anterior;
  }
  free(sup_elemento->dato);
  free(sup_elemento);
  lista->tamanio--;
  return numTLB;
}
void destruir(lista *lista){
  while(lista->tamanio > 0)
    borrar(lista,1);
}
void inicializarConfiguracion() {
	t_config *configuracion = config_create("config.txt");
	log_trace(log, "inicializarConfiguracion");

	if (verificarConfiguracion(configuracion)) {
		config.puerto_cpu = config_get_int_value(configuracion, "PUERTO_CPU");
		config.puerto_nucleo = config_get_int_value(configuracion,
				"PUERTO_NUCLEO");
		config.ip_swap = config_get_string_value(configuracion, "IP_SWAP");
		log_trace(log, "LA IP NO SALE DE ACA", config.ip_swap);
		config.puerto_swap = config_get_int_value(configuracion, "PUERTO_SWAP");
		config.marcos = config_get_int_value(configuracion, "MARCOS");
		config.marcos_size = config_get_int_value(configuracion, "MARCOS_SIZE");
		config.marco_x_proc = config_get_int_value(configuracion,
				"MARCO_X_PROC");
		config.entradas_tlb = config_get_int_value(configuracion,
				"ENTRADAS_TLB");
		config.retardo = config_get_int_value(configuracion, "RETADO");
//		config.algoritmoReemplazo = config_get_string_value(configuracion,
	//			"algoritmoReemplazo");
		log_trace(log, "Se cargaron todas las propiedades");
		config_destroy(configuracion);
		log_trace(log, "Finalizado inicializarConfiguracion");

		if (config.algoritmoReemplazo == "Clock") {
			config.algoritmo = 1;
		} else {
			if (config.algoritmoReemplazo == "ClockM"
					|| config.algoritmoReemplazo == "ClockModificado") {
				config.algoritmo = 2;
			} else {
				config.algoritmo = 0;
			}
		}
		log_trace(log, "config.puerto_cpu: %d", config.puerto_cpu);
		log_trace(log, "config.puerto_nucleo: %d", config.puerto_nucleo);
		log_trace(log, "config.ip_swap: %s", config.ip_swap);
		log_trace(log, "config.puerto_swap: %d", config.puerto_swap);
		log_trace(log, "config.marcos: %d", config.marcos);
		log_trace(log, "config.marcos_size: %d", config.marcos_size);
		log_trace(log, "config.marco_x_proc: %d", config.marco_x_proc);
		log_trace(log, "config.entradas_tlb: %d", config.entradas_tlb);
		log_trace(log, "config.retardo: %d", config.retardo);
		log_trace(log, "config.algoritmoReemplazo: %d",
				config.algoritmoReemplazo);
	}
}
int verificarConfiguracion(t_config *configuracion) {
	log_trace(log, "verificarConfiguracion");
	if (!config_has_property(configuracion, "PUERTO_CPU"))
		log_error(log, "No existe PUERTO_CPU en la configuracion");
	if (!config_has_property(configuracion, "PUERTO_NUCLEO"))
		log_error(log, "No existe PUERTO_NUCLEO en la configuracion");
	if (!config_has_property(configuracion, "IP_SWAP"))
		log_error(log, "No existe IP_SWAP en la configuracion");
	if (!config_has_property(configuracion, "PUERTO_SWAP"))
		log_error(log, "No existe PUERTO_SWAP en la configuracion");
	if (!config_has_property(configuracion, "MARCOS"))
		log_error(log, "No existe MARCOS en la configuracion");
	if (!config_has_property(configuracion, "MARCOS_SIZE"))
		log_error(log, "No existe MARCOS_SIZE en la configuracion");
	if (!config_has_property(configuracion, "MARCO_X_PROC"))
		log_error(log, "No existe MARCO_X_PROC en la configuracion");
	if (!config_has_property(configuracion, "ENTRADAS_TLB"))
		log_error(log, "No existe ENTRADAS_TLB en la configuracion");
	if (!config_has_property(configuracion, "RETADO"))
		log_error(log, "No existe RETADO en la configuracion");
	if (!config_has_property(configuracion, "algoritmoReemplazo"))
		log_error(log, "No existe algoritmoReemplazo en la configuracion");
	log_trace(log, "Finalizado verificarConfiguracion");
	return 1;
}
