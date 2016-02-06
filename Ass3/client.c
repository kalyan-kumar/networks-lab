#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>			// Required if ioctl is used to configure the network device
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ether.h>
#include <linux/if_packet.h>

#define PORT_NUM 21435
#define SRC_ADDR "10.5.16.222"
#define DST_ADDR "10.5.16.181"

int sequence, acknowledge;

struct rtlphdr
{
    u_int32_t checksum;
    u_int16_t src_port;
    u_int16_t dst_port;
    u_int32_t seq_num;
    u_int32_t ack_num;
};

unsigned short csum(unsigned short *ptr,int nbytes) 
{
    register long sum;
    unsigned short oddbyte;
    register short answer;
 
    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }
    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;
    return(answer);
}

void makePacket(char datagram[], int seq, int ack, char A[])
{
    int iph_size, rth_size, msg_size, tot_size;
    iph_size = sizeof(struct iphdr);
    rth_size = sizeof(struct rtlphdr);
    char *data;
    struct iphdr *iph = (struct iphdr *) datagram;
    struct rtlphdr *rth = (struct rtlphdr *) (datagram + iph_size);
    memset(datagram, 0, 4096);
    data = datagram + iph_size + rth_size;
    strcpy(data , A);
    msg_size = strlen(data);
    tot_size = iph_size + rth_size + msg_size;

    // IP Header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = tot_size;
    iph->id = htonl (54321);
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_RAW;
    iph->check = 0;
    iph->saddr = inet_addr (SRC_ADDR);
    iph->daddr = inet_addr (DST_ADDR);
    iph->check = csum ((unsigned short *) datagram, iph->tot_len);

    // RTLP Header
    rth->src_port = htons(PORT_NUM);
    rth->dst_port = htons(PORT_NUM);
    rth->seq_num = seq;
    rth->ack_num = ack;
    rth->checksum = 0;
}

void threeWayHandshake(int sfd, struct sockaddr_in srv_addr)
{
    struct sockaddr_in cli_addr;
    int tot_size, addrlen = sizeof(cli_addr);
    char rec_buf[4096], pack[4096], A[4096];
    
    memset(A, 0, 4096);
    strcpy(A, "Let's begin with a firm handshake");
    sequence = rand()%9 + 1;
    makePacket(pack, sequence, 0, A);
    tot_size = sizeof(struct iphdr) + sizeof(struct rtlphdr) + strlen(A);
    if (sendto (sfd, pack, tot_size,  0, (struct sockaddr *) &srv_addr, sizeof (srv_addr)) < 0)
        perror("sendto failed");
    else
        printf ("Packet Sent. Length : %d \n" , tot_size);

    memset(rec_buf, 0, 4096);
    int rn = recvfrom(sfd, (char *)&rec_buf, sizeof(rec_buf), 0, (struct sockaddr *)&cli_addr, &addrlen);
    if(rn < 0)
        perror("packet receive error:");
    if(rn == 0)
        printf("the peer has performed an orderly shutdown\n");
    printf("Received data - %s\n", rec_buf + 36);
    
    struct rtlphdr *rec_rth = (struct rtlphdr *) (rec_buf + sizeof(struct iphdr));
    memset(A, 0, 4096);
    strcpy(A, rec_buf+ sizeof(struct iphdr) + sizeof(struct rtlphdr));
    if(sequence!=rec_rth->ack_num)
    {
        perror("Connection Failed");
        exit(1);
    }
    printf("seq - %d\n", sequence);
    acknowledge = rec_rth->seq_num;
    printf("ack - %d\n", acknowledge);
    memset(pack, 0, 4096);
    makePacket(pack, sequence, acknowledge, A);
    tot_size = sizeof(struct iphdr) + sizeof(struct rtlphdr) + strlen(A);
    if (sendto (sfd, pack, tot_size,  0, (struct sockaddr *) &srv_addr, sizeof (srv_addr)) < 0)
        perror("sendto failed");
    else
        printf ("Packet Sent. Length : %d \n" , tot_size);
    printf("Successful\n");
}

void sendPacket(int sfd, char A[], struct sockaddr_in srv_addr)
{
    struct sockaddr_in cli_addr;
    char pack[4096], rec_buf[4096];
    struct rtlphdr *rec_rth;
    int tot_size, rn, byte_size = strlen(A), addrlen = sizeof(cli_addr);
    sequence += byte_size;
    makePacket(pack, sequence, acknowledge, A);
    tot_size = sizeof(struct iphdr) + sizeof(struct rtlphdr) + byte_size;
    fd_set readfds, masterfds;
    struct timeval timeout;

    do{
        if (sendto (sfd, pack, tot_size,  0, (struct sockaddr *) &srv_addr, sizeof (srv_addr)) < 0)
            perror("sendto failed");
        else
            printf ("Packet Sent. Length : %d \n" , tot_size);
        memset(rec_buf, 0, 4096);
        // Add timeout code here
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        FD_ZERO(&readfds);
        FD_SET(sfd, readfds);

        if(select(sfd+1, &readfds, NULL, NULL, &timeout) < 0)
        {
            perror("on select");
            exit(1);
        }
        if(FD_ISSET(sockfd, &readfds))
        {
            rn = recvfrom(sfd, (char *)&rec_buf, sizeof(rec_buf), 0, (struct sockaddr *)&cli_addr, &addrlen);
            if(rn < 0)
                perror("packet receive error:");
            if(rn == 0)
                printf("the peer has performed an orderly shutdown\n");
            printf("Received datas is  - %s\n", rec_buf + 36);
            rec_rth = (struct rtlphdr *) (rec_buf + sizeof(struct iphdr));
        }
    }while(rec_rth->ack_num < sequence);
    if(rec_rth->seq_num > acknowledge)
    {
        memset(A, 0, 4096);
        strcpy(A, rec_buf+ sizeof(struct iphdr) + sizeof(struct rtlphdr));
        // Handle Response
    }
    acknowledge = rec_rth->seq_num;
    // Respond if necessary
}

int main()
{
    srand(time(NULL));
	int sfd, tot_size;
    struct sockaddr_in srv_addr, cli_addr;
    char A[4096], packet[4096], rec_buf[4096];
    // strcpy(A,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    // makePacket(packet, 1, 0, A);
    //tot_size = sizeof(struct iphdr) + sizeof(struct rtlphdr) + strlen(A);

	sfd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
    if(sfd == -1)
    {
        perror("Failed to create socket");
        exit(1);
    }
    
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(PORT_NUM);
    srv_addr.sin_addr.s_addr = inet_addr(DST_ADDR);

    threeWayHandshake(sfd, srv_addr);
    sendPacket(sfd,"lolwa",srv_addr);
    

    /*if (sendto (sfd, packet, tot_size,  0, (struct sockaddr *) &srv_addr, sizeof (srv_addr)) < 0)
        perror("sendto failed");
    else
        printf ("Packet Sent. Length : %d \n" , tot_size);
    
    memset(rec_buf, 0, 1000);
    int addrlen = sizeof(cli_addr);
    int rn = recvfrom(sfd, (char *)&rec_buf, sizeof(rec_buf), 0, (struct sockaddr *)&cli_addr, &addrlen);
    if( rn < 0)
        perror("packet receive error:");
    if (rn == 0)
        printf("the peer has performed an orderly shutdown\n");
    printf("Received data - %s\n", rec_buf);
    */
    return 0;
}