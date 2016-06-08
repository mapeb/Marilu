#ifndef KERNEL_H_
#define KERNEL_H_

#include <sys/socket.h>

#include "commons/sockets.h"

bool crearConexionKernel();
bool enviarHandshake();

void finalizarCpu();

int32_t	solcitarVariableCompartidaAKernel(t_nombre_compartida variable);
bool		enviarAKernelNuevoValorVariableCompartida(t_nombre_compartida variable, t_valor_variable valor);
bool		enviarAKernelImprimir		( t_valor_variable valor );
bool		enviarAKernelImprimirTexto	( char * texto );
bool		enviarAKernelEntradaSalida	(t_nombre_dispositivo dispositivo, int tiempo);
bool 		enviarAKernelSignal(t_nombre_semaforo identificador_semaforo);
bool 		enviarAKernelWait(t_nombre_semaforo identificador_semaforo);


bool recibirYProcesarMensajesKernel();
int escucharYEjecutarInstruccionesKernel();

bool enviarPCB();

void scWait(t_nombre_semaforo identificador_semaforo);
void scSignal(t_nombre_semaforo identificador_semaforo);

extern int socketKernel;

#endif /* KERNEL_H_ */



