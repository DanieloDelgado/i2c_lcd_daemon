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

static int server(int client_socket_fd)
{	
/*	The write socket should be limited to strings no bigger than buffer size*/
	char buffer[256];
	int retval;

	do{
		retval = read(client_socket_fd, buffer, 256);
		if (retval > 0)
			;			/*Do something with i2c*/
		else if (retval < 0)
			return -1;
	} while (retval > 0);

	return 0;
}
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
	printf("usage: %s [-d] [-f sockfile]\n\
	Options include: \n\
	-d			= run as daemon\n\
	-f sockfile = specify socket location",
	argv[0]);
}

int main(int argc, char *argv[]){
	int socket_i2c;
	pid_t pid = getpid();
	bool daemonize = false;
	char *filename = NULL;
	char buffer[3];
	buffer[0]='a';
	int option;
	while ((option = getopt(argc, argv, "df:")) != -1){
		switch (option){
			case 'd':
				daemonize = true;
				break;
			case 'f':
				filename = optarg;
				break;
			case 'h':
			case '?':
			default:
				usage(argv);
				exit(EXIT_SUCCESS);
		}
	}

	if (filename == NULL)
		filename = "./i2c.socket";

	if ((socket_i2c = filesock(filename)) == -1)
		exit(1);

	if (daemonize){
		if(daemon(0,0) != 0)
			exit(1);
	}

	int a=0;
	while (a< 5){

		struct sockaddr_un client_name;
		socklen_t client_name_len;
		int client_socket_fd;

		client_socket_fd = accept (socket_i2c, (struct sockaddr *)&client_name, &client_name_len);
		if (client_socket_fd < 0)
			continue;
		/* Handle the connection. */
/*		server (client_socket_fd);*/
		/* Close our end of the connection. */
		close (client_socket_fd);
		a++;
	}
	close(socket_i2c);
	unlink(filename);
	return 0;
}
