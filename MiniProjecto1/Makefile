CC = gcc
WALL = -Wall

all: ADRCMP1

ADRCMP1: ADRCMP1.o Utilities.o
	$(CC) $(WALL) -o ADRCMP1 ADRCMP1.o Utilities.o

ADRCMP1.o: ADRCMP1.c Utilities.o
	$(CC) -c -o ADRCMP1.o ADRCMP1.c

Utilities.o: Utilities.c
	$(CC) -c -o Utilities.o Utilities.c

clean:
	rm *.o


