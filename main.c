#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>

static void usage(char *argv[])
{
	printf("usage: %s [-d]\n\
	Options include: \n\
	-d		= run as daemon\n",
	argv[0]);
}

int main(int argc, char *argv[]){
	pid_t pid = getpid();
	bool daemonize = false;
	int option;
	while ((option = getopt(argc, argv, "d")) != -1){
		switch (option){
			case 'd':
				daemonize = true;
				break;
			case 'h':
			case '?':
			default:
				usage(argv);
				exit(EXIT_SUCCESS);
		}
	}
	if (daemonize){
		if(daemon(0,0) != 0)
			exit(1);
	}

	int a=0;
	while (a< 5){
		sleep (1);
		a++;
	}
	return 0;
}
