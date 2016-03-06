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

int count1 = 0;

void parseTicket(char inp[1000], char *outfile)
{
	printf("Ticket Booked!! .. Details :\n");
	int i, j;
	ofstream myfile;
	char *tokens[100], *coach, *berth, *route, *buf, mod, path[1000];
	coach = strtok(inp, "#");
	route = strtok(NULL, "#");
	buf = strtok(NULL, "#");
	strcpy(path, "/home/kalyan/networks-lab/Ass2/ReservationLists/");
	strcat(path, outfile);
	myfile.open(path, ios::app);
	if(strcmp(coach, "12321"))
	{
		myfile << "Booking id: " << ++count1 << "\nTrain No: 12321\t\tTrain Name: Superfast Express\n" << route << "\n";
		mod = 8;
	}
	else
	{
		myfile << "Booking id: " << ++count1 << "\nTrain No: 12301\t\tTrain Name: Rajdhani Express\n" << route << "\n";
		mod = 6;
	}
	tokens[0] = (char*)malloc(100*sizeof(char));
    tokens[0] = strtok(buf, "$");
    i=1;
    while(tokens[i-1] != NULL)
    {
        tokens[i] = (char *)malloc(100*sizeof(char));
        tokens[i] = strtok(NULL, "$");
        i++;
    }
    myfile << "Coach\tSeat\n";
    for(j=0;j<i-1;j++)
    {
    	coach = strtok(tokens[j], "*");
    	berth = strtok(NULL, "*");
    	myfile << coach << "\t\t" << berth << "\t\n";
    }
    myfile << "\n";
}

int main(int argc, char *argv[])
{
	int sfd, cfd, size, i, j;
	char ch, buf[1000] = {'\0'}; 
	struct sockaddr_in srv_addr, cli_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	memset(&srv_addr, 0, addrlen);
	if(argc<3)
	{
		printf("Enter reservation list name and output file name\n");
		return 0;
	}
	strcpy(buf, "/home/kalyan/networks-lab/Ass2/ReservationLists/");
	strcat(buf, argv[1]);
	printf("%s\n", buf);
	FILE* infile = fopen(buf, "r");;
	FILE* otfile;

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd == -1)
	{
		perror("Clent: socket error");
		exit(1);
	}
	printf("Socket fd=%d\n\n", sfd);

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
		printf("Sending request - %s\n", buf);
		if(send(sfd, buf, 1000, 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
		memset(buf, '\0', 1000);
		if(recv(sfd, buf, 1000, 0) == -1)
	    {
	       	perror ("Client: Receive failed");
	       	exit (1);
	    }
	    parseTicket(buf, argv[2]);
	    sleep(2);
	}
    return 0;
}