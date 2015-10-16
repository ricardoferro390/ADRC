#include "Utilities.h"

node * NewNode(){
	node * blankNode;
	blankNode = malloc(sizeof(node));
	blankNode->nextHop = -1;
	strcpy(blankNode->prefix, "");
	blankNode->leftChild = NULL;
	blankNode->rightChild = NULL;
	return blankNode;
};

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
};

boolean isRoot(node * aux){
	if(strcmp(aux->prefix, "*") == 0) return TRUE;
	else return FALSE;
};

void checkArguments(int argc, char ** argv){
	if(argc>2){
		printf("\nToo many arguments!\n\n");
		exit(0);
	}
	else if(argc == 2){
		readAtStart = TRUE;
		strcpy(path, argv[1]);
	}
};

void clearWorkspace(node * aux){
	
	if(aux == NULL){
		printf("No memory to free\n");
		return;
	}
	
	if(hasLeftChild(aux))
		clearWorkspace(aux->leftChild);
	if(hasRightChild(aux))
		clearWorkspace(aux->rightChild);
	
	if(isRoot(aux)) printf("Memory cleared!\n");
	free(aux);
	
	return;
};

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
};
