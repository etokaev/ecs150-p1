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

struct LinkedList{
	char arrData[MAX_NUM_ARGS];
	struct LinkedList *next;
};

typedef struct LinkedList *node;

node createNode(){
	node temp;
	temp = (node)malloc(sizeof(struct LinkedList)*MAX_NUM_ARGS);
	temp->next = NULL;
	return temp;
}


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

void inputParse(char *lineInput){

	char* token = strtok(lineInput, " ");
	int i = 0;
	while(token){

		printf("%2d %s\n",i++,token);
		token = strtok(NULL," ");
	}
}

/*
void inputParse(char* lineInput){

	char tempArray[31];//SUBSTITUTE TO ACTUAL Array data in LinkedList
	int inputLen = strlen (lineInput);
	int start = 0;
	int end = 0;
	int arrIndex = 0;//for the nodeW
	for(int i = 0; i <= inputLen;i++){
		printf("%d",i);
		if((lineInput[i] != ' ') && (i != inputLen)){
			//skip
		}
		else{
			//parsing and saving
			end = i-1;
			//tempArray[arrIndex] = substring[start][end];
			memcpy(tempArray,&lineInput[start], end-start+1);

			arrIndex++;
		}
		while(lineInput[i] == ' '){
			i++;
			start = i+1;
		}

	}


	for (int j = 0; j < strlen(tempArray);j++){
		printf("\ntempArr: %c\n",tempArray[j]);
	}//return array of parsed strings
}

*/



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

void trimString(char* str){
	str[strcspn(str,"\n")] = 0;
} //trims null character at the end of user input

int main(int argc, char *argv[])  //first line comment//
{

	int status = 0;
	pid_t pid;
	while(1){
			display_prompt();
			char* lineInput = get_input();
			trimString(lineInput);
			char *cmd[] = {lineInput,NULL};

			inputParse(lineInput);
			//read_command(command);
			pid = fork();
			if (pid != 0){
				/*Parent*/
				waitpid(-1, &status, 0);
				fprintf(stderr, "+ completed '%s' [%d]\n", "ls", status);
				//exit(0);
			} else { //FIND OUT why not printing inside child
					execvp(cmd[0],cmd);
					printf("\n here is the error: %d\n",(errno));
					//execv(command[0],command);
					perror("execvp");
					exit(1);
				//fprintf(stderr, "+ completed '%s' [%d]\n", "ls", status);
			}
	}

  	return EXIT_SUCCESS;
}
