CC = gcc
INC = -I../

tests: tests.o funciones.o
	$(CC) tests.o funciones.o -o tests

tests.o: tests.c tests.h ../funciones.h
	$(CC) $(INC) -c tests.c

funciones.o: ../funciones.c ../funciones.h
	$(CC) $(INC) -c ../funciones.c

clean:
	rm *.o

run:
	./tests