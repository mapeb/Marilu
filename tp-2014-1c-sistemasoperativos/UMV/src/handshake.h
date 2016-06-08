#ifndef HANDSHAKE_H_
#define HANDSHAKE_H_

#include <stdio.h>
#include <sys/socket.h>

#include "commons/sockets.h"
#include "commons/log.h"
#include "commons/config.h"

#include "Kernel.h"
#include "CPU.h"
#include "Programa.h"

void * handShake( void * socket );
void * crearConexiones();

#endif /* HANDSHAKE_H_ */
