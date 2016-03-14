#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 21635
#define POPPORT 21835

int makeNode(const char* src, int port)
{
	int sfd, reuse_addr = 1;
	struct sockaddr_in srv_addr;
	printf("%s\n",src );
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
		if(send(sfd, buf,strlen(buf), 0) == -1)
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
		printf("%s\n", recbuf);

		if(strcmp(recbuf,"250 OK")==0)
			flag=0;
		else if (strcmp(recbuf,"new")==0)
		{
			printf("looks like it is your first time connecting to our server. Enter a Password :D\n");
			scanf("%s",buf);
		}
		else if(strcmp(recbuf,"354 start mail input")==0)
			return 1;
	}
return 0;
}

void smtpClient(char *ip)
{
	char A[100], B[100],body[1000],buf[2000];
	int sfd = makeNode(ip, PORT);
	if (recv (sfd, buf, 2000, 0) == -1)
		{
			perror ("Client: Receive failed");
			exit (1);
		}
	memset(buf,0,2000);
	strcpy(buf,"HELO");
	if(send(sfd, buf,strlen(buf), 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
	if (recv (sfd, buf, 2000, 0) == -1)
	{
		perror ("Client: Receive failed");
		exit (1);
	}
	printf("%s\n",buf );

	memset(buf,0,2000);
	// sleep(1);
	printf("Mail from?\n");
	scanf("%s", A);
	char user[100];
	sprintf(user,"MAIL FROM: %s",A);
	printf("%s\n",user );
	
	printf("here\n");	
	sendrecieve(user,1,sfd);
	printf("Mail to?\n");
	scanf("%s", B);
	memset(user,0,100);
	sprintf(user,"RCPT TO: %s",B);

	sendrecieve(user,1,sfd);
	printf("enter data\n");
	int flag=1;
	char data[1000];
	if(sendrecieve("DATA",2,sfd));
	memset(data,0,1000);
	fgets(data,1000,stdin);
	while(1)
	{	
		memset(data,0,1000);
		fgets(data,1000,stdin);

		if(send(sfd, data,strlen(data), 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
		printf("%s\n",data );
		sleep(1);
		if(strcmp(data,".\n")==0)
			break;
 	}
 	printf("sent all\n");
 	if (recv (sfd, buf, 2000, 0) == -1)
	{
		perror ("Client: Receive failed");
		exit (1);
	}
	printf("%s\n", buf);
	memset(buf,0,2000);

	

	if(send(sfd, "QUIT",2000, 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
	memset(buf,0,2000);
	if (recv (sfd, buf, 2000, 0) == -1)
	{
		perror ("Client: Receive failed");
		exit (1);
	}
	
	
	
}

 int popsend(char buf[],int sfd,int type)
{
	printf("%s\n",buf );
	if(send(sfd, buf,strlen(buf), 0) == -1)
	{
		perror("Server write failed");
		exit(1);
	}
	printf("sent\n");
	char recbuf[2000];
	if (recv (sfd, recbuf, 2000, 0) == -1)
	{
		perror ("Client: Receive failed");
		exit (1);
	}
	if(type==2)
	{
		if(strcmp(recbuf,"wrong Password"))
		{
			return 0;
		}
	}
	else if(strcmp(recbuf,"OK"))
	{
		printf("%s\n",buf );
	}
	printf("%s\n",recbuf );
	return 1;
	
}

void popClient(char* ip)
{
	int sfd = makeNode(ip, POPPORT);
	char buf[1000];
	char A[100],B[100];

	printf("Username\n");
	scanf("%s", A);
	popsend(A,sfd,1);
	printf("Password?\n");
	scanf("%s", B);

	// sprintf(B,"PASS: %s",B);
	popsend(B,sfd,2);
	printf("came here\n");
	popsend("LIST",sfd,1);
	// sprintf(buf,"%s,%s",A,B);
	int flag=1;
	// int flag2=0;
	while(flag)
	{	
		char sends[100];
		printf("which one do you want to read\n");
		int num;
		scanf("%d",&num);
		sprintf(sends,"%d",num);

		if(send(sfd, sends,strlen(sends), 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
		if (recv (sfd, buf, 2000, 0) == -1)
		{
			perror ("Client: Receive failed");
			exit (1);
		}
		printf("%s\n",buf );
		printf("Enter 1 to read more and 2 to exit\n");
		int exitnow;
		scanf("%d",&exitnow);
		if(exitnow)
			flag=0;

	}
	if(send(sfd, "END",4, 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}


	
}

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		printf("Pass arguments correctly\n");
		exit(0);
	}
	char ip[100];
	strcpy(ip,argv[1]);
	int a;
	while(1)
	{
		printf("Do you want to (1) send or (2) receive emails?\n");
		scanf("%d", &a);
		if(a==1)
			smtpClient(ip);
		else
			popClient(ip);
	}
	return 0;
}
