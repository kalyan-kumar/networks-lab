all: Server client

Server: server-garuda.cpp
	sudo g++ -std=c++11  -g -I/usr/include/cppconn -o Server server-garuda.cpp -L/usr/lib -lmysqlcppconn


client: client-garuda.cpp
	g++ client-garuda.cpp -o client

clean:
	rm serG cliG