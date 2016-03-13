#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 21634

char direct[1000], file[1000], srvip[1000], from[1000];
int mk_cli, sm_cli;
FILE *fp;

int makeNode(const char* src, int port, int cors)
{
	int sfd, reuse_addr = 1;
	struct sockaddr_in srv_addr;
	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sfd==-1)
	{
		perror("Server: socket error");
		exit(1);
	}
	printf("Socket fd = %d\n", sfd);
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr,sizeof(reuse_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port   = htons(port);
	if(inet_pton(AF_INET, src, &srv_addr.sin_addr) <= 0)
	{
		perror("Network address conversion.\n");
		exit(1);
	}
	if(cors==0)
	{
		if(bind(sfd, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_in)) == -1)
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
	}
	else
	{
		if(connect(sfd, (struct sockaddr *) &srv_addr, sizeof(struct sockaddr_in)) == -1)
		{
			perror("Client: Connect failed.");
			exit(1);
		}
	}
	return sfd;
}

int smtpHelo(int cfd)
{
	char buf[1000];
	memset(buf, 1000, 0);
	if(recv(cfd, buf, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	if(strncmp(buf, "HELO", strlen("HELO"))!=0)
		return -1;
	if(send(cfd, "250 OK", 6, 0)==-1)
	{
		perror("Server write failed");
		exit(1);
	}
	return 1;
}

int smtpMail(int cfd)
{
	char buf[1000], email[1000];
	memset(buf, 1000, 0);
	if(recv(cfd, buf, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	if(strncmp(buf, "MAIL FROM", strlen("MAIL FROM"))!=0)
		return -1;
	// File handling code
	int i, j, x=strlen(buf), flag;
	for(i=0,j=0,flag=0;i<x;i++)
	{
		if(flag==1)
		{
			email[j] = buf[i];
			j++;
		}
		else
		{
			if(buf[i]==':')
			{
				flag = 1;
				i++;
			}
		}
	}
	strcpy(from, email);
	if(send(cfd, "250 OK", 6, 0)==-1)
	{
		perror("Server write failed");
		exit(1);
	}
	return 1;	
}

int smtpRcpt(int cfd)
{
	char buf[1000], email[1000], domain[1000];
	memset(buf, 1000, 0);
	if(recv(cfd, buf, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	if(strncmp(buf, "RCPT TO", strlen("RCPT TO"))!=0)
		return -1;
	int i, j, x=strlen(buf), flag;
	for(i=0,j=0,flag=0;i<x;i++)
	{
		if(flag==1)
		{
			email[j] = buf[i];
			j++;
		}
		else
		{
			if(buf[i]==':')
			{
				flag = 1;
				i++;
			}
		}
	}
	email[j] = '\0';
	strcpy(file, direct);
	strcat(file, email);
	x = strlen(email);
	for(i=0,j=0,flag=0;i<x;i++)
	{
		if(flag==1)
		{
			domain[j] = email[i];
			j++;
		}
		else
		{
			if(email[i]=='@')
				flag = 1;
		}
	}
	domain[j] = '\0';
	if(strcmp(domain, srvip)!=0)
		mk_cli = 1;
	if(mk_cli==1)
	{
		sm_cli = makeNode(domain, 21834, 1);
		char buf1[1000];
		memset(buf1, 1000, 0);
		if(recv(sm_cli, buf1, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
		if(send(sm_cli, strcat("HELO: ", domain), 6+strlen(domain), 0)==-1)
		{
			perror("Server write failed");
			exit(1);
		}
		memset(buf1, 1000, 0);
		if(recv(sm_cli, buf1, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
		memset(buf1, 1000, 0);
		strcpy(buf1, "MAIL FROM: ");
		strcat(buf1, from);
		if(send(sm_cli, buf1, strlen(buf1), 0)==-1)
		{
			perror("Server write failed");
			exit(1);
		}
		memset(buf1, 1000, 0);
		if(recv(sm_cli, buf1, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
		if(send(sm_cli, buf, 6+strlen(domain), 0)==-1)
		{
			perror("Server write failed");
			exit(1);
		}
		memset(buf1, 1000, 0);
		if(recv(sm_cli, buf1, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
	}
	fp = fopen(file, "a");
	fprintf(fp, "to:%s\n", email);
	fclose(fp);
	if(send(cfd, "250 OK", 6, 0)==-1)
	{
		perror("Server write failed");
		exit(1);
	}
	return 1;
}

int smtpData(int cfd)
{
	char buf[1000], buf1[1000];
	memset(buf, 1000, 0);
	if(recv(cfd, buf, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	if(mk_cli==1)
	{
		if(send(sm_cli, buf, strlen(buf), 0)==-1)
		{
			perror("Server write failed");
			exit(1);
		}
		memset(buf1, 1000, 0);
		if(recv(cfd, buf1, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
	}
	if(strncmp(buf, "DATA", strlen("DATA"))!=0)
		return -1;
	fp = fopen(file, "a");
	if(send(cfd, "354 start mail input", 20, 0)==-1)
	{
		perror("Server write failed");
		exit(1);
	}
	do
	{
		memset(buf, 1000, 0);
		if(recv(cfd, buf, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
		if(mk_cli==1)
		{
			if(send(sm_cli, buf, strlen(buf), 0)==-1)
			{
				perror("Server write failed");
				exit(1);
			}
		}
		// Write mail content to file here
		fprintf(fp, "%s\n", buf);
	}while(strcmp(buf, ".")!=0);
	fprintf(fp, "Termination sequence\n\n");
	fclose(fp);
	memset(buf1, 1000, 0);
	if(recv(sm_cli, buf1, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	if(send(cfd, "250 OK", 6, 0)==-1)
	{
		perror("Server write failed");
		exit(1);
	}
	return 1;
}

int smtpQuit(int cfd)
{
	char buf[1000], buf1[1000];
	memset(buf, 1000, 0);
	if(recv(cfd, buf, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	if(mk_cli==1)
	{
		if(send(sm_cli, buf, strlen(buf), 0)==-1)
		{
			perror("Server write failed");
			exit(1);
		}
	}
	if(strncmp(buf, "QUIT", strlen("QUIT"))!=0)
		return -1;
	if(send(cfd, "250 OK", 6, 0)==-1)
	{
		perror("Server write failed");
		exit(1);
	}
	if(mk_cli==1)
	{
		char buf1[1000];
		memset(buf1, 1000, 0);
		if(recv(sm_cli, buf, strlen(buf), 0)==-1)
		{
			perror("Server write failed");
			exit(1);
		}
		if(close(sm_cli)==-1)
	    {
	        perror ("Client close failed");
	        exit (1);
	    }
	}
	return 1;
}

void smtpServer(char* src)
{
	int sfd = makeNode(src, 21434, 0), cfd;
	struct sockaddr_in cli_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	memset(&cli_addr, 0, addrlen);
	pid_t child;
	while(1)
	{
		mk_cli = 0;
		cfd = accept(sfd, (struct sockaddr *) &cli_addr, &addrlen);
		if(cfd==-1)
		{
			perror("Server: Accept failed");
			exit(1);
		}
		child = fork();
		if(child<0)
		{
			perror("Fork error");
			exit(1);
		}
		else if(child==0)
		{
			if(send(cfd, "220 service ready", 17, 0)==-1)
			{
				perror("Server write failed");
				exit(1);
			}
			if(smtpHelo(cfd)==-1);
			{
				perror("HELO is expected");
				exit(1);
			}
			if(smtpMail(cfd)==-1);
			{
				perror("MAIL FROM is expected");
				exit(1);
			}
			if(smtpRcpt(cfd)==-1);
			{
				perror("RCPT TO is expected");
				exit(1);
			}
			if(smtpData(cfd)==-1);
			{
				perror("DATA is expected");
				exit(1);
			}
			if(smtpQuit(cfd)==-1);
			{
				perror("QUIT is expected");
				exit(1);
			}
		}
	}
}

int popUser(int cfd)
{
	char buf[1000];
	memset(buf, 1000, 0);
	if(recv(cfd, buf, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	if(strncmp(buf, "user-name", strlen("user-name"))!=0)
		return -1;
	if(send(cfd, "OK", 2, 0)==-1)
	{
		perror("Server write failed");
		exit(1);
	}
	strcpy(file, direct);
	strcat(file, buf);
	return 1;
}

int popPass(int cfd)
{
	char buf[1000], pass[1000];
	memset(buf, 1000, 0);
	if(recv(cfd, buf, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	if(strncmp(buf, "password", strlen("password"))!=0)
		return -1;
	fp = fopen(file, "r");
	fscanf(fp, "%s", pass);
	if(strcmp(pass, buf)!=0)
		return -1;
	if(send(cfd, "OK", 2, 0)==-1)
	{
		perror("Server write failed");
		exit(1);
	}
	return 1;
}

int popList(int cfd)
{
	char buf[1000], pass[1000];
	memset(buf, 1000, 0);
	if(recv(cfd, buf, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	if(strncmp(buf, "list", strlen("list"))!=0)
		return -1;
	return 1;
}

int popRetr(int cfd)
{
	return 1;
}

void popServer(char* src)
{
	int sfd = makeNode(src, 21634, 0);
	struct sockaddr_in cli_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	memset(&cli_addr, 0, addrlen);
	pid_t child;
	while(1)
	{
		cfd = accept(sfd, (struct sockaddr *) &cli_addr, &addrlen);
		if(cfd==-1)
		{
			perror("Server: Accept failed");
			exit(1);
		}
		child = fork();
		if(child<0)
		{
			perror("Fork error");
			exit(1);
		}
		else if(child==0)
		{
			if(popUser(cfd)==-1);
			{
				perror("username is expected");
				exit(1);
			}
			if(popPass(cfd)==-1);
			{
				perror("password is expected");
				exit(1);
			}
			if(popList(cfd)==-1);
			{
				perror("list is expected");
				exit(1);
			}
			if(popRetr(cfd)==-1);
			{
				perror("retrieve is expected");
				exit(1);
			}
		}
	}
}

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		printf("Run the executable in <exec-path> <IP-of-server> format\n");
		return 0;
	}
	strcpy(direct, "./Data/");
	strcat(direct, argv[1]);
	strcat(direct, "/");
	char A[100];
	strcpy(A, argv[1]);
	strcpy(srvip, argv[1]);
	pid_t s;
	s = fork();
	if(s<0)
	{
		perror("Fork error");
		exit(1);
	}
	else if(s==0)
		smtpServer(A);
	else
		popServer(A);
	return 0;	
}