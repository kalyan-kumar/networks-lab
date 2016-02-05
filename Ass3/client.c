#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
#define DST_ADDR "10.146.163.221"

struct rtlphdr
{
    u_int16_t src_port;
    u_int16_t dst_port;
    u_int32_t seq_num;
    u_int32_t ack_num;
    u_int32_t checksum;
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



int main()
{
	int sfd, iph_size, rth_size, msg_size, tot_size;
    iph_size = sizeof(struct iphdr);
    rth_size = sizeof(struct rtlphdr);
	char datagram[4096] , *data , *pseudogram, rec_buf[1000];
	struct iphdr *iph = (struct iphdr *) datagram;
    struct rtlphdr *rth = (struct rtlphdr *) (datagram + iph_size);
    struct sockaddr_in srv_addr, cli_addr;

	sfd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW);
    if(sfd == -1)
    {
        perror("Failed to create socket");
        exit(1);
    }
    memset(datagram, 0, 4096);
    data = datagram + iph_size + rth_size;
    strcpy(data , "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
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
    rth->seq_num = 1;
    rth->ack_num = 2;
    rth->checksum = 0;
    
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(PORT_NUM);
    srv_addr.sin_addr.s_addr = inet_addr(DST_ADDR);
    printf("%s\n", datagram + 20);

    if (sendto (sfd, datagram, iph->tot_len ,  0, (struct sockaddr *) &srv_addr, sizeof (srv_addr)) < 0)
        perror("sendto failed");
    else
        printf ("Packet Sent. Length : %d \n" , iph->tot_len);
    
    memset(rec_buf, 0, 1000);
    int addrlen = sizeof(cli_addr);
    int rn = recvfrom(sfd, (char *)&rec_buf, sizeof(rec_buf), 0, (struct sockaddr *)&cli_addr, &addrlen);
    if( rn < 0)
        perror("packet receive error:");
    if (rn == 0)
        printf("the peer has performed an orderly shutdown\n");
    printf("Received data - %s\n", rec_buf);

    return 0;
}