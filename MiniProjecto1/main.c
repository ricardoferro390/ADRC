#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
	printf("Please select an option\n");
}

short AddPrefix(node * tree, char * prefix, int nextHop){
	int i;
	char buffer[PREFIX_SIZE];
	node * aux;
	
	if(!validPrefix(prefix)) return -1;
	if(!validNextHop(nextHop)) return -2;
	
	aux = tree;
		
	if(strcmp(prefix, "*") == 0){
		tree->nextHop = nextHop;
		return 0;
	}
			
	for(i=0; i<strlen(prefix); i++){
		if(prefix[i] == '0'){
			if(!hasLeftChild(aux))
				aux->leftChild = NewNode(strncpy(buffer, prefix, i+1));
			aux = aux->leftChild;
		}
		else if(prefix[i] == '1'){
			if(!hasRightChild(aux))
				aux->rightChild = NewNode(strncpy(buffer, prefix, i+1));
			aux = aux->rightChild;
		}
		
		if(i == strlen(prefix)-1)
			aux->nextHop = nextHop;
	}
	return 0;
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
		if(treeAux->nextHop != -1) printf("%s\t\t%hd\n", treeAux->prefix, treeAux->nextHop);
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

void menuHandler(){
	node * tree = NULL;
	char buffer[BUFFER_SIZE];
	char option, arg1[BUFFER_SIZE];
	short arg2;
	short nArgs;
	
	if(readAtStart) tree = ReadTable(path);
	
	while(TRUE){
		
		fgets (buffer, BUFFER_SIZE, stdin);
		nArgs = sscanf(buffer,"%c %s %hd", &option, arg1, &arg2);
		//printf("%d %d %d %d\n", strlen(buffer), 1, strlen(arg1), (arg2/10)+1);
		
		switch(option){
			case 'f':
				if(nArgs==2){
					tree = ReadTable(arg1);
					break;
				}			
			case 'a':
				if(nArgs==3){
					AddPrefix(tree, arg1, arg2);
					break;
				}
			case 'd':
				if(nArgs==2){
					DeletePrefix(tree, arg1);
					break;
				}
			case 'p':
				if(nArgs==1){
					PrintTable(tree);
					break;
				}
			case 'c':
				if(nArgs==1){
					TwoTree(tree);
					break;
				}
			case 'l':
				if(nArgs==2){
					AddressLookUp(tree, arg1);
					break;
				}
			case 'h':
				if(nArgs==1){
					printMenu();
					break;
				}
			case 'q':
				if (nArgs==1) return;
			default:
				printf("Invalid command\n");
		}
	}
};

int main(int argc, char* argv[]){

	if(argc>2){
		printf("\nToo many arguments!\n\n");
		exit(0);
	}
	else if(argc == 2){
		readAtStart = TRUE;
		strcpy(path, argv[1]);
	}

	printMenu();
	/*  AddPrefix error handling test
	short x;
	x = AddPrefix(tree, "0110", -3);
	switch(x){
		case 0:
			printf("prefix added\n");
			break;
		case -1:
			printf("prefix not valid\n");
			break;
		case -2:
			printf("nextHop not valid\n");
			break;
	}*/
	menuHandler();
	printf("Goodbye!\n");
	exit(0);
};
