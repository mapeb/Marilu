#include "umc.h"

void menu();

void* memoria;
pthread_mutex_t MUTEXTLB = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t MUTEXMARCOS = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t MUTEXCONFIG = PTHREAD_MUTEX_INITIALIZER;

fd_set listaDeSockets; // conjunto de sockets para el select
int socketsEscucha;
pthread_t hiloConsola;
pthread_t cpuThread;
tlb* TLB; // cache rapida para el indice de marcos los marcos
tMarco* marco; //lista con el indice  de cada marco
void*memoriaPrueba;
nodoProcesos* listaProcesos;
int clock;

int main() {
	log = log_create("log", "umc", 1, 0);
	inicializarConfiguracionUMC();
	iniciarTablas();
	memoria = malloc(config.marcos_size * config.marcos);
	hacerElHandShakeConNucleo();
	//pthread_create(&hiloConsola, NULL, menu, NULL);
	atenderServidor(); // funcion Donde se atiende Tanto a cpu como nucleo
	//crearHiloParaAtenderSwap(); no se cuando voy a atender a la swap
	//	hiloaAtenderCpu();
	//atenderNucleo();
	pthread_join(hiloConsola, NULL);
	pthread_join(cpuThread, NULL);

	return 0;
}

void inicializarConfiguracionUMC() {
	t_config *configuracion = config_create("config.txt");
	log_trace(log, "inicializarConfiguracion");

	if (verificarConfiguracion(configuracion)) {
		config.puerto_cpu = config_get_int_value(configuracion, "PUERTO_CPU");
		config.puerto_nucleo = config_get_int_value(configuracion,
				"PUERTO_NUCLEO");
		config.ip_swap = config_get_string_value(configuracion, "IP_SWAP");
		config.puerto_swap = config_get_int_value(configuracion, "PUERTO_SWAP");
		config.marcos = config_get_int_value(configuracion, "MARCOS");
		config.marcos_size = config_get_int_value(configuracion, "MARCOS_SIZE");
		config.marco_x_proc = config_get_int_value(configuracion,
				"MARCO_X_PROC");
		config.entradas_tlb = config_get_int_value(configuracion,
				"ENTRADAS_TLB");
		config.retardo = config_get_int_value(configuracion, "RETADO");
		config.algoritmoReemplazo = config_get_string_value(configuracion,
				"algoritmoReemplazo");
		log_trace(log, "Se cargaron todas las propiedades");
		config_destroy(configuracion);

		log_trace(log, "config.puerto_cpu: %d", config.puerto_cpu);
		log_trace(log, "config.puerto_nucleo: %d", config.puerto_nucleo);
		log_trace(log, "config.ip_swap: %s", config.ip_swap);
		log_trace(log, "config.puerto_swap: %d", config.puerto_swap);
		log_trace(log, "config.marcos: %d", config.marcos);
		log_trace(log, "config.marcos_size: %d", config.marcos_size);
		log_trace(log, "config.marco_x_proc: %d", config.marco_x_proc);
		log_trace(log, "config.entradas_tlb: %d", config.entradas_tlb);
		log_trace(log, "config.retardo: %d", config.retardo);
		log_trace(log, "config.algoritmoReemplazo: %s",
				config.algoritmoReemplazo);
		if (strcmp(config.algoritmoReemplazo, "Clock") == 0) {
			config.algoritmo = 1;
			clock = 0;
		} else {
			if (strcmp(config.algoritmoReemplazo, "ClockM") == 0
					|| strcmp(config.algoritmoReemplazo, "ClockModificado")
							== 0) {
				config.algoritmo = 2;
			} else {
				config.algoritmo = 0;
			}
		}
		log_trace(log, "Finalizado inicializarConfiguracion");
	}
}
int verificarConfiguracionUMC(t_config *configuracion) {
	log_trace(log, "verificarConfiguracion");
	if (!config_has_property(configuracion, "PUERTO_CPU"))
		log_error(log, "No existe PUERTO_CPU en la configuracion");
	if (!config_has_property(configuracion, "PUERTO_NUCLEO"))
		log_error(log, "No existe PUERTO_NUCLEO en la configuracion");
	if (!config_has_property(configuracion, "IP_SWAP"))
		log_error(log, "No existe IP_SWAP en la configuracion");
	if (!config_has_property(configuracion, "PUERTO_SWAP"))
		log_error(log, "No existe PUERTO_SWAP en la configuracion");
	if (!config_has_property(configuracion, "MARCOS"))
		log_error(log, "No existe MARCOS en la configuracion");
	if (!config_has_property(configuracion, "MARCOS_SIZE"))
		log_error(log, "No existe MARCOS_SIZE en la configuracion");
	if (!config_has_property(configuracion, "MARCO_X_PROC"))
		log_error(log, "No existe MARCO_X_PROC en la configuracion");
	if (!config_has_property(configuracion, "ENTRADAS_TLB"))
		log_error(log, "No existe ENTRADAS_TLB en la configuracion");
	if (!config_has_property(configuracion, "RETADO"))
		log_error(log, "No existe RETADO en la configuracion");
	if (!config_has_property(configuracion, "algoritmoReemplazo"))
		log_error(log, "No existe algoritmoReemplazo en la configuracion");
	log_trace(log, "Finalizado verificarConfiguracion");
	return 1;
}

int inic_program(int pid, int cantPag) { // 0: reservo marcos, -1: Stack overfloat guardar en  swap
	if (marco != NULL) {
		int i;

		if ((config.marcos - cantMarcosOcupados() - cantPag) > 0) {
			pthread_mutex_lock(&MUTEXMARCOS);
			for (i = 0; ((cantPag != 0) && (i < (config.marcos))); i++) {
				if (marco[i].indice == -1) {
					marco[i].pid = pid;
					marco[i].nPag = i;
					marco[i].indice = 0; // esta ocupado
					marco[i].modif = 0;
					cantPag--;
				}
				pthread_mutex_unlock(&MUTEXMARCOS);
			}
			return 0;
		} else {
			printf("STACK OVERFLOAT: HAy que guardar marcos en Swap creo!!!");

			return -1;
		}

	}

	return -1;
}
char* solicitar_bytesPagina(int numPag, int offset, int tamanio, int pid) { // esta funcion esta mal
	int numMarco = numeroMarcoTLB(numPag, pid);
	char * retorno;
	if (numMarco < 0) {
		log_trace(log, "No se encuentra en la TLB  esa pagina");
		numMarco = marcoDeTablaPaginas(numPag, pid);
		if (numMarco == -1) {
			log_trace(log,
					"No se encuentra en la Tabla de paginas  esa pagina");
			pageFault(numPag, pid);
			return solicitar_bytesPagina(numPag, offset, tamanio, pid);
		}
	}
	retorno = malloc(sizeof(char) * tamanio);
	memcpy(memoria + config.marcos_size * numPag + offset, &retorno, tamanio);

	return retorno;
}
void almacenar_pagina(int numPag, int offset, int tamanio, char *buffer,
		int pid) {
	int numMarco = numeroMarcoTLB(numPag, pid);
	if (numMarco < 0) {
		log_trace(log, "No se encuentra en la TLB  esa pagina");
		numMarco = marcoDeTablaPaginas(numPag, pid);
		if (numMarco == -1) {
			log_trace(log,
					"No se encuentra en la Tabla de paginas  esa pagina");
			pageFault(numPag, pid);
			almacenar_pagina(numPag, offset, tamanio, *buffer, pid);
		} else {
			memcpy(buffer, &(memoria[config.marcos_size * numPag + offset]),
					tamanio);
		}
	}
}
void finalizar_programa(int pid) {
	int i = 0;
	int paginaInvalida;
	pthread_mutex_lock(&MUTEXTLB);
	for (i = 0; i < config.entradas_tlb; i++) {
		if (TLB[i].pagina.pid == pid) {
			TLB[i].indice = -1;
		}
	}
	pthread_mutex_unlock(&MUTEXTLB);
	pthread_mutex_lock(&MUTEXMARCOS);
	for (i = 0; i < config.marcos; i++) {
		if (marco[i].pid == pid) {
			if (marco[i].modif == 1) {
				guardarMarcoSwap(marco[i], i);
			}
			marco[i].indice = -1;
			paginaInvalida = marco[i].nPag; // todavia no actualizo la validez de las paginas

		}
	}
	pthread_mutex_unlock(&MUTEXMARCOS);
}

void hacerElHandShakeConNucleo(void) {
	int socketDeNucleo = crearSocketEscucha(config.puerto_nucleo);
	mensaje_NUCLEO_UMC mensajeArecibir;
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);
	int socketDeNucleoAceptado = accept(socketDeNucleo,
			(struct sockaddr *) &addr, &addrlen);
	recibirMensajeNUCLEO_UMC(socketDeNucleoAceptado, &mensajeArecibir,
			config.marcos_size);
	if (mensajeArecibir.instruccion == INICIAR) {
		mensaje_UMC_NUCLEO mensajeAenviar;
		mensajeAenviar.instruccion = TAMPAG;
		mensajeAenviar.tamanio = config.marcos_size;
		enviarMensajeUMC_NUCLEO(socketDeNucleoAceptado, mensajeAenviar);
		close(socketDeNucleoAceptado);
		close(socketDeNucleo);
	}
}
void atenderServidor() {
	int socketServidorNucleo = crearSocketEscucha(config.puerto_nucleo);
	int socketServidorCpu = crearSocketEscucha(config.puerto_cpu);
	struct sockaddr_in client_addr;
	socklen_t size_addr = 0;
	/*pthread_t hiloEscucha;
	 int hiloId = pthread_create(&hiloEscucha, NULL,
	 (void *) agregarSocketsALaLista,
	 NULL);
	 if (hiloId == 0)
	 printf("Nose que hacer con esta devolucion");
	 ELIMINO LOS THREADS ... NO ME SALE USARLOS BIEN :(*/
	size_addr = sizeof(struct sockaddr_in);
	int activated = 1;
	while (activated) {
		FD_ZERO(&listaDeSockets);
		FD_SET(socketServidorNucleo, &listaDeSockets);
		FD_SET(socketServidorCpu, &listaDeSockets);

		if (select(sizeof(listaDeSockets), &listaDeSockets, NULL, NULL,
		NULL)) {

			if (FD_ISSET(socketServidorNucleo, &listaDeSockets)) {
				accept(socketServidorNucleo, (struct sockaddr*) &client_addr,
						&size_addr);
				printf("\nSe ha conectado un Nucleo\n");
				atiendeClienteNucleo(socketServidorNucleo, client_addr);
			}

			if (FD_ISSET(socketServidorCpu, &listaDeSockets)) {
				accept(socketServidorCpu, (struct sockaddr*) &client_addr,
						&size_addr);
				printf("\nSe ha conectado un Cpu\n");
				hiloAtenderCpu(socketServidorCpu, client_addr);
			}
		}
	}

	close(socketServidorCpu);
	close(socketServidorNucleo);
}
void atiendeClienteCpu(tclienteParaAtender *cliente) {
	uint32_t pid;
	int sem_t = 1;
	int socket = cliente->socketParaAtender;
	struct sockaddr_in addr = cliente->client_addrress;
	char *buffer;
	mensaje_CPU_UMC* mensajeRecibido;
	mensaje_UMC_CPU* mensajeAMandar;
	while (sem_t) {
		recibirMensajeCPU_UMC(socket, mensajeRecibido);
		switch (mensajeRecibido->instruccion) {
		case INICIAR:
			pid = mensajeRecibido->pid;
			break;
		case LEER:
			buffer = solicitar_bytesPagina(mensajeRecibido->pagina,
					mensajeRecibido->offset, mensajeRecibido->tamanio,
					mensajeRecibido->pid);
			mensajeAMandar->tamanoMensaje = sizeof(buffer);
			mensajeAMandar->texto = buffer;
			enviarMensajeUMC_CPU(socket, mensajeAMandar);
			break;
		case ESCRIBIR:
			almacenar_pagina(mensajeRecibido->pagina, mensajeRecibido->offset,
					mensajeRecibido->tamanio, mensajeRecibido->buffer,
					mensajeRecibido->pid);
			break;
		case FINALIZAR:
			finalizar_programa(mensajeRecibido->pid);
			break;

		}
	}
	close(socket);
}
void hiloAtenderCpu(int socket, struct sockaddr_in addr) {
	tclienteParaAtender* cliente;
	cliente->socketParaAtender = socket;
	cliente->client_addrress = addr;
	pthread_t newThread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&newThread, &attr, atiendeClienteCpu, (void *) cliente);
// Inicializar programa();
// finalizarPRograma();
	pthread_attr_destroy(&attr);
}
void atiendeClienteNucleo(int socket, struct sockaddr_in addr) {
	int fin = 0;
	mensaje_NUCLEO_UMC* mensajeArecibir;
	mensaje_UMC_NUCLEO* mensajeAenviar;
	mensaje_SWAP_UMC* mensajeArecibirSWAP;
	mensaje_UMC_SWAP* mensajeAenviarSWAP;
	while (!fin) {
		recibirMensajeNUCLEO_UMC(socket, mensajeArecibir, config.marcos_size);
		switch (mensajeArecibir->instruccion) {
		case INICIAR:
			mensajeAenviarSWAP->instruccion = INICIAR;
			mensajeAenviarSWAP->parametro = mensajeArecibir->cantidadPaginas;
			enviarMensajeUMC_SWAP(config.puerto_swap, mensajeAenviarSWAP,
					config.marcos_size);
			recibirMensajeSWAP_UMC(config.puerto_swap, mensajeArecibirSWAP,
					config.marcos_size);
			if (mensajeArecibirSWAP->instruccion != ERROR) {
				mensajeAenviar->instruccion = TAMPAG;
				mensajeAenviar->tamanio = config.marcos_size;
				enviarMensajeUMC_NUCLEO(socket, mensajeAenviar);
			} else {

				mensajeAenviar->instruccion = ERROR;
				enviarMensajeUMC_NUCLEO(socket, mensajeAenviar);
				fin = 1;
			}
			break;
		case ESCRIBIR:
			mensajeAenviarSWAP->instruccion = ESCRIBIR;
			mensajeAenviarSWAP->pid = mensajeArecibir->pid;
			mensajeAenviarSWAP->contenidoPagina = mensajeArecibir->script;
			mensajeAenviarSWAP->parametro = mensajeArecibir->cantidadPaginas;
			enviarMensajeUMC_SWAP(config.puerto_swap, mensajeAenviarSWAP,
					config.marcos_size);
			fin = 1;
			break;
		}
	}
	close(socket);
}

void iniciarTablas() {
	// Si devuelve -1 hubo fallo al inicializar la tabla
	int i;
	if (config.entradas_tlb > 0) {
		TLB = malloc(sizeof(TLB) * (config.entradas_tlb));
		if (TLB == NULL)
			log_error(log, "Error en la creacion de la TLB");
		for (i = 0; i < config.entradas_tlb; i++) {
			TLB[i].pagina.pid = -1;
			TLB[i].indice = -1;
			TLB[i].pagina.numeroPagina = 0;
			TLB[i].pagina.nroMarco = -1;
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
void TLBFlush(void) {
	pthread_mutex_lock(&MUTEXTLB);
	int i = 0;
	for (i = 0; i < config.entradas_tlb; i++) {
		TLB[i].pagina.pid = -1;
		TLB[i].indice = -1;
		TLB[i].pagina.numeroPagina = 0;
		TLB[i].pagina.nroMarco = -1;
	}
	pthread_mutex_unlock(&MUTEXTLB);
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
 }*/
int cantMarcosOcupados() {
	int cuenta = 0;
	int i;
	pthread_mutex_lock(&MUTEXMARCOS);
	for (i = 0; i < config.marcos; i++) {
		if (marco[i].indice != -1)
			cuenta++;
	}
	pthread_mutex_unlock(&MUTEXMARCOS);
	return cuenta;
}
int numeroMarcoTLB(int numPag, int pid) { // si esta devuelve el numero de marco
	int i;
	pthread_mutex_lock(&MUTEXTLB);
	if (0 < config.entradas_tlb) {
		for (i = 0; i < config.entradas_tlb; i++) {
			if (TLB[i].pagina.numeroPagina == numPag && TLB[i].pagina.pid == pid
					&& TLB[i].pagina.valido == 1) {
				pthread_mutex_unlock(&MUTEXTLB);
				return TLB[i].pagina.nroMarco;

			}
		}
	}
	pthread_mutex_unlock(&MUTEXTLB);
	return -1;
}
int marcoDeTablaPaginas(int numPag, int pid) {
	sleep(config.retardo); //aca penaliza por acceder
	int i;
	pthread_mutex_lock(&MUTEXMARCOS);
	for (i = 0; i <= config.marcos; i++) {
		if (marco[i].nPag == numPag && marco[i].pid == pid)
			pthread_mutex_unlock(&MUTEXMARCOS);
		return i;
	}
	return -1;
} // devuelve la pagina i o -1 si no esta

void menu() {
	char comando[70];
	char instruccion[40];
	char parametro[40];
	while (strcmp("CERRAR", comando) != 0) {
		printf("Consola atenta: /n");
		scanf("%d", &comando);
		separarInstruccionParametro(comando, instruccion, parametro);
		if (strcmp("setRetardo", instruccion) == 0) {
			setRetardo(parametro);
		}
		if (strcmp("dump", instruccion) == 0) {
			dump(parametro);
		}
		if (strcmp("flush", instruccion) == 0
				&& strcmp("tlb", parametro) == 0) {
			flush(1);
		}
		if (strcmp("flush", instruccion) == 0
				&& strcmp("memory", parametro) == 0) {
			flush(2);
		}
	}
}
void separarInstruccionParametro(char*linea, char*instruccion, char parametro[]) {
	int i = 0;
	int x = 0;
	while (linea[i] != ' ' && linea[i] != 0 && linea[i] != '\n') {
		instruccion[i] = linea[i];
		i++;
	}
	instruccion[i] = 0;
	while (linea[i] != 0 && linea[i] != '\n') {
		if (linea[i] != ' ') {
			parametro[x] = linea[i];
			x++;
		}
		i++;
	}
	parametro[x] = 0;
	return;
}

void setRetardo(int num) {
	if (num > 0) {
		pthread_mutex_lock(&MUTEXCONFIG);
		config.retardo = num;
		pthread_mutex_unlock(&MUTEXCONFIG);
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
		pthread_mutex_lock(&MUTEXMARCOS);
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
		pthread_mutex_unlock(&MUTEXMARCOS);
	}
	if (numPid != 0) {
		pthread_mutex_lock(&MUTEXMARCOS);
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
		pthread_mutex_unlock(&MUTEXMARCOS);
	}
}
void mostrarContenidoMemoria(int numPid, t_log* logMemoria) {
	/*pthread_mutex_lock(&MUTEXLP);
	 pthread_mutex_lock(&MUTEXTM);
	 pthread_mutex_lock(&MUTEXTLB);
	 pthread_mutex_lock(&MUTEXLOG);
	 */
	int pHijo = fork();
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
		free(contenido);
		exit(0);
	} else {
		wait(NULL); //ESPERA A LA FINALIZACION DEL HIJO
		/*pthread_mutex_unlock(&MUTEXLOG); //ESPERA A QUE TERMINE EL DUMP PARA SEGUIR
		 pthread_mutex_unlock(&MUTEXLP);
		 pthread_mutex_unlock(&MUTEXTM);
		 pthread_mutex_unlock(&MUTEXTLB);
		 */
		printf("DUMP REALIZADO CON EXITO\n");
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
		pthread_mutex_lock(&MUTEXMARCOS);
		for (i = 0; i < config.marcos; i++) {
			//if (marco[i] != -1) no podes comparar un struct con un int
			marco[i].modif = 1;
		}
		pthread_mutex_unlock(&MUTEXMARCOS);
		break;
	default:
		log_trace(log, "No se econocio la opcion");
	}
}

void pageFault(int numPag, int pid) {
	int socketSwap = crearSocketCliente(config.puerto_swap);
	mensaje_UMC_SWAP* mensaje;
	mensaje->instruccion = LEER;
	mensaje->pid = pid;
	mensaje->parametro = numPag;
	enviarMensajeUMC_SWAP(socketSwap, mensaje, config.marcos_size);
	mensaje_SWAP_UMC *mensajeARecibir;
	char* contenidoPagina = malloc(config.marcos_size);
	contenidoPagina = recibirMensajeSWAP_UMC(socketSwap, mensajeARecibir,
			config.marcos_size);
// actualizar TLB
// escribir en tabla de paginas

}
void agregarATLB(int numPag, int pid) {
	int i, indice, estaEnTabla = 0;
	clock_t tiempoACambiar = TLB[0].tiempo;
	for (i = 0; i < config.entradas_tlb; i++) {
		if (TLB[i].pagina.valido == -1) {
			indice = i;
			TLB[indice].pagina.numeroPagina = numPag;
			TLB[indice].pagina.pid = pid;
			TLB[indice].pagina.valido = 0;
			return;
		}
		if (TLB[i].tiempo >= tiempoACambiar) {
			tiempoACambiar = TLB[i].tiempo;
			indice = i;
		}

	}
	TLB[indice].pagina.numeroPagina = numPag;
	TLB[indice].pagina.pid = pid;
	TLB[indice].pagina.valido = 0;
}
void escribirTablaPAginas(int pid, int pagina) {
	sleep(config.retardo);
	mensaje_ADM_SWAP mensajeParaSWAP;
	mensaje_SWAP_ADM mensajeDeSWAP;
	int aReemplazar;
	int entradaTLBVieja;
}

int reemplazarMarco(int pid, int pagina) //REEMPLAZA EL MARCO QUE HAYA QUE SACAR Y PONE LOS DATOS DEL NUEVO
{
	sleep(config.retardo); //ESPERA PORQUE ENTRO A MEMORIA
	mensaje_ADM_SWAP mensajeParaSWAP;
	mensaje_SWAP_ADM mensajeDeSWAP;
	nodoProcesos* nodoProcesoViejo;
	nodoProcesos* nodoProceso;
	nodoProceso = buscarProceso(pid);
	int aReemplazar;
	int entradaTLBVieja;

	aReemplazar = entradaTMarcoAReemplazar(pid);
//	pthread_mutex_lock(&MUTEXLOG);
	log_info(log, "Se va a reemplazar el marco N: %d", aReemplazar);
//	pthread_mutex_unlock(&MUTEXLOG);
	if (marco[aReemplazar].indice != -1 && marco[aReemplazar].pid != -1
			&& marco[aReemplazar].modif == 1) //LA ENTRADA HAY QUE GUARDARLA EN SWAP PRIMERO
					{
		//printf("VOY A REEMPLAXAR MARCO MODIFICADO\n");
//		pthread_mutex_lock(&MUTEXLOG);
		log_info(log, "El marco se encuentra modificado. Se envia a SWAP");
//		pthread_mutex_unlock(&MUTEXLOG);
		guardarMarcoSwap(marco[aReemplazar], aReemplazar);
		nodoProcesoViejo = buscarProceso(marco[aReemplazar].pid);
		nodoProcesoViejo->marcosAsignados--;
		nodoProcesoViejo->tabla[marco[aReemplazar].nPag].valido = 0; //MARCA COMO INVALIDO SU MARCO
		if ((entradaTLBVieja = numeroMarcoTLB(marco[aReemplazar].pid,
				marco[aReemplazar].nPag)) != -1) {
			TLB[entradaTLBVieja].indice = -1; //BORRA SU ENTRADA EN LA TLB
		}
	}
	if (marco[aReemplazar].indice != -1 && marco[aReemplazar].modif == 0) //SI NO HAY QUE GUARDAR LA PAG PERO SI BORRAR OS DATOS
			{
//		pthread_mutex_lock(&MUTEXLOG);
		log_info(log, "El marco reemplazado no habia sido modificado");
//		pthread_mutex_unlock(&MUTEXLOG);
		nodoProcesoViejo = buscarProceso(marco[aReemplazar].pid);
		nodoProcesoViejo->marcosAsignados--;
		nodoProcesoViejo->tabla[marco[aReemplazar].nPag].valido = 0; //MARCA COMO INVALIDO SU MARCO
		if ((entradaTLBVieja = numeroMarcoTLB(marco[aReemplazar].pid,
				marco[aReemplazar].nPag)) != -1) {
			TLB[entradaTLBVieja].indice = -1; //BORRA SU ENTRADA EN LA TLB
		}
	}
	//PEDIMOS PAGINA BUSCADA AL SWAP
//	pthread_mutex_lock(&MUTEXLOG);
	log_info(log, "Se solicita al SWAP la pagina %d del proceso de PID: %d",
			pagina, pid);
//	pthread_mutex_unlock(&MUTEXLOG);
	mensajeParaSWAP.pid = pid;
	mensajeParaSWAP.instruccion = LEER;
	mensajeParaSWAP.parametro = pagina;
	mensajeParaSWAP.contenidoPagina = NULL;
	enviarDeADMParaSwap(socketSWAP, &mensajeParaSWAP, config.marcos_size);
	if (mensajeParaSWAP.contenidoPagina != NULL) {
		free(mensajeParaSWAP.contenidoPagina);
		mensajeParaSWAP.contenidoPagina = NULL;
	}
	recibirMensajeDeSwap(socketSWAP, &mensajeDeSWAP, config.marcos_size);

	//printf("ASGINADOS: %d MARCO %d BYTE: %d\n",nodoProceso->marcosAsignados,aReemplazar,(aReemplazar*configuracion.TAMANIO_MARCO));
	memcpy(&memoria[aReemplazar * config.marcos_size],
			mensajeDeSWAP.contenidoPagina, config.marcos_size);
	if (mensajeDeSWAP.contenidoPagina != NULL) {
		free(mensajeDeSWAP.contenidoPagina);
	}
	marco[aReemplazar].modif = 0;
	marco[aReemplazar].indice = 1; //USO INDICE COMO BIT U
	marco[aReemplazar].pid = pid;
	marco[aReemplazar].nPag = pagina;
	nodoProceso->marcosAsignados++;
	(nodoProceso->tabla)[pagina].valido = 1; //CARGAMOS LA PAGINA COMO VALIDA  ////***
	(nodoProceso->tabla)[pagina].numMarco = aReemplazar;
//	pthread_mutex_lock(&MUTEXLOG);
	log_info(log, "El estado de los marcos luego del reemplazdo es:");
//	pthread_mutex_unlock(&MUTEXLOG);
	logearTMarcos();
	return aReemplazar;
}

int entradaTMarcoAReemplazar(int pid) {
	nodoProcesos* nodo;
	nodo = buscarProceso(pid);
	int i = 0;
	int j = 0; //Solo lo uso para correr 2 veces el for, si no encuentra hay un error.

	if (nodo->marcosAsignados < config.marco_x_proc) { //BUSCA QUE PUEDAN DARLE UN MARCO LIBRE
		for (i = 0; i < config.marcos; i++) //PRIMERO CHEQUEO LAS LIBRES
				{
			if (marco[i].indice == -1)
				return i;
		}
	}
	if (config.algoritmo == 2) {
		//NO HAY LIBRES, CORRO EL PRIMER RECORRIDO, SIN CAMBIAR U BUSCO U=0(INDICE) y M=0 (MODIF)
		for (j = 0; j < 2; j++) { //SI NO ENCUENTRA EL ULTIMO LOOP ENTRA DENUEVO PARA HACER EL 1 y 2, si con 2 repeticiones no encuentra, hay un erro de impelemtnacion
			for (i = 0; i < config.marcos; i++) {
				if (nodo->indiceClockM >= config.marcos)
					nodo->indiceClockM = 0; //Devuelve el indice a 0
				if (marco[nodo->indiceClockM].pid == pid
						&& marco[nodo->indiceClockM].indice == 0
						&& marco[nodo->indiceClockM].modif == 0) {
					nodo->indiceClockM++; //INCREMENTA PARA EL PROXIMO USO
					return (nodo->indiceClockM - 1);
				}
				nodo->indiceClockM++;
			}
			for (i = 0; i < config.marcos; i++) //NO HAY LIBRES BUSCO U=0 M=1 si no vale eso pongo u en 0
					{
				if (nodo->indiceClockM >= config.marcos)
					nodo->indiceClockM = 0; //Devuelve el indice a 0
				if (marco[nodo->indiceClockM].pid == pid
						&& marco[nodo->indiceClockM].indice == 0
						&& marco[nodo->indiceClockM].modif == 1) {
					nodo->indiceClockM++; //INCREMENTA PARA EL PROXIMO USO
					return (nodo->indiceClockM - 1);
				}
				marco[nodo->indiceClockM].indice = 0;
				nodo->indiceClockM++;
			}
		}

	}

	if (config.algoritmo == 1) {
		for (i = 0; i < config.marcos; i++) //NO HAY LIBRES BUSCO U=0 M=1 si no vale eso pongo u en 0
				{
			if (clock == config.marcos)
				clock = 0;
			if (nodo->indiceClockM == 1) {
				nodo->indiceClockM = 0; //Devuelve el indice a 0
				continue;
			}
			if (nodo->indiceClockM == 0) {
				nodo->indiceClockM = 1;
				return clock;
			}

		}
	}
}

void guardarMarcoSwap(tMarco *marco, int numMarco) {
	mensaje_UMC_SWAP mensajeParaSWAP;
	mensaje_SWAP_UMC mensajeDeSWAP;
	int socketsSwap = crearSocketCliente(config.puerto_swap);

	log_info(log,
			"Se envia el contenido del marco %d al SWAP. (Pagina: %d || PID: %d)",
			numMarco, marco->nPag, marco->pid);
	mensajeParaSWAP.pid = marco->pid;
	mensajeParaSWAP.instruccion = ESCRIBIR;
	mensajeParaSWAP.parametro = marco->nPag;
	mensajeParaSWAP.contenidoPagina = malloc(config.marcos_size);
//strcpy(mensajeParaSWAP.contenidoPagina,tMarcos[i].contenido);
	memcpy(mensajeParaSWAP.contenidoPagina,
			&memoria[numMarco * config.marcos_size], config.marcos_size);
//	enviarDeUMCParaSwap(socketsSwap, &mensajeParaSWAP, config.marcos_size); //MANDA LAPAGINA A ESCRIBIRSE
	if (mensajeParaSWAP.contenidoPagina != NULL) {
		free(mensajeParaSWAP.contenidoPagina);
		mensajeParaSWAP.contenidoPagina = NULL;
	}
	free(socketsSwap);
}

nodoProcesos* buscarProceso(int pid) //Retorna NULL si no lo encuentra
{
	nodoProcesos* aux;
	aux = listaProcesos;
	while (aux != NULL) {
		if (aux->pid == pid) {
			return aux;
		}
		aux = aux->sgte;
	}

	return NULL;
}
