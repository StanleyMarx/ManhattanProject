#!/bin/sh
NAME=$(echo $1 | egrep -o '[A-Za-z0-9\ ]+')
gcc -I./ev3dev-c/source/ev3 -O2 -std=gnu99 -W -Wall -Wno-comment -c $1 -o $NAME.o
gcc $NAME.o -Wall -lm -lev3dev-c -o $NAME
