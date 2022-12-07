CC = gcc
CFLAGS = -Wall
INCLUDE = -Iheaders

othello: main.o funciones.o
	$(CC) $(CFLAGS) main.o funciones.o -o othello

tests: tests.o funciones.o
	$(CC) $(CFLAGS) tests.o funciones.o -o tests

main.o: main.c headers/funciones.h
	$(CC) $(CFLAGS) $(INCLUDE) -c main.c

tests.o: tests.c headers/tests.h headers/funciones.h
	$(CC) $(CFLAGS) $(INCLUDE) -c tests.c

funciones.o: funciones.c headers/funciones.h
	$(CC) $(CFLAGS) $(INCLUDE) -c funciones.c

clean:
	rm *.o