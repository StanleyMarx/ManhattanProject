all : openFile

openFile : open-file.o
	gcc open-file.o -Wall -o openFile
	
open-file.o : open-file.c
	gcc -W -Wall -c open-file.c -o open-file.o

run : openFile
	rm open-file.o
	./openFile

