#!/bin/bash

clear
echo "¿Que quiere iniciar?"
echo "1- UMV"
echo "2- Kernel"
echo "3- CPU"
echo "4- Programa"

read n
clear

case $n in
 1 )
  	cd UMV/Debug
	clear
	./UMV ../resources/config.cfg
	;;
 2 )
  	cd Kernel/Debug
	clear
	./Kernel ../resources/config.cfg
	;;
 3 )
  	cd CPU/Debug
	clear
	./CPU ../resources/config.cfg
	;;
 4)
	cd Programa/resources
	echo "Ingrese script a ejecutar (no hace falta .ansisop)"
	echo " "
	ls
	read s
	cd ../Debug
	export ANSISOP_CONFIG=../resources/config.cfg
	clear
	./Programa ../resources/$s.ansisop
	;;
 esac

exit