#ifndef UMV_H_
#define UMV_H_
#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <semaphore.h>

#include "commons/log.h"
#include "commons/config.h"
#include "commons/sockets.h"
#include "commons/parser/metadata_program.h"

bool conectarUMV();

bool solicitarCreacionSegmentos(uint32_t scriptSize, t_metadata_program *scriptMetadata);
bool respuestaCreacionSegmentos();
bool enviarSegmentos(uint32_t pid, char *script, uint32_t scriptSize, t_metadata_program *scriptMetadata);
bool respuestaSegmentos(socket_umvpcb *umvpcb);
bool borrarSegmentos(uint32_t pid);

#endif /* UMV_H_ */
