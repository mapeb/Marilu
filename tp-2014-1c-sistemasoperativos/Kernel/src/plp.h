#ifndef PLP_H_
#define PLP_H_

#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <pthread.h>
#include <semaphore.h>

#include "commons/log.h"
#include "commons/config.h"
#include "commons/sockets.h"
#include "commons/pcb.h"
#include "commons/parser/metadata_program.h"

void *IniciarPlp(void *arg);
bool iniciarServidorProgramas();

void MoverNewAReady();
void puedoMoverNewAReady();

void desconexionCliente();

bool recibirScriptAnsisop(int socketPrograma, char **script, uint32_t *scriptSize);
pcb_t *crearPCB(int socketPrograma, socket_umvpcb *umvpcb, t_metadata_program *scriptMetadata);
bool crearPrograma(int socketPrograma, char *script, uint32_t scriptSize, t_metadata_program *scriptMetadata);
bool recibirYprocesarScript(int socketPrograma);

void mensajeYDesconexionPrograma(int programaSocket, char *mensaje);

#endif /* PLP_H_ */
