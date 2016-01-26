#include <vector>
#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <algorithm>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "news_header.h"
#include <dirent.h>


using namespace std;

vector <article> academic, nacademic;
bool dateComp(article a, article b)
{
	int dd1, dd2, mm1, mm2, yy1, yy2;
	sscanf(a.date.c_str(), "%d/%d/%d", &dd1, &mm1, &yy1);
	printf("%d/%d/%d\n", &dd1, &mm1, &yy1 );
	sscanf(b.date.c_str(), "%d/%d/%d", &dd1, &mm1, &yy1);
	printf("%d/%d/%d\n", &dd1, &mm1, &yy1 );


	if(yy1 > yy2)
		return true;
	else if(yy1 < yy2)
		return false;
	else if(yy1==yy2)
	{
		if(mm1 > mm2)
            return true;
        else if(mm1 < mm2)
            return false;
        else if(mm1 == mm2)
        {
        	if(dd1 > dd2)
                return true;
            else
                return false;
        }
	}
}


int getfiles(char direct[1000], bool flag)
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
		for(i=0;(dp=readdir(d))!=NULL;i++)
		{
			if(strcmp(dp->d_name, ".")==0 || strcmp(dp->d_name, "..")==0)
			{
				i--;
				continue;
			}
			if(flag)
			{
				article temp;
				temp.file=dp->d_name;
				temp.readFromFile(dp->d_name, true);
				academic.push_back(temp);
			}
			else
				{
					article temp;
					temp.file=dp->d_name;
					temp.readFromFile(dp->d_name, false);
					nacademic.push_back(temp);
				}
		}
	}
	else
		perror("Cant open directory");
	closedir(d);
	return i;
}

void handleReader(int cfd)
{
	int siz, siz1, i;
	string path1;
	article tmp;
	char buf1[1000] = {'\0'}, buf2[1000] = {'\0'}, direct[1000] = {'\0'}, path[1000] = {'\0'}, paths[1000] = {'\0'}, paths1[1000] = {'\0'};
	getcwd(direct, 1000);
	strcpy(path, direct);
	strcat(path, "/Data/Academic");
	siz = getfiles(path, true);
	strcpy(path, direct);
	strcat(path, "/Data/Non-Academic");
	siz1 = getfiles(path, false);

	// printf("asdfsdf\n");
	// printf("%d\n",academic.size() );
	// sort(academic.begin(), academic.end(),academic[0].dateCompare;
	// printf("asdfsdf\n");
	// sort(nacademic.begin(), nacademic.end(),nacademic[0].dateCompare());
	// printf("asdfsdf\n");
	for(i=1;i<=siz;i++)
	{
		strcat(buf1, to_string(i).c_str());
		strcat(buf1, " ");
		strcat(buf1, academic[i-1].heading.c_str());
		strcat(buf1, "\n");
	}
	for(i=1;i<=siz1;i++)
	{
		strcat(buf2, to_string(i).c_str());
		strcat(buf2, " ");
		strcat(buf2, nacademic[i-1].heading.c_str());
		strcat(buf2, "\n");
	}
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
		tmp = academic[stoi(paths1)-1];
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
	// printf("%s\n",buf1 );
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
		// printf("%s\n",buf1 );

	temp.heading=buf1;
	temp.file=temp.heading+".txt";
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
	string path4;
	if(temp.academic)
		path4 = "Data/Academic/" + temp.file;
	else
		path4 = "Data/Non-Academic/" + temp.file;
	printf("Succesfully added news\n");
	int pid=fork();
	if(pid==0)
	{	
		int sen=execlp("xterm", "xterm", "-hold", "-e", "more",path4.c_str(),(const char*)NULL);
		if(sen==-1)
			perror("exec error");
	}	
	
}

void handleAdministrator(int sfd)
{	
	int siz, siz1;
	int BUF_SIZE=100;
	char buf[1000] = {'\0'},direct[1000] = {'\0'}, path[1000] = {'\0'},buf2[1000]={'0'};
	struct sockaddr_in cl_addr;
    socklen_t addrlen = sizeof (struct sockaddr_in);
    memset (&cl_addr, 0, addrlen);
    getcwd(direct, 1000);
	strcpy(path, direct);
	strcat(path, "/Data/Academic");
	siz = getfiles(path, true);
	strcpy(path, direct);
	strcat(path, "/Data/Non-Academic");
	siz1 = getfiles(path, false);
    int  b_recv = recvfrom (sfd, buf, BUF_SIZE, 0, 
                        (struct sockaddr *) &cl_addr, &addrlen);


    if (b_recv == -1)
    {
        perror ("Server: recvfrom failed");
        exit (1);
    }
    if(strcmp(buf,"Administrator"))
    	{
    		perror("Not Administrator");
    		exit(1);
    	}
    int rst = sendto (sfd, "Done", sizeof (buf), 0, 
                    (struct sockaddr *) &cl_addr, 
                    sizeof (struct sockaddr_in));
    if (rst < 0)
    {
        perror ("Server: Couldn't send");
        exit (1);
    }
    b_recv = recvfrom (sfd, buf, BUF_SIZE, 0, 
                        (struct sockaddr *) &cl_addr, &addrlen);



    if (b_recv == -1)
    {
        perror ("Server: recvfrom failed");
        exit (1);
    }
    if(strcmp(buf,"password"))
    {
      rst = sendto (sfd, "Accepted", sizeof (buf), 0, 
                    (struct sockaddr *) &cl_addr, 
                    sizeof (struct sockaddr_in));
	    if (rst < 0)
	    {
	        perror ("Server: Couldn't send");
	        exit (1);
	    }
	   b_recv = recvfrom (sfd, buf2, BUF_SIZE, 0, 
                        (struct sockaddr *) &cl_addr, &addrlen);
   		
    if (b_recv == -1)
    {
        perror ("Server: recvfrom failed");
        exit (1);
    }
    printf("%s\n",buf2);
    	article temp;
    	temp.date=buf2;
    	
   		for(article temp2:academic)
		{
			if(dateComp(temp,temp2))
			{
				string files="./Data/Academic/";
				cout<<files+temp2.file<<endl;
				 if(remove((files+temp2.file).c_str())!=0)  
   				 perror( "Error deleting file" );
			}
		}
		for(article temp2:nacademic)
		{
			if(dateComp(temp,temp2))
			{
				 string files="./Data/Non-Academic/";
				 if(remove((files+temp2.file).c_str())!=0) 
   				 perror( "Error deleting file" );
			}
		}
    	
    }
    else
    {
    	 if(strcmp(buf,"password"))
    {
    	 rst = sendto (sfd, "Try", sizeof (buf), 0, 
                    (struct sockaddr *) &cl_addr, 
                    sizeof (struct sockaddr_in));
	    if (rst < 0)
	    {
	        perror ("Server: Couldn't send");
	        exit (1);
	    }	
    }
    }

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
		else
			handleReporter(cfd);
		printf("Successfully finished a request\n");
		exit(1);
	}
	else if(pidc > 0)
	{

	}
	
}

int main(int argc, char **argv)
{
	pid_t pida;
	pida = fork();
	if(pida < 0)
	{
		perror("Fork error");
		exit(1);
	}
	else if (pida == 0)
	{
		int sfd;
		struct sockaddr_in srv_addr;
    	socklen_t addrlen = sizeof (struct sockaddr_in);
    	memset (&srv_addr, 0, addrlen);
    	sfd = socket (AF_INET, SOCK_DGRAM, 0); 
	    if (sfd == -1) 
	    {
	        perror ("Server_1.c socket error");
	        exit (1);
	    }
	    printf ("Socket fd = %d\n", sfd);
	    srv_addr.sin_family = AF_INET;
    	srv_addr.sin_port   = htons (23465);
	    if (inet_pton (AF_INET, "127.0.0.1", &srv_addr.sin_addr) <= 0)
	    {
	        perror ("Server Presentation to network address conversion.\n");
	        exit (1);
	    }
	    if (bind (sfd, (struct sockaddr *) &srv_addr, addrlen) < 0)
	    {
	        perror ("Server: bind failed");
	        exit (1);
	    }
	    while(1)
	    {
	    	handleAdministrator(sfd);//sfd
	    }

	}
	else
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
	}
	return 0;
}