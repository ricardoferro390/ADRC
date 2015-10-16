#include "Utilities.h"

short lastHopFound = -1;
boolean readAtStart = FALSE;
char path[BUFFER_SIZE];

short AddPrefix(node * tree, char * prefix, int nextHop){
	int i;
	node * aux;
	
	// Checking if prefix and nextHop are valid
	if(!validPrefix(prefix)){
		printf("Invalid Prefix %s\n", prefix);
		return -2;
	}
	if(!validNextHop(nextHop)){
		printf("Invalid next-hop %d\n", nextHop);
		return -3;
	}
	
	aux = tree;
	
	// If prefix is the default (*)
	if(strcmp(prefix, "*") == 0){
		tree->nextHop = nextHop;
		printf("Added prefix %s with next-hop = %d\n", prefix, nextHop);
		return 0;
	}
	
	// Travels throught the tree
	for(i=0; i<strlen(prefix); i++){
		// if next step is 0
		if(prefix[i] == '0'){
			// if the child doesn't exist
			if(!hasLeftChild(aux)){
				aux->leftChild = NewNode();
				strcpy(aux->leftChild->prefix, prefix);
				aux->leftChild->prefix[i+1] = '\0';
			}
			aux = aux->leftChild;
		}
		// if next step is 1
		else if(prefix[i] == '1'){
			// if the child doesn't exist
			if(!hasRightChild(aux)){
				aux->rightChild = NewNode();
				strcpy(aux->rightChild->prefix, prefix);
				aux->rightChild->prefix[i+1] = '\0';
			}
			aux = aux->rightChild;
		}
		
		// Arrived ate specified prefix
		if(i == strlen(prefix)-1)
			aux->nextHop = nextHop;
	}
	printf("Added prefix %s with next-hop = %d\n", prefix, nextHop);
	return 0;
};


boolean DeletePrefix(node * aux, char * prefix){
	boolean deletedChild, isLeft;
	short currentLevel;
	
	// tree doesn't exist
	if(aux == NULL){
		printf("Please load or create a tree first\n");
		return FALSE;
	}
	
	// currentLevel used to travel throught the tree
	if(isRoot(aux)) currentLevel = 0;
	else currentLevel = strlen(aux->prefix);
	
	if(strcmp(prefix, aux->prefix) == 0){
		// prefix found but doesn't have next hop
		if(!hasNextHop(aux)){
			printf("Prefix %s not found in the tree\n", prefix);
			return FALSE;
		}
		else{
			// prefix found and has no children (delete node)
			if(isLeaf(aux)){
				free(aux);
				printf("Deleted prefix %s\n", prefix);
				return TRUE;
			}
			// prefix found and has children (just update next hop)
			else{
				aux->nextHop = -1;
				printf("Deleted prefix %s\n", prefix);
				return FALSE;
			}
		}		
	}
	else{
		// travel through the tree until prefix match is found
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
	
	// current node had child removed
	if(deletedChild){
		// updating deleted child pointer to NULL
		if(isLeft) aux->leftChild = NULL;
		else aux->rightChild = NULL;
		
		// if current node had child removed, is not a leaf and doesn't have nextHop (deletes himself)
		if(!hasNextHop(aux) && isLeaf(aux)){
			free(aux);
			return TRUE;
		}
	}
	
	return FALSE;
}


void TwoTree(node * aux){
	char buffer[PREFIX_SIZE];
	
	// tree doesn't exist
	if(aux == NULL){
		printf("Please load or create a tree first\n");
		return;
	}
	// tree doesn't have * prefix, so conversion is not valid
	if(isRoot(aux) && !hasNextHop(aux)){
		printf("This tree doesn't have a default (*) next-hop\n");
		return;
	}
	
	// if current node is leaf no action is due. Return to previous node
	if(isLeaf(aux))
		return;
		
	// found node with more specific next hop. registers last hop found
	if(hasNextHop(aux))
		lastHopFound = aux->nextHop;
	else
		aux->nextHop = lastHopFound;

	
	// Recursive call on left child if it exists
	if(hasLeftChild(aux))
		TwoTree(aux->leftChild);
	else{
		// creates left child with last found hop
		strcpy(buffer, aux->prefix);
		aux->leftChild = NewNode();
		aux->leftChild->nextHop = lastHopFound;
		if(isRoot(aux))
			strcpy(buffer, "0");
		else
			strcat(buffer, "0");
		strcpy(aux->leftChild->prefix, buffer);
	}
	
	// Reset lastHopFound and clear hop from node
	if(hasNextHop(aux)){
		lastHopFound = aux->nextHop;
		aux->nextHop = -1;
	}
	
	// Recursive call on right child if it exists
	if(hasRightChild(aux))
		TwoTree(aux->rightChild);
	else{
		// creates right child with last found hop
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
	
	// tree doesn't exist
	if(tree == NULL){
		printf("Please load or create a tree first\n");
		return -2;
	}
	// checks if prefix is valid
	if(!validPrefix(prefix)){
		printf("Invalid Prefix %s\n", prefix);
		return -3;
	}

	aux = tree;
	result = aux->nextHop;
	
	// travels through the tree and gets the most specific nexthop
	for(i=0; i<strlen(prefix); i++){
		if(prefix[i] == '0' && hasLeftChild(aux))
			aux = aux->leftChild;
		else if(prefix[i] == '1' && hasRightChild(aux)) 
			aux = aux->rightChild;
		if(hasNextHop(aux)) result = aux->nextHop;
	}
	
	// if no nexthop is valid
	if(result == -1) printf("The prefix %s has no possible next-hop in this tree\n", prefix);
	return result;
			
			
};

node * ReadTable(char * inputPath){
	FILE * fp;
	char line[PREFIX_SIZE + MAX_HOP_NUM_DIGITS + 2], prefix[PREFIX_SIZE];
	short nextHop;
	node * tree;
	
	// opening input file
	fp = fopen(inputPath, "r");
	if(fp == NULL){
		printf("Error opening file: %s\n", strerror(errno));
		return NULL;
	}
	
	// initializing tree
	tree = NewNode();
	strcpy(tree->prefix, "*");
	
	// for each line, adds prefix to the tree
	while(fgets(line, 12, fp) != NULL){
		if(sscanf(line, "%s %hd", prefix, &nextHop) == 2)
			AddPrefix(tree, prefix, nextHop);
		else
			printf("Line with invalid or few arguments\n");
   	}
   	fclose(fp);
   	printf("Tree loaded from %s\n", inputPath);
	return tree;
};


int PrintTable(node * tree){
	fifo * nodeFifo, * fifoEnd;
	node * treeAux;
	
	// tree doesn't exist
	if(tree == NULL){
		printf("Please load or create a tree first\n");
		return -1;
	}
	
	// creation of FIFO with nodes to print
	nodeFifo = NewFifo();
	fifoEnd = nodeFifo;
	nodeFifo->value = tree;
	
	printf("\nPrefix\t\tNext Hop\n");
	
	// travels through the tree
	while(nodeFifo != NULL){
		treeAux = nodeFifo->value;
		// print current node
		if(hasNextHop(treeAux)) printf("%s\t\t%hd\n", treeAux->prefix, treeAux->nextHop);
		// adds the childs of the current node to the end of the FIFO
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
		// goes to the next node on the FIFO
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
	
	// printing menu
	printMenu();
	if(readAtStart) tree = ReadTable(path);
	printf("Please select an option\n");
	
	tree = ReadTable("input1.ft");
	
	while(TRUE){
		// gets user commands and arguments
		printf("-> ");
		fgets (buffer, BUFFER_SIZE, stdin);
		nArgs = sscanf(buffer, "%c %s %hd %s", &option, arg1, &arg2, garbageDetector);
		
		if(tree == NULL && option == 'a' && nArgs == 3){
			tree = NewNode();
			strcpy(tree->prefix, "*");
		}
		
		
		
		// selects function based on user option
		if(option == 'f' && nArgs == 2) tree = ReadTable(arg1);
		else if(option == 'a' && nArgs == 3) AddPrefix(tree, arg1, arg2);
		else if(option == 'd' && nArgs == 2) treeDeleted = DeletePrefix(tree, arg1);
		else if(option == 'p' && nArgs == 1) PrintTable(tree);
		else if(option == 'c' && nArgs == 1) TwoTree(tree);
		else if(option == 'l' && nArgs == 2){
			lookupResult = AddressLookUp(tree, arg1);
			if(lookupResult > 0) printf("Next-hop(%s) = %hd\n", arg1, lookupResult);
		}
		else if(option == 'h' && nArgs == 1) printMenu();
		else if(option == 'q' && nArgs == 1){
			clearWorkspace(tree);
			printf("Project by: Diogo Salgueiro 72777 and Ricardo Ferro 72870\n");
			printf("Goodbye!\n");
			return;
		}
		else printf("Invalid command\n");
		
		// handle case where tree becomes empty after delete prefix
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
