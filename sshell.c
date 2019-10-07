#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

/*
int display_prompt(){
	
	printf("sshell$ ");
	return 0;
}

int read_command(char** command){
	
	return 0;
}
*/
int main(int argc, char *argv[])  //first line comment//
{
	char **command	= NULL;
	int status = 0;
	pid_t pid;
	while(1){
			//display_prompt();
			//read_command(command);
			
			pid = fork(); 	
			if (pid != 0){
				/*Parent*/
				waitpid(-1, &status, 0);
				printf("inside parent ");
			} else { //FIND OUT HOW TO ENTER THIS BRANCH!
				printf("inside child");	
				execv(command[0],command);
				perror("execv");
				exit(1);
				fprintf(stderr, "+ completed '%s' [%d]\n", command[0], status);
			}
	}
	
  	return EXIT_SUCCESS;
}
