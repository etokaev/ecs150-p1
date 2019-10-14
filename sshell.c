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
//char* checkRedir[10];

struct LinkedList{
	char* arrData[MAX_NUM_ARGS];
	struct LinkedList *next;
	//char filename[512];
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
				//found tokenf
				printf("found token: %d\n",i);
				return i;
		}
		i++;
	}

	return -1;
}


node addNode(node head){
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


void inputParse(char *lineInput, node *headNode){ // ANY NODE works

	char* token1 = strtok(lineInput, " ");
	int i = 0;
	while(token1){
		if(i > 15){
			fprintf(stderr, "%s\n","Error: too many process arguments" );
			isError = 1;
			//exit(0);
		}
		//printf("%s\n",token);
		(*headNode)->arrData[i] = token1;
		//printf("%s\n",(*headNode)->arrData[i]);
		i++;
		token1 = strtok(NULL," ");
	}

	//return headNode;
}


void append(struct LinkedList** head,char* cmd){

  struct LinkedList* new_node = createNode();
  struct LinkedList *last = *head;
  //printf("tokens to assign to the LinkedList: %s\n",cmd);
  //check for redirect and save to filename if there is redirect
  //(strcpy(head->filename, inputRedir(lineInput,"<>");//FIXME after no redir pipe works
  inputParse(cmd,head);
  new_node->next = NULL;
  //printf("head->arrData[0]: %s\n", (*head)->arrData[0]);

  if(*head == NULL){
    *head = new_node;
    return;
  }
  while (last->next != NULL){
    last = last->next;
  }
  last->next = new_node;
  //inputParse inside
}

int pipeParse(char* lineInput, node *headNode){

	struct LinkedList* currNode = *headNode;
	char* rest = lineInput;
	char* token = strtok_r(rest, "|",&rest);
	int i = 0;
	while(token){
		//addNode(*headNode);
		while(currNode->next != NULL){
			currNode = currNode->next;
		}
		//printf("Pipe#%d: %s\n",i+1,token);

		append(&currNode,token);//Assigning of lines to the LinkedList
    // printf("headNode->arrData[0]: %s\n", (*headNode)->arrData[0]);
    // printf("headNode->next->arrData[0]: %s\n", (*headNode)->next->arrData[0]);
		//inputParse(token, headNode);//inserting parsed commands into LinkedList
		token = strtok_r(NULL,"|",&rest);
		i++;

	}
  return i;//num of cmdsW
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

void trimEndNull(char* str){
	str[strcspn(str,"\n")] = 0;
} //trims null character at the end of user input


void trimLeading(char * str)
{
    int index, i;

    index = 0;

    /* Find last index of whitespace character */
    while(str[index] == ' ' || str[index] == '\t' || str[index] == '\n')
    {
        index++;
    }


    if(index != 0)
    {
        /* Shit all trailing characters to its left */
        i = 0;
        while(str[i + index] != '\0')
        {
            str[i] = str[i + index];
            i++;
        }
        str[i] = '\0'; // Make sure that string is NULL terminated
    }
}//FIXME https://codeforwin.org/2016/04/c-program-to-trim-leading-white-spaces-in-string.html



char* inputRedir(char* lineInput, char* delim){

		char* tokenRedir = strtok(lineInput, delim);
		int j = 0;
		//char *cmdStr;
		char *fileName;
		while(tokenRedir){

			if(j == 0){
				//cmdStr = tokenRedir;
				tokenRedir = strtok(NULL,delim);
			}else if(j == 1){
				fileName = tokenRedir;
			}else{
				break;
			}
			j++;
			//printf("FileName is %s\n", fileName);

		}//extracts filename from lineInput
		trimLeading(fileName);
		return fileName;
}
void redirSTDIN(char* fileName){

	int fd = open(fileName, 0);
	dup2(fd, 0);
	close(fd);
}

void redirSTDOUT(char* fileName){

	int fd = open(fileName, O_RDWR);
	dup2(fd, STDOUT_FILENO);
	close(fd);
}

int checkRedirSymbol(char* lineInputCopy){

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
	//printf("\n");
	//char* redirSymbol;
	//redirSymbol = strchr(lineInputCopy,'<');
	//printf("RedirSymbol is: %s\n",redirSymbol);
	// if(redirSymbol != NULL){
	// 	return 1;
	// 	}
	return 0;

}


void execSingleCmd(char* lineInput, node headNode){
	printf("exec single cmd\n");
}

void makePipe(int numCmds,node headNode, char* lineInput){

	if(numCmds == 1){
		execSingleCmd(lineInput, headNode);
		return;
	}

	//int status;
	int fdPrev[2];
	int fdCurr[2];
	fdPrev[0] = -1;

	pid_t pid;
	struct LinkedList* currNode = headNode;


  for (int i = 0; i < numCmds; i++){

		//FIXME for each cmds iterate through the currNode;
    pipe(fdCurr);
    pid = fork();
    if(pid == 0){
			//child
			if(i == 0){
				//first cmd
				//check for redirect input
				//redirSTDIN("file");
				close(fdCurr[0]);
				dup2(fdCurr[1],STDOUT_FILENO);
				close(fdCurr[1]);
				execvp(*(currNode)->arrData,(currNode)->arrData);
				fprintf(stderr,"Error: command not found\n");
				exit(1);
			}
			else if (i == (numCmds-1)){
				//last cmd
				//check for redirect output
				//redirSTDOUT("file");
				dup2(fdPrev[0],STDOUT_FILENO);
				close(fdPrev[0]);
				close(fdCurr[0]);
				close(fdCurr[1]);
				execvp(*(currNode)->arrData,(currNode)->arrData);
				fprintf(stderr,"Error: command not found\n");
				exit(1);
			}
			else {
				close(fdCurr[0]);
				dup2(fdPrev[0],STDIN_FILENO);
				dup2(fdCurr[1],STDOUT_FILENO);
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
			//take care of PID if have time

		}
		if (currNode->next != NULL){
			printf("node check: (%s)\n", currNode->arrData[0]);
			currNode = currNode->next;
		}
		/*
		if( i != 0){
			for (int i = 0; i < numCmds; i++)
		}
		*/

}

}


int main(int argc, char *argv[])  //first line comment//
{

	int status = 0;
	pid_t pid;
  int numCmds;

	while(1){

			node headNode = createNode();

			display_prompt();
			char *lineInput = get_input();
			char fileName[512];
			char lineInputCopy[512];
			strcpy(lineInputCopy, lineInput);
			numCmds = pipeParse(lineInput,&headNode);
			printf("\n\n%d\n",numCmds);
      makePipe(numCmds,headNode,lineInput);


      // printf("number of commands is %d\n", numCmds);
      // printf("headNode->arrData[0]: %s\n", (headNode)->arrData[0]);
      // printf("headNode->next->arrData[0]: %s\n", (headNode)->next->arrData[0]);
      // printf("headNode->next->next->arrData[0]: %s\n", (headNode)->next->next->arrData[0]);
			trimEndNull(lineInput);
			strcpy(fileName, inputRedir(lineInput,"<>"));
			//inputParse(lineInput, &headNode); //prepped a single cmd to exec prior to piping

			builtinCommands(headNode);


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

					//checks for input redirection
					if(checkRedirSymbol(lineInputCopy) == 1){
						redirSTDOUT(fileName);
					}
					else if(checkRedirSymbol(lineInputCopy) == 2){
						redirSTDIN(fileName);
					}
					printf("*(headNode)->arrData: %s\n,(headNode)->arrData): %s\n",*(headNode)->arrData,(headNode)->arrData[1]);
					//execvp(*(headNode)->arrData,(headNode)->arrData);
					printf("\n here is the error: %d\n",(errno));
					//execv(command[0],command);
					perror("execvp");
					exit(1);
				  //fprintf(stderr, "+ completed '%s' [%d]\n", "ls", status);
			}
	}

  	return EXIT_SUCCESS;
}
