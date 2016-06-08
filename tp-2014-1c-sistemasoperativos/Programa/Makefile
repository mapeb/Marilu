OBJS += \
./Debug/Programa.o  \
../commons/Debug/bitarray.o  \
../commons/Debug/config.o  \
../commons/Debug/error.o  \
../commons/Debug/log.o  \
../commons/Debug/pcb.o  \
../commons/Debug/process.o  \
../commons/Debug/sockets.o  \
../commons/Debug/string.o  \
../commons/Debug/temporal.o  \
../commons/Debug/txt.o  \
../commons/Debug/collections/dictionary.o  \
../commons/Debug/collections/list.o  \
../commons/Debug/collections/queue.o  \
../commons/Debug/parser/metadata_program.o  \
../commons/Debug/parser/parser.o

all: crearDir Programa.o Programa

Programa: $(OBJS)
	gcc -lpthread -pthread -o ./Debug/Programa $(OBJS)

Programa.o: ./src/Programa.c
	gcc -I ../commons -c ./src/Programa.c -o ./Debug/Programa.o

clear:
	rm ./Debug/*.o ./Debug/Programa

crearDir:
	mkdir -p Debug
