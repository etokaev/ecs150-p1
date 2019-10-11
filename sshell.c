#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include<sys/stat.h>
#include <fcntl.h>
//#include <linked.h>

#define MAX_NUM_ARGS 16
#define MAX_NUM_CHARS 512
int isError = 0;
int isInterrupt = 0;

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

int cmprStr(node headNode, char *str1){
	int result = strcmp(headNode->arrData[0],str1);
	return result;
}

int arrDataSearch(node headNode, char* token){

	//int sizeArrData = sizeof(headNode->arrData);
	//int size = sizeArrData/sizeof(headNode->arrData[0]);
	//printf("%d\n",size);
	int i = 0;
	while(headNode->arrData[i] != NULL){
		if(strcmp(headNode->arrData[i],token) == 0){
				//found token
				printf("found token: %d\n",i);
				return i;
		}
		i++;
	}

	return -1;
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




void isExit(node headNode){

	if(cmprStr(headNode,"exit") == 0){
		fprintf(stderr, "%s\n","Bye..." );
		exit(0);
	}
}

void isPwd(node headNode){
	char cwd[MAX_NUM_CHARS];

	if(cmprStr(headNode,"pwd") == 0){
		getcwd(cwd,sizeof(cwd));
		printf("%s",cwd);
		printf("\n");
		isInterrupt = 1;//to prevent execvp from running pwd
	}

}

void printArrData(node headNode){

	printf("inside printArrData, length: %ld\n",strlen(*(headNode)->arrData));
	for (int i = 0; i < MAX_NUM_ARGS; i++){
		printf("element number %d: %s\n",i,headNode->arrData[i]);
	}
}


void isCD(node headNode){

	if(cmprStr(headNode,"cd") == 0){
		isInterrupt = 1;
		if(chdir(headNode->arrData[1]) == 0){
		} else{
			fprintf(stderr, "%s\n","Error: no such directory");
		}
	}
}

void builtinCommands(node headNode){
	isExit(headNode);
	isPwd(headNode);
	isCD(headNode);
}


void inputRedir(node headNode){

	int i = arrDataSearch(headNode, "<");
	//fprintf(stdout, "inside inputRedit, i = %d\n", i);
	if (i != -1){ //found "<" in arrData[i]
		if(i == 0){ //lineInput starts with "<"
			fprintf(stderr, "%s\n","Error: missing command" );
			isError = 1;
		}

		// char *tempArr[i];
		// for(int j = 0; j < i; j++){
		// 	tempArr[j] = headNode->arrData[j]; //tempArr holds args before from inputLine before "<"
		// 	fprintf(stdout, "tempArr[%d] = %s\n", j, tempArr[j]);
		// }
		// char *fileName = headNode->arrData[i + 1];
		// printf("filename after '<' : %s\n", fileName);
		//
		// int fd[2];
		// pipe(fd); /* Create pipe */
		// if (fork() != 0) {
		// 	int fd_fileName = open(fileName, O_RDONLY);
		// 	printf("fd_fileName: %d\n", fd_fileName);
		// 	close(fd[0]); /* Don't need read access to pipe */
		// 	dup2(fd[1], STDOUT_FILENO); /* Replace stdout with the pipe */
		// 	close(fd[1]); /* Close now unused file descriptor */
		//
		// 	//fgets(input,MAX_NUM_CHARS + 1,stdin);
		// 	char buf[256];
		// 	char *cmd[] = {"read", fileName, buf, "256"};
		// 	execvp(*cmd, cmd);
		// }

	}


	if(cmprStr(headNode,"<") == 0){

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
			isInterrupt = 0;
			node headNode = createNode();

			display_prompt();
			char *lineInput = get_input();
			trimString(lineInput);
			inputParse(lineInput, &headNode);

			//arrDataSearch(headNode,"<");
			builtinCommands(headNode);
			inputRedir(headNode);
			//printArrData(headNode);

			//read_command(command);
			pid = fork();
			if (pid != 0){
				/*Parent*/
				waitpid(-1, &status, 0);


				fprintf(stderr, "+ completed '%s' [%d]\n", lineInput, status);
				//exit(0);
			} else { //FIND OUT why not printing inside child
					if(isError == 1 || isInterrupt == 1){
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
