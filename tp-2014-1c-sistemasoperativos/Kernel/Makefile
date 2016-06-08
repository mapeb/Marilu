OBJS += \
./Debug/Kernel.o \
./Debug/colas.o \
./Debug/config.o \
./Debug/io.o \
./Debug/pcp.o \
./Debug/plp.o \
./Debug/umv.o \
./Debug/systemcalls.o \
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

all: crearDir colas.o config.o io.o pcp.o plp.o systemcalls.o umv.o Kernel.o Kernel

Kernel: $(OBJS)
	gcc -lpthread -pthread -o ./Debug/Kernel $(OBJS)

colas.o: ./src/colas.c ./src/colas.h
	gcc -I ../commons -c ./src/colas.c -o ./Debug/colas.o

config.o: ./src/config.c ./src/config.h 
	gcc -I ../commons -c ./src/config.c -o ./Debug/config.o

io.o: ./src/io.c ./src/io.h 
	gcc -I ../commons -c ./src/io.c -o ./Debug/io.o

pcp.o: ./src/pcp.c ./src/pcp.h 
	gcc -c -I ../commons ./src/pcp.c -o ./Debug/pcp.o

plp.o: ./src/plp.c ./src/plp.h 
	gcc -c -I ../commons ./src/plp.c -o ./Debug/plp.o

systemcalls.o: ./src/systemcalls.c ./src/systemcalls.h 
	gcc -c -I ../commons ./src/systemcalls.c -o ./Debug/systemcalls.o

umv.o: ./src/umv.c ./src/umv.h 
	gcc -c -I ../commons ./src/umv.c -o ./Debug/umv.o

Kernel.o: ./src/Kernel.c 
	gcc -I ../commons -c ./src/Kernel.c -o ./Debug/Kernel.o

clear: 
	rm ./Debug/Kernel ./Debug/*.o

crearDir:
	mkdir -p Debug
