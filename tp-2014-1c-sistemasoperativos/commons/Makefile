all: crearDir bitarray.o config.o error.o log.o pcb.o process.o sockets.o string.o temporal.o txt.o dictionary.o list.o queue.o node.o metadata_program.o parser.o sintax.o

bitarray.o: ./commons/bitarray.c ./commons/bitarray.h 
	gcc -c ./commons/bitarray.c -o ./Debug/bitarray.o

config.o: ./commons/config.c ./commons/config.h 
	gcc -c ./commons/config.c -o ./Debug/config.o

error.o: ./commons/error.c ./commons/error.h 
	gcc -c ./commons/error.c -o ./Debug/error.o

log.o: ./commons/log.c ./commons/log.h 
	gcc -c ./commons/log.c -o ./Debug/log.o

pcb.o: ./commons/pcb.c ./commons/pcb.h 
	gcc -c ./commons/pcb.c -o ./Debug/pcb.o

process.o: ./commons/process.c ./commons/process.h 
	gcc -c ./commons/process.c -o ./Debug/process.o

sockets.o: ./commons/sockets.c ./commons/sockets.h 
	gcc -c ./commons/sockets.c -o ./Debug/sockets.o

string.o: ./commons/string.c ./commons/string.h 
	gcc -c ./commons/string.c -o ./Debug/string.o

temporal.o: ./commons/temporal.c ./commons/temporal.h 
	gcc -c ./commons/temporal.c -o ./Debug/temporal.o

txt.o: ./commons/txt.c ./commons/txt.h 
	gcc -c ./commons/txt.c -o ./Debug/txt.o

dictionary.o: ./commons/collections/dictionary.c ./commons/collections/dictionary.h 
	gcc -c ./commons/collections/dictionary.c -o ./Debug/collections/dictionary.o

list.o: ./commons/collections/list.c ./commons/collections/list.h 
	gcc -c ./commons/collections/list.c -o ./Debug/collections/list.o

queue.o: ./commons/collections/queue.c ./commons/collections/queue.h 
	gcc -c ./commons/collections/queue.c -o ./Debug/collections/queue.o

node.o: ./commons/collections/node.h 
	gcc -c ./commons/collections/node.h -o ./Debug/collections/node.o

metadata_program.o: ./commons/parser/metadata_program.c ./commons/parser/metadata_program.h 
	gcc -c ./commons/parser/metadata_program.c -o ./Debug/parser/metadata_program.o

parser.o: ./commons/parser/parser.c ./commons/parser/parser.h 
	gcc -c ./commons/parser/parser.c -o ./Debug/parser/parser.o

sintax.o: ./commons/parser/sintax.h 
	gcc -c ./commons/parser/sintax.h -o ./Debug/parser/sintax.o

clear:
	rm ./Debug/*.o ./Debug/collections/*.o ./Debug/parser/*.o

crearDir:
	mkdir -p Debug
	mkdir -p Debug/collections
	mkdir -p Debug/parser