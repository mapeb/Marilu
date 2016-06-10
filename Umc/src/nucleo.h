/*
 * nucleo.h
 *
 *  Created on: 18/5/2016
 *      Author: utnso
 */

#ifndef SRC_NUCLEO_H_
#define SRC_NUCLEO_H_

#include <tiposDato.h>
#include <sockets.h>
#include "umc.h"


void AtiendeClienteNucleo(int socket, struct sockaddr_in addr);

#endif /* SRC_NUCLEO_H_ */
