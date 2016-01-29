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
	int master_socket, cfd;
	struct sockaddr_in srv_addr, cli_addr;
	char buf[1000];
	socklen_t addrlen = sizeof(struct sockaddr_in);
	fd_set rd, wr, er;
	struct timeval tm;
	memset(&srv_addr, 0, addrlen);
	FD_ZERO(&rd);
	FD_ZERO(&wr);
	FD_ZERO(&er);
	tm.tv_sec = 10;
	tm.tv_usec = 0;

	master_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(master_socket == -1)
	{
		perror("Server: Socket error");
		exit(1);
	}
	printf("Socket FD = %d\n", master_socket);

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port   = htons(21435);
	if(inet_pton(AF_INET, "127.0.0.1", &srv_addr.sin_addr) <= 0)
	{
		perror("Error in Network address conversion");
		exit(1);
	}

	if(bind(master_socket, (struct sockaddr *) &srv_addr, addrlen) == -1)
	{
		perror("Server: Bind failed");
		exit(1);
	}

	printf("Max connections allowed to wait: %d\n", 15); // Hard-coded 15 here 
	if(listen(master_socket, 15) == -1)
	{
		perror("Server: Listen failed");
		exit(1);
	}

	while(1)
	{
		FD_SET(master_socket, &rd);
		if(select(master_socket + 1, &rd, &wr, &er, &tm)<0)
		{
			perror("Select error");
			exit(1);
		}

		if(FD_ISSET(master_socket, &rd))
		{
			cfd = accept(master_socket, (struct sockaddr *) &cli_addr, &addrlen);
			if(cfd==-1)
			{
				perror("Server: Accept failed");
				exit(1);
			}
			recv(cfd, buf, 1000, 0);
			printf("Client type - %s\n", buf);
		}
	}

	if(close(master_socket)==-1)
	{
		perror("Server close failed");
		exit(1);
	}
	printf("THE END\n");
	return 0;
}