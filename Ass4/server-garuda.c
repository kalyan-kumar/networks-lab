#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 21634

int makeNode(int port, int cors)
{
	int sfd, reuse_addr = 1;
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd==-1)
	{
		perror("Server: socket error");
		exit(1);
	}
	printf("Socket fd = %d\n", sfd);
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr,sizeof(reuse_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port   = htons(port);
	if(inet_pton(AF_INET, "127.0.0.1", &srv_addr.sin_addr) <= 0)
	{
		perror("Network address conversion.\n");
		exit(1);
	}
	if(bind(sfd, (struct sockaddr *) &srv_addr, addrlen) == -1)
	{
		perror("Server: Bind failed");
		exit(1);
	}
	if(cors==0)
	{
		printf("Max connections allowed to wait: %d\n", 15); // Hard-coded 15 here 
		if(listen(sfd, 15) == -1)
		{
			perror("Server: Listen failed");
			exit(1);
		}
	}
	else
	{
		if(connect(sfd, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_in)) == -1)
		{
			perror("Client: Connect failed.");
			exit(1);
		}
	}
	return sfd;
}

void smtpServer()
{
	int sfd = makeNode(21434, 0);
	
}

void smtpClient()
{
	int sfd = makeNode(21634, 1);
}

void popClient()
{
	int sfd = makeNode(21834, 1);
}

int main(int argc, char* argv[])
{
	pid_t s, c;
	s = fork();
	if(s<0)
	{
		perror("Fork error");
		exit(1);
	}
	else if(s==0)
		smtpServer();
	else
	{
		c = fork();
		if(c<0)
		{
			perror("Fork error");
			exit(1);
		}
		else if(c==0)
			smtpClient();
		else
			popClient();
	}
	return 0;	
}