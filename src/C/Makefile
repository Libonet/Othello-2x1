CC = gcc
CFLAGS = -Wall

default: othello

othello: othello.c funciones.o
	$(CC) $(CFLAGS) othello.c funciones.o -o othello

funciones.o: funciones.c funciones.h
	$(CC) $(CFLAGS) -c funciones.c

clean:
	rm *.o