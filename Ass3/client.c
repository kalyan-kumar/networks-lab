#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>			// Required if ioctl is used to configure the network device
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ether.h>
#include <linux/if_packet.h>

#define PORT_NUM 21435
#define SRC_ADDR "10.145.65.97"
#define DST_ADDR "10.146.80.36"

int sequence, acknowledge;

struct rtlphdr
{
    u_int32_t checksum;
    u_int16_t src_port;
    u_int16_t dst_port;
    u_int32_t seq_num;
    u_int32_t ack_num;
};

unsigned short csum(unsigned short *buf, int len)
{
        unsigned long sum;
        for(sum=0; len>0; len--)
                sum += *buf++;
        sum = (sum >> 16) + (sum &0xffff);
        sum += (sum >> 16);
        return (unsigned short)(~sum);
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
    rth->checksum = csum((unsigned short *)datagram+sizeof(struct iphdr)+4,strlen(datagram)-4-sizeof(struct iphdr));
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
    // else
        // printf ("Packet Sent. Length : %d \n" , tot_size);

    memset(rec_buf, 0, 4096);
    int rn = recvfrom(sfd, (char *)&rec_buf, sizeof(rec_buf), 0, (struct sockaddr *)&cli_addr, &addrlen);
    if(rn < 0)
        perror("packet receive error:");
    if(rn == 0)
        printf("the peer has performed an orderly shutdown\n");
    // printf("Received data - %s\n", rec_buf + 36);
    
    struct rtlphdr *rec_rth = (struct rtlphdr *) (rec_buf + sizeof(struct iphdr));
    memset(A, 0, 4096);
    strcpy(A, rec_buf+ sizeof(struct iphdr) + sizeof(struct rtlphdr));
    if(sequence!=rec_rth->ack_num)
    {
        perror("Connection Failed");
        exit(1);
    }
    // printf("seq - %d\n", sequence);
    acknowledge = rec_rth->seq_num;
    // printf("ack - %d\n", acknowledge);
    memset(pack, 0, 4096);
    makePacket(pack, sequence, acknowledge, A);
    tot_size = sizeof(struct iphdr) + sizeof(struct rtlphdr) + strlen(A);
    if (sendto (sfd, pack, tot_size,  0, (struct sockaddr *) &srv_addr, sizeof (srv_addr)) < 0)
        perror("sendto failed");
    // else
    //     printf ("Packet Sent. Length : %d \n" , tot_size);
    // printf("Successful\n");
}

void connectiontermination(int sfd, struct sockaddr_in srv_addr)
{
    struct sockaddr_in cli_addr;
    int tot_size, addrlen = sizeof(cli_addr);
    char rec_buf[4096], pack[4096], A[4096];
    
    memset(A, 0, 4096);
    strcpy(A, "Let's End It");
    sequence = rand()%9 + 1;
    makePacket(pack, 0, 0, A);
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
    printf("%s\n", rec_buf + 36);
    struct rtlphdr *rec_rth = (struct rtlphdr *) (rec_buf + sizeof(struct iphdr));

     if(csum((unsigned short*)rec_buf+sizeof(struct iphdr)+4,strlen(rec_buf)-4-sizeof(struct iphdr))!=rec_rth->checksum)
    {
        printf("checksum error\n");
    }
    memset(A, 0, 4096);
    strcpy(A, rec_buf+ sizeof(struct iphdr) + sizeof(struct rtlphdr));
    // printf("%s    ack_num = %d\n", A, rec_rth->ack_num);
    if(rec_rth->ack_num!=0)
    {
        perror("Connection Failed");
        exit(1);
   
    }
    memset(pack, 0, 4096);
    makePacket(pack, 0, rec_rth->seq_num, A);
    tot_size = sizeof(struct iphdr) + sizeof(struct rtlphdr) + strlen(A);
    if (sendto (sfd, pack, tot_size,  0, (struct sockaddr *) &srv_addr, sizeof (srv_addr)) < 0)
        perror("sendto failed");
    else
        printf ("Packet Sent. Length : %d \n" , tot_size);
    printf("Successfully terminated\n");
    exit(1);

}

void sendPacket(int sfd, char A[], struct sockaddr_in srv_addr)
{
    int x;
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
        // printf("after adding - %d\n", sequence);
        if (sendto (sfd, pack, tot_size,  0, (struct sockaddr *) &srv_addr, sizeof (srv_addr)) < 0)
            perror("sendto failed");
        else
            printf ("Packet Sent. Length : %d \n" , tot_size);
        memset(rec_buf, 0, 4096);

        timeout.tv_sec = 0;
        timeout.tv_usec = 100;
        FD_ZERO(&readfds);
        FD_SET(sfd, &readfds);
        if(select(sfd+1, &readfds, NULL, NULL, &timeout) < 0)
        {
            perror("on select");
            exit(1);
        }
        if(FD_ISSET(sfd, &readfds))
        {
            printf("sdfasdf\n");
            rn = recvfrom(sfd, (char *)&rec_buf, sizeof(rec_buf), 0, (struct sockaddr *)&cli_addr, &addrlen);
            if(rn < 0)
                perror("packet receive error:");
            if(rn == 0)
                printf("the peer has performed an orderly shutdown\n");
            printf("%s\n", rec_buf + 36);
            rec_rth = (struct rtlphdr *) (rec_buf + sizeof(struct iphdr));
            if(csum((unsigned short*)rec_buf+sizeof(struct iphdr)+4,strlen(rec_buf)-4-sizeof(struct iphdr))!=rec_rth->checksum)
            {
                printf("checksum error1\n");
            }
        }
        printf("asdfsafasdfasd\n");
        // int x;
        if(rec_buf==NULL)
            x = -1;
        else
           x = rec_rth->ack_num; 
    }while(x < sequence);
    acknowledge = rec_rth->seq_num;
}

int main()
{
    srand(time(NULL));
	int sfd, tot_size;
    struct sockaddr_in srv_addr, cli_addr;
    char A[4096], packet[4096], rec_buf[4096];
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
    int i=0;
    int n;
    scanf("%d",&n);
    for(i=0;i<n;i++)
    {
     char data[100];
     sprintf(data,"ECHO RES %d",i);   
     sendPacket(sfd,data,srv_addr);
    }

    connectiontermination(sfd, srv_addr);
    return 0;
}