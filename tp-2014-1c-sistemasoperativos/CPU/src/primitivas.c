#include "kernel.h"
#include "config.h"

#include "primitivas.h"
#include "umv.h"

#include "commons/log.h"
#include "commons/parser/parser.h"

extern pcb_t PCB_enEjecucion;
extern t_log * logger;
extern uint32_t quantumRestante;


AnSISOP_funciones * crearAnSISOP_funciones()
{
	log_debug(logger, "Setando primitivas");

	AnSISOP_funciones * funciones = malloc(sizeof(AnSISOP_funciones));

	funciones->AnSISOP_definirVariable			= &definirVariable;
	funciones->AnSISOP_obtenerPosicionVariable	= &obtenerPosicionVariable;
	funciones->AnSISOP_dereferenciar			= &dereferenciar;
	funciones->AnSISOP_asignar					= &asignar;
	funciones->AnSISOP_obtenerValorCompartida	= &obtenerValorCompartida;
	funciones->AnSISOP_asignarValorCompartida	= &asignarValorCompartida;
	funciones->AnSISOP_irAlLabel				= &irAlLabel;
	funciones->AnSISOP_llamarSinRetorno 		= &llamarSinRetorno;
	funciones->AnSISOP_llamarConRetorno 		= &llamarConRetorno;
	funciones->AnSISOP_finalizar				= &finalizar;
	funciones->AnSISOP_retornar					= &retornar;
	funciones->AnSISOP_imprimir					= &imprimir;
	funciones->AnSISOP_imprimirTexto			= &imprimirTexto;
	funciones->AnSISOP_entradaSalida			= &entradaSalida;

	return funciones;

}

AnSISOP_kernel * crearAnSISOP_kernel()
{
	AnSISOP_kernel *kernel = malloc(sizeof(AnSISOP_kernel));

	kernel->AnSISOP_wait =   &scWait;
	kernel->AnSISOP_signal = &scSignal;

	return kernel;
}

/************************************************************************************/


t_puntero definirVariable(t_nombre_variable identificador_variable)
{
	uint32_t puntero = apilarVariable( identificador_variable );
	log_trace( logger, "Llamada a definirVariable, el identificador es: %c y esta en la posicion %d", identificador_variable, puntero );
	return puntero;
}

t_puntero obtenerPosicionVariable(t_nombre_variable identificador_variable)
{
	uint32_t puntero = obtenerOffsetVarible( identificador_variable );
	if(puntero == 0){
		PCB_enEjecucion.lastErrorCode = 3;
		quantumRestante = 0;
		log_error(logger, "Se solicito una posicion de una variable inexistente %c", identificador_variable);
	}else{
		log_trace( logger, "Llamada a obtenerPosicionVariable, el identificador es: %c, y esta en %d", identificador_variable, puntero );
	}
	return (t_puntero) puntero;
}

t_valor_variable dereferenciar(t_puntero direccion_variable)
{
	log_trace( logger, "Llamada a dereferenciar, direccion: %d", direccion_variable );
	return obtenerValor( (uint32_t) direccion_variable ) ;
}

void asignar(t_puntero direccion_variable, t_valor_variable valor)
{
	log_trace( logger, "Llamada a asignar [ %d ] = %d ", direccion_variable, valor );
	modificarVariable( (uint32_t) direccion_variable, (uint32_t) valor );
}


/************************************************************************************/


void irAlLabel(t_nombre_etiqueta t_nombre_etiqueta)
{
	PCB_enEjecucion.programCounter = obtenerLineaDeLabel( t_nombre_etiqueta );
	if( PCB_enEjecucion.programCounter == -1 ){
		PCB_enEjecucion.lastErrorCode = 5;
		log_error( logger, "Se quiso saltar a un label inexistente: %s", t_nombre_etiqueta);
		quantumRestante = 0;
		return;
	}
	log_trace( logger, "Saltando al label %s con programCounter = %d", t_nombre_etiqueta, PCB_enEjecucion.programCounter );
}


void llamarSinRetorno(t_nombre_etiqueta etiqueta)
{
	log_trace( logger, "Llamada a llamarSinRetorno" );
	apilarFuncionSinRetorno();
	irAlLabel( etiqueta );
}

void llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar)
{
	log_trace( logger, "Llamada a llamarConRetorno" );
	apilarFuncionConRetorno( donde_retornar );
	irAlLabel( etiqueta );
}

void finalizar(void)
{
	if(PCB_enEjecucion.stackCursor - (PCB_enEjecucion.contextSize*5) == 0){
		log_trace(logger, "Finalizando el programa");
		PCB_enEjecucion.lastErrorCode = 1;
		quantumRestante = 0;
	}else{
		log_trace(logger, "Retornando VOID");
		retornarVoid();
	}
}

void retornar(t_valor_variable retorno)
{
	log_trace( logger, "Llamada a retornar" );
	if (!retornarValor(retorno)){
		PCB_enEjecucion.lastErrorCode = 6;
	}
}



/****************************
*	FUNCIONES DEL KERNEL
****************************/


t_valor_variable obtenerValorCompartida(t_nombre_compartida variable)
{
	log_trace( logger, "Llamada a obtenerValorCompartida" );
	return solcitarVariableCompartidaAKernel(variable);
}

t_valor_variable asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor)
{
	log_trace( logger, "Llamada a asignarValorCompartida" );
	return  enviarAKernelNuevoValorVariableCompartida(variable, valor);
}

void imprimir(t_valor_variable valor_mostrar)
{
	log_trace( logger, "Llamada a imprimir variable: %d", valor_mostrar);
	enviarAKernelImprimir( valor_mostrar );
}

void imprimirTexto(char* texto)
{
	log_debug( logger, "Llamada a imprimirTexto" );
	enviarAKernelImprimirTexto( texto );
}

void entradaSalida(t_nombre_dispositivo dispositivo, int tiempo)
{
	log_trace( logger, "Llamada a entradaSalida" );
	enviarAKernelEntradaSalida(dispositivo, tiempo);
}




/**************************************************************************************
*
*			SYS CALLS
*
****************************************************************************************/



void scWait(t_nombre_semaforo identificador_semaforo)
{
	log_trace( logger, "Llamada a Wait, semaforo: %s", identificador_semaforo );
	if( !enviarAKernelWait(identificador_semaforo)){
		PCB_enEjecucion.lastErrorCode = 6;
	}
}

void scSignal(t_nombre_semaforo identificador_semaforo)
{
	log_trace( logger, "Llamada a Signal" );
	enviarAKernelSignal(identificador_semaforo);
}















