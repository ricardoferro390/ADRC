#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Run to compile
// gcc -Wall -o main main.c -g

typedef struct n{
	int nextHop;
	struct n * leftChild;
	struct n * rightChild;
} node;

node * NewNode(){
	node * blankNode;
	blankNode = malloc(sizeof(node));
	blankNode->nextHop = -1;
	blankNode->leftChild = NULL;
	blankNode->rightChild = NULL;
	return blankNode;
}

node * AddPrefix(node * tree, char * prefix, int nextHop){
	int i;
	node * aux, * newNode;
	printf("\nread prefix %s\n", prefix);
	aux = tree;
		
	if(prefix[0] == '*'){
		tree->nextHop = nextHop;
		printf("* prefix detected\n");
		return tree;
	}
			
	for(i=0; i<strlen(prefix); i++){
		if(prefix[i] == '0'){
			printf("went left\n");
			if(aux->leftChild == NULL){
				newNode = NewNode();
				printf("created new node\n");
				aux->leftChild = newNode;
			}
			else printf("node already exists\n");
			aux = aux->leftChild;
		}
		else if(prefix[i] == '1'){
			printf("went rigt\n");
			if(aux->rightChild == NULL){
				newNode = NewNode();
				printf("created new node\n");
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
	return tree;
};

node * ReadTable(char * inputPath){
	FILE * fp;
	char line[12], prefix[8];
	short nextHop;
	node * tree;
	
	
	fp = fopen(inputPath, "r");

	tree = NewNode();
	
	while(fgets(line, 12, fp) != NULL){
		sscanf(line, "%s %hd", prefix, &nextHop);
		tree = AddPrefix(tree, prefix, nextHop);
   	}
   	fclose(fp);
	return tree;
} 


int main(){
	node * tree;
	
	tree = ReadTable("input1.ft");

	exit(0);
}
