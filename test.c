#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//char *socket_path = "./socket";


static void usage(char *argv[])
{
	printf("usage: %s [-f sockfile] TEXT \n\
	Options include: \n\
	-f sockfile = specify socket location\n",
	argv[0]);
}

int main(int argc, char *argv[]) {
	struct sockaddr_un addr;
	char *socket_path;
	char *text;
	char buf[100];

	int socket_fd,rc;
	
	int c;
	while ((c = getopt (argc, argv, "s:h?")) != -1){
		switch (c){
			case 'f':
				socket_path = optarg;
				break;
			case '?':
			case 'h':
			default:
				usage(argv);
		        exit(EXIT_SUCCESS);
		}
	}
	
	if (socket_path == NULL)
		socket_path = "./i2c.socket";
	
	
	if (optind < argc)
		text=argv[optind];
	else{
		usage(argv);
		exit(EXIT_SUCCESS);
	}

	if ( (socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		exit(-1);
	}

	if (strlen(socket_path) < sizeof(addr.sun_path))
		strcpy(addr.sun_path, socket_path);
	else
		exit(-1);
	addr.sun_family = AF_UNIX;

	if (connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("connect error");
	exit(-1);
	}
	if (write(socket_fd, text, strlen(text)+1) != strlen(text) + 1)
		printf("String too long\n");
	return 0;
}
