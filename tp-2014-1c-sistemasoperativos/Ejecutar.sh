#!/bin/bash

ubicacion=$(pwd)
clear
read -p "Ingrese cantidad de CPU: " n

if [ "$n"  \< 0 ];then
	n=`expr $n \* -1`
fi

clear

cd $ubicacion
cd UMV/Debug
xterm -T UMV -sb -hold -geometry "120x25" -e "./UMV ../resources/config.cfg" &
sleep 0.1

cd $ubicacion
cd Kernel/Debug
xterm -T Kernel -sb -hold -geometry "120x25" -e "./Kernel ../resources/config.cfg" &
sleep 0.2

cant=1

cd $ubicacion
cd CPU/Debug

while [ $cant -le $n ]; do
		xterm -T CPU$cant -sb -hold -geometry "120x30" -e "./CPU ../resources/config.cfg" &
    		let cant=$cant+1
done


cd $ubicacion
cd Programa/resources
echo "Ingrese script a ejecutar (no hace falta .ansisop)"
echo "Ingrese '1' para agregar una CPU o '0' para salir"
echo " "
ls
read s

while [ $s != 0 ]; do
		
		if [ $s = 1 ]; then
			cd $ubicacion
			cd CPU/Debug
			xterm -T CPU$cant -sb -hold -geometry "120x30" -e "./CPU ../resources/config.cfg" &
    			let cant=$cant+1
		else
			clear
			read -p "¿Cuantas instancias de $s quiere? " i
			if [ "$i"  \< 0 ];then
				i=`expr $i \* -1`
			fi

			while [ $i != 0 ]; do

			cd $ubicacion
			cd Programa/Debug
			export ANSISOP_CONFIG=../resources/config.cfg
			xterm -T $s -sb -hold -geometry "120x25" -e "./Programa ../resources/$s.ansisop" &
		
			 i=`expr $i - 1`
			done
		fi

		clear
		cd $ubicacion
		cd Programa/resources
		echo "Ingrese script a ejecutar (no hace falta .ansisop)"
		echo "Ingrese '1' para agregar una CPU o '0' para salir"
		echo " "
		ls
		read s

done

ps ax | grep xterm | grep resources/config.cfg | awk '{print $1}' | xargs kill
ps ax | grep xterm | grep ansisop | awk '{print $1}' | xargs kill
clear

exit 0