#!/bin/bash

for voltage in {10..45..5}
do
	echo "Doing $voltage volt run"	
	sed -i "s/MAX_VOLTAGE=\t[0-9]*/MAX_VOLTAGE=\t$voltage/" ./flyE.conf
	./Release/FlyE ./flyE.conf
	mv  ~/FlyEfiles/outFile.h5 ~/FlyEfiles/outFile$voltage.h5
done
