#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using std::string; 
using namespace std;    // (or using namespace std if you want to use more of std.)  

void sendNews(string title){
	ifstream myfile;
	string path;
	path = "Data/Academic/" + title + ".txt";
	myfile.open(path.c_str());
	string heading,date,text;
	if(myfile.is_open())
	{
		getline(myfile, heading);
		if(send(sfd, heading, 20, 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
		
		getline(myfile, date);
		if(send(sfd, date, 20, 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
		getline(myfile, text);
		if(send(sfd, "Get me academic news", 20, 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
		myfile.close();
	}
	else
		cout << "Unable to open file.\n";
}

int main(int argc, char **argv)
{
	int sfd, cfd;
	
	struct sockaddr_in srv_addr, cli_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	memset(&srv_addr, 0, addrlen);
	// FILE * pFile;
 // 	pFile = fopen ("myfile.txt","r");
 	

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd == -1)
	{
		perror("Clent: socket error");
		exit(1);
	}
	printf("Socket fd=%d\n", sfd);

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port   = htons(21436);

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
	cout<<"Enter the filename"<<endl;
	string title;
	cin>>title;
	sendNews(title);
	

	char buf[1000] = {'\0'}; 
    
    if (recv (sfd, buf, 1000, 0) == -1)
    {
        perror ("Client: Receive failed");
        exit (1);
    }
    
    printf ("Received message = |%s|\n", buf);

    if (close(sfd) == -1)
    {
        perror ("Client close failed");
        exit (1);
    }

}