#include <stdio.h>
#include <stdlib.h>
#define MAX_NUM_ARGS 16
#define MAX_NUM_CHARS 512


struct LinkedList{
	char* arrData[MAX_NUM_ARGS];
	struct LinkedList *next;
};

typedef struct LinkedList *node;

node createNode(){
	node temp;
	temp = (node)malloc(sizeof(struct LinkedList)*MAX_NUM_ARGS);
	temp->next = NULL;
	return temp;
}


/*
node addNode(node head, int val){
	node temp1, temp2;
	temp1 = createNode();
	//temp1->arrData[0] = val; //FIXME WHEN GET TO PIPE
	if(head == NULL){
		head = temp1;
	}//when empty list
	else{
		temp2 = head;
		while(temp2->next != NULL){
			temp2 = temp2->next;
		}
		temp2->next = temp1;
	}
	return head;
}
*/
