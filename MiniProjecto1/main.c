#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define PREFIX_SIZE 8
#define MAX_HOP_NUM_DIGITS 2
#define BUFFER_SIZE 128

// Run to compile
// gcc -Wall -o main main.c -g

typedef enum{
  FALSE,
  TRUE
} boolean;



short lastHopFound = -1;
boolean readAtStart = FALSE;
char path[BUFFER_SIZE];



typedef struct n{
	short nextHop;
	char prefix[PREFIX_SIZE+1];
	struct n * leftChild;
	struct n * rightChild;
} node;

node * NewNode(){
	node * blankNode;
	blankNode = malloc(sizeof(node));
	blankNode->nextHop = -1;
	strcpy(blankNode->prefix, "");
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
boolean hasNextHop(node * aux){
	if(aux->nextHop != -1) return TRUE;
	else return FALSE;
};
boolean validPrefix(char * prefix){
	int i;
	if(strcmp(prefix, "*") == 0) return TRUE;
	for(i=0; i<strlen(prefix); i++){
		if(prefix[i]=='0' || prefix[i]=='1') continue;
		else return FALSE;
	}
	return TRUE;
};
boolean validNextHop(int nextHop){
	if(nextHop > 0) return TRUE;
	else return FALSE;
}
boolean isRoot(node * aux){
	if(strcmp(aux->prefix, "*") == 0) return TRUE;
	else return FALSE;
}

void checkArguments(int argc, char ** argv){
	if(argc>2){
		printf("\nToo many arguments!\n\n");
		exit(0);
	}
	else if(argc == 2){
		readAtStart = TRUE;
		strcpy(path, argv[1]);
	}
}

void printMenu(){
	printf("\n\tADRC - Mini Project I - Forwarding traffic in the internet\n");
	printf("\tBy: Diogo Salgueiro 72777, Ricardo Ferro 72870\n");
	printf("\n\t- Load tree from file:\t\tf [file]\n");
	printf("\t- Add Prefix:\t\t\ta [prefix] [next hop]\n");
	printf("\t- Delete Prefix:\t\td [prefix]\n");
	printf("\t- Print Address Table:\t\tp\n");
	printf("\t- Convert to 2-tree:\t\tc\n");
	printf("\t- Adress Look-Up:\t\tl [prefix]\n");
	printf("\t- Help:\t\t\t\th\n");
	printf("\t- Quit:\t\t\t\tq\n\n");
}

short AddPrefix(node * tree, char * prefix, int nextHop){
	int i;
	char buffer[PREFIX_SIZE];
	node * aux;
	
	if(!validPrefix(prefix)){
		printf("Invalid Prefix %s\n", prefix);
		return -2;
	}
	if(!validNextHop(nextHop)){
		printf("Invalid next-hop %d\n", nextHop);
		return -3;
	}
	
	aux = tree;
		
	if(strcmp(prefix, "*") == 0){
		tree->nextHop = nextHop;
		printf("Added prefix %s with next-hop = %d\n", prefix, nextHop);
		return 0;
	}
			
	for(i=0; i<strlen(prefix); i++){
		if(prefix[i] == '0'){
			if(!hasLeftChild(aux)){
				aux->leftChild = NewNode();
				strcpy(aux->leftChild->prefix, strncpy(buffer, prefix, i+1));
			}
			aux = aux->leftChild;
		}
		else if(prefix[i] == '1'){
			if(!hasRightChild(aux)){
				aux->rightChild = NewNode();
				strcpy(aux->rightChild->prefix, strncpy(buffer, prefix, i+1));
			}
			aux = aux->rightChild;
		}
		
		if(i == strlen(prefix)-1)
			aux->nextHop = nextHop;
	}
	printf("Added prefix %s with next-hop = %d\n", prefix, nextHop);
	return 0;
};


boolean DeletePrefix(node * aux, char * prefix){
	boolean deletedChild, isLeft;
	short currentLevel;
	
	if(aux == NULL){
		printf("Please load or create a tree first\n");
		return FALSE;
	}
	
	if(isRoot(aux)) currentLevel = 0;
	else currentLevel = strlen(aux->prefix);
	
	if(strcmp(prefix, aux->prefix) == 0){
		if(!hasNextHop(aux)){
			printf("Prefix %s not found in the tree\n", prefix);
			return FALSE;
		}
		else{
			if(isLeaf(aux)){
				free(aux);
				printf("Deleted prefix %s\n", prefix);
				return TRUE;
			}
			else{
				aux->nextHop = -1;
				printf("Deleted prefix %s\n", prefix);
				return FALSE;
			}
		}		
	}
	else{
		if(prefix[currentLevel] == '0' && hasLeftChild(aux)){
			deletedChild = DeletePrefix(aux->leftChild, prefix);
			isLeft = TRUE;
		}
		else if(prefix[currentLevel] == '1' && hasRightChild(aux)){
			deletedChild = DeletePrefix(aux->rightChild, prefix);
			isLeft = FALSE;
		}
		else{
			printf("Prefix %s not found in the tree\n", prefix);
			return FALSE;
		}
	}
	
	if(deletedChild){
		if(isLeft) aux->leftChild = NULL;
		else aux->rightChild = NULL;
		
		if(!hasNextHop(aux) && isLeaf(aux)){
			free(aux);
			return TRUE;
		}
	}
	
	return FALSE;
}


void TwoTree(node * aux){
	char buffer[PREFIX_SIZE];
	
	if(isRoot(aux) && !hasNextHop(aux)){
		printf("This tree doesn't have a default (*) next-hop\n");
		return;
	}
	
	if(aux == NULL){
		printf("Please load or create a tree first\n");
		return;
	}
	
	if(isLeaf(aux))
		return;
	
	if(hasNextHop(aux)){
		lastHopFound = aux->nextHop;
		aux->nextHop = -1;
	}
	
	if(hasLeftChild(aux))
		TwoTree(aux->leftChild);
	else{
		strcpy(buffer, aux->prefix);
		aux->leftChild = NewNode();
		aux->leftChild->nextHop = lastHopFound;
		if(isRoot(aux))
			strcpy(buffer, "0");
		else
			strcat(buffer, "0");
		strcpy(aux->leftChild->prefix, buffer);
	}
	if(hasRightChild(aux))
		TwoTree(aux->rightChild);
	else{
		strcpy(buffer, aux->prefix);
		aux->rightChild = NewNode();
		aux->rightChild->nextHop = lastHopFound;
		if(isRoot(aux))
			strcpy(buffer, "1");
		else
			strcat(buffer, "1");
		strcpy(aux->rightChild->prefix, buffer);
	}
	if(isRoot(aux)) printf("Binary tree converted to 2-tree!\n");
	return;
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
	if(fp == NULL){
		printf("Error opening file: %s\n", strerror(errno));
		return NULL;
	}
	
	tree = NewNode();
	strcpy(tree->prefix, "*");

	while(fgets(line, 12, fp) != NULL){
		sscanf(line, "%s %hd", prefix, &nextHop);
		AddPrefix(tree, prefix, nextHop);
   	}
   	fclose(fp);
   	printf("Tree loaded from %s\n", inputPath);
	return tree;
};


int PrintTable(node * tree){
	fifo * nodeFifo, * fifoEnd;
	node * treeAux;
	
	if(tree == NULL){
		printf("Please load or create a tree first\n");
		return -1;
	}
	
	nodeFifo = NewFifo();
	fifoEnd = nodeFifo;
	nodeFifo->value = tree;
	
	printf("\nPrefix\t\tNext Hop\n");
	
	while(nodeFifo != NULL){
		treeAux = nodeFifo->value;
		if(hasNextHop(treeAux)) printf("%s\t\t%hd\n", treeAux->prefix, treeAux->nextHop);
		if(hasLeftChild(treeAux)){
			fifoEnd->next = NewFifo();
			fifoEnd = fifoEnd->next;
			fifoEnd->value = treeAux->leftChild;
		}
		if(hasRightChild(treeAux)){
			fifoEnd->next = NewFifo();
			fifoEnd = fifoEnd->next;
			fifoEnd->value = treeAux->rightChild;
		}
		nodeFifo = nodeFifo->next;
	}
	printf("\n");
	return 0;
};

void MenuHandler(){
	node * tree = NULL;
	char option, buffer[BUFFER_SIZE], arg1[BUFFER_SIZE], garbageDetector[BUFFER_SIZE];
	short arg2, nArgs, lookupResult;
	boolean treeDeleted = FALSE;
	
	printMenu();
	if(readAtStart) tree = ReadTable(path);
	printf("Please select an option\n");
	
	while(TRUE){
		printf("-> ");
		fgets (buffer, BUFFER_SIZE, stdin);
		nArgs = sscanf(buffer, "%c %s %hd %s", &option, arg1, &arg2, garbageDetector);
		
		if(tree == NULL && option == 'a' && nArgs == 3){
			tree = NewNode();
			strcpy(tree->prefix, "*");
		}
		
		if(option == 'f' && nArgs == 2) tree = ReadTable(arg1);
		else if(option == 'a' && nArgs == 3) AddPrefix(tree, arg1, arg2);
		else if(option == 'd' && nArgs == 2) treeDeleted = DeletePrefix(tree, arg1);
		else if(option == 'p' && nArgs == 1) PrintTable(tree);
		else if(option == 'c' && nArgs == 1) TwoTree(tree);
		else if(option == 'l' && nArgs == 2){
			lookupResult = AddressLookUp(tree, arg1);
			if(lookupResult == -1) printf("The prefix %s has no possible next-hop in this tree\n", arg1);
			else printf("Next-hop(%s) = %hd\n", arg1, lookupResult);
		}
		else if(option == 'h' && nArgs == 1) printMenu();
		else if(option == 'q' && nArgs == 1){
			printf("Project by: Diogo Salgueiro 72777 and Ricardo Ferro 72870\n");
			printf("Goodbye!\n");
			return;
		}
		else printf("Invalid command\n");
		
		if(treeDeleted){
			tree = NULL;
			treeDeleted = FALSE;
		}
	}
};

int main(int argc, char ** argv){

	checkArguments(argc, argv);
	MenuHandler();
	exit(0);
	
};
