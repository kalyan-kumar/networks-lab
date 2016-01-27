all: server reader reporter administrator

server: news_server.cpp news_item.cpp
	g++ -std=c++11 news_server.cpp news_item.cpp -o server

reader: reader.cpp
	g++ -std=c++11 reader.cpp -o reader

reporter: reporter.cpp
	g++ -std=c++11 reporter.cpp -o reporter

administrator: administrator.cpp
	g++ administrator.cpp -o administrator

clean:
	rm server reader reporter administrator