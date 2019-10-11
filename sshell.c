#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
//#include <linked.h>

#define MAX_NUM_ARGS 16
#define MAX_NUM_CHARS 512
int isError = 0;


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
node addNode(node head,char val){
	node temp1, temp2;
	temp1 = createNode();
	temp1->arrData[0] = val; //FIXME WHEN GET TO PIPE
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

void inputParse(char *lineInput, node *headNode){

	char* token = strtok(lineInput, " ");
	int i = 0;
	while(token){
		if(i > 15){
			fprintf(stderr, "%s\n","Error: too many process arguments" );
			isError = 1;
			//exit(0);
		}
		//printf("%s\n",token);
		(*headNode)->arrData[i] = token;
		//printf("%s\n",(*headNode)->arrData[i]);
		i++;
		token = strtok(NULL," ");
	}
	//return headNode;
}


int display_prompt(){

	printf("sshell$ ");
	return 0;
}


char* get_input(){

  char* input;
	input = (char*)malloc(MAX_NUM_CHARS + 1);
	fgets(input,MAX_NUM_CHARS + 1,stdin);
	return input;
}

int checkErrors(char* lineIn){

	return 0;
}

void isExit(node headNode){
	char strExit[] = "exit";
	//printf("in headNode");
  int result = strcmp(*(headNode)->arrData,strExit);
	if(result == 0){
		fprintf(stderr, "%s\n","Bye..." );
		exit(0);
	}
}
void trimString(char* str){
	str[strcspn(str,"\n")] = 0;
} //trims null character at the end of user input

int main(int argc, char *argv[])  //first line comment//
{

	int status = 0;
	pid_t pid;

	while(1){
			isError = 0;
			node headNode = createNode();

			display_prompt();
			char* lineInput = get_input();
			trimString(lineInput);
			inputParse(lineInput, &headNode);
			//printf("\ncmd[0] = %s\n",*cmd[0]);
			isExit(headNode);


			//read_command(command);
			pid = fork();
			if (pid != 0){
				/*Parent*/
				waitpid(-1, &status, 0);
				//fprintf(stderr, "+ completed '%s' [%d]\n", *(headNode)->arrData, status);
				//exit(0);
			} else { //FIND OUT why not printing inside child
					if(isError == 1){
						//reset global error checker
						exit(0);
					}
					execvp(*(headNode)->arrData,(headNode)->arrData);
					printf("\n here is the error: %d\n",(errno));
					//execv(command[0],command);
					perror("execvp");
					exit(1);
				//fprintf(stderr, "+ completed '%s' [%d]\n", "ls", status);
			}
	}

  	return EXIT_SUCCESS;
}
