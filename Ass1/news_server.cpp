#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>


int main(int argc, char **argv)
{
	int sfd, cfd;
	pid_t pidc;
	char buf[1000];
	struct sockaddr_in srv_addr, cli_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	memset(&srv_addr, 0, addrlen);
	memset(&cli_addr, 0, addrlen);

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd==-1)
	{
		perror("Server: socket error");
		exit(1);
	}
	printf("Socket fd = %d\n", sfd);
	
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port   = htons(21436);
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

	printf("Max connections allowed to wait: %d\n", 15); // Hard-coded 15 here 
	if(listen(sfd, 15) == -1)
	{
		perror("Server: Listen failed");
		exit(1);
	}

	while(1)
	{
		cfd = accept(sfd, (struct sockaddr *) &cli_addr, &addrlen);
		if(cfd==-1)
		{
			perror("Server: Accept failed");
			exit(1);
		}

		if(recv(cfd, buf, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
		printf("Client asked for %s\n", buf);


		pidc = fork();
		if(pidc < 0)
		{
			perror("Fork error");
			exit(1);
		}
		else if(pidc==0)	// Child Process
		{
			if(send(cfd, "put the message you want to send here", 37, 0) == -1)
				perror("Server write failed");
			exit(1);
		}
		else				// Parent Process
		{
			continue;
		}
		
	}

	if(close(sfd)==-1)
	{
		perror("Server close failed");
		exit(1);
	}
	printf("THE END\n");

	return 0;
}