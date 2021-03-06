#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "mysql_connection.h"
#include <driver.h>
#include <exception.h>
#include <string>
#include <cstring>
#include <resultset.h>
#include <statement.h>
#include <prepared_statement.h>
#include <iostream>
#include <vector>
using namespace std;

#define PORT 21635
#define POPPORT 21835
#define SMTPCLIENT 21735

char direct[1000], file[1000], srvip[1000];
string dom, to, from, other, pop_email, dom1;
int mk_cli, sm_cli;
FILE *fp;
sql::Driver *driver;
sql::Connection *con;
sql::Statement *stmt;
sql::ResultSet *res;
struct popemails
{
	string from,to,body;
	int id;

};
vector<struct popemails> allemails;
sql::PreparedStatement *pstmt;

int makeNode(const char* src, int port, int cors)
{
	printf("%s\n",src );
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
	char buf[2000];
	memset(buf, 0, 1000);
	if(recv(cfd, buf, 2000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	printf("%s\n",buf );
	if(strcmp(buf, "HELO")!=0)
	{
		fprintf(stderr,"here\n");
		return -1;
	}
	printf("%s\n",buf );
	if(send(cfd, "250 OK", 7, 0)==-1)
	{
		perror("Server write failed");
		exit(1);
	}
	return 1;
}

int smtpMail(int cfd)
{
	char buf[2000], email[1000];
	memset(buf, 0, 2000);
	if(recv(cfd, buf, 2000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	printf("%s\n", buf);
	if(strncmp(buf, "MAIL FROM:", strlen("MAIL FROM"))!=0)
		return -1;
	strcpy(email,strchr(buf,':')+2);
	from=email;
	if(send(cfd, "250 OK", 6, 0)==-1)
	{
		perror("Server write failed");
		exit(1);
	}
	return 1;	
}

int smtpRcpt(int cfd)
{
	char buf[1000], email[1000], domain[1000], frdom[1000];
	memset(buf, 0, 1000);
	if(recv(cfd, buf, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	printf("%s\n", buf);
	if(strncmp(buf, "RCPT TO", strlen("RCPT TO"))!=0)
		return -1;
	strcpy(email,strchr(buf,':')+2);
	strcpy(domain,strchr(buf,'@')+1);
	
	strcpy(file, direct);
	strcat(file, email);
	to=email;
	if(strcmp(domain, dom1.c_str())!=0)
		mk_cli = 1;
	if(mk_cli==1)
	{
		printf("asdfasdf\n");
		sm_cli = makeNode(other.c_str(), PORT, 1);
		char buf1[1000];
		memset(buf1, 0, 1000);
		if(recv(sm_cli, buf1, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
		printf("%s\n", buf1);
		if(send(sm_cli, "HELO", 4, 0)==-1)
		{
			perror("Server write failed");
			exit(1);
		}
		memset(buf1, 0, 1000);
		if(recv(sm_cli, buf1, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
		memset(buf1, 0, 1000);
		strcpy(buf1, "MAIL FROM: ");
		strcat(buf1, from.c_str());
		if(send(sm_cli, buf1, strlen(buf1), 0)==-1)
		{
			perror("Server write failed");
			exit(1);
		}
		memset(buf1, 0, 1000);
		if(recv(sm_cli, buf1, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
		if(send(sm_cli, buf, strlen(buf), 0)==-1)
		{
			perror("Server write failed");
			exit(1);
		}
		memset(buf1, 0, 1000);
		if(recv(sm_cli, buf1, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
		printf("%s\n", buf1);
	}
	printf("wrote in file\n");
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
	string message;
	memset(buf, 0, 1000);
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
		usleep(50);
		memset(buf1, 0, 1000);
		if(recv(sm_cli, buf1, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
		printf("%s\n", buf1);
		printf("%s\n", buf);
	}
	if(strncmp(buf, "DATA", strlen("DATA"))!=0)
		return -1;
	if(send(cfd, "354 start mail input", 20, 0)==-1)
	{
		perror("Server write failed");
		exit(1);
	}
	do
	{
		memset(buf, 0, 1000);
		if(recv(cfd, buf, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
		printf("%s\n", buf);
		message += buf;
		message += "\n";
		if(mk_cli==1)
		{
			if(send(sm_cli, buf, strlen(buf), 0)==-1)
			{
				perror("Server write failed");
				exit(1);
			}
		}
	}while(strcmp(buf, ".\n")!=0);
	string a="INSERT INTO "+dom+"(from_email,to_email,body,viewed) VALUES(?,?,?,?)";
	pstmt=con->prepareStatement(a.c_str());
	pstmt->setString(1,from);
	pstmt->setString(2,to);
	pstmt->setString(3,message);
	pstmt->setInt(4,0);
	pstmt->execute();
	memset(buf1, 0, 1000);
	if(mk_cli==1)
	{
		if(recv(sm_cli, buf1, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
	}
	if(send(cfd, "250 OK", 6, 0)!=-1)
	{
		printf("yo\n");
		// perror("Server write failed");
		// exit(1);
	}
	printf("sent it\n");
	return 1;
}

int smtpQuit(int cfd)
{
	char buf[1000], buf1[1000];
	memset(buf, 0, 1000);
	if(recv(cfd, buf, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	printf("came here\n");
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
		memset(buf1, 0, 1000);
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
	int sfd = makeNode(src, PORT, 0), cfd;
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
			if(smtpHelo(cfd) == -1)
			{
				perror("HELO is expected");
				exit(1);
			}
			if(smtpMail(cfd)==-1)
			{
				perror("MAIL FROM is expected");
				exit(1);
			}
			if(smtpRcpt(cfd)==-1)
			{
				perror("RCPT TO is expected");
				exit(1);
			}
			if(smtpData(cfd)==-1)
			{
				perror("DATA is expected");
				exit(1);
			}
			printf("Random \n");
			if(smtpQuit(cfd)==-1)
			{
				perror("QUIT is expected");
				exit(1);
			}
			exit(1);
		}
	}
}

int popUser(int cfd)
{
	char buf[1000];
	memset(buf, 0, 1000);
	if(recv(cfd, buf, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	printf("%s\n", buf);
	pop_email=buf;
	cout << pop_email << endl;
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
	printf("Came to password\n");
	char buf[1000], pass[1000];
	memset(buf, 0, 1000);
	if(recv(cfd, buf, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	printf("%s\n", buf);
	string a="select password from passwords where email=?";
	pstmt=con->prepareStatement(a.c_str());
	pstmt->setString(1,pop_email);
	res=pstmt->executeQuery();
	string password;
	while(res->next())
	{
		password=res->getString("password");
	}
	if(strcmp(password.c_str(),buf))
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
	memset(buf, 0,1000);
	if(recv(cfd, buf, 1000, 0) == -1)
	{
		printf("Didnt get what the client requested for\n");
		exit(1);
	}
	printf("%s\n", buf);
	if(strncmp(buf, "LIST", strlen("LIST"))!=0)
		return -1;
	cout << dom << endl;
	cout << pop_email << endl;
	string a="select * from "+dom+" where to_email=? and viewed= ?";
	pstmt=con->prepareStatement(a.c_str());
	pstmt->setString(1,pop_email);
	pstmt->setInt(2,0);

	res=pstmt->executeQuery();
	string to_send;
	int id=0;
	while(res->next())
	{
		printf("HEHHHE\n");
		struct popemails temp;
		temp.to=res->getString("to_email");
		temp.from=res->getString("from_email");
		temp.body=res->getString("body");
		temp.id=res->getInt("id");
		allemails.push_back(temp);
		to_send+=to_string(id)+ " "+temp.from+" "+to_string(temp.body.length())+"\n";
		id++;
	}
	printf("%s\n", to_send.c_str());
	if(to_send.length() == 0)
		to_send = "empty";
	if(send(cfd, to_send.c_str(), to_send.length(), 0)==-1)
	{
		perror("Server write failed");
		exit(1);
	}
	return 1;
}

int popRetr(int cfd)
{
	char buf[1000], pass[1000];
	memset(buf, 0, 1000);
	while(1)
	{
		memset(buf, 0, 1000);
		if(recv(cfd, buf, 1000, 0) == -1)
		{
			printf("Didnt get what the client requested for\n");
			exit(1);
		}
		printf("%s\n", buf);
		if(strcmp(buf,"END")==0)
			return 1;
		struct popemails temp=allemails[atoi(buf)];
		string sendmail="From : "+temp.from+"\n body : \n"+temp.body+"\n";
		if(send(cfd,sendmail.c_str(), sendmail.length(), 0)==-1)
		{
			perror("Server write failed");
			exit(1);
		}
		string a="update "+dom+" set viewed= 1 where id=?";
		pstmt=con->prepareStatement(a.c_str());
		pstmt->setInt(1,temp.id);
		pstmt->executeQuery();
	}
}

void popServer(char* src)
{
	int sfd = makeNode(src, POPPORT, 0);
	struct sockaddr_in cli_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	memset(&cli_addr, 0, addrlen);
	pid_t child;
	while(1)
	{
		int cfd = accept(sfd, (struct sockaddr *) &cli_addr, &addrlen);
		printf("GOT IT\n");
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
			if(popUser(cfd)==-1)
			{
				perror("username is expected");
				exit(1);
			}
			if(popPass(cfd)==-1)
			{
				perror("password is expected");
				exit(1);
			}
			if(popList(cfd)==-1)
			{
				perror("list is expected");
				exit(1);
			}
			if(popRetr(cfd)==-1)
			{
				perror("retrieve is expected");
				exit(1);
			}
		}
	}
}

string replaceStrChar(string str, const string& replace, char ch) {

	// set our locator equal to the first appearance of any character in replace
	size_t i = str.find_first_of(replace);
	int found;
	str[i]=ch;
	// while (found != string::npos) { // While our position in the sting is in range.
	//   str[found] = ch; // Change the character at position.
	//   found = str.find_first_of(replace, found+1); // Relocate again.
	// }

	return str; // return our new string.
}

int main(int argc, char* argv[])
{
	if(argc != 4)
	{
		printf("Run the executable in <exec-path> <IP-of-server> <domain-of-server> <IP-of-other-server>format\n");
		return 0;
	}
	try {
		driver = get_driver_instance();
		dom = argv[2];
		dom1 = dom;
		dom = replaceStrChar(dom,".",'_');
		cout << dom1 << " connected\n";
	 	con = driver->connect("tcp://127.0.0.1:3306", "root", "user12");
	 	
	 	stmt = con->createStatement();
	  	stmt->execute("CREATE DATABASE IF NOT EXISTS networks");
	  	/* Connect to the MySQL test database */
	 	con->setSchema("networks");
	 	char exec[100];
	 	sprintf(exec,"CREATE TABLE IF NOT EXISTS %s (id int not null auto_increment,from_email varchar(50),to_email varchar(50),body varchar(5000),viewed int not null, primary key(id))",dom.c_str());
	 	stmt->execute(exec);
 	}
 	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

	other = argv[3];
	strcpy(direct, "./Data/");
	strcat(direct, argv[1]);
	strcat(direct, "/");
	char A[100];
	strcpy(A, argv[1]);
	printf("%s\n", A);
	strcpy(srvip, argv[1]);
	pid_t s;
	s = fork();
	if(s<0)
	{
		perror("Fork error");
		exit(1);
	}
	else if(s==0)//fork for both
		smtpServer(A);
	else
		popServer(A);
	return 0;	
}