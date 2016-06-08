#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/sendfile.h>

#include "commons/config.h"
#include "commons/sockets.h"

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Modo de empleo: ./Programa ScriptAnsiSOp\n");
		return EXIT_SUCCESS;
	}

	char *config_path = getenv("ANSISOP_CONFIG");

	if(config_path == NULL) {
		printf("Falta definir la variable de entorno ANSISOP_CONFIG\n");
		return EXIT_SUCCESS;
	}

	char *logName = string_from_format("log_%d.txt", process_getpid());
	t_log *log = log_create(logName, "Programa", 1, LOG_LEVEL_TRACE);

	t_config *config = config_create(config_path);
	if( !config_has_property(config, "IP") || !config_has_property(config, "Puerto") ) {
		log_error(log, "Configuracion invalida");
		return EXIT_SUCCESS;
	}

	FILE *script = fopen(argv[1], "r"); //argv[1] = nombre del script a correr
	off_t scriptBase = 0;
	size_t scriptSize;

	if (script == NULL ) {
		log_error(log, "Error al leer el script: %s", argv[1]);
		return EXIT_SUCCESS;
	}

	//Obteniendo tamaño del archivo
	fseek(script, 0, SEEK_END);
	scriptSize = ftell(script);
	fseek(script, 0, SEEK_SET);

	log_info(log, "Iniciando Programa (script: %s log: %s)", argv[1], logName);

	int sock = conectar(config_get_string_value(config, "IP"), config_get_int_value(config, "Puerto"), log);

	if (sock == -1) {
		log_error(log, "No se pudo conectar al kernel");
		return EXIT_SUCCESS;
	}

	log_info(log, "Conectado al kernel");

	socket_header header;
	header.size = sizeof(header)+scriptSize;

	if( send(sock, &header, sizeof(socket_header), 0) != sizeof(socket_header) || sendfile(sock, script->_fileno, &scriptBase, scriptSize) != scriptSize ) {
		log_error(log, "No se pudo enviar el archivo");
		return EXIT_SUCCESS;
	}

	socket_msg msg;

	while (recv(sock, &msg, sizeof(socket_msg), MSG_WAITALL) == sizeof(socket_msg) )
	{
		if(msg.type == 0)
			log_info(log, "%s", msg.msg);
		else
			log_error(log, "%s", msg.msg);
	}


	log_info(log, "Desconectado del servidor");

	close(sock);
	fclose(script);
	config_destroy(config);
	log_destroy(log);

	return EXIT_SUCCESS;
}


























