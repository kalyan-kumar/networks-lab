#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>         // Required if ioctl is used to configure the network device
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/ether.h>
#include <linux/if_packet.h>

#define PORT_NUM 21435
#define SRC_ADDR "10.146.69.7"
#define DST_ADDR "10.145.29.133"

int sequence, acknowledge;

struct rtlphdr
{
    u_int32_t checksum;
    u_int16_t src_port;
    u_int16_t dst_port;
    u_int32_t seq_num;
    u_int32_t ack_num;
};



// unsigned short csum(unsigned short *ptr,int nbytes) 
// {
//     register long sum;
//     unsigned short oddbyte;
//     register short answer;
 
//     sum=0;
//     while(nbytes>1) {
//         sum+=*ptr++;
//         nbytes-=2;
//     }
//     if(nbytes==1) {
//         oddbyte=0;
//         *((u_char*)&oddbyte)=*(u_char*)ptr;
//         sum+=oddbyte;
//     }
 
//     sum = (sum>>16)+(sum & 0xffff);
//     sum = sum + (sum>>16);
//     answer=(short)~sum;
//     return(answer);
// }
unsigned short csum (unsigned short *buf, int nwords)
{
  unsigned long sum;
  for (sum = 0; nwords > 0; nwords--)
    sum += *buf++;
  sum = (sum >> 16) + (sum & 0xffff);
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
    iph->check = csum((unsigned short*)datagram+sizeof(struct iphdr)+4,strlen(datagram)-4-sizeof(struct iphdr));

    // RTLP Header
    rth->src_port = htons(PORT_NUM);
    rth->dst_port = htons(PORT_NUM);
    rth->seq_num = seq;
    rth->ack_num = ack;
    rth->checksum = 0;
}

void threeWayHandshake(int sfd)
{
    struct sockaddr_in srv_addr, cli_addr;
    int tot_size, addrlen = sizeof(cli_addr);
    char rec_buf[4096], pack[4096], A[4096];
    
    memset(rec_buf, 0, 4096);
    int rn = recvfrom(sfd, (char *)&rec_buf, sizeof(rec_buf), 0, (struct sockaddr *)&cli_addr, &addrlen);
    if(rn < 0)
        perror("packet receive error:");
    if(rn == 0)
        printf("the peer has performed an orderly shutdown\n");
    printf("Received data - %s\n", rec_buf + 36);
    
    struct rtlphdr *rec_rth = (struct rtlphdr *) (rec_buf + sizeof(struct iphdr));
    strcpy(A, rec_buf+ sizeof(struct iphdr) + sizeof(struct rtlphdr));
    acknowledge = rec_rth->seq_num;
    printf("seq - %d\n", acknowledge);
    sequence = rand()%10;
    printf("ack - %d\n", sequence);
    
    makePacket(pack, sequence, acknowledge, A);
    tot_size = sizeof(struct iphdr) + sizeof(struct rtlphdr) + strlen(A);
    if (sendto (sfd, pack, tot_size,  0, (struct sockaddr *) &cli_addr, sizeof (cli_addr)) < 0)
        perror("sendto failed");
    else
        printf ("Packet Sent. Length : %d \n" , tot_size);

    memset(rec_buf, 0, 4096);

    rn = recvfrom(sfd, (char *)&rec_buf, sizeof(rec_buf), 0, (struct sockaddr *)&cli_addr, &addrlen);
    if( rn < 0)
        perror("packet receive error:");
    if (rn == 0)
        printf("the peer has performed an orderly shutdown\n");
    rec_rth = (struct rtlphdr *) (rec_buf + sizeof(struct iphdr));
    if(csum((unsigned short*)rec_buf+sizeof(struct iphdr)+4,strlen(rec_buf)-4-sizeof(struct iphdr))!=rec_rth->checksum)
    {
        printf("checksum error\n");
    }
    printf("received - %s\n", rec_buf+36);

    if(rec_rth->seq_num!=acknowledge || rec_rth->ack_num!=sequence)
    {
        perror("Connection Broken");
        exit(1);
    }
    printf("Successful\n");
}
void connectiontermination(int sfd,struct sockaddr_in  cli_addr)
{
    struct sockaddr_in srv_addr;
    int tot_size, addrlen = sizeof(cli_addr);
    char rec_buf[4096], pack[4096], A[4096];
    
    memset(rec_buf, 0, 4096);
    // int rn = recvfrom(sfd, (char *)&rec_buf, sizeof(rec_buf), 0, (struct sockaddr *)&cli_addr, &addrlen);
    // if(rn < 0)
    //     perror("packet receive error:");
    // if(rn == 0)
    //     printf("the peer has performed an orderly shutdown\n");
    // printf("Received data - %s\n", rec_buf + 36);
    
    // struct rtlphdr *rec_rth = (struct rtlphdr *) (rec_buf + sizeof(struct iphdr));
    // strcpy(A, rec_buf+ sizeof(struct iphdr) + sizeof(struct rtlphdr));
    // acknowledge = rec_rth->seq_num;
    // printf("seq - %d\n", acknowledge);
    // sequence = rand()%10;
    // printf("ack - %d\n", sequence);
    printf("came here\n");
    strcpy(A,"ending");
    makePacket(pack, -1, 0, A);
    
    struct rtlphdr * rec_rth1 = (struct rtlphdr *) (pack + sizeof(struct iphdr));
   
    tot_size = sizeof(struct iphdr) + sizeof(struct rtlphdr) + strlen(A);
    if (sendto (sfd, pack, tot_size,  0, (struct sockaddr *) &cli_addr, sizeof (cli_addr)) < 0)
        perror("sendto failed");
    else
        printf ("Packet Sent. Length : %d \n" , tot_size);

    memset(rec_buf, 0, 4096);

    int rn = recvfrom(sfd, (char *)&rec_buf, sizeof(rec_buf), 0, (struct sockaddr *)&cli_addr, &addrlen);
    if( rn < 0)
        perror("packet receive error:");
    if (rn == 0)
        printf("the peer has performed an orderly shutdown\n");
    struct rtlphdr * rec_rth = (struct rtlphdr *) (rec_buf + sizeof(struct iphdr));
     if(csum((unsigned short*)rec_buf+sizeof(struct iphdr)+4,strlen(rec_buf)-4-sizeof(struct iphdr))!=rec_rth->checksum)
    {
        printf("checksum error\n");
    }
    
    if(rec_rth->seq_num==0 || rec_rth->ack_num==-1)
    {
         printf("Successfully Terminated\n");
         exit(1);
    }
   
}

char* recvPacket(int sfd)
{
    struct sockaddr_in srv_addr, cli_addr;  
    int tot_size, rn, addrlen = sizeof(cli_addr);
    char rec_buf[4096], pack[4096], dat[4096],tocheck[sizeof(struct rtlphdr)];
    struct rtlphdr *rec_rth;
    static  char  A[4096];
    int flag=0,quitflag=0;
    memset(rec_buf, 0, 4096);
    do{
        printf("losdasd\n");
        memset(rec_buf, 0, 4096);
        memset(A, 0, 4096);
        memset(dat, 0, 4096);
        memset(pack, 0, 4096);
        memset(tocheck,0,sizeof(struct rtlphdr));
        rn = recvfrom(sfd, (char *)&rec_buf, sizeof(rec_buf), 0, (struct sockaddr *)&cli_addr, &addrlen);
        if(rn < 0)
            perror("packet receive error:");
        if(rn == 0)
            printf("the peer has performed an orderly shutdown\n");
        printf("Received datas - %s\n", rec_buf + 36);
        rec_rth = (struct rtlphdr *) (rec_buf + sizeof(struct iphdr));
        strcpy(A, rec_buf+ sizeof(struct iphdr) + sizeof(struct rtlphdr));

        if(csum((unsigned short*)rec_buf+sizeof(struct iphdr)+4,strlen(rec_buf)-4-sizeof(struct iphdr))==rec_rth->checksum)
        {
            strcpy(dat, "Received");
            if(rec_rth->seq_num==0)
                connectiontermination(sfd, cli_addr);               // Put response here
            makePacket(pack, sequence+strlen(dat), acknowledge+strlen(A), dat);
            tot_size = sizeof(struct iphdr) + sizeof(struct rtlphdr) + strlen(dat);
            if (sendto (sfd, pack, tot_size,  0, (struct sockaddr *) &cli_addr, sizeof (cli_addr)) < 0)
                perror("sendto failed");
            else
                printf ("Packet Sent. Length : %d \n" , tot_size);
            flag=1;
            
        }
        quitflag=rec_rth->seq_num-acknowledge-strlen(A);
        if(!quitflag)
            break;
    }while(quitflag);
    acknowledge += strlen(A);
    sequence += strlen(dat);
    if (flag)
        return A;
    return NULL;
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
    threeWayHandshake(sfd);
    while(1)
        strcpy(A,recvPacket(sfd));

    // memset(rec_buf, 0, 1000);
    // int addrlen = sizeof(cli_addr);
 //    int rn = recvfrom(sfd, (char *)&rec_buf, sizeof(rec_buf), 0, (struct sockaddr *)&cli_addr, &addrlen);
 //    if( rn < 0)
    //  perror("packet receive error:");
 //    if (rn == 0)
 //        printf("the peer has performed an orderly shutdown\n");
    // printf("Received datass - %s\n", rec_buf + 36);
 //    strcpy(A, "Dibya Jyoti Roy");
    // makePacket(packet, 1, 0, A);
    // tot_size = sizeof(struct iphdr) + sizeof(struct rtlphdr) + strlen(A);
 //    if (sendto (sfd, packet, tot_size,  0, (struct sockaddr *) &cli_addr, sizeof (cli_addr)) < 0)
 //        perror("sendto failed");
 //    else
 //        printf ("Packet Sent. Length : %d \n" , tot_size);
    return 0;
}