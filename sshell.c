#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int display_prompt(){

	printf("sshell$ ");
	return 0;
}


char* get_input(){

  char* input;
	input = (char*)malloc(513);
	fgets(input,513,stdin);
	return input;
}

void trimString(char* str){
	str[strcspn(str,"\n")] = 0;
} //trims null character at the end of user input

int main(int argc, char *argv[])  //first line comment//
{
	//char **command;

	int status = 0;
	pid_t pid;
	while(1){
			display_prompt();
			char* lineInput = get_input();
			trimString(lineInput);
			char *cmd[] = {lineInput,NULL};

			//read_command(command);
			pid = fork();
			if (pid != 0){
				/*Parent*/
				waitpid(-1, &status, 0);
				fprintf(stderr, "+ completed '%s' [%d]\n", "ls", status);
				//exit(0);
			} else { //FIND OUT why not printing inside child
					execvp(cmd[0],cmd);

					//execv(command[0],command);
					perror("execvp");
					exit(1);
				//fprintf(stderr, "+ completed '%s' [%d]\n", "ls", status);
			}
	}

  	return EXIT_SUCCESS;
}
