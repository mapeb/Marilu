#ifndef SYSTEMCALLS_H_
#define SYSTEMCALLS_H_

#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <pthread.h>

#include "commons/sockets.h"
#include "commons/pcb.h"
#include "commons/log.h"
#include "commons/collections/dictionary.h"

bool syscallIO(int socketCPU);
bool syscallObtenerValor(int socketCPU);
bool syscallGrabarValor(int socketCPU);
bool syscallWait(int socketCPU);
bool syscallSignal(int socketCPU);
bool syscallImprimirTexto(int socketCPU);




#endif /* SYSTEMCALLS_H_ */
