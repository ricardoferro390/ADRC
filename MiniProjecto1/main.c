#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Run to compile
// gcc -Wall -o main main.c -g

typedef struct n{
	int nextHop;
	char * prefix;
	struct n * leftChild;
	struct n * rightChild;
} node;

node * NewNode(int prefixSize){
	node * blankNode;
	blankNode = malloc(sizeof(node));
	blankNode->nextHop = -1;
	blankNode->prefix = malloc(prefixSize * sizeof(char));
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


void AddPrefix(node * tree, char * prefix, int nextHop){
	int i;
	node * aux, * newNode;
	printf("\nread prefix %s\n", prefix);
	aux = tree;
		
	if(prefix[0] == '*'){
		strncpy(tree->prefix, prefix, 1);
		tree->nextHop = nextHop;
		printf("* prefix detected\n");
		return;
	}
			
	for(i=0; i<strlen(prefix); i++){
		if(prefix[i] == '0'){
			printf("went left\n");
			if(aux->leftChild == NULL){
				newNode = NewNode(i+1);
				printf("created new node\n");
				strncpy(newNode->prefix, prefix, i+1);
				aux->leftChild = newNode;
			}
			else printf("node already exists\n");
			aux = aux->leftChild;
		}
		else if(prefix[i] == '1'){
			printf("went rigt\n");
			if(aux->rightChild == NULL){
				newNode = NewNode(i+1);
				printf("created new node\n");
				strncpy(newNode->prefix, prefix, i+1);
				aux->rightChild = newNode;
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


int DeletePrefix(node * tree, char * prefix){
		int i;
		node * aux;
		
		aux = tree;
		
		for(i=0; i<strlen(prefix); i++){
			if(prefix[i] == '*') break;
			else if(prefix[i] == '0' && aux->leftChild != NULL) aux = aux->leftChild;
			else if(prefix[i] == '1' && aux->rightChild != NULL) aux = aux->rightChild;
			else return -1;
		}
		
		if(aux->nextHop == -1) return -1;
		aux->nextHop = -1;
		printf("Prefix %s deleted\n", prefix);
		return 0;
};


node * ReadTable(char * inputPath){
	FILE * fp;
	char line[12], prefix[8];
	short nextHop;
	node * tree;
	
	
	fp = fopen(inputPath, "r");

	tree = NewNode(1);
	
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


int main(){
	node * tree;
	int x;
	
	tree = ReadTable("input1.ft");
	PrintTable(tree);
	
	x = DeletePrefix(tree, "010");
	if(x==-1) printf("Prefix not in the tree\n");
	
	PrintTable(tree);
	
	AddPrefix(tree, "01001", 5);
	
	PrintTable(tree);

	exit(0);
}
