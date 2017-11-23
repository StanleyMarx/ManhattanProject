all:
	gcc -I./ev3dev-c/source/ev3 -O2 -std=gnu99 -W -Wall -Wno-comment -c jb-test-10cm.c -o jb-test-10cm.o
	gcc jb-test-10cm.o -Wall -lm -lev3dev-c -o jb-test-10cm
