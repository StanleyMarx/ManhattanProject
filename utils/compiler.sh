#!/bin/sh

gcc -I./ev3dev-c/source/ev3 -O2 -std=gnu99 -W -Wall -Wno-comment -c $1.c -o $1.o
gcc $1.o -Wall -lm -lev3dev-c -o $1
