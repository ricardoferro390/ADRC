#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_LINE_SIZE 20

int numberOfNodes = 0;


typedef enum{
  FALSE,
  TRUE
} boolean;

typedef struct l{
	int nodeId;
	struct l * next;
} adj;

typedef struct n{
	adj * customers;
	adj * peers;
	adj * providers;
} node;



node * CreateNetwork(int numberOfNodes){
	int i;
	node * network;
	
	network = malloc((numberOfNodes + 1) * sizeof(node));
	for(i = 0; i <= numberOfNodes; i++){
		network[i].customers = NULL;
		network[i].peers = NULL;
		network[i].providers = NULL;
	}
	return network;
}

adj * NewAdj(int nodeId){
	adj * newAdj;
	newAdj = malloc(sizeof(adj));
	newAdj->nodeId = nodeId;
	newAdj->next = NULL;
	return newAdj;
}

adj * AddAdj(adj * list, adj * newAdj){
	if(list == NULL){
		return newAdj;
	}else{
		newAdj->next = list;
		return newAdj;
	}
}



void AddEdge(node * network, int tail, int head, int role){
	
	adj * newAdj;
	newAdj = NewAdj(head);
	
	if(role==1){
		network[tail].customers = AddAdj(network[tail].customers,newAdj);
	}else if(role == 2){
		network[tail].peers = AddAdj(network[tail].peers,newAdj);
	}else if(role == 3){
		network[tail].providers = AddAdj(network[tail].providers,newAdj);
	}
	return;
};

int findNumberOfNodes(char * inputPath){
	FILE * fp;
	int tail, head, role, total = 0;
	char line[MAX_LINE_SIZE];
	
	fp = fopen(inputPath, "r");
	while(fgets(line, MAX_LINE_SIZE, fp) != NULL){
		if(sscanf(line, "%d %d %d", &tail, &head, &role) == 3){
			if(tail > total) total = tail;
		}
		else{
			printf("Line with invalid or few arguments\n");
		}
	}
	fclose(fp);
	return total;
}

node * ReadNetwork(char * inputPath){
	FILE * fp;
	char line[MAX_LINE_SIZE];
	int tail, head, role;
	node * network;
	
	fp = fopen(inputPath, "r");
	if(fp == NULL){
		printf("Error opening file: %s\n", strerror(errno));
		return NULL;
	}
	
	numberOfNodes = findNumberOfNodes(inputPath);
	
	network = CreateNetwork(numberOfNodes);
	
	
	while(fgets(line, MAX_LINE_SIZE, fp) != NULL){
		if(sscanf(line, "%d %d %d", &tail, &head, &role) == 3)
			AddEdge(network, tail, head, role);
		else
			printf("Line with invalid or few arguments\n");
   	}
   	fclose(fp);
   	return network;
   	
};

void printAdjList(node * network){
	int i;
	adj * aux;
	
	for(i=1; i<=numberOfNodes; i++){
		
		printf("Node: %d\n", i);
		printf("Customers:\t");
		aux = network[i].customers;
		while(aux != NULL){
			printf("%d\t", aux->nodeId);
			aux = aux->next;
		}
			
		printf("\nPeers:\t");
		aux = network[i].peers;
		while(aux != NULL){
			printf("%d\t", aux->nodeId);
			aux = aux->next;
		}
			
		printf("\nProviders:\t");
		aux = network[i].providers;
		while(aux != NULL){
			printf("%d\t", aux->nodeId);
			aux = aux->next;
		}
		printf("\n\n");
	}
		
}



int main(){
	node * network;
	
	network = ReadNetwork("NewLargeNetwork.txt");
	printAdjList(network);
	printf("\n");
	
exit(0);
}
