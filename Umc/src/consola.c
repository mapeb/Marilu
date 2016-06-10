/*
 * consola.c
 *
 *  Created on: 22/5/2016
 *      Author: utnso
 */

#include "memoria.h"

/*void menu() {
	char** comando;

	while (comando[0] != "CERRAR") {
		printf("Consola atenta: ");
		scanf("%d", &comando);

		if (comando[0] == "setRetardo"){
			setRetardo(comando[1]);}
		if (comando[0] == "dump"){
			dump(comando[1]);}
		if (comando[0] == "flush") {
			if (comando[1] == "tlb") {
				flush(1);
			} else if (comando[1] == "memory") {
				flush(2);
			}
		}
	}
}

void setRetardo(int num) {
	if (num > 0) {
		config.retardo = num;
	} else {
		log_error(log, "EL retardo no puede ser negativo");
	}
}
void dump(int numPid) {
	t_log* logMemoria = log_create("logMemoria", "umc", 1, 0);
	mostrarEstructurasDeMemoria(numPid, logMemoria);
	mostrarContenidoMemoria(numPid, logMemoria);
}
void mostrarEstructurasDeMemoria(int numPid, t_log* logMemoria) {
	int i;

	if (numPid == 0) {
		for (i = 0; i < config.marcos_size; i++) {
			if (marco[i].indice != -1 && marco[i].pid != -1) {
				if (config.algoritmo == 0 || config.algoritmo == 1) {
					log_info(logMemoria,
							"\n Marco: %d \t\t PID: %d \t\t Pagina: %d \t\t Indice: %d",
							i, marco[i].pid, marco[i].nPag, marco[i].indice);
				}
				if (config.algoritmo == 2) {
					log_info(log,
							"\n Marco: %d \t\t PID: %d \t\t Pagina: %d \t\t Bit Uso: %d \t\t Bit Modificado: %d",
							i, marco[i].pid, marco[i].nPag, marco[i].indice,
							marco[i].modif);
				}
			}
		}
	}
	if (numPid != 0) {
		for (i = 0; i < config.marcos_size; i++) {
			if (marco[i].indice != -1 && marco[i].pid == numPid) {
				if (config.algoritmo == 0 || config.algoritmo == 1) {
					log_info(logMemoria,
							"\n Marco: %d \t\t PID: %d \t\t Pagina: %d \t\t Indice: %d",
							i, marco[i].pid, marco[i].nPag, marco[i].indice);
				}
				if (config.algoritmo == 2) {
					log_info(logMemoria,
							"\n Marco: %d \t\t PID: %d \t\t Pagina: %d \t\t Bit Uso: %d \t\t Bit Modificado: %d",
							i, marco[i].pid, marco[i].nPag, marco[i].indice,
							marco[i].modif);

				}

				log_info(logMemoria, "\n ***********FIN************");
				// pthread_mutex_unlock(&MUTEXLOG);
			}
		}
	}
}
void mostrarContenidoMemoria(int numPid, t_log* logMemoria) {
	/*pthread_mutex_lock(&MUTEXLP);
	 pthread_mutex_lock(&MUTEXTM);
	 pthread_mutex_lock(&MUTEXTLB);
	 pthread_mutex_lock(&MUTEXLOG);
	 */
/*	int pHijo = fork();
	if (pHijo == -1) {
		printf("Fallo la creacion del hijo\n");
		return;
	}
	if (pHijo == 0) //PROCESO HIJO
			{
		int i;
		char * contenido;
		contenido = malloc((config.marcos_size * sizeof(char)) + 1);
		log_info(log, "\t\t\t\t******DUMP DE MEMORIA*******");
		log_info(log, "\t\t\t\t Marcos Libres: %d Marcos Ocupados: %d",
				config.marcos - cantMarcosOcupados(), cantMarcosOcupados());
		for (i = 0; i < config.marcos; i++) {
			if (marco[i].indice == -1) {
				continue;
				if (numPid == 0) {
					memcpy(contenido, &memoria[i * config.marcos_size],
							config.marcos_size);
					contenido[config.marcos_size] = '\0';
					log_info(log,
							"Marco: %d \t\t PID: %d \t\t Pagina: %d \t\t Contenido: %s",
							i, marco[i].pid, marco[i].nPag, contenido);
				}
			} else {
				if (marco[i].pid == numPid) {
					memcpy(contenido, &memoria[i * config.marcos_size],
							config.marcos_size);
					contenido[config.marcos_size] = '\0';
					log_info(log,
							"Marco: %d \t\t PID: %d \t\t Pagina: %d \t\t Contenido: %s",
							i, marco[i].pid, marco[i].nPag, contenido);
				}
			}

		}
		log_info(log, "\t\t\t\t\t***********FIN************");
		free(contenido);
		exit(0);
	} else {
		wait(NULL); //ESPERA A LA FINALIZACION DEL HIJO
		/*pthread_mutex_unlock(&MUTEXLOG); //ESPERA A QUE TERMINE EL DUMP PARA SEGUIR
		 pthread_mutex_unlock(&MUTEXLP);
		 pthread_mutex_unlock(&MUTEXTM);
		 pthread_mutex_unlock(&MUTEXTLB);
		 */
/*		printf("DUMP REALIZADO CON EXITO\n");
		return;
	}
}
void flush(int tipo) {
	int i;
	switch (tipo) {
	case 1:
		TLBFlush();
		break;
	case 2:
		for (i = 0; i < config.marcos; i++) {
			//if (marco[i] != -1) no podes comparar un struct con un int
				marco[i].modif = 1;
		}
		break;
	default:
		log_trace(log, "No se econocio la opcion");
	}
}*/
