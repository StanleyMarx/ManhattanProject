all:
	gcc -I./ev3dev-c/source/ev3 -O2 -std=gnu99 -W -Wall -Wno-comment -c avancer.c -o avancer.o
	gcc -I./ev3dev-c/source/ev3 -O2 -std=gnu99 -W -Wall -Wno-comment -c deux-moteurs.c -o deux-moteurs.o
	gcc avancer.o -Wall -lm -lev3dev-c -o avancer
	gcc deux-moteurs.o -Wall -lm -lev3dev-c -o deux-moteurs
