#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main()
{
	int sfd, cfd;
	char buf[1000] = {'\0'}; 
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
	srv_addr.sin_port   = htons(21435);

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

	if(send(sfd, "Reader", 6, 0) == -1)
	{
		perror("Server write failed");
		exit(1);
	}
}