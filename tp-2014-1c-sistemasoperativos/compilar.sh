#!/bin/bash

ubicacion=$(pwd)

if [ $1 = clean ]; then

	cd commons
	rm -r Debug
	cd ../UMV
	rm -r Debug
	cd ../Kernel
	rm -r Debug
	cd ../CPU
	rm -r Debug
	cd ../Programa
	rm -r Debug
	
else

	cd commons
	make all

	cd $ubicacion
	cd UMV
	make all

	cd $ubicacion
	cd Kernel
	make all

	cd $ubicacion
	cd CPU
	make all

	cd $ubicacion
	cd Programa
	make all
fi
	
clear
exit 

