OBJS += \
./Debug/CPU.o \
 ./Debug/config.o \
./Debug/ejecucion.o \
./Debug/kernel.o \
./Debug/primitivas.o \
./Debug/stack.o \
./Debug/umv.o \
../commons/Debug/bitarray.o \
../commons/Debug/config.o \
../commons/Debug/error.o \
../commons/Debug/log.o \
../commons/Debug/pcb.o \
../commons/Debug/process.o \
../commons/Debug/sockets.o \
../commons/Debug/string.o \
../commons/Debug/temporal.o \
../commons/Debug/txt.o \
../commons/Debug/collections/dictionary.o \
../commons/Debug/collections/list.o \
../commons/Debug/collections/queue.o \
../commons/Debug/parser/metadata_program.o \
../commons/Debug/parser/parser.o

all: crearDir config.o ejecucion.o kernel.o primitivas.o stack.o umv.o CPU.o CPU
CPU: $(OBJS)
	gcc -lpthread -pthread -o ./Debug/CPU $(OBJS)

config.o: ./src/config.c ./src/config.h 
	gcc -I ../commons -c ./src/config.c -o ./Debug/config.o

ejecucion.o: ./src/ejecucion.c ./src/ejecucion.h 
	gcc -I ../commons -c ./src/ejecucion.c -o ./Debug/ejecucion.o

kernel.o: ./src/kernel.c ./src/kernel.h 
	gcc -I ../commons -c ./src/kernel.c -o ./Debug/kernel.o

primitivas.o: ./src/primitivas.c ./src/primitivas.h 
	gcc -I ../commons -c ./src/primitivas.c -o ./Debug/primitivas.o

stack.o: ./src/stack.c ./src/stack.h 
	gcc -I ../commons -c ./src/stack.c -o ./Debug/stack.o

umv.o: ./src/umv.c ./src/umv.h 
	gcc -I ../commons -c ./src/umv.c -o ./Debug/umv.o

CPU.o: ./src/CPU.c 
	gcc -I ../commons -c ./src/CPU.c -o ./Debug/CPU.o

clear:
	rm ./Debug/*.o ./Debug/CPU

crearDir:
	mkdir -p Debug
