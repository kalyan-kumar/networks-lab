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

struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
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
	int sfd;
	char datagram[4096] , *data , *pseudogram, rec_buf[1000];
	struct iphdr *iph = (struct iphdr *) datagram;
    struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof (struct ip));
    struct sockaddr_in srv_addr, cli_addr;
    struct pseudo_header psh;

	sfd = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);
    if(sfd == -1)
    {
        perror("Failed to create socket");
        exit(1);
    }
    //IP_HDRINCL to tell the kernel that headers are included in the packet
    int one = 1;
    const int *val = &one; 
    if (setsockopt (sfd, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
    {
        perror("Error setting IP_HDRINCL");
        exit(0);
    }

	memset(datagram, 0, 4096);
    data = datagram + sizeof(struct iphdr) + sizeof(struct tcphdr);
    strcpy(data , "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(PORT_NUM);
    srv_addr.sin_addr.s_addr = inet_addr ("10.5.16.222");						// What is this??

    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof (struct iphdr) + sizeof (struct tcphdr) + strlen(data);
    iph->id = htonl (54321); //Id of this packet
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_TCP;
    iph->check = 0;      //Set to 0 before calculating checksum
    iph->saddr = inet_addr ("127.0.0.1");    //Spoof the source ip address
    iph->daddr = srv_addr.sin_addr.s_addr;

    // IP Checksum
    iph->check = csum ((unsigned short *) datagram, iph->tot_len);			// What is this?

    // TCP Header
    tcph->source = htons (1234);
    tcph->dest = htons (80);
    tcph->seq = 0;
    tcph->ack_seq = 0;
    tcph->doff = 5;  //tcp header size
    tcph->fin=0;
    tcph->syn=1;
    tcph->rst=0;
    tcph->psh=0;
    tcph->ack=0;
    tcph->urg=0;
    tcph->window = htons (5840); /* maximum allowed window size */
    tcph->check = 0; //leave checksum 0 now, filled later by pseudo header
    tcph->urg_ptr = 0;

    // Now the TCP checksum
    psh.source_address = inet_addr("127.0.0.1");
    psh.dest_address = srv_addr.sin_addr.s_addr;
    //printf("%d\n", psh.dest_address);
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcphdr) + strlen(data) );

    int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + strlen(data);
    pseudogram = malloc(psize);

    memcpy(pseudogram , (char*) &psh , sizeof (struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header) , tcph , sizeof(struct tcphdr) + strlen(data));

    tcph->check = csum( (unsigned short*) pseudogram , psize);

     
    //loop if you want to flood :)
    //while (1)
    //{
        //Send the packet
        if (sendto (sfd, datagram, iph->tot_len ,  0, (struct sockaddr *) &srv_addr, sizeof (srv_addr)) < 0)
            perror("sendto failed");
        //Data send successfully
        else
        {
            printf ("Packet Sent. Length : %d \n" , iph->tot_len);
        }
    //}
    
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