CC = gcc
WALL = -Wall
G = -g

all: ADRCMP1

ADRCMP1: ADRCMP1.o Utilities.o
	$(CC) $(WALL) -o ADRCMP1 ADRCMP1.o Utilities.o $(G)

ADRCMP1.o: ADRCMP1.c Utilities.o
	$(CC) -c -o ADRCMP1.o ADRCMP1.c $(G)

Utilities.o: Utilities.c
	$(CC) -c -o Utilities.o Utilities.c $(G)

clean:
	rm *.o


