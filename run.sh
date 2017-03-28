gcc -o functions.o functions.c -c -g
ar rcs lib0936196.a functions.o
ar t lib0936196.a
gcc -static -g 0936196.c -L. -I. -l0936196 -lm -o exec

