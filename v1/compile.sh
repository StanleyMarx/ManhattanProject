#./compile.sh <progname>
gcc -std=gnu99 -c $1.c -o $1.o
gcc $1.o -Wall -lm -lev3dev-c -lbluetooth -o $1
rm $1.o
