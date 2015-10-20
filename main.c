#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdbool.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <syslog.h>
#include <fcntl.h>

#define BAD_SOCKET(s)	((s) == -1)
#define CLIENTS 5

int send_string(char *i2cdev, char *text)
{
    int file;
    int size = strlen(text) + 1; //Includes \0
    const int addr = 0x48;


    if ((file = open(i2cdev,O_RDWR)) < 0) {
	    syslog(LOG_ERR, "Failed to open the i2c bus.");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        return 1;
    }

    if (ioctl(file,I2C_SLAVE,addr) < 0) {
	    syslog(LOG_ERR, "Failed to acquire i2c bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        return 1;
    }
    if (write(file, text, size) != size) {
        /* ERROR HANDLING: i2c transaction failed */
        syslog(LOG_ERR, "Failed to write to the i2c bus.\n");
        printf("\n\n");
        return 1;
    }
    return 0;
}

static int server(int client_socket_fd, char *i2cdev)
{	
/*	The write socket should be limited to strings no bigger than buffer size*/
	char buffer[256];
	int retval;

	do{
		retval = read(client_socket_fd, buffer, 256);
		if (retval > 0){
				if(send_string(i2cdev,buffer) != 0)
					return -1;
		}
		else if (retval < 0)
			return -1;
	} while (retval > 0);

	return 0;
}
static int filesock(char *socket_file)
{
    struct sockaddr_un addr;
    int sock;

    if (BAD_SOCKET(sock = socket(AF_UNIX, SOCK_STREAM, 0))) {
		return -1;
    }
	int a =  strlen(socket_file);
    if (strlen(socket_file) < sizeof(addr.sun_path))
		strcpy(addr.sun_path, socket_file);
	else
		return -1;
    addr.sun_family = (sa_family_t)AF_UNIX;
    
    if (bind(sock, (struct sockaddr *)&addr, (int)sizeof(addr)) < 0) {
			close(sock);
	return -1;
    }
    if (listen(sock, CLIENTS) == -1) {
		close(sock);
		unlink(socket_file);
		return -1;
    }

    return sock;	
}

static void usage(char *argv[])
{
	printf("usage: %s [-b i2cbus] [-d] [-f sockfile]\n\
	Options include: \n\
	-b i2cbus	= /dev/i2c-# [default = 1]\n\
	-d			= run as daemon\n\
	-f socket_file = specify socket location\n",
	argv[0]);
}

int main(int argc, char *argv[]){
	int socket_i2c;
	int i2cbus = 1;
	pid_t pid = getpid();
	bool daemonize = false;
	char *socket_file = NULL;
	int option;
    char i2cdev[15];
    
	while ((option = getopt(argc, argv, "b:df:h?")) != -1){
		switch (option){
			case 'b':
				i2cbus = atoi(optarg);
			case 'd':
				daemonize = true;
				break;
			case 'f':
				socket_file = optarg;
				break;
			case 'h':
			case '?':
			default:
				usage(argv);
				exit(EXIT_SUCCESS);
		}
	}

	snprintf(i2cdev, sizeof(socket_file),"/dev/i2c-%d", i2cbus);
	
	if (socket_file == NULL)
		socket_file = "./i2c.socket";

	if ((socket_i2c = filesock(socket_file)) == -1)
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
		server(client_socket_fd, i2cdev);
		close (client_socket_fd);
		a++;
	}
	close(socket_i2c);
	unlink(socket_file);
	return 0;
}
