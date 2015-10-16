#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define PREFIX_SIZE 8
#define MAX_HOP_NUM_DIGITS 2
#define BUFFER_SIZE 128

typedef struct n{
	short nextHop;
	char prefix[PREFIX_SIZE+1];
	struct n * leftChild;
	struct n * rightChild;
} node;

typedef struct f{
	node * value;
	struct f * next;
} fifo;

typedef enum{
  FALSE,
  TRUE
} boolean;

extern short lastHopFound;
extern boolean readAtStart;
extern char path[BUFFER_SIZE];

node * NewNode();
fifo * NewFifo();
boolean hasLeftChild(node * aux);
boolean hasRightChild(node * aux);
boolean isLeaf(node * aux);
boolean hasNextHop(node * aux);
boolean validPrefix(char * prefix);
boolean validNextHop(int nextHop);
boolean isRoot(node * aux);
void checkArguments(int argc, char ** argv);
void clearWorkspace(node * aux);
void printMenu();


