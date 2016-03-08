#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

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
int sendrecieve(char buf[],int type,int sfd)
{
	int flag=1;
	while(flag)
	{	
		char recbuf[2000];
		if(send(sfd, buf,2000, 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
		if(type==3)
		{
			sendrecieve(".",1,sfd);
			break;
		}
		if (recv (sfd, recbuf, 2000, 0) == -1)
		{
			perror ("Client: Receive failed");
			exit (1);
		}


		if(strcmp(recbuf,"ok")==0)
			flag=0;
		else if (strcmp(recbuf,"new")==0)
		{
			printf("looks like it is your first time connecting to our server. Enter a Password :D\n");
			scanf("%s",buf);
		}
		else if(strcmp(recbuf,'354 start mail input')==0)
			return 1;
	}
return 0;
}

void smtpClient(int sfd)
{
	char A[100], B[100],body[1000],buf[2000];
	if (recv (sfd, buf, 2000, 0) == -1)
		{
			perror ("Client: Receive failed");
			exit (1);
		}
	memset(buf,0,2000);
	strcpy(buf,"Halo");
	if(send(sfd, buf,2000, 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
	if (recv (sfd, buf, 2000, 0) == -1)
	{
		perror ("Client: Receive failed");
		exit (1);
	}
	printf("Mail from?\n");
	scanf("%s\n", A);
	sprintf(A,"MAIL FROM: %s",A);
	sendrecieve(A,1,sfd);
	printf("Mail to?\n");
	scanf("%s\n", B);
	sprintf(B,"RCPT TO: %s",B);

	sendrecieve(B,1,sfd);
	
	int flag=1;
	char data[1000];
	if(sendrecieve("DATA",2,sfd))
		scanf("%d",&data);
	sendrecieve(data,3,sfd);
	
	if(send(sfd, "Quit",2000, 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
	if (recv (sfd, buf, 2000, 0) == -1)
	{
		perror ("Client: Receive failed");
		exit (1);
	}
	
	
}

void popsend(char buf[],int sfd)
{
	if(send(sfd, buf,2000, 0) == -1)
	{
		perror("Server write failed");
		exit(1);
	}
	if (recv (sfd, buf, 1000, 0) == -1)
	{
		perror ("Client: Receive failed");
		exit (1);
	}
	if(strcmp(buf,"ok"))
	{
		printf("%s\n",buf );
	}
}

void popClient(int sfd)
{
	char A[100], B[100],buf[2000];

	printf("Username\n");
	scanf("%s\n", A);
	sprintf(A,"USER: %s",A);
	popsend(A,sfd);
	printf("Password\n");
	scanf("%s\n", B);
	sprintf(B,"PASS: %s",B);
	popsend(B,sfd);
	popsend("LIST",sfd);
	// sprintf(buf,"%s,%s",A,B);
	int flag=1;
	// int flag2=0;
	while(flag)
	{

		if(send(sfd, A,100, 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
		if (recv (sfd, buf, 1000, 0) == -1)
		{
			perror ("Client: Receive failed");
			exit (1);
		}
		printf("Enter 1 to read more and 2 to exit\n");
		int exitnow;
		scanf("%s\n",&exitnow);
		if(exitnow)
			flag=0;

	}
	
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
