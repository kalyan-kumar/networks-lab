#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 21634

int makeNode(const char* src, int port, int cors)
{
	int sfd, reuse_addr = 1;
	struct sockaddr_in srv_addr;
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
	if(inet_pton(AF_INET, src, &srv_addr.sin_addr) <= 0)
	{
		perror("Network address conversion.\n");
		exit(1);
	}
	if(cors==0)
	{
		if(bind(sfd, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_in)) == -1)
		{
			perror("Server: Bind failed");
			exit(1);
		}
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

void smtpClient()
{
	// int sfd = makeNode(21634, 1);
}

void smtpServer(char* src)
{
	int sfd = makeNode(src, 21434, 0), cfd, flag;
	struct sockaddr_in cli_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	memset(&cli_addr, 0, addrlen);
	pid_t child;
	while(1)
	{
		cfd = accept(sfd, (struct sockaddr *) &cli_addr, &addrlen);
		if(cfd==-1)
		{
			perror("Server: Accept failed");
			exit(1);
		}
		child = fork();
		if(child<0)
		{
			perror("Fork error");
			exit(1);
		}
		else if(child==0)
		{
			char buf[1000];
			if(send(cfd, "220 service ready", 17, 0)==-1)
			{
				perror("Server write failed");
				exit(1);
			}
			memset(buf, 0, 1000);		// HELO: server
			if(recv(cfd, buf, 1000, 0) == -1)
			{
				printf("Didnt get what the client requested for\n");
				exit(1);
			}
			printf("Client says - %s\n", buf);
			if(send(cfd, "250 OK", 6, 0)==-1)
			{
				perror("Server write failed");
				exit(1);
			}
			


			memset(buf, 0, 1000);		// MAIL FROM: email
			if(recv(cfd, buf, 1000, 0) == -1)
			{
				printf("Didnt get what the client requested for\n");
				exit(1);
			}
			printf("Client says - %s\n", buf);
			if(send(cfd, "250 OK", 6, 0)==-1)
			{
				perror("Server write failed");
				exit(1);
			}
			memset(buf, 0, 1000);		// RCPT TO: email
			if(recv(cfd, buf, 1000, 0) == -1)
			{
				printf("Didnt get what the client requested for\n");
				exit(1);
			}
			if(strcmp(src, strstr(buf, "@")+1) != 0)
				flag=1;
			else
				flag=0;
			printf("Client says - %s\n", buf);
			if(send(cfd, "250 OK", 6, 0)==-1)
			{
				perror("Server write failed");
				exit(1);
			}



			memset(buf, 0, 1000);		// DATA
			if(recv(cfd, buf, 1000, 0) == -1)
			{
				printf("Didnt get what the client requested for\n");
				exit(1);
			}
			printf("Client says - %s\n", buf);
			if(send(cfd, "354 start mail input", 20, 0)==-1)
			{
				perror("Server write failed");
				exit(1);
			}
			memset(buf, 0, 1000);		// body
			if(recv(cfd, buf, 1000, 0) == -1)
			{
				printf("Didnt get what the client requested for\n");
				exit(1);
			}
			printf("Client says - %s\n", buf);
			if(send(cfd, "250 OK", 6, 0)==-1)
			{
				perror("Server write failed");
				exit(1);
			}



			memset(buf, 0, 1000);		// QUIT
			if(recv(cfd, buf, 1000, 0) == -1)
			{
				printf("Didnt get what the client requested for\n");
				exit(1);
			}
			printf("Client says - %s\n", buf);
			if(send(cfd, "221 service closed", 6, 0)==-1)
			{
				perror("Server write failed");
				exit(1);
			}
		}
	}
}

void popServer(char* src)
{
	int sfd = makeNode(src, 21634, 0);
	struct sockaddr_in cli_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	memset(&cli_addr, 0, addrlen);
	pid_t child;
	while(1)
	{
		cfd = accept(sfd, (struct sockaddr *) &cli_addr, &addrlen);
		if(cfd==-1)
		{
			perror("Server: Accept failed");
			exit(1);
		}
		child = fork();
		if(child<0)
		{
			perror("Fork error");
			exit(1);
		}
		else if(child==0)
		{
			char buf[1000];
			memset(buf, 0, 1000);
			if(recv(cfd, buf, 1000, 0) == -1)
			{
				printf("Didnt get what the client requested for\n");
				exit(1);
			}
			printf("username - %s\n", buf);
			if(send(cfd, "OK", 2, 0)==-1)
			{
				perror("Server write failed");
				exit(1);
			}
			memset(buf, 0, 1000);
			if(recv(cfd, buf, 1000, 0) == -1)
			{
				printf("Didnt get what the client requested for\n");
				exit(1);
			}
			printf("password - %s\n", buf);
			if(send(cfd, "OK", 2, 0)==-1)
			{
				perror("Server write failed");
				exit(1);
			}
			
		}
	}
}

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		printf("Run the executable in <exec-path> <IP-of-server> format\n");
		return 0;
	}
	char A[100];
	strcpy(A, argv[1]);
	pid_t s;
	s = fork();
	if(s<0)
	{
		perror("Fork error");
		exit(1);
	}
	else if(s==0)
		smtpServer(A);
	else
		popServer(A);
	return 0;	
}