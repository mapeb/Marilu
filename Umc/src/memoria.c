#include "memoria.h"
#include "tiposDato.h"
#include "umc.h"
/*void iniciarTablas(){
	// Si devuelve -1 hubo fallo al inicializar la tabla
	int i = 0;
	if (config.entradas_tlb > 0) {
		TLB = malloc(sizeof(TLB) * (config.entradas_tlb));
		if (TLB == NULL)
			log_error(log, "Error en la creacion de la TLB");
		for (i = 0; i < config.entradas_tlb; i++) {
			TLB[i].pid = -1;
			TLB[i].indice = -1;
			TLB[i].nPag = 0;
			TLB[i].numMarco = -1;
		}
	}
	marco = malloc(sizeof(tMarco) * (config.marcos));
	if (marco != NULL) {
		for (i = 0; i < (config.marcos); i++) //Inicia los marcos con el tamanio de cada uno.
				{
			marco[i].indice = -1; //Inicializamos todos los marcos como libres
			marco[i].pid = -1;
		}
	} else {
		log_error(log, "Error en la creacion de los marcos");
	}
	memoria = malloc((sizeof(char)) * config.marcos_size * config.marcos);
	if (memoria == NULL)
		log_error(log, "Error en la creacion de memoria");

	log_trace(log, "Se crearon las tablas");
}
void TLBFlush(void){
	int i = 0;

//pthread_mutex_lock(&MUTEXTLB);
	for (i = 0; i < config.entradas_tlb; i++) {
		TLB[i].pid = -1;
		TLB[i].indice = -1;
		TLB[i].nPag = 0;
		TLB[i].numMarco = -1;
	}
//pthread_mutex_unlock(&MUTEXTLB);
	printf("TLB BORRADA\n");
	return;
}
/*
void MPFlush(int socketsSwap) {
	int i;
	nodoListaTP* aux;
	nodoListaTP*nodo;
	mensaje_UMC_SWAP mensajeParaSWAP;
	mensaje_SWAP_UMC mensajeDeSWAP;

	for (i = 0; i < config.marcos; i++) {
		if (marco[i].pid != -1 && marco[i].indice != -1 && marco[i].modif == 1) //LA ENTRADA HAY QUE GUARDARLA EN SWAP PRIMERO
				{
			log_info(log,
					"Se envia el contenido del marco %d al SWAP. (Pagina: %d || PID: %d)",
					i, marco[i].nPag, marco[i].pid);
			mensajeParaSWAP.pid = marco[i].pid;
			nodo = buscarProceso(marco[i].pid);
			nodo->cantPaginasAcc++;
			mensajeParaSWAP.instruccion = ESCRIBIR;
			mensajeParaSWAP.parametro = marco[i].nPag;
			mensajeParaSWAP.contenidoPagina = malloc(config.marcos_size);
//strcpy(mensajeParaSWAP.contenidoPagina,tMarcos[i].contenido);
			memcpy(mensajeParaSWAP.contenidoPagina,
					&memoria[i * config.marcos_size], config.marcos_size);
			enviarDeUMCParaSwap(socketsSwap, &mensajeParaSWAP,
					config.marcos_size); //MANDA LAPAGINA A ESCRIBIRSE
			if (mensajeParaSWAP.contenidoPagina != NULL) {
				free(mensajeParaSWAP.contenidoPagina);
				mensajeParaSWAP.contenidoPagina = NULL;
			}
			recibirMensajeDeSwap(socketsSwap, &mensajeDeSWAP,
					config.marcos_size);
			if (mensajeDeSWAP.contenidoPagina != NULL)
				free(mensajeDeSWAP.contenidoPagina);
			mensajeDeSWAP.contenidoPagina = NULL;
		}
		marco[i].indice = -1; //libero Marcos
		marco[i].modif = 0;
	}
	if (config.entradas_tlb >= 1) { //BORRO LA TLB PUES SE BORRARON TODAS LAS PAGINSA EN MEMORIA
		for (i = 0; i < config.entradas_tlb; i++) {
			TLB[i].pid = -1;
			TLB[i].indice = -1;
			TLB[i].nPag = 0;
			TLB[i].numMarco = -1;
		}
	}
	// aux = raizTP;
	while (aux != NULL) {
		for (i = 0; i < aux->cantPaginas; i++) {
			(aux->tabla)[i].valido = 0; //Pone todas las paginas como no disponibles en memoria
		}
		aux->marcosAsignados = 0;
		aux = aux->sgte;
	}
	printf("MEMORIA PRINCIPAL BORRADA\n");
	/*pthread_mutex_unlock(&MUTEXLOG);
	 pthread_mutex_unlock(&MUTEXLP);
	 pthread_mutex_unlock(&MUTEXTM);
	 pthread_mutex_unlock(&MUTEXTLB);

	// flushMemoria = 0;
	return;
}
int cantMarcosOcupados() {
	int cuenta = 0;
	int i;
	for (i = 0; i < config.marcos; i++) {
		if (marco[i].indice != -1)
			cuenta++;
	}
	return cuenta;
}
void inic_program(int pid, int cantPag) {
	if (marco != NULL) {
		int i;
		for (i = 0; ((cantPag != 0) && (i < (config.marcos))); i++) {
			if (marco[i].indice == -1) {
				marco[i].pid = pid;
				marco[i].nPag = i;
				marco[i].indice = 0; // esta ocupado
				marco[i].modif = 0;
				cantPag--;
			}
			if ((i == (config.marcos - 1)) && cantPag < 0)
				printf("HAY QUE LIBERAR MEMORIA"); // HELP---------
		}
	}

}
char* solicitar_bytesPagina(int numPag, int offset, int tamanio) {
	int numMarco = estaLaPagina(numPag);
	if (marco == 0) {
		log_trace(log, "No se encuentra en la TLB  esa pagina");
		printf("ACA SOLICITO A SWAP LA PAGINA ");
	}
	char* retorno = malloc(sizeof(char) * tamanio);
	//char algo = strncpy(retorno, marco[numMarco].tamMarco); no existe tamMarco
	return retorno; // No hice un carajo con el offset
}
void almacenar_pagina(int numPag, int offset, int tamanio, char *buffer) {
	int numMarco = estaLaPagina(numPag);
	if (marco == 0) {
		log_trace(log, "No se encuentra en la TLB  esa pagina");
		printf("ACA SOLICITO A SWAP LA PAGINA ");
	}
	// falta aca
}
void finalizar_programa(int pid) {
	int i = 0;
	int paginaInvalida;
for (i = 0; i < config.entradas_tlb; i++) {
	if (TLB[i].pid == pid) {
		TLB[i].indice = -1;
	}
}
for (i = 0; i < config.marcos; i++) {
	if (marco[i].pid == pid) {
		marco[i].indice = -1;
		paginaInvalida = marco[i].nPag; // todavia no actualizo la validez de las paginas

	}
}
}
int estaLaPagina(int numPag) { // si esta devuelve el numero de marco
int i;
if (0 < config.entradas_tlb) {
	for (i = 0; i < config.entradas_tlb; i++) {
		if (TLB[i].nPag == numPag)
			return TLB[i].numMarco;
	}
}
return 0;
}*/

