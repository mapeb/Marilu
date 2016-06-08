#ifndef CPU_H_
#define CPU_H_
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "Programa.h"
#include "memoria.h"
#include "Consola.h"

#include "commons/collections/list.h"
#include "commons/log.h"
#include "commons/sockets.h"
#include "commons/sockets.h"


void fnNuevoCpu(int socketCPU);
bool recibirYProcesarPedidoCpu(int socketCPU);

bool leerMemoria(int socketCPU);
bool escribirMemoria(int socketCPU);

#endif /* CPU_H_ */
