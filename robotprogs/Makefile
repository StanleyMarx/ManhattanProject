all : MAIN avancer

MAIN : MAIN.o avancer
	gcc -o MAIN MAIN.o

avancer : avancer.o
	gcc -o avancer avancer.c

MAIN.o : MAIN.c
	gcc -c MAIN.c

avancer.o : avancer.c
	gcc -c avancer.c	

clean :
	rm MAIN avancer *.o
