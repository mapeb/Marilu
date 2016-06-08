#ifndef IO_H_
#define IO_H_

#include <pthread.h>
#include <semaphore.h>
#include "commons/config.h"
#include "commons/collections/dictionary.h"
#include "commons/collections/queue.h"

 typedef struct {
	int retardo;
	pthread_t thread;
	t_queue *cola;
	pthread_mutex_t mutex;
	sem_t semaforo;
} io_t;

typedef struct {
	uint32_t pid;
	uint32_t tiempo;
} data_cola_t;

void *hilo_io(void *ptr);
io_t *crear_registro(char* hioRetardo);
void crear_pedido(char *dispositivo, uint32_t pid, uint32_t unidades);

void destruir_dispositivos();

#endif /* IO_H_ */
