#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

void parseTicket(char buf[1000])
{
	printf(" Ticket Booked!! .. Details :\n");
	int i, j;
	char *tokens[100], *coach, *berth;
	tokens[0] = (char*)malloc(100*sizeof(char));
    tokens[0] = strtok(buf, "$");
    i=1;
    while(tokens[i-1] != NULL)
    {
        tokens[i] = (char *)malloc(100*sizeof(char));
        tokens[i] = strtok(NULL, "$");
        i++;
    }
    for(j=0;j<i-1;j++)
    {
    	coach = strtok(tokens[j], "*");
    	berth = strtok(NULL, "*");
    	printf("%s %s\n", coach, berth);
    }
}

int main()
{
	int sfd, cfd, size, i, j;
	char ch, buf[1000] = {'\0'}; 
	struct sockaddr_in srv_addr, cli_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	memset(&srv_addr, 0, addrlen);
	FILE* infile = fopen("/home/kalyan/networks-lab/Ass2/Booking.csv", "r");;
	FILE* otfile;

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
	
	while(fgets(buf, 1000, infile))
	{
		if(buf[strlen(buf)-1] == '\n')
			buf[strlen(buf)-1] = '\0';
		printf("%s\n", buf);
		if(send(sfd, buf, 1000, 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
		printf("Message sent\n");
		memset(buf, '\0', 1000);
		if(recv(sfd, buf, 1000, 0) == -1)
	    {
	       	perror ("Client: Receive failed");
	       	exit (1);
	    }
	    parseTicket(buf);
	    sleep(2);
	}
    return 0;
}