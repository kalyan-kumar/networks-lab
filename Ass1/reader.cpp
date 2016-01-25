#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

void pingServer(int sfd)
{
	int i;
	char buf[1000] = {'\0'};
	if(send(sfd, "Reader", 6, 0) == -1)
	{
		perror("Server write failed");
		exit(1);
	}

	strcpy(buf, "\0");
	if (recv (sfd, buf, 1000, 0) == -1)
    {
       	perror ("Client: Receive failed");
       	exit (1);
    }
    printf("%s\n", buf);

	printf("Which category of news do you want?\n");
	scanf("%s", buf);
	
	if(send(sfd, buf, strlen(buf), 0) == -1)
	{
		perror("Server write failed");
		exit(1);
	}

	printf("Folowing news are availabe -\n");

	strcpy(buf, "\0");
	if (recv (sfd, buf, 1000, 0) == -1)
    {
       	perror ("Client: Receive failed");
       	exit (1);
    }
    printf("%s\n", buf);

	printf("Enter the article number you want to read\n");
	scanf("%d", &i);
	
	if(send(sfd, to_string(i).c_str(), to_string(i).length(), 0) == -1)
	{
		perror("Server write failed");
		exit(1);
	}
}

void getArticle(int sfd)
{
	char rec_str[1000] = {'\0'};
	if (recv(sfd, rec_str, 1000, 0) == -1)
    {
       	perror ("Client: Receive failed");
        exit (1);
    }
    if(send(sfd, "Got it!", 7, 0) == -1)
	{
		perror("Server write failed");
		exit(1);
	}
    printf("\n%s\n", rec_str);
    if (recv(sfd, rec_str, 1000, 0) == -1)
    {
       	perror ("Client: Receive failed");
        exit (1);
    }
    if(send(sfd, "Got it!", 7, 0) == -1)
	{
		perror("Server write failed");
		exit(1);
	}
	printf("\nDated: %s\n", rec_str);
    if (recv(sfd, rec_str, 1000, 0) == -1)
    {
       	perror ("Client: Receive failed");
        exit (1);
    }
    if(send(sfd, "Got it!", 7, 0) == -1)
	{
		perror("Server write failed");
		exit(1);
	}
    printf("\n%s\n", rec_str);
}

int main(int argc, char **argv)
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

	pingServer(sfd);
	getArticle(sfd);
    
    if (close(sfd) == -1)
    {
        perror ("Client close failed");
        exit (1);
    }
    return 0;
}