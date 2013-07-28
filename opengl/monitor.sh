#!/bin/bash
pt=$(ls -l --time-style=+%s circle.cpp|cut -d' ' -f6);
while(/bin/true); do
	ct=$(ls -l --time-style=+%s circle.cpp|cut -d' ' -f6);
	if [ "$ct" -ne "$pt" ];then
		echo "************************************************";
		killall triangle 2>&-;
		make clean;
		make;
		if [ "0" -eq "$?" ];then
			times ./triangle
			./triangle
		fi;
	fi
	pt=$ct;
	sleep 1;
done
