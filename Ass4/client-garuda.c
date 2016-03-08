#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 21634

int makeNode(const char* src, int port)
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
	if(connect(sfd, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_in)) == -1)
	{
		perror("Client: Connect failed.");
		exit(1);
	}
	return sfd;
}

void smtpClient(int sfd)
{
	char A[100], B[100];
	printf("Mail from?\n");
	scanf("%s\n", A);
	printf("Mail to?\n");
	scanf("%s\n", B);
	
}

void popClient(int sfd)
{
	char A[100], B[100];
	printf("Username\n");
	scanf("%s\n", A);
	printf("Password\n");
	scanf("%s\n", B);
	
}

int main(int argc, char* argv[])
{
	int a, sfd = makeNode("127.0.0.1", 21834);
	while(1)
	{
		printf("Do you want to (1) send or (2) receive emails?\n");
		scanf("%d", &a);
		if(a==1)
			smtpClient(sfd);
		else
			popClient(sfd);
	}
	return 0;
}
