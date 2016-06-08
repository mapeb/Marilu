#ifndef COLAS_H_
#define COLAS_H_

#include <pthread.h>
#include "commons/collections/queue.h"
#include "pcp.h"

extern t_queue *newQueue; //La usa el PLP
extern t_queue *readyQueue; //La usa el PLP, PCP y el IO
extern t_queue *execQueue; //La usa el PCP y el Dispatcher
extern t_queue *exitQueue; //La usa el PCP
extern t_queue *blockQueue; //Las usa el PCP y el IO

extern pthread_mutex_t readyQueueMutex;
extern pthread_mutex_t blockQueueMutex;
extern pthread_mutex_t execQueueMutex;
extern pthread_mutex_t exitQueueMutex;


extern t_queue *cpuReadyQueue;
extern t_queue *cpuExecQueue;

extern pthread_mutex_t cpuReadyQueueMutex;
extern pthread_mutex_t cpuExecQueueMutex;

extern t_list *programasConectados;
extern pthread_mutex_t programasConectadosMutex;

typedef struct {
	uint32_t pid;
	int programaSocket;
} conectados_t;

void crear_colas();
void destruir_colas();

pcb_t *list_remove_pcb_by_pid(t_list *self, uint32_t pid);
cpu_info_t *list_remove_cpuInfo_by_socketCpu(t_list *self, int socket);
void MoverAExit(pcb_t *pcb);
void moverABlock(pcb_t *pcb);
void moverAExec(pcb_t *pcb);
void moverAReady(pcb_t *pcb);
void moverCpuAReady(cpu_info_t *cpuInfo);
void moverCpuAExec(cpu_info_t *cpuInfo);
pcb_t *sacarDeExec(uint32_t pid);
pcb_t *sacarDeBlock(uint32_t pid);
cpu_info_t *sacarCpuDeExec(int socketCPU);
cpu_info_t *sacarCpuDeReady(int socketCPU);
conectados_t *removerProgramaConectadoPorSocket(int socketPrograma);
conectados_t *buscarProgramaConectado(uint32_t id);

#endif /* COLAS_H_ */
