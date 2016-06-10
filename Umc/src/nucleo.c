#include "nucleo.h"
#include "umc.h"
#include "swap.h"
/*
void AtiendeClienteNucleo(int socket, struct sockaddr_in addr) {
	int fin = 0;
	mensaje_NUCLEO_UMC* mensajeArecibir;
	mensaje_UMC_NUCLEO* mensajeAenviar;
	while (!fin) {
	recibirMensajeDeNucleo(socket, mensajeArecibir);
	switch (mensajeArecibir->instruccion) {
	case INICIAR:
		mensajeAenviar->instruccion = TAMPAG;
		mensajeAenviar->tamanio = config.marcos_size;
		enviarMensajeAlNucleo(socket, mensajeAenviar);
		break;
	case LEER:
		solicitarPaginas(mensajeArecibir->script,mensajeArecibir->tamanioPaginas);
		///mensajeArecibir->script; este seria el script que te envia el nucleo

		break;
	case FINALIZAR:
		log_trace(log, "finalizo la escucha de nucleo");
		fin = 1;
		break;
	}
	}
		close(socket);
	}
*/
