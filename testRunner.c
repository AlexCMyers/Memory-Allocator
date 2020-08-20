/* Alexandra Myers
 * leakcount.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	int i = 0;
	char* arguments[3];
	char num[10];
	//uses a shim I made to pass as environment variable later on
	char *const environment[] = {"LD_PRELOAD=./libmyalloc.so", NULL};
	arguments[2] = NULL;
	
	arguments[0] = argv[1];
	for(i = 0; i < 1000; i++){
		sprintf(num, "%d", i);
		arguments[1] = num;
	//	printf("%d\n", i);	
	
	//fork, child goes into if block
		if(fork() == 0){
			execve(argv[1], arguments, environment);
		}
		wait(NULL);
	}
	//waiting for all child processes to finish

	return 0;
}


