
#ifndef UMC_SRC_UMC_H_
#define UMC_SRC_UMC_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/config.h>
#include <tiposDato.h>
#include <sockets.h>


typedef struct t_initialConfig {
	int puerto_cpu;
	int puerto_nucleo;
	char* ip_swap;
	int puerto_swap;
	int marcos;
	int marcos_size;
	int marco_x_proc;
	int entradas_tlb;
	int retardo;
	char *algoritmoReemplazo;
	int algoritmo;
};


struct t_initialConfig config;
t_log* log;
void atenderServidor();
int inic_program(int pid,int pag);
char* solicitar_bytesPagina(int numPag, int offset, int tamanio, int pid);
void almacenar_pagina(int numPag, int offset, int tamanio, char *buffer, int pid);
void finalizar_programa(int pid);

#endif /* UMC_SRC_UMC_H_ */
