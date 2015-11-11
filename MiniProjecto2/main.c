#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_LINE_SIZE 20
#define PROVIDER_ROUTE 1
#define PEER_ROUTE 2
#define CUSTOMER_ROUTE 3
#define MAXIMUM_NUMBER_OF_HOPS 50


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

typedef struct f{
	int nodeId;
	int previousNode;
	int currentRouteType;
	int currentHops;
	struct f * next;
} fifo;

typedef struct r{
	int routeType;
	int hops;
} routingTable;

typedef struct s{
	boolean sentToCustomers;
	boolean sentToPeers;
	boolean sentToProviders;
} sentRecords;

typedef struct st{
	int numberOfLinkedNodes;
	int numberPairOfNodes;
	int numberOfUnusableRoutes;
	int numberOfProviderRoutes;
	int numberOfPeerRoutes;
	int numberOfCustomerRoutes;
	int numberOfHops[MAXIMUM_NUMBER_OF_HOPS];
} statistics;


routingTable * NewRoutingTable(int numberOfNodes){
	int i;
	routingTable * newRoutingTable;
	newRoutingTable = malloc((numberOfNodes + 1) * sizeof(routingTable));
	for(i = 0; i <= numberOfNodes; i++){
		newRoutingTable[i].routeType = -1;
		newRoutingTable[i].hops = -1;
	}
	return newRoutingTable;
}

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

sentRecords * NewRecords(int numberOfNodes){
	int i;
	sentRecords * records;
	
	records = malloc((numberOfNodes + 1) * sizeof(sentRecords));
	for(i = 0; i <= numberOfNodes; i++){
		records[i].sentToCustomers = FALSE;
		records[i].sentToPeers = FALSE;
		records[i].sentToProviders = FALSE;
	}
	return records;
}


statistics * NewStatistics(){
	
	int i;
	statistics * newStatistics;
	
	newStatistics = malloc(sizeof(statistics));
	
	newStatistics->numberOfLinkedNodes = 0;
	newStatistics->numberPairOfNodes = 0;
	newStatistics->numberOfUnusableRoutes = 0;
	newStatistics->numberOfPeerRoutes = 0;
	newStatistics->numberOfCustomerRoutes = 0;
	newStatistics->numberOfProviderRoutes = 0;
		
	for(i = 0; i <= numberOfNodes; i++)
		newStatistics->numberOfHops[i] = 0;
	
	return newStatistics;
}


fifo * NewFifoElement(int previousNode, int nodeId, int currentRouteType, int currentHops){
	fifo * newElement;
	newElement = malloc(sizeof(fifo));
	newElement->previousNode = previousNode;
	newElement->nodeId = nodeId;
	newElement->currentRouteType = currentRouteType;
	newElement->currentHops = currentHops;
	newElement->next = NULL;
	return newElement;
}

fifo * InsertFifo(fifo * fifoEnd, fifo * newElement){
	if(fifoEnd == NULL) return newElement;
	fifoEnd->next = newElement;
	return fifoEnd->next;
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
		if(sscanf(line, "%d %d %d", &tail, &head, &role) == 3){
			if(tail<2000 || head<2000) 
				AddEdge(network, tail, head, role);
		}
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

void printRoutingTable(node * network, routingTable * results){
	int i;
	for(i = 1; i <= numberOfNodes; i++){
		if(network[i].providers != NULL || network[i].peers != NULL || network[i].customers != NULL){
			if(results[i].routeType == -1) printf("Node:  %d\tUnusable Route\n", i);
			else if(results[i].routeType == 0) printf("Node:  %d\tDestination Node\n", i);
			else if(results[i].routeType == 1) printf("Node:  %d\tProvider Route\t\tHops: %d\n", i, results[i].hops);
			else if(results[i].routeType == 2) printf("Node:  %d\tPeer Route\t\tHops: %d\n", i, results[i].hops);
			else if(results[i].routeType == 3) printf("Node:  %d\tCustomer Route\t\tHops: %d\n", i, results[i].hops);
		}
	}
	return;
}	
		
		
routingTable * findRoutesToNode(node * network, int destinationNode){
	fifo * currentNode = NULL, * fifoEnd = NULL, * aux = NULL;
	adj * cursor = NULL;
	routingTable * results;
	sentRecords * records;
	
	results = NewRoutingTable(numberOfNodes);
	records = NewRecords(numberOfNodes);
	
	// first node only (destination node)
	results[destinationNode].hops = 0;
	results[destinationNode].routeType = 0;
	cursor = network[destinationNode].providers;
	while(cursor != NULL){
		if(currentNode == NULL){
			currentNode = InsertFifo(currentNode, NewFifoElement(destinationNode, cursor->nodeId, CUSTOMER_ROUTE, 1));
			fifoEnd = currentNode;
		}
		else fifoEnd = InsertFifo(fifoEnd, NewFifoElement(destinationNode, cursor->nodeId, CUSTOMER_ROUTE, 1));
		cursor = cursor->next;
	}
	records[destinationNode].sentToProviders = TRUE;
	
	cursor = network[destinationNode].peers;
	while(cursor != NULL){
		if(currentNode == NULL){
			currentNode = InsertFifo(currentNode, NewFifoElement(destinationNode, cursor->nodeId, PEER_ROUTE, 1));
			fifoEnd = currentNode;
		}
		else fifoEnd = InsertFifo(fifoEnd, NewFifoElement(destinationNode, cursor->nodeId, PEER_ROUTE, 1));
		cursor = cursor->next;
	}
	records[destinationNode].sentToPeers = TRUE;
	
	cursor = network[destinationNode].customers;
	while(cursor != NULL){
		if(currentNode == NULL){
			currentNode = InsertFifo(currentNode, NewFifoElement(destinationNode, cursor->nodeId, PROVIDER_ROUTE, 1));
			fifoEnd = currentNode;
		}
		else fifoEnd = InsertFifo(fifoEnd, NewFifoElement(destinationNode, cursor->nodeId, PROVIDER_ROUTE, 1));
		cursor = cursor->next;
	}
	records[destinationNode].sentToCustomers = TRUE;
	////////////////////////////////////////

	while(currentNode != NULL){
		//printf("Current Node = %d\n", currentNode->nodeId);
		
		if(currentNode->currentRouteType > results[currentNode->nodeId].routeType || (currentNode->currentRouteType == results[currentNode->nodeId].routeType && currentNode->currentHops < results[currentNode->nodeId].hops)){
			results[currentNode->nodeId].routeType = currentNode->currentRouteType;
			results[currentNode->nodeId].hops = currentNode->currentHops;
		}
		
		if(currentNode->currentRouteType == CUSTOMER_ROUTE){
			cursor = network[currentNode->nodeId].providers;
			while(cursor != NULL && !records[currentNode->nodeId].sentToProviders){
				if(cursor->nodeId != currentNode->previousNode && cursor->nodeId != destinationNode) fifoEnd = InsertFifo(fifoEnd, NewFifoElement(currentNode->nodeId, cursor->nodeId, CUSTOMER_ROUTE, currentNode->currentHops + 1));
				cursor = cursor->next;
			}
			records[currentNode->nodeId].sentToProviders = TRUE;
			cursor = network[currentNode->nodeId].peers;
			while(cursor != NULL && !records[currentNode->nodeId].sentToPeers){
				if(cursor->nodeId != currentNode->previousNode && cursor->nodeId != destinationNode) fifoEnd = InsertFifo(fifoEnd, NewFifoElement(currentNode->nodeId, cursor->nodeId, PEER_ROUTE, currentNode->currentHops + 1));
				cursor = cursor->next;
			}
			records[currentNode->nodeId].sentToPeers = TRUE;
		}
		
		cursor = network[currentNode->nodeId].customers;
		while(cursor != NULL && !records[currentNode->nodeId].sentToCustomers){
			if(cursor->nodeId != currentNode->previousNode && cursor->nodeId != destinationNode) fifoEnd = InsertFifo(fifoEnd, NewFifoElement(currentNode->nodeId, cursor->nodeId, PROVIDER_ROUTE, currentNode->currentHops + 1));
			cursor = cursor->next;
		}
		records[currentNode->nodeId].sentToCustomers = TRUE;
				
		aux = currentNode;
		currentNode = currentNode->next;
		free(aux);
	}
	
	//printRoutingTable(network, results);
	return results;
}

/*
routingTable * findRoutesToNodeOld(node * network, int destinationNode){
	fifo * currentNode = NULL, * fifoEnd = NULL, * aux = NULL;
	adj * cursor = NULL;
	routingTable * results;
	
	results = NewRoutingTable(numberOfNodes);
	
	// first node only (destination node)
	results[destinationNode].hops = 0;
	results[destinationNode].routeType = 0;
	cursor = network[destinationNode].providers;
	while(cursor != NULL){
		if(currentNode == NULL){
			currentNode = InsertFifo(currentNode, NewFifoElement(destinationNode, cursor->nodeId, CUSTOMER_ROUTE, 1));
			fifoEnd = currentNode;
		}
		else fifoEnd = InsertFifo(fifoEnd, NewFifoElement(destinationNode, cursor->nodeId, CUSTOMER_ROUTE, 1));
		cursor = cursor->next;
	}
	
	cursor = network[destinationNode].peers;
	while(cursor != NULL){
		if(currentNode == NULL){
			currentNode = InsertFifo(currentNode, NewFifoElement(destinationNode, cursor->nodeId, PEER_ROUTE, 1));
			fifoEnd = currentNode;
		}
		else fifoEnd = InsertFifo(fifoEnd, NewFifoElement(destinationNode, cursor->nodeId, PEER_ROUTE, 1));
		cursor = cursor->next;
	}
	
	cursor = network[destinationNode].customers;
	while(cursor != NULL){
		if(currentNode == NULL){
			currentNode = InsertFifo(currentNode, NewFifoElement(destinationNode, cursor->nodeId, PROVIDER_ROUTE, 1));
			fifoEnd = currentNode;
		}
		else fifoEnd = InsertFifo(fifoEnd, NewFifoElement(destinationNode, cursor->nodeId, PROVIDER_ROUTE, 1));
		cursor = cursor->next;
	}
	////////////////////////////////////////

	while(currentNode != NULL){
		//printf("Current Node = %d\n", currentNode->nodeId);
		
		if(currentNode->currentRouteType > results[currentNode->nodeId].routeType || (currentNode->currentRouteType == results[currentNode->nodeId].routeType && currentNode->currentHops < results[currentNode->nodeId].hops)){
			results[currentNode->nodeId].routeType = currentNode->currentRouteType;
			results[currentNode->nodeId].hops = currentNode->currentHops;
		}
		
		if(currentNode->currentRouteType == CUSTOMER_ROUTE){
			cursor = network[currentNode->nodeId].providers;
			while(cursor != NULL){
				if(cursor->nodeId != currentNode->previousNode && cursor->nodeId != destinationNode) fifoEnd = InsertFifo(fifoEnd, NewFifoElement(currentNode->nodeId, cursor->nodeId, CUSTOMER_ROUTE, currentNode->currentHops + 1));
				cursor = cursor->next;
			}
			cursor = network[currentNode->nodeId].peers;
			while(cursor != NULL){
				if(cursor->nodeId != currentNode->previousNode && cursor->nodeId != destinationNode) fifoEnd = InsertFifo(fifoEnd, NewFifoElement(currentNode->nodeId, cursor->nodeId, PEER_ROUTE, currentNode->currentHops + 1));
				cursor = cursor->next;
			}
		}
		
		cursor = network[currentNode->nodeId].customers;
		while(cursor != NULL){
			if(cursor->nodeId != currentNode->previousNode && cursor->nodeId != destinationNode) fifoEnd = InsertFifo(fifoEnd, NewFifoElement(currentNode->nodeId, cursor->nodeId, PROVIDER_ROUTE, currentNode->currentHops + 1));
			cursor = cursor->next;
		}
		
		aux = currentNode;
		currentNode = currentNode->next;
		free(aux);
	}
	
	//printRoutingTable(network, results);
	return results;
}
*/

statistics * GetStatistics(node * network){

	int i;
	statistics * stats;
	routingTable * results;
	
	stats = NewStatistics();
	
	for(i=1;i<=numberOfNodes;i++){
		results = findRoutesToNode(network, i);
		
	}
	
	return stats;
}

/*
boolean compareResults(node * network, int destinationNode){
	int i;
	routingTable * newResults, * oldResults;
	
	newResults = findRoutesToNode(network, destinationNode);
	printf("Optimized Version is done!\n");
	oldResults = findRoutesToNodeOld(network, destinationNode);
	
	for(i = 1; i <= numberOfNodes; i++){
		if(newResults[i].routeType != oldResults[i].routeType || newResults[i].hops != oldResults[i].hops){
			printf("Node:  %d\tRoute Type:  %d\t\tHops: %d\n", i, newResults[i].routeType, newResults[i].hops);
			printf("Node:  %d\tRoute Type:  %d\t\tHops: %d\n", i, oldResults[i].routeType, oldResults[i].hops);
			return FALSE;
		}
	}
	return TRUE;
}
*/


int main(){
	node * network;
	
	network = ReadNetwork("NewLargeNetwork.txt");
	//network = ReadNetwork("Enunciado.txt");
	//network = ReadNetwork("Enunciado2.txt");
	//printAdjList(network);
	
	//findRoutesToNode(network, 4);
	
	//printf("%d\n", compareResults(network, 4));
	
exit(0);
}
