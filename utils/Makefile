all:
	gcc -I./ev3dev-c/source/ev3 -O2 -std=gnu99 -W -Wall -Wno-comment -c avancer.c -o avancer.o
	gcc avancer.o -Wall -lm -lev3dev-c -o avancer
