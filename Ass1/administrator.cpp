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
    int rst; // Return status of functions.
    int sfd; // Socket file descriptor.
    


    /***************** Create a socket *******************************/
    sfd = socket (AF_INET, SOCK_DGRAM, 0); /* AF_INET --> IPv4,
                * SOCK_DGRAM --> UDP Protocol, 0 --> for the protocol. */
    if (sfd == -1) 
    {
        perror ("Client_1.c socket error");
        exit (1);
    }
    printf ("Socket fd = %d\n", sfd);
        






    /****************** Send - receive messages **********************/
    // char msg[20] = "Hello";
    // char msg[20] = {'\0'};
    // msg[0]='H';    msg[1]='e';    msg[2]='l';    msg[3]='l';
    // msg[4]='o';  Sent size remains 20 for both types of message.
    //     * Conclusion: NULL terminated strings are not treated
    //     * differently. 

    char msg_len = 20;
    /* int  flags   = 0 | MSG_DONTWAIT; /* Client doesn't wait even if 
        * server is not running.
        * The client will return with EAGAIN if the send-buffer is full.
        * */
    int flags = 0; /* Even Now the client doesn't wait even if server 
        * is not running. 
        * Now the client will wait if its send-buffer is full.
        * */
    
    struct sockaddr_in dest_addr; /* sockaddr_in because we are using 
            * IPv4. Type casted to struct sockaddr * at time of 
            * various system calls. */
            
    socklen_t addrlen = sizeof (struct sockaddr_in); 
    
    
    // Initializing destination address.
    memset (&dest_addr, 0, addrlen); // Initializes address to zero.

    dest_addr.sin_family = AF_INET;  // Address is in IPv4 format.
    dest_addr.sin_port   = htons (port_num);  // Port number of the server.
    
    
    rst = inet_pton (AF_INET, "127.0.0.1", &dest_addr.sin_addr); /* Note
            * that third field should point to an in_addr (in6_addr). */
    if (rst <= 0)
    {
        perror ("Client Presentation to network address conversion.\n");
        exit (1);
    }        
    cout<<"Enter Password"<<endl;
    string message;
    cin>>message;
    int message_length=message.length();
    
    /* Sending message to the server. */
    rst = sendto (sfd, message.c_str(), message_length, flags, (struct sockaddr *) &dest_addr, 
                    sizeof (struct sockaddr_in)); /* Value of rst is 20,
        * on successful transmission; i.e. It has nothing to do with a 
        * NULL terminated string.
        */
    if (rst < 0)
    {
        perror ("Client: Sendto function call failed");
        exit (1);
    }
    else 
    {
        printf ("Sent data size = %d\n", rst);
    }

    
    
    char buf[BUF_SIZE];
    struct sockaddr_in sender_addr;
    socklen_t sender_len;
    /* Receive a message from the server. */
    rst = recvfrom (sfd, buf, BUF_SIZE, flags, 
                    (struct sockaddr *) &sender_addr, 
                    &sender_len);
    if (rst < 0)
    {
        perror ("Client: couldn't receive");
        exit (1);
    }
    // printf ("Message from server = |%s|\n", buf);
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

                     /* Value of rst is 20,
            }
        }
    }
    /* Address of the server. */
    const char *buf2 = inet_ntop (AF_INET, (struct sockaddr *) &sender_addr, buf, 
                        BUF_SIZE);
    if (buf2 == NULL)
    {
        perror ("Client: Conversion of sender's address to presentation failed");
        exit (1);
    }
    
    printf ("Servers address = %s\n", buf2);
    
    





    /****************** Close ****************************************/
    rst = close (sfd); // Close the socket file descriptor.
    if (rst < 0)
    {
        perror ("Client close failed");
        exit (1);
    }
}