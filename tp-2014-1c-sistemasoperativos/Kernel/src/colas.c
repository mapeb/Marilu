#include "colas.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "commons/collections/queue.h"

extern t_log *logpcp;

t_queue *newQueue, *readyQueue, *execQueue, *exitQueue, *blockQueue;
pthread_mutex_t readyQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t blockQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t execQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t exitQueueMutex = PTHREAD_MUTEX_INITIALIZER;

t_queue *cpuReadyQueue, *cpuExecQueue;
pthread_mutex_t cpuReadyQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cpuExecQueueMutex = PTHREAD_MUTEX_INITIALIZER;

t_list *programasConectados;
pthread_mutex_t programasConectadosMutex = PTHREAD_MUTEX_INITIALIZER;

void crear_colas()
{
	newQueue = queue_create();
	readyQueue = queue_create();
	execQueue = queue_create();
	exitQueue = queue_create();
	blockQueue = queue_create();

	cpuReadyQueue = queue_create();
	cpuExecQueue = queue_create();

	programasConectados = list_create();
}

void destruir_colas()
{
	queue_destroy_and_destroy_elements(newQueue, free);
	queue_destroy_and_destroy_elements(readyQueue, free);
	queue_destroy_and_destroy_elements(execQueue, free);
	queue_destroy_and_destroy_elements(exitQueue, free);
	queue_destroy_and_destroy_elements(blockQueue, free);

	queue_destroy_and_destroy_elements(cpuReadyQueue, free);
	queue_destroy_and_destroy_elements(cpuExecQueue, free);

	list_destroy_and_destroy_elements(programasConectados, free);
}

pcb_t *list_remove_pcb_by_pid(t_list *self, uint32_t pid)
{
	bool matchearPCB(pcb_t *pcb) {
		return pcb->id == pid;
	}
	return list_remove_by_condition(self, matchearPCB);
}

cpu_info_t *list_remove_cpuInfo_by_socketCpu(t_list *self, int socket)
{
	bool matchearCPU(cpu_info_t *cpuInfo){
		return cpuInfo->socketCPU == socket;
	}
	return list_remove_by_condition(self, matchearCPU);
}


void moverAExit(pcb_t *pcb)
{
	log_debug(logpcp, "Moviendo PCB: %d a la cola Exit",pcb->id);
	pthread_mutex_lock(&exitQueueMutex);
	queue_push(exitQueue, pcb);
	pthread_mutex_unlock(&exitQueueMutex);
	borrarSegmentos(pcb->id);
}

void moverABlock(pcb_t *pcb)
{
	log_debug(logpcp, "Moviendo PCB: %d a la cola Block",pcb->id);
	pthread_mutex_lock(&blockQueueMutex);
	queue_push(blockQueue, pcb);
	pthread_mutex_unlock(&blockQueueMutex);
}

void moverAExec(pcb_t *pcb)
{
	log_debug(logpcp, "Moviendo PCB: %d a la cola Exec",pcb->id);
	pthread_mutex_lock(&execQueueMutex);
	queue_push(execQueue, pcb);
	pthread_mutex_unlock(&execQueueMutex);
}

void moverAReady(pcb_t *pcb)
{
	log_debug(logpcp, "Moviendo PCB: %d a la cola Ready",pcb->id);
	pthread_mutex_lock(&readyQueueMutex);
	queue_push(readyQueue, pcb);
	pthread_mutex_unlock(&readyQueueMutex);
}

void moverCpuAReady(cpu_info_t *cpuInfo)
{
	log_debug(logpcp, "Moviendo CPU: %d a la cola cpuReady",cpuInfo->socketCPU);
	pthread_mutex_lock(&cpuReadyQueueMutex);
	queue_push(cpuReadyQueue, cpuInfo);
	pthread_mutex_unlock(&cpuReadyQueueMutex);
}

void moverCpuAExec(cpu_info_t *cpuInfo)
{
	log_debug(logpcp, "Moviendo CPU: %d a la cola cpuExec",cpuInfo->socketCPU);
	pthread_mutex_lock(&cpuExecQueueMutex);
	queue_push(cpuExecQueue, cpuInfo);
	pthread_mutex_unlock(&cpuExecQueueMutex);
}

pcb_t *sacarDeReady(uint32_t pid)
{
	log_debug(logpcp, "Sacando PCB: %d de la cola Ready",pid);
	pthread_mutex_lock(&readyQueueMutex);
	pcb_t *pcb = list_remove_pcb_by_pid(readyQueue->elements, pid);
	pthread_mutex_unlock(&readyQueueMutex);
	return pcb;
}

pcb_t *sacarDeExec(uint32_t pid)
{
	log_debug(logpcp, "Sacando PCB: %d de la cola Exec",pid);
	pthread_mutex_lock(&execQueueMutex);
	pcb_t *pcb = list_remove_pcb_by_pid(execQueue->elements, pid);
	pthread_mutex_unlock(&execQueueMutex);
	return pcb;
}

pcb_t *sacarDeBlock(uint32_t pid)
{
	log_debug(logpcp, "Sacando PCB: %d de la cola Block",pid);
	pthread_mutex_lock(&blockQueueMutex);
	pcb_t *pcb = list_remove_pcb_by_pid(blockQueue->elements, pid);
	pthread_mutex_unlock(&blockQueueMutex);
	return pcb;
}

cpu_info_t *sacarCpuDeExec(int socketCPU)
{
	log_debug(logpcp, "Sacando CPU: %d de la cola cpuExec",socketCPU);
	pthread_mutex_lock(&cpuExecQueueMutex);
	cpu_info_t *cpuInfo = list_remove_cpuInfo_by_socketCpu(cpuExecQueue->elements, socketCPU);
	pthread_mutex_unlock(&cpuExecQueueMutex);
	return cpuInfo;
}

cpu_info_t *sacarCpuDeReady(int socketCPU)
{
	log_debug(logpcp, "Sacando CPU: %d de la cola cpuReady",socketCPU);
	pthread_mutex_lock(&cpuReadyQueueMutex);
	cpu_info_t *cpuInfo = list_remove_cpuInfo_by_socketCpu(cpuReadyQueue->elements, socketCPU);
	pthread_mutex_unlock(&cpuReadyQueueMutex);
	return cpuInfo;
}


conectados_t *removerProgramaConectadoPorSocket(int socketPrograma){

	bool matchearPID(conectados_t *conectado) {
		return conectado->programaSocket == socketPrograma;
	}

	pthread_mutex_lock(&programasConectadosMutex);
	conectados_t *conectado = list_remove_by_condition(programasConectados,matchearPID);
	pthread_mutex_unlock(&programasConectadosMutex);
	return conectado;
}

conectados_t *removerProgramaConectadoId(uint32_t id){

	bool matchearPID(conectados_t *conectado) {
		return conectado->pid == id;
	}

	pthread_mutex_lock(&programasConectadosMutex);
	conectados_t *conectado = list_remove_by_condition(programasConectados,matchearPID);
	pthread_mutex_unlock(&programasConectadosMutex);
	return conectado;
}

conectados_t *buscarProgramaConectado(uint32_t id){

	bool matchearPID(conectados_t *conectado) {
		return conectado->pid == id;
	}

	pthread_mutex_lock(&programasConectadosMutex);
	conectados_t *conectado = list_find(programasConectados,matchearPID);
	pthread_mutex_unlock(&programasConectadosMutex);
	return conectado;
}
