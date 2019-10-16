#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include<sys/stat.h>
#include <fcntl.h>

#define MAX_NUM_ARGS 16
#define MAX_NUM_CHARS 512

int isError = 0;
int isInterrupt = 0;

struct LinkedList
{
	char* arrData[MAX_NUM_ARGS];
	struct LinkedList *next;
	int pid;
	int status;
};

typedef struct LinkedList *node;

node createNode()
{
	node temp;
	temp = (node)malloc(sizeof(struct LinkedList)*MAX_NUM_ARGS);
	temp->next = NULL;
	return temp;
}

void execSingleCmd(char* lineInput, node headNode);
void trimEndNull(char* str);
void pipeParse(char* lineInput, node *headNode);
void makePipe(int numCmds,node headNode,char* lineInput);



int cmprStr(node headNode, char *str1)
{
	int result = strcmp(headNode->arrData[0],str1);
	return result;
}


void trimLeading(char * str)
{
	int index, i;
	index = 0;
	/* Find last index of whitespace character */
	while(str[index] == ' ' || str[index] == '\t' || str[index] == '\n')
	{
		index++;
	}
	if(index != 0){
	/* Shift all trailing characters to its left */
		i = 0;
	while(str[i + index] != '\0'){
		str[i] = str[i + index];
		i++;
        }
	str[i] = '\0'; // Make sure that string is NULL terminated
	}
}//From https://codeforwin.org/2016/04/c-program-to-trim-leading-white-spaces-in-string.html

void inputParse(char *lineInput, node *headNode)
{
	// for any NODE works

	char* token1 = strtok(lineInput, " \n");
	int i = 0;
	while(token1){
		if(i >= 16){
			fprintf(stderr, "%s\n","Error: too many process arguments" );
			isError = 1;
		}
		trimLeading(token1);
		trimEndNull(token1);
		(*headNode)->arrData[i] = token1;

		i++;
		token1 = strtok(NULL," \n");
	}

}

void append(struct LinkedList** head,char* cmd)
{
	struct LinkedList* new_node = createNode();
	struct LinkedList *last = *head;
 	//check for redirect and save to filename if there is redirect
	inputParse(cmd,head);
	new_node->next = NULL;
	if(*head == NULL){
		*head = new_node;
		return;
	}
  	while (last->next != NULL){
		last = last->next;
  	}
	last->next = new_node;
}

int display_prompt()
{
	printf("sshell$ ");
	return 0;
}

char* get_input()
{
  char* input;
	input = (char*)malloc(MAX_NUM_CHARS + 1);
	fgets(input,MAX_NUM_CHARS + 1,stdin);
	return input;
}


void isExit(node headNode)
{
	if(cmprStr(headNode,"exit") == 0){
		fprintf(stderr, "%s\n","Bye..." );
		exit(0);
	}
}

void isPwd(node headNode)
{
	char cwd[MAX_NUM_CHARS];
	if(cmprStr(headNode,"pwd") == 0){
		getcwd(cwd,sizeof(cwd));
		printf("%s",cwd);
		printf("\n");
		isInterrupt = 1;//to prevent execvp from running pwd
	}

}

void printArrData(node headNode)
{
	//printf("inside printArrData, length: %ld\n",strlen(*(headNode)->arrData));
	for (int i = 0; i < MAX_NUM_ARGS; i++){
		printf("element number %d: %s\n",i,headNode->arrData[i]);
	}
}
void isCD(node headNode)
{
	if(cmprStr(headNode,"cd") == 0){
			isInterrupt = 1;
		if(chdir(headNode->arrData[1]) == 0){
		} else{
				fprintf(stderr, "%s\n","Error: no such directory");
		}
	}
}

void builtinCommands(node headNode)
{
	isExit(headNode);
	isPwd(headNode);
	isCD(headNode);
}

void trimEndNull(char* str)
{
	str[strcspn(str,"\n")] = 0;
} //trims null character at the end of user input

char* inputRedir(char* lineInput, char* delim,node* headNode)
{
		char *fileName;
		char* rest = lineInput;
		strcpy(rest, lineInput);
		char* tokenRedir = strtok_r(rest, delim,&rest);
		int j = 0;

		while(tokenRedir){
			if(j == 0){
				tokenRedir = strtok_r(NULL,delim,&rest);
			} else if(j == 1){
				fileName = tokenRedir;
			} else{
				break;
			}
			j++;
		}//extracts filename from lineInput
		trimLeading(fileName);
		return fileName;
}

void redirSTDIN(char* fileName)
{
	int fd = open(fileName, 0);
	dup2(fd, 0);
	close(fd);
}

void redirSTDOUT(char* fileName)
{
	int fd = open(fileName, O_CREAT| O_TRUNC | O_RDWR, 0644);
	dup2(fd, STDOUT_FILENO);
	close(fd);
}

void printLineInput(char* lineInput)
{
	char*ptr;
	ptr = lineInput;
	while(*ptr!='\0'){
		printf("%s",ptr);
		ptr++;
	}
}

int checkRedirSymbol(char* lineInputCopy)
{
	char*ptr;
	ptr = lineInputCopy;
	while(*ptr!='\0'){
		ptr++;
		if(*ptr=='>'){
			return 1;
		}
		else if(*ptr == '<'){
			return 2;
		}
	}
	return 0;
}


void execSingleCmd(char* lineInput, node headNode)
{
	int status = 0;
	pid_t pid;
	char fileName[512];
	char lineInputCopy[512];

	strcpy(lineInputCopy, lineInput);
	trimEndNull(lineInput);
	strcpy(fileName, inputRedir(lineInput,"<>",&headNode));//if placed after input parse - segfault
	inputParse(lineInput, &headNode);

	builtinCommands(headNode);
	pid = fork();
	if (pid != 0){
		//Parent
		waitpid(-1, &status, 0);
		trimEndNull(lineInputCopy);
		fprintf(stderr, "+ completed '%s' [%d]\n", lineInputCopy, status);
	} else {
		if(isError == 1 || isInterrupt == 1){
			exit(0);
		}//checks for input redirection
		if(checkRedirSymbol(lineInputCopy) == 1){
			redirSTDOUT(fileName);
		}
		else if(checkRedirSymbol(lineInputCopy) == 2){
			redirSTDIN(fileName);
		}
		execvp(*(headNode)->arrData,(headNode)->arrData);
		printf("\nhere is the error: %d\n",(errno));
		perror("execvp");
		exit(1);
	}
}

void pipeParse(char* lineInput, node *headNode)
{
	struct LinkedList* currNode = *headNode;
	char* rest = lineInput;
	char* token = strtok_r(rest, "|",&rest);
	int i = 0;
	while(token){

		while(currNode->next != NULL){
			currNode = currNode->next;
		}
		trimLeading(token);
		append(&currNode,token);//Assigning of lines to the LinkedList
		token = strtok_r(NULL,"|",&rest);
		i++;
	}
	makePipe(i,*headNode, lineInput);
}

void makePipe(int numCmds,node headNode, char* lineInput)
{
	char lineInputCopy[512];
	strcpy(lineInputCopy, lineInput);

	char fileName[512];
	int status;
	int fdPrev[2];//pipe1
	int fdCurr[2];//pipe2
	fdPrev[1] = -1;
	pid_t pid;
	struct LinkedList* currNode = headNode;

	strcpy(fileName, inputRedir(lineInput,"<>",&headNode));
	for (int i = 0; i < numCmds; i++){

	pipe(fdCurr);
	pid = fork();
	if(pid == 0){
		//child
		if(i == 0){
			//first command
			close(fdCurr[0]);
			dup2(fdCurr[1],1);
			//output to pipe
			close(fdCurr[1]);
			execvp(*(currNode)->arrData,(currNode)->arrData);
			fprintf(stderr,"Error: command not found\n");
			exit(1);
		} else if (i == (numCmds-1)){
			//last command
			dup2(fdPrev[0],0);
			close(fdPrev[0]);
			close(fdCurr[0]);
			close(fdCurr[1]);
			execvp(*(currNode)->arrData,(currNode)->arrData);
			fprintf(stderr,"Error: command not found\n");
			exit(1);
		} else {
			//middle commands
			close(fdCurr[0]);
			dup2(fdPrev[0],0);
			dup2(fdCurr[1],1);
			close(fdPrev[0]);
			close(fdCurr[1]);
			execvp(*(currNode)->arrData,(currNode)->arrData);
			fprintf(stderr,"Error: command not found\n");
			exit(1);
		}
	}
	else if (pid < 0){
		perror("fork");
		exit(1);
		//Error
	}
	else {
		//Parent
		close(fdCurr[1]);
		currNode->pid = pid;
	}
	if (currNode->next != NULL){
		currNode = currNode->next;
	}
	if(i != 0){
		close(fdPrev[0]);
	}
	fdPrev[0] = fdCurr[0];
	}
	close(fdCurr[0]);

	for (int j = 0; j < numCmds; j++){
		pid_t gotPid = waitpid(-1, &status, 0);
		for (int x = 0; x < numCmds; x++){
			if(gotPid == (currNode->pid)){
				currNode->status = WEXITSTATUS(status);
			}
		}
	}
	//printing status of the pipe commands
	currNode = headNode;
	trimEndNull(lineInput);

	fprintf(stderr,"+ completed '%s' ",lineInput);
	while(currNode->next!=NULL){
		printf("[%d]", currNode->status);
		currNode = currNode->next;
	}
	printf("\n");
}

int countPipes(char* lineInput)
{
	int counter = 1;
	size_t i = 0;
	char lineInputCopy[MAX_NUM_CHARS+1];
	strcpy(lineInputCopy,lineInput);

	for (;lineInputCopy[i];i++){
		//printf("%c\n",lineInputCopy[i]);
		if(lineInputCopy[i] == '|'){
			counter++;
		}
	}
	return counter;
}

void resetGlobalVars()
{
	isError = 0;
	isInterrupt = 0;
}

void missingCmd(char* cmd){

	if(cmd[0] == '|'||cmd[0] == '<'||cmd[0] == '>'||cmd[0] == '&'){
		isError = 1;
		fprintf(stderr, "Error: missing command\n");
	}
}

int main(int argc, char *argv[])  //first line comment//
{
	char cmd[512];
 	int numCmds = 0;

	while(1){
		resetGlobalVars();//resets isError and isInterrupt
		node headNode = createNode();
		char *nl;
		printf("sshell$ ");
		fflush(stdout);
		/* Get command line */
		fgets(cmd, MAX_NUM_CHARS, stdin);
		/* Print command line if we're not getting stdin from the
		 * terminal */
		if (!isatty(STDIN_FILENO)) {
			printf("%s", cmd);
			fflush(stdout);
		}

		/* Remove trailing newline from command line */
		nl = strchr(cmd, '\n');
		if (nl)
			*nl = '\0';
		numCmds = countPipes(cmd);
		missingCmd(cmd);
		if(isError == 1 || isInterrupt == 1){
		}
		else{
			if(numCmds == 1){
				execSingleCmd(cmd,headNode);
			}
			else{
				pipeParse(cmd,&headNode);
				}
			}
	}
	return EXIT_SUCCESS;
}
