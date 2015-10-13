#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PREFIX_SIZE 8
#define MAX_HOP_NUM_DIGITS 2

// Run to compile
// gcc -Wall -o main main.c -g

short lastHopFound = -1;

typedef enum{
  FALSE,
  TRUE
} boolean;


typedef struct n{
	short nextHop;
	char * prefix;
	struct n * leftChild;
	struct n * rightChild;
} node;

node * NewNode(char * prefix){
	node * blankNode;
	blankNode = malloc(sizeof(node));
	blankNode->nextHop = -1;
	blankNode->prefix = malloc(strlen(prefix) * sizeof(char));
	strncpy(blankNode->prefix, prefix, strlen(prefix));
	blankNode->leftChild = NULL;
	blankNode->rightChild = NULL;
	return blankNode;
};


typedef struct f{
	node * value;
	struct f * next;
} fifo;

fifo * NewFifo(){
	fifo * blankFifo;
	blankFifo = malloc(sizeof(node));
	blankFifo->next = NULL;
	return blankFifo;
};


boolean hasLeftChild(node * aux){
	if(aux->leftChild != NULL) return TRUE;
	else return FALSE;
};
boolean hasRightChild(node * aux){
	if(aux->rightChild != NULL) return TRUE;
	else return FALSE;
};
boolean isLeaf(node * aux){
	if(aux->leftChild == NULL && aux->rightChild == NULL) return TRUE;
	else return FALSE;
};
short getNumberOfChilds(node * aux){
	if(hasLeftChild(aux) && hasRightChild(aux)) return 2;
	if(isLeaf(aux)) return 0;
	else return 1;
};
boolean hasNextHop(node * aux){
	if(aux->nextHop != -1) return TRUE;
	else return FALSE;
};



void AddPrefix(node * tree, char * prefix, int nextHop){
	int i;
	char buffer[PREFIX_SIZE];
	node * aux;
	printf("\nread prefix %s\n", prefix);
	aux = tree;
		
	if(strcmp(prefix, "*") == 0){
		tree->nextHop = nextHop;
		printf("* prefix detected\n");
		return;
	}
			
	for(i=0; i<strlen(prefix); i++){
		if(prefix[i] == '0'){
			printf("went left\n");
			if(!hasLeftChild(aux)){
				aux->leftChild = NewNode(strncpy(buffer, prefix, i+1));
				printf("created new node\n");
			}
			else printf("node already exists\n");
			aux = aux->leftChild;
		}
		else if(prefix[i] == '1'){
			printf("went rigt\n");
			if(!hasRightChild(aux)){
				aux->rightChild = NewNode(strncpy(buffer, prefix, i+1));
				printf("created new node\n");
			}
			else printf("node already exists\n");
			aux = aux->rightChild;
		}
		
		if(i == strlen(prefix)-1){
			aux->nextHop = nextHop;
			printf("nextHop defined as %d\n", aux->nextHop);
		}
	}
	return;
};

boolean DeletePrefix(node * aux, char * prefix){
		boolean deletedChild, isLeft;
		short currentLevel;
		
		if(strcmp(aux->prefix, "*") == 0) currentLevel = 0;
		else currentLevel = strlen(aux->prefix);
		
		
		if(strcmp(aux->prefix, prefix) != 0){
			if(prefix[currentLevel] == '0' && hasLeftChild(aux)){
				deletedChild = DeletePrefix(aux->leftChild, prefix);
				isLeft = TRUE;
			}	
			else if(prefix[currentLevel] == '1' && hasRightChild(aux)){
				deletedChild = DeletePrefix(aux->rightChild, prefix);
				isLeft = FALSE;
			}
			else
				return FALSE;			
		}else{
			if(isLeaf(aux)){
				free(aux);
				return TRUE;
			}
			else{
				aux->nextHop = -1;
				return FALSE;
			} 
		}
		

		if(deletedChild){
			if(isLeft) aux->leftChild = NULL;
			else aux->rightChild = NULL;
			
			if(hasLeftChild(aux) || hasRightChild(aux) || hasNextHop(aux)) return FALSE;
			else{
				free(aux);
				return TRUE;
			}
		}
		else return FALSE;		
};


void TwoTree(node * aux){
	char buffer[PREFIX_SIZE];
	
	if(isLeaf(aux))
		return;
	
	if(hasNextHop(aux)){
		lastHopFound = aux->nextHop;
		aux->nextHop = -1;
	}
	
	if(getNumberOfChilds(aux) == 2){
		TwoTree(aux->leftChild);
		TwoTree(aux->rightChild);
	}
	else{
		strcpy(buffer, aux->prefix);
		if(hasLeftChild(aux)){
			aux->rightChild = NewNode("");
			aux->rightChild->nextHop = lastHopFound;
			strcat(buffer, "1");
			strcpy(aux->rightChild->prefix, buffer);
		}else{
			aux->leftChild = NewNode("");
			aux->leftChild->nextHop = lastHopFound;
			strcat(buffer, "0");
			strcpy(aux->leftChild->prefix, buffer);
		}
		return;
	}
};


short AddressLookUp(node * tree, char * prefix){
	int i, result;
	node * aux;
	
	aux = tree;
	result = aux->nextHop;
	
	for(i=0; i<strlen(prefix); i++){
		
		if(prefix[i] == '0' && hasLeftChild(aux))
			aux = aux->leftChild;
		else if(prefix[i] == '1' && hasRightChild(aux)) 
			aux = aux->rightChild;
		if(hasNextHop(aux)) result = aux->nextHop;
	}
	
	return result;
			
			
};

node * ReadTable(char * inputPath){
	FILE * fp;
	char line[PREFIX_SIZE + MAX_HOP_NUM_DIGITS + 2], prefix[PREFIX_SIZE];
	short nextHop;
	node * tree;
	
	
	fp = fopen(inputPath, "r");

	tree = NewNode("*");
	
	while(fgets(line, 12, fp) != NULL){
		sscanf(line, "%s %hd", prefix, &nextHop);
		AddPrefix(tree, prefix, nextHop);
   	}
   	fclose(fp);
	return tree;
};


int PrintTable(node * tree){
	if(tree == NULL) return -1;
	
	printf("\nPrefix\t\tNext Hop\n");
	
	fifo * nodeFifo, * fifoEnd;
	node * treeAux;
	
	nodeFifo = NewFifo();
	fifoEnd = nodeFifo;
	nodeFifo->value = tree;
	
	while(nodeFifo != NULL){
		treeAux = nodeFifo->value;
		if(treeAux->nextHop != -1) printf("%s\t\t%d\n", treeAux->prefix, treeAux->nextHop);
		if(treeAux->leftChild != NULL){
			fifoEnd->next = NewFifo();
			fifoEnd = fifoEnd->next;
			fifoEnd->value = treeAux->leftChild;
		}
		if(treeAux->rightChild != NULL){
			fifoEnd->next = NewFifo();
			fifoEnd = fifoEnd->next;
			fifoEnd->value = treeAux->rightChild;
		}
		nodeFifo = nodeFifo->next;
	}
	
	return 0;
};


int main(int argc, char* argv[]){
	node * tree;
	
	if(argc!=2){
		printf("\nPlease specify the input file as a single argument\n\n");
		exit(0);
	}
	
	tree = ReadTable(argv[1]);
	PrintTable(tree);

	printf("AddressLookUp = %d\n", AddressLookUp(tree, "0101001"));
	
	TwoTree(tree);
	PrintTable(tree);
	
	printf("AddressLookUp = %d\n", AddressLookUp(tree, "0101001"));

	exit(0);
};
