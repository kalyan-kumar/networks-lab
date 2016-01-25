#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "news_header.h"
#include <dirent.h>

void handleReader(int cfd)
{
	int siz;
	string path1;
	char buf1[1000] = {'\0'}, buf2[1000] = {'\0'}, direct[1000], path[1000];
	if(recv(cfd, buf1, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	getcwd(direct, 1000);
	DIR *d;
	struct dirent *dp;
	if(!strcmp(buf1, "Academic"))
	{
		strcat(direct, "/Data/Academic");
		article tmp;
		printf("%s\n", direct);
		d = opendir(direct);
		if(d)
		{
			while((dp=readdir(d))!=NULL)
			{
				tmp.readFromFile(dp->d_name, true);
				siz = tmp.heading.size();
				if(send(cfd, tmp.heading.c_str(), siz, 0) == -1)
				{
					perror("Server write failed");
					exit(1);
				}
			}
		}
		else
			perror("Cant open directory");
		closedir(d);
	}
	else
	{
		strcat(direct, "/Data/Non-Academic");
		article tmp;
		d = opendir(direct);
		if(d)
		{
			while((dp=readdir(d))!=NULL)
			{
				tmp.readFromFile(dp->d_name, false);
				tmp.print();
				siz = tmp.heading.size();
				if(send(cfd, tmp.heading.c_str(), siz, 0) == -1)
				{
					perror("Server write failed");
					exit(1);
				}
			}
		}
		else
			perror("Cant open directory");
		closedir(d);
	}
	if(recv(cfd, buf2, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	if(!strcmp(buf1, "Academic"))
	{
		strcat(direct, "/Data/Academic");
		article tmp;
		d = opendir(direct);
		if(d)
		{
			while((dp=readdir(d))!=NULL)
			{
				tmp.readFromFile(dp->d_name, true);
				if(!strcmp(tmp.heading.c_str(), buf2))
				{
					siz = tmp.heading.size();
					if(send(cfd, tmp.heading.c_str(), siz, 0) == -1)
					{
						perror("Server write failed");
						exit(1);
					}
					siz = tmp.date.size();
					if(send(cfd, tmp.date.c_str(), siz, 0) == -1)
					{
						perror("Server write failed");
						exit(1);
					}
					siz = tmp.text.size();
					if(send(cfd, tmp.text.c_str(), siz, 0) == -1)
					{
						perror("Server write failed");
						exit(1);
					}
					break;
				}
			}
		}
		else
			perror("Cant open directory");
		closedir(d);
	}
	else
	{
		strcat(direct, "/Data/Non-Academic");
		article tmp;
		d = opendir(direct);
		if(d)
		{
			strcpy(path, direct);
			while((dp=readdir(d))!=NULL)
			{
				tmp.readFromFile(dp->d_name, false);
				if(!strcmp(tmp.heading.c_str(), buf2))
				{
					siz = tmp.heading.size();
					if(send(cfd, tmp.heading.c_str(), siz, 0) == -1)
					{
						perror("Server write failed");
						exit(1);
					}
					siz = tmp.date.size();
					if(send(cfd, tmp.date.c_str(), siz, 0) == -1)
					{
						perror("Server write failed");
						exit(1);
					}
					siz = tmp.text.size();
					if(send(cfd, tmp.text.c_str(), siz, 0) == -1)
					{
						perror("Server write failed");
						exit(1);
					}
					break;
				}
			}
		}
		else
			perror("Cant open directory");
		closedir(d);
	}

}

void handleReporter(int cfd)
{
	article temp;
	char buf1[1000] = {'\0'}, buf2[1000] = {'\0'}, direct[1000], path[1000];
	if(recv(cfd, buf1, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	printf("%s\n",buf1 );
	if(send(cfd,"done",20, 0) == -1)
				{
					perror("Server write failed");
					exit(1);
				}
	if(strcmp(buf1,"Academic")==0)
	{
		temp.academic=true;

	}
	else
		temp.academic=false;
	if(recv(cfd, buf1, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
		printf("%s\n",buf1 );

	temp.heading=buf1;
	if(send(cfd,"done",20, 0) == -1)
				{
					perror("Server write failed");
					exit(1);
				}
	if(recv(cfd, buf1, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
		

	temp.date=buf1;
	if(send(cfd,"done",20, 0) == -1)
				{
					perror("Server write failed");
					exit(1);
				}
	
	if(recv(cfd, buf1, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
		

	temp.text=buf1;
	temp.writeToFile();
	printf("Succesfully added news\n");
}

void handleAdministrator(int cfd)
{

}

void handleClient(int cfd)
{
	pid_t pidc;
	char buf[1000];
	if(recv(cfd, buf, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	printf("Client type - %s\n", buf);

	pidc = fork();
	if(pidc<0)
	{

	}
	else if(pidc == 0)
	{
		if(!strcmp(buf, "Reader"))
			handleReader(cfd);
		else if(!strcmp(buf, "Reporter"))
			handleReporter(cfd);
		else if(!strcmp(buf, "Administrator"))
			handleAdministrator(cfd);
		exit(1);
	}
	else if(pidc > 0)
	{

	}
	
}

int main(int argc, char **argv)
{
	int sfd, cfd;
	char buf[1000];
	struct sockaddr_in srv_addr, cli_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	memset(&srv_addr, 0, addrlen);
	memset(&cli_addr, 0, addrlen);

	sfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sfd==-1)
	{
		perror("Server: socket error");
		exit(1);
	}
	printf("Socket fd = %d\n", sfd);
	
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port   = htons(21455);
	if(inet_pton(AF_INET, "127.0.0.1", &srv_addr.sin_addr) <= 0)
	{
		perror("Network address conversion.\n");
		exit(1);
	}

	if(bind(sfd, (struct sockaddr *) &srv_addr, addrlen) == -1)
	{
		perror("Server: Bind failed");
		exit(1);
	}

	printf("Max connections allowed to wait: %d\n", 15); // Hard-coded 15 here 
	if(listen(sfd, 15) == -1)
	{
		perror("Server: Listen failed");
		exit(1);
	}

	while(1)
	{
		cfd = accept(sfd, (struct sockaddr *) &cli_addr, &addrlen);
		if(cfd==-1)
		{
			perror("Server: Accept failed");
			exit(1);
		}
		handleClient(cfd);
	}

	if(close(sfd)==-1)
	{
		perror("Server close failed");
		exit(1);
	}
	printf("THE END\n");

	return 0;
}