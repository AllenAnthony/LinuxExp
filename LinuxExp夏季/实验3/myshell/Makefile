OPTIONS	= -g -fno-builtin
myshell: myshell.o struct.o
	gcc $(OPTIONS) -o myshell myshell.o struct.o   
myshell.o: myshell.c myshell.h Macros.h struct.c
	gcc $(OPTIONS) -c myshell.c
struct.o: myshell.h Macros.h struct.c
	gcc $(OPTIONS) -c struct.c


