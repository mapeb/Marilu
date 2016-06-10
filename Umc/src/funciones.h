/*
 * funciones.h
 *
 *  Created on: 27/4/2016
 *      Author: utnso
 */

#ifndef SRC_FUNCIONES_H_
#define SRC_FUNCIONES_H_

void inicializarConfiguracion();
int verificarConfiguracion(t_config *configuracion);
void conectarSwap();
/*int enviarDeSwapAlADM(int socket, mensaje_SWAP_ADM* mensajeAEnviar,
		int tamPagina);
int recibirMensajeDeSwap(int socket, mensaje_SWAP_ADM* mensajeRecibido,
		int tamPagina);
*/

#endif  /*SRC_FUNCIONES_H_*/


