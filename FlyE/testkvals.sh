#!/bin/bash

for k in {1..24}
do
	echo "k = $k"
	sed -i "s/k = [0-9]*/k = $k/" ./flyE.conf
	./Release/FlyE ./flyE.conf

	mv ~/FlyEfiles/outFile.h5 ~/FlyEfiles/${k}k.h5
done
