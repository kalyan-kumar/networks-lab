#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ipc.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <pthread.h>
#include <poll.h>
#include <fcntl.h>
#include <signal.h>

#define SA (struct sockaddr*)

unsigned short csum (unsigned short *buf, int nwords)
{
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return ~sum;
}

int main (int argc, char *argv[])
{
    if (argc != 2)
    {
        printf ("need destination for tracert\n");
        exit (0);
    }
    int sfd = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP);
    char buf[4096] = { 0 };
    struct ip *ip_hdr = (struct ip *) buf;
    int hop = 0;
    int one = 1;
    const int *val = &one;
    if (setsockopt (sfd, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
        printf ("Cannot set HDRINCL!\n");
    struct sockaddr_in addr;
    addr.sin_port = htons (7);
    addr.sin_family = AF_INET;
    inet_pton (AF_INET, argv[1], &(addr.sin_addr));
    fd_set readfds;
    struct timeval timeout;
    while (1)
    {
        ip_hdr->ip_hl = 5;
        ip_hdr->ip_v = 4;
        ip_hdr->ip_tos = 0;
        ip_hdr->ip_len = 20 + 8;
        ip_hdr->ip_id = 10000;
        ip_hdr->ip_off = 0;
        ip_hdr->ip_ttl = hop;
        ip_hdr->ip_p = IPPROTO_ICMP;
        inet_pton (AF_INET, "10.117.11.175", &(ip_hdr->ip_src));
        inet_pton (AF_INET, argv[1], &(ip_hdr->ip_dst));
        ip_hdr->ip_sum = csum ((unsigned short *) buf, 9);
        struct icmphdr *icmphd = (struct icmphdr *) (buf + 20);
        icmphd->type = ICMP_ECHO;
        icmphd->code = 0;
        icmphd->checksum = 0;
        icmphd->un.echo.id = 0;
        icmphd->un.echo.sequence = hop + 1;
        icmphd->checksum = csum ((unsigned short *) (buf + 20), 4);
        double times[3];
        int k=3;
        struct sockaddr_in addr2;
        struct icmphdr *icmphd2;
        while(k--)
        {
            struct timespec tstart={0,0},tfinal={0,0};
            clock_gettime(CLOCK_MONOTONIC, &tstart);
            sendto (sfd, buf, sizeof(struct ip) + sizeof(struct icmphdr), 0, SA & addr, sizeof addr);  
            char buff[4096] = { 0 };
            socklen_t len = sizeof (struct sockaddr_in);
            recvfrom (sfd, buff, sizeof(buff), 0, SA & addr2, &len);
            clock_gettime(CLOCK_MONOTONIC, &tfinal);
            times[k]=((double)tfinal.tv_sec + 1.0e-9*tfinal.tv_nsec)-((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
            icmphd2 = (struct icmphdr *) (buff + 20);
        }
        hop++;
        if(hop==1)
            continue;
        if (icmphd2->type != 0)
            printf ("%d Address:%s time: %f\t%f\t%f\t \n", hop-1, inet_ntoa (addr2.sin_addr),times[2],times[1],times[0]);
        else
        {
            printf ("Reached destination:%s with hop limit:%d\n", inet_ntoa (addr2.sin_addr), hop);
            exit (0);
        }
    }
    return 0;
}