#./compile.sh <progname>
gcc -std=gnu99 -c $1.c -o $1.o -lpthread
gcc $1.o -Wall -lm -lev3dev-c -lbluetooth -o $1 -lpthread
rm $1.o
