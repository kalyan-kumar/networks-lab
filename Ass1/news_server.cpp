#include <string>
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

using namespace std;


/*bool sortDayMonthYear(const news firstnews, const news secondnews)
{
    string first = firstnews.date;
    string second = secondnews.date;
    int dd1, dd2, mm1, mm2, yy1, yy2;
    sscanf(first.c_str(), "%d/%d/%d", &dd1, &mm1, &yy1);
    sscanf(second.c_str(), "%d/%d/%d", &dd2, &mm2, &yy2);
    // Algorithm to find out which string is "lesser" than the other
    if(yy1 > yy2)
        return true;
    else if(yy1 < yy2)
        return false;
    else if(yy1 == yy2)
        if(mm1 > mm2)
            return true;
        else if(mm1 < mm2)
            return false;
        else if(mm1 == mm2)
            if(dd1 >= dd2)
                return true;
            else
                return false;
}*/

article academic[100], nacademic[100];

char *getfiles(char direct[1000], bool flag)
{
	int i;
	char buf[1000] ;
	DIR *d;
	struct dirent *dp;
	article tmp;
	d = opendir(direct);
	strcpy(buf, "");
	if(d)
	{
		for(i=1;(dp=readdir(d))!=NULL;i++)
		{
			if(strcmp(dp->d_name, ".")==0 || strcmp(dp->d_name, "..")==0)
				continue;
			strcat(buf, "\n");
			strcat(buf, to_string(i).c_str());
			strcat(buf, " ");
			if(flag)
			{
				academic[i].readFromFile(dp->d_name, true);
				strcat(buf, academic[i].heading.c_str());
			}
			else
			{
				nacademic[i].readFromFile(dp->d_name, true);
				strcat(buf, nacademic[i].heading.c_str());
			}
		}
	}
	else
		perror("Cant open directory");
	closedir(d);
	return buf;
}

void handleReader(int cfd)
{
	int siz;
	string path1;
	article tmp;
	char buf1[1000] = {'\0'}, buf2[1000] = {'\0'}, direct[1000] = {'\0'}, path[1000] = {'\0'}, paths[1000] = {'\0'}, paths1[1000] = {'\0'};
	getcwd(direct, 1000);
	strcpy(path, direct);
	strcat(path, "/Data/Academic");
	strcpy(buf1, getfiles(path, true));
	strcpy(path, direct);
	strcat(path, "/Data/Non-Academic");
	strcpy(buf2, getfiles(path, false));
	
	if(send(cfd, "Welcome to the database!", 24, 0) == -1)
	{
		perror("Server write failed");
		exit(1);
	}
	
	if(recv(cfd, paths, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	if(!strcmp(paths, "Academic"))
	{
		if(send(cfd, buf1, strlen(buf1), 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
	}
	else
	{
		if(send(cfd, buf2, strlen(buf2), 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
	}
	if(recv(cfd, paths1, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	if(!strcmp(paths, "Academic"))
	{
		tmp = academic[stoi(paths1)];
		siz = tmp.heading.size();
		if(send(cfd, tmp.heading.c_str(), siz, 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
		if(recv(cfd, paths1, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
		siz = tmp.date.size();
		if(send(cfd, tmp.date.c_str(), siz, 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
		if(recv(cfd, paths1, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
		siz = tmp.text.size();
		if(send(cfd, tmp.text.c_str(), siz, 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
		if(recv(cfd, paths1, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
	}
	else
	{
		tmp = nacademic[stoi(paths1)];
		siz = tmp.heading.size();
		if(send(cfd, tmp.heading.c_str(), siz, 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
		if(recv(cfd, paths1, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
		siz = tmp.date.size();
		if(send(cfd, tmp.date.c_str(), siz, 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
		if(recv(cfd, paths1, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
		siz = tmp.text.size();
		if(send(cfd, tmp.text.c_str(), siz, 0) == -1)
		{
			perror("Server write failed");
			exit(1);
		}
		if(recv(cfd, paths1, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
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
	if(recv(cfd, buf1, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
		printf("%s\n",buf1 );

	temp.date=buf1;
	printf("lol here?\n");
	// if(recv(cfd, buf1, 1000, 0) == -1)
	// {
	// 	printf("Didnt get what the client requested for\n");
	// 	exit(1);
	// }
	// 	printf("lol %s\n",buf1 );

	// temp.text=buf1;
	temp.writeToFile();

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
		printf("Successfully finished a request\n");
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
	srv_addr.sin_port   = htons(21435);
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