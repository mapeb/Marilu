#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "io.h"
#include "colas.h"
#include "config.h"
#include "commons/pcb.h"

extern t_log *logpcp;

extern sem_t dispatcherReady, dispatcherCpu;

void *hilo_io(void *ptr){
	io_t *parametros = (io_t *) ptr;

	while(1){
		sem_wait(&parametros->semaforo);

		//saco el primer elemento de la cola del dispositivo para activarlo
		pthread_mutex_lock(&parametros->mutex);
		data_cola_t *orden_activa = queue_pop(parametros->cola);
		pthread_mutex_unlock(&parametros->mutex);

		log_trace(logpcp, "Procesando trabajo de IO, esperando %d ms",(orden_activa->tiempo) * (parametros->retardo));

		//aplico el retardo
		usleep((orden_activa->tiempo) * (parametros->retardo) * 1000);

		log_trace(logpcp, "Concluyo trabajo de IO");


		moverAReady(sacarDeBlock(orden_activa->pid));
		sem_post(&dispatcherReady);

		free(orden_activa);
	}

	return 0;
}

io_t *crear_registro(char* hioRetardo){

	//creo el registro del dispositivo io que va incluido en
	//la lista de dispositivos
	io_t *nuevo_registro = malloc(sizeof(io_t));
	
	//le asigno los datos al registro
	nuevo_registro->retardo = atoi(hioRetardo);
	nuevo_registro->cola = queue_create();

	pthread_mutex_init(&nuevo_registro->mutex, NULL);
	sem_init(&nuevo_registro->semaforo, 0, 0);
	pthread_create(&(nuevo_registro->thread), NULL, &hilo_io, (void *)nuevo_registro);

	return nuevo_registro;
}

void crear_pedido(char *dispositivo, uint32_t pid, uint32_t unidades)
{
	log_trace(logpcp, "Obteniendo dispositivo desde el diccionario de dispositivos");

	if( !dictionary_has_key(dispositivos, dispositivo))
		log_trace(logpcp, "El dispositivo %s no se encuentra en el diccionario. Segmentation fault", dispositivo);

	io_t *disp = dictionary_get(dispositivos, dispositivo);
	data_cola_t *pedido = malloc(sizeof(data_cola_t));

	pedido->pid = pid;
	pedido->tiempo = unidades;

	log_trace(logpcp, "Cargando nuevo trabajo en el dispositivo %s", dispositivo);

	//Cargando nuevo trabajo a la cola
	pthread_mutex_lock(&disp->mutex);
	queue_push(disp->cola, pedido);
	pthread_mutex_unlock(&disp->mutex);

	sem_post(&disp->semaforo);
}

void destruir_dispositivos()
{
	void destruir_dispositivo(io_t *disp)
	{
		queue_destroy_and_destroy_elements(disp->cola, free);
		pthread_cancel(disp->thread);
		pthread_join(disp->thread, NULL);
		free(disp);
	}
	dictionary_destroy_and_destroy_elements(dispositivos, destruir_dispositivo);
}
