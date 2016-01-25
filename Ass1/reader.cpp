#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
	int sfd, cfd;
	struct sockaddr_in srv_addr, cli_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	memset(&srv_addr, 0, addrlen);

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd == -1)
	{
		perror("Clent: socket error");
		exit(1);
	}
	printf("Socket fd=%d\n", sfd);

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port   = htons(21436);

	if(inet_pton(AF_INET, "127.0.0.1", &srv_addr.sin_addr) <= 0)
	{
		perror("IP address Conversion error.\n");
		exit(1);
	}

	if(connect(sfd, (struct sockaddr *) &srv_addr, addrlen) == -1)
	{
		perror("Client: Connect failed.");
		exit(1);
	}

	if(send(sfd, "Get me academic news", 20, 0) == -1)
	{
		perror("Server write failed");
		exit(1);
	}

	char buf[1000] = {'\0'}; 
    
    if (recv (sfd, buf, 1000, 0) == -1)
    {
        perror ("Client: Receive failed");
        exit (1);
    }
    
    printf ("Received message = |%s|\n", buf);

    if (close(sfd) == -1)
    {
        perror ("Client close failed");
        exit (1);
    }

}