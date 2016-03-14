all: serG cliG

serG: server-garuda.cpp
	sudo g++ -std=c++11  -g -I/usr/include/cppconn -o Server server-garuda.cpp -L/usr/lib -lmysqlcppconn


cliG: client-garuda.cpp
	g++ client-garuda.cpp -o client

clean:
	rm serG cliG