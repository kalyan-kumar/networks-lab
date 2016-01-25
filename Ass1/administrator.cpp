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
using namespace std; 
int port_num = 23465;
#define BUF_SIZE 30

int main () 
{
	string message;
	char buf[BUF_SIZE];
    int sfd, rst, flags=0;
    struct sockaddr_in dest_addr, sender_addr;
    socklen_t addrlen = sizeof (struct sockaddr_in), sender_len;; 
    memset (&dest_addr, 0, addrlen);
    
    sfd = socket (AF_INET, SOCK_DGRAM, 0);
    if (sfd == -1) 
    {
        perror ("Administrator: socket error");
        exit (1);
    }
    printf ("Socket fd = %d\n", sfd);
    
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port   = htons (port_num);
    
    if (inet_pton (AF_INET, "127.0.0.1", &dest_addr.sin_addr); <= 0)
    {
        perror ("Client Presentation to network address conversion.\n");
        exit (1);
    }

    rst = sendto (sfd, "Administrator", 13, flags, (struct sockaddr *) &dest_addr, sizeof (struct sockaddr_in));
    if (rst < 0)
    {
        perror ("Client: Sendto function call failed");
        exit (1);
    }
    else
        printf ("Sent data size = %d\n", rst);

    cout << "Enter Password" << endl;
    cin >> message;
    int message_length=message.length();
    
    rst = sendto (sfd, message.c_str(), message_length, flags, (struct sockaddr *) &dest_addr, sizeof (struct sockaddr_in));
    if (rst < 0)
    {
        perror ("Client: Sendto function call failed");
        exit (1);
    }
    else
        printf ("Sent data size = %d\n", rst);

    rst = recvfrom (sfd, buf, BUF_SIZE, flags, (struct sockaddr *) &sender_addr, &sender_len);
    if (rst < 0)
    {
        perror ("Client: couldn't receive");
        exit (1);
    }
    if(strcmp(buf,"yes")==0)
    {
        while(1)
        {
            cout<<"delete one file(Enter 1) or delete time based( Enter 2)?"<<endl;
            int i;
            cin>>i;
            if(i==1)
            {
                cout<<"Enter the Title"<<endl;
                string title;
                cin>>title;
                title="title"+title;
                message_length=title.length();
                 rst = sendto (sfd, title.c_str(), message_length, flags, (struct sockaddr *) &dest_addr, 
                       sizeof (struct sockaddr_in));
                 if (rst < 0)
                {
                    perror ("Client: couldn't receive");
                    exit (1);
                }
                break;

            }
            if(i==2)
            {
                cout<<"Enter the Date and time"<<endl;
                 string title;
                 cin>>title;
                 title="date"+title;
                 message_length=title.length();
                 rst = sendto (sfd, title.c_str(), message_length, flags, (struct sockaddr *) &dest_addr, 
                       sizeof (struct sockaddr_in));
                 if (rst < 0)
                {
                    perror ("Client: couldn't receive");
                    exit (1);
                }
                break;
            }
        }
    }       

    const char *buf2 = inet_ntop (AF_INET, (struct sockaddr *) &sender_addr, buf, BUF_SIZE);
    if (buf2 == NULL)
    {
        perror ("Client: Conversion of sender's address to presentation failed");
        exit (1);
    }
    printf ("Server's address = %s\n", buf2);
    rst = close (sfd); // Close the socket file descriptor.
    if (rst < 0)
    {
        perror ("Client close failed");
        exit (1);
    }
}