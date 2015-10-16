#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char *argv[]){
	pid_t pid = getpid();
	printf("%d\n",pid);
	if(daemon(0,0) != 0)
		exit(1);
	while (1){
	sleep (1);
	}
	return 0;
}
