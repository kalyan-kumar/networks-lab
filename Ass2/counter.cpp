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

#include "classes.h"

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

Ticket getSeats(vector<Coach> x, Booking y)
{
	int num_coaches = x.size(), i, prob = -1, num_seats, j, here=0, tmp, t0, t1, t2, t3, t4, t5;
	Ticket final;
	for(i=0;i<num_coaches;i++)
	{
		if(((x[i].total)-(x[i].reserved)) >= y.berths)
		{
			tmp = 0;
			t0 = y.prefers[0];
			t1 = (y.prefers[1]-x[i].num_LB);
			t2 = (y.prefers[2]-x[i].num_MB);
			t3 = (y.prefers[3]-x[i].num_UB);
			t4 = (y.prefers[4]-x[i].num_SL);
			t5 = (y.prefers[5]-x[i].num_SU);
			if(t1 <= 0)
				tmp += y.prefers[1];
			else
				tmp += x[i].num_LB;
			if(t2 <= 0)
				tmp += y.prefers[2];
			else
				tmp += x[i].num_MB;
			if(t3 <= 0)
				tmp += y.prefers[3];
			else
				tmp += x[i].num_UB;
			if(t4 <= 0)
				tmp += y.prefers[4];
			else
				tmp += x[i].num_SL;
			if(t5 <= 0)
				tmp += y.prefers[5];
			else
				tmp += x[i].num_SU;
			if(tmp >= here)
			{
				here = tmp;
				prob = i;
			}
			if(t1<=0 && t2<=0 && t3<=0 && t4<=0 && t5<=0)
			{
				t0 = y.prefers[0];
				t1 = y.prefers[1];
				t2 = y.prefers[2];
				t3 = y.prefers[3];
				t4 = y.prefers[4];
				t5 = y.prefers[5];
				num_seats = x[i].seats.size();
				for(j=0;j<num_seats;j++)
				{
					if(x[i].seats[j].booked)
						continue;
					switch (x[i].seats[j].b)
					{
						case LB:
						{
							if(t1 > 0)
							{
								final.s1.push_back(make_pair(i+1, j+1));
								x[i].seats[j].booked = true;
								x[i].reserved++;
								x[i].num_LB--;
								t1--;
							}
							break;
						}
						case MB:
						{
							if(t2 > 0)
							{
								final.s1.push_back(make_pair(i+1, j+1));
								x[i].seats[j].booked = true;
								x[i].reserved++;
								x[i].num_MB--;
								t2--;
							}
							break;
						}
						case UB:
						{
							if(t3 > 0)
							{
								final.s1.push_back(make_pair(i+1, j+1));
								x[i].seats[j].booked = true;
								x[i].reserved++;
								x[i].num_UB--;
								t3--;
							}
							break;
						}
						case SL:
						{
							if(t4 > 0)
							{
								final.s1.push_back(make_pair(i+1, j+1));
								x[i].seats[j].booked = true;
								x[i].reserved++;
								x[i].num_SL--;
								t4--;
							}
							break;
						}
						case SU:
						{
							if(t5 > 0)
							{
								final.s1.push_back(make_pair(i+1, j+1));
								x[i].seats[j].booked = true;
								x[i].reserved++;
								x[i].num_SU--;
								t5--;
							}
							break;
						}
					}
				}
				for(j=0;j<num_seats && t0>0;j++)
				{
					if(x[i].seats[j].booked)
						continue;
					final.s1.push_back(make_pair(i+1, j+1));
					x[i].seats[j].booked = true;
					x[i].reserved++;
					switch (x[i].seats[j].b)
					{
						case LB:
							x[i].num_LB--;
							break;
						case MB:
							x[i].num_MB--;
							break;
						case UB:
							x[i].num_UB--;
							break;
						case SL:
							x[i].num_SL--;
							break;
						case SU:
							x[i].num_SU--;
							break;
					}
					t0--;
				}
				return final;
			}
		}
	}
	if(prob!=-1)
	{
		i = prob;
		num_seats = x[i].seats.size();
		t0 = y.prefers[0];
		t1 = y.prefers[1];
		t2 = y.prefers[2];
		t3 = y.prefers[3];
		t4 = y.prefers[4];
		t5 = y.prefers[5];		
		for(j=0;j<num_seats;j++)
		{
			if(x[i].seats[j].booked)
				continue;
			switch (x[i].seats[j].b)
			{
				case LB:
				{
					if(t1 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_LB--;
						t1--;
					}
					break;
				}
				case MB:
				{
					if(t2 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_MB--;
						t2--;
					}
					break;
				}
				case UB:
				{
					if(t3 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_UB--;
						t3--;
					}
					break;
				}
				case SL:
				{
					if(t4 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_SL--;
						t4--;
					}
					break;
				}
				case SU:
				{
					if(t5 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_SU--;
						t5--;
					}
					break;
				}
			}
		}
		tmp = t0+t1+t2+t3+t4+t5;
		for(j=0;j<num_seats && tmp>0;j++)
		{
			if(x[i].seats[j].booked)
				continue;
			final.s1.push_back(make_pair(i+1, j+1));
			x[i].seats[j].booked = true;
			x[i].reserved++;
			switch (x[i].seats[j].b)
			{
				case LB:
					x[i].num_LB--;
					break;
				case MB:
					x[i].num_MB--;
					break;
				case UB:
					x[i].num_UB--;
					break;
				case SL:
					x[i].num_SL--;
					break;
				case SU:
					x[i].num_SU--;
					break;
			}
			tmp--;
		}
		return final;
	}
	t0 = y.prefers[0];
	t1 = y.prefers[1];
	t2 = y.prefers[2];
	t3 = y.prefers[3];
	t4 = y.prefers[4];
	t5 = y.prefers[5];
	for(i=0;i<num_coaches;i++)
	{
		if(x[i].reserved==x[i].total)
			continue;
		num_seats = x[i].seats.size();
		for(j=0;j<num_seats;j++)
		{
			if(x[i].seats[j].booked)
				continue;
			switch (x[i].seats[j].b)
			{
				case LB:
				{
					if(t1 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_LB--;
						t1--;
					}
					break;
				}
				case MB:
				{
					if(t2 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_MB--;
						t2--;
					}
					break;
				}
				case UB:
				{
					if(t3 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_UB--;
						t3--;
					}
					break;
				}
				case SL:
				{
					if(t4 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_SL--;
						t4--;
					}
					break;
				}
				case SU:
				{
					if(t5 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_SU--;
						t5--;
					}
					break;
				}
			}
		}
	}
	tmp = t0+t1+t2+t3+t4+t5;
	for(i=0;i<num_coaches && tmp>0;i++)
	{
		if(x[i].reserved==x[i].total)
			continue;
		num_seats = x[i].seats.size();
		for(j=0;j<num_seats && tmp>0;j++)
		{
			if(x[i].seats[j].booked)
				continue;
			final.s1.push_back(make_pair(i+1, j+1));
			x[i].seats[j].booked = true;
			x[i].reserved++;
			switch (x[i].seats[j].b)
			{
				case LB:
					x[i].num_LB--;
					break;
				case MB:
					x[i].num_MB--;
					break;
				case UB:
					x[i].num_UB--;
					break;
				case SL:
					x[i].num_SL--;
					break;
				case SU:
					x[i].num_SU--;
					break;
			}
			tmp--;
		}
	}
	return final;
}

void assignSeats(Booking x, Train y)
{
	if(x.coach)
	{
		if(y.ac_avail < x.berths)
		{
			printf("Not available\n");
			return ;
		}
		getSeats(y.ac_coaches, x);
	}
	else
	{
		if(y.nonac_avail < x.berths)
		{
			printf("Not available\n");
			return ;
		}
		getSeats(y.nonac_coaches, x);
	}
}

Booking parseBooking(char line[1000])
{
	char* tokens[100];
    tokens[0] = (char*)malloc(100*sizeof(char));
    tokens[0] = strtok(line, ",");
    i==1;
    while(tokens[i-1] != NULL)
    {
        tokens[i] = (char *)malloc(100*sizeof(char));
        tokens[i] = strtok(NULL, " ");
        i++;
    }
}

void handleClient(int i)
{
	char buf[1000] = {'\0'};
	if(recv(cfd[i], buf, 1000, 0) < -1)
    {
    	printf("Connection lost: FD=%d\n", cfd[i]);
       	close(cfd[i]);
       	cfd[i] = 0;
    }
    printf("%s\n", buf);
    if(send(cfd[i], "Got the message", 15, 0) == -1)
	{
		perror("Server write failed");
		exit(1);
	}
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
	printf("There are %d clients\n", i);
	if(tmp!=-1)
	{
		printf("No room left for more clients\n");
		close(tmp);
	}
}

void readSocks()
{
	int i;
	if(FD_ISSET(master_socket, &cur))
		handle_new_connection();
	for(i=0;i<CLIENT_LIMIT;i++)
	{
		if(FD_ISSET(cfd[i], &cur))
			handleClient(i);
	}
}

int main()
{
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
		printf("New select\n");
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
		sleep(2);
	}

	if(close(master_socket)==-1)
	{
		perror("Server close failed");
		exit(1);
	}
	printf("THE END\n");
	return 0;
}