all : MAIN

MAIN : MAIN.o
	gcc MAIN.o -Wall -lm -lev3dev-c -o MAIN

MAIN.o : MAIN.c
	gcc -I./ev3dev-c/source/ev3 -O2 -std=gnu99 -W -Wall -Wno-comment -c MAIN.c -o MAIN.o

clean :
	rm MAIN avancer *.o

run : MAIN
	./MAIN
