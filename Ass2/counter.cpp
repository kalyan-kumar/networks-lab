/*
Kalyan Kumar	13CS10023
Nitesh Sekhar	13CS10033
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sstream>
#include <string>
#include <algorithm>
#include <signal.h>

#include "classes.h"

using namespace std;

#define CLIENT_LIMIT 15

int master_socket, cfd[CLIENT_LIMIT], highsock;
struct timeval tm;
fd_set cur;
Train rajdhani(13, 0, 2, 9);
Train superfast(3, 12, 3, 9);

void setnonblocking(int sock)
{
	int opts;
	opts = fcntl(sock, F_GETFL, 0);
	if(opts < 0)
	{
		perror("fcntl error");
		exit(1);
	}
	if(fcntl(sock, F_SETFL, opts | O_NONBLOCK) < 0)
	{
		perror("2ns fcntl error");
		exit(1);
	}
}

void build_select_list()
{
	int i;
	tm.tv_sec = 10;
	tm.tv_usec = 0;
	FD_ZERO(&cur);
	FD_SET(master_socket, &cur);
	highsock = master_socket;
	for(i=0;i<CLIENT_LIMIT;i++)
	{
		if(cfd[i]!=0)
		{
			FD_SET(cfd[i], &cur);
			if(cfd[i] > highsock)
				highsock = cfd[i];
		}
	}
}

Booking parseBooking(int k)
{
	char line[1000] = {'\0'};
	Booking cur;
	cur.seniors = 0;
	if(recv(cfd[k], line, 1000, 0) == 0)
    {
    	printf("Connection lost: FD=%d\n", cfd[k]);
       	close(cfd[k]);
       	cfd[k] = 0;
       	cur.berths = -1;
       	return cur;
    }
    cout << "Request - " << line << endl;
	int i, j, tmp;
	char *tokens[100], *tmps[100];
	cur.cli_fd = cfd[k];
	for(i=0;i<10;i++)
		cur.prefers.push_back(0);
	stringstream ss;
    tokens[0] = (char*)malloc(100*sizeof(char));
    tokens[0] = strtok(line, ",");
    i=1;
    while(tokens[i-1] != NULL)
    {
        tokens[i] = (char *)malloc(100*sizeof(char));
        tokens[i] = strtok(NULL, ",");
        i++;
    }
    ss << tokens[0];
    ss >> cur.id;
    ss.str(string());
    ss.clear();
    ss << tokens[1];
    ss >> cur.route;
    ss.str(string());
    ss.clear();
    if(!strcmp(tokens[2], "Sleeper"))
    	cur.coach = false;
    else
    	cur.coach = true;
    ss << tokens[3];
    ss >> cur.berths;
    ss.str(string());
    ss.clear();
    tmps[0] = (char*)malloc(100*sizeof(char));
    tmps[0] = strtok(tokens[4], "-");
    i=1;
    while(tmps[i-1] != NULL)
    {
        tmps[i] = (char *)malloc(100*sizeof(char));
        tmps[i] = strtok(NULL, "-");
        i++;
    }
    for(j=0;j<i-1;j++)
    {
    	if(strstr(tmps[j], "NA"))
    		cur.prefers[0] += cur.berths;
    	else if(strstr(tmps[j], "LB"))
    		cur.prefers[1]++;
    	else if(strstr(tmps[j], "MB"))
    		cur.prefers[2]++;
    	else if(strstr(tmps[j], "UB"))
    		cur.prefers[3]++;
    	else if(strstr(tmps[j], "SL"))
    		cur.prefers[4]++;
    	else if(strstr(tmps[j], "SU"))
    		cur.prefers[5]++;
    }
    tmps[0] = (char*)malloc(100*sizeof(char));
    tmps[0] = strtok(tokens[4], "-");
    i==1;
    while(tmps[i-1] != NULL)
    {
        tmps[i] = (char *)malloc(100*sizeof(char));
        tmps[i] = strtok(NULL, "-");
        i++;
    }
    for(j=0;j<i-1;j++)
    {
    	ss << tmps[j];
    	ss >> tmp;
    	ss.str(string());
    	ss.clear();
    	cur.ages.push_back(tmp);
    	if(tmp>=60)
    		cur.seniors++;
    }
    return cur;
}

void sendTicket(Ticket b, int j)
{
	char buf[1000] = {'\0'}, tmp[1000] = {'\0'};
	sprintf(buf, "%d#", b.ticket_number);
	if(b.ticket_type)
		strcat(buf, "AC Class#");
	else
		strcat(buf, "Sleeper Class#");
	int i, x = b.s1.size();
	for(i=0;i<x;i++)
	{
		memset(tmp, '\0', 1000);
		sprintf(tmp, "%d", b.s1[i].first);
		strcat(buf, tmp);
		strcat(buf, "*");
		memset(tmp, '\0', 1000);
		sprintf(tmp, "%d", b.s1[i].second);
		strcat(buf, tmp);
		strcat(buf, "$");
	}
	if(send(j, buf, strlen(buf), 0) == -1)
	{
		perror("Server write failed");
		exit(1);
	}
}

void handleClient(Booking b)
{
	int j, x;
	Ticket final;
	if(b.route==12321)
    {
    	cout << "Booking in Superfast Express" << endl;
    	final.ticket_number = 12321;
    	superfast.assignSeats(b, final);
	}
    else
    {
    	cout << "Booking in Rajdhani Express" << endl;
    	final.ticket_number = 12301;
    	rajdhani.assignSeats(b, final);
    }
    for(j=0,x=final.s1.size();j<x;j++)
    	printf("%d %d\n", final.s1[j].first, final.s1[j].second);
    cout << endl;
    sendTicket(final, b.cli_fd);
}

void handle_new_connection()
{
	int i, tmp;
	struct sockaddr_in cli_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	tmp = accept(master_socket, (struct sockaddr *) &cli_addr, &addrlen);
	if(tmp==-1)
	{
		perror("Server: Accept failed");
		exit(1);
	}
	setnonblocking(tmp);
	for(i=0;i<CLIENT_LIMIT;i++)
	{
		if(cfd[i]==0)
		{
			cfd[i] = tmp;
			tmp = -1;
			break;
		}
	}
	if(tmp!=-1)
	{
		printf("No room left for more clients\n");
		close(tmp);
	}
}

int myfunction(Booking b1, Booking b2)
{
	if(b1.seniors==b2.seniors)
		return (b1.berths>b2.berths);
	else
    	return (b1.seniors>b2.seniors);
}

void readSocks()
{
	int i;
	Booking tmp;
	vector<Booking> slips;
	if(FD_ISSET(master_socket, &cur))
		handle_new_connection();
	for(i=0;i<CLIENT_LIMIT;i++)
	{
		if(FD_ISSET(cfd[i], &cur))
		{
			tmp = parseBooking(i);
			if(tmp.berths!=-1)
				slips.push_back(tmp);
		}
	}
	sort(slips.begin(), slips.end(), myfunction);
	for(i=0;i<slips.size();i++)
		handleClient(slips[i]);
}

int main()
{
	pid_t pidc;
	int reuse_addr = 1, sel_res;
	struct sockaddr_in srv_addr;
	char buf[1000];
	socklen_t addrlen = sizeof(struct sockaddr_in);
	memset(&srv_addr, 0, addrlen);
	FD_ZERO(&cur);
	memset((char *) &cfd, 0, sizeof(cfd));

	master_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(master_socket == -1)
	{
		perror("Server: Socket error");
		exit(1);
	}
	printf("Socket FD = %d\n", master_socket);
	setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_addr,sizeof(reuse_addr));
	setnonblocking(master_socket);	

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port   = htons(21435);
	if(inet_pton(AF_INET, "127.0.0.1", &srv_addr.sin_addr) <= 0)
	{
		perror("Error in Network address conversion");
		exit(1);
	}

	if(bind(master_socket, (struct sockaddr *) &srv_addr, addrlen) == -1)
	{
		perror("Server: Bind failed");
		exit(1);
	}

	printf("Max connections allowed to wait: %d\n", CLIENT_LIMIT);
	if(listen(master_socket, CLIENT_LIMIT) == -1)
	{
		perror("Server: Listen failed");
		exit(1);
	}

	while(1)
	{
		pidc = fork();
		if(pidc < 0)
		{
			perror("Fork error");
			exit(1);
		}
		else if(pidc==0)
		{
			int status;
			scanf("%d", &status);
			if(status==1)
			{
				printf("\tTrain\t\tTotal[Booked/Available]\t\t\tTotal[Booked/Available]\n\t\t\t\t(AC)\t\t\t\t(Sleeper)\n");
				printf("Superfast Express:\t\t%d[%d/%d]\t\t\t%d[%d/%d]\n", superfast.ac_avail+superfast.ac_reserved, superfast.ac_reserved, superfast.ac_avail, superfast.nonac_avail+superfast.nonac_reserved, superfast.nonac_reserved, superfast.nonac_avail);
				printf("Rajdhani Express:\t\t%d[%d/%d]\t\t\t-\n", rajdhani.ac_avail+rajdhani.ac_reserved, rajdhani.ac_reserved, rajdhani.ac_avail);
			}
			exit(1);
		}
		else
		{
			build_select_list();
			sel_res = select(highsock + 1, &cur, (fd_set *) 0, (fd_set *) 0, &tm);
			if(sel_res < 0)
			{
				perror("Select error");
				exit(1);
			}
			else if(sel_res == 0)
				printf("No clients\n");
			else
				readSocks();
			kill(pidc, SIGKILL);
			//sleep(2);
		}
	}

	if(close(master_socket)==-1)
	{
		perror("Server close failed");
		exit(1);
	}
	printf("THE END\n");
	return 0;
}