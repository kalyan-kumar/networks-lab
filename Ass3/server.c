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
#define SRC_ADDR "10.146.119.73"
#define DST_ADDR "10.109.67.65"

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

char *makePacket(char datagram[], int seq, int ack, char A[])
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

    return datagram;
}

void threeWayHandshake(int sfd)
{
	struct sockaddr_in srv_addr, cli_addr;
	int tot_size, syn, ack, addrlen = sizeof(cli_addr);
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
	syn = rec_rth->seq_num;
	printf("seq - %d\n", syn);
	ack = rand()%10;
	printf("ack - %d\n", ack);
	
	makePacket(pack, ack, rec_rth->seq_num, A);
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

	if(rec_rth->seq_num!=syn || rec_rth->ack_num!=ack)
	{
		perror("Connection Broken");
		exit(1);
	}
	printf("Successful\n");
}

void recvPacket(int sfd, char A[])
{
    struct sockaddr_in srv_addr, cli_addr;  
    int tot_size, syn, ack, addrlen = sizeof(cli_addr);
    char rec_buf[4096], pack[4096];
    
    memset(rec_buf, 0, 4096);
    int rn = recvfrom(sfd, (char *)&rec_buf, sizeof(rec_buf), 0, (struct sockaddr *)&cli_addr, &addrlen);
    if(rn < 0)
        perror("packet receive error:");
    if(rn == 0)
        printf("the peer has performed an orderly shutdown\n");
    printf("Received data - %s\n", rec_buf + 36);
    
    struct rtlphdr *rec_rth = (struct rtlphdr *) (rec_buf + sizeof(struct iphdr));
    strcpy(A, rec_buf+ sizeof(struct iphdr) + sizeof(struct rtlphdr));
    acknowledge +=strlen(A);
    char servdata[4096];
    strcpy(servdata,"\0");
    makePacket(pack, sequence, acknowledge, servdata);
    acknowledge-=strlen(A)
    tot_size = sizeof(struct iphdr) + sizeof(struct rtlphdr)+strlen(servdata) ;
    if (sendto (sfd, pack, tot_size,  0, (struct sockaddr *) &cli_addr, sizeof (cli_addr)) < 0)
        perror("sendto failed");
    else
        printf ("Packet Sent. Length : %d \n" , tot_size);
    
    
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
    memset(rec_buf, 0, 1000);
    int addrlen = sizeof(cli_addr);
    int rn = recvfrom(sfd, (char *)&rec_buf, sizeof(rec_buf), 0, (struct sockaddr *)&cli_addr, &addrlen);
    if( rn < 0)
		perror("packet receive error:");
    if (rn == 0)
        printf("the peer has performed an orderly shutdown\n");
	printf("Received data - %s\n", rec_buf + 36);

    strcpy(A, "Dibya Jyoti Roy");
	makePacket(packet, 1, 0, A);
	tot_size = sizeof(struct iphdr) + sizeof(struct rtlphdr) + strlen(A);
    if (sendto (sfd, packet, tot_size,  0, (struct sockaddr *) &cli_addr, sizeof (cli_addr)) < 0)
        perror("sendto failed");
    else
        printf ("Packet Sent. Length : %d \n" , tot_size);
    return 0;
}