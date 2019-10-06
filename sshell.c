#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])  //first line comment//
{
	pid_t pid;
	char *cmd[4] ={"/bin/date","-u",NULL};
	//int retval;
	int status;
        pid = fork(); 	
        if (pid == 0){
		/*Child*/
		execvp(cmd[0],cmd);
		perror("execvp");
		exit(1);
	} else if (pid>0){
		/*Parent*/
		waitpid(-1, &status, 0);
		//printif("Child exited with status: %d\n", WEXITSTATUS(status));
		fprintf(stderr, "+ completed '%s' [%d]\n", cmd[0], status);
	} else {
		perror("fork");
		exit(1);

	}
	//retval = system(cmd);
        //fprintf(stdout, "Return status value for '%s': %d\n", cmd[0], retval);

	return EXIT_SUCCESS;
}
