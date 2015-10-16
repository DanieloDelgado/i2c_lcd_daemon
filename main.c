#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdbool.h>

#define BAD_SOCKET(s)	((s) == -1)
#define CLIENTS 5

static int filesock(char *filename)
{
    struct sockaddr_un addr;
    int sock;

    if (BAD_SOCKET(sock = socket(AF_UNIX, SOCK_STREAM, 0))) {
		return -1;
    }
	int a =  strlen(filename);
    if (strlen(filename) < sizeof(addr.sun_path))
		strcpy(addr.sun_path, filename);
	else
		return -1;
    addr.sun_family = (sa_family_t)AF_UNIX;
    
    if (bind(sock, (struct sockaddr *)&addr, (int)sizeof(addr)) < 0) {
			close(sock);
	return -1;
    }
    if (listen(sock, CLIENTS) == -1) {
		close(sock);
		unlink(filename);
		return -1;
    }

    return sock;
}

static void usage(char *argv[])
{
	printf("usage: %s [-d]\n\
	Options include: \n\
	-d		= run as daemon\n",
	argv[0]);
}

int main(int argc, char *argv[]){
	int socket_i2c;
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
	char *filename = "./i2c.socket";
	socket_i2c = filesock(filename);
	int a=0;
	while (a< 5){
		sleep (5);
		a++;
	}
	close(socket_i2c);
	unlink(filename);
	return 0;
}
