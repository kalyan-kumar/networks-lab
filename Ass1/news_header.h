#ifndef NEWS_H
#define NEWS_H

#include <string>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

class article
{
public:
	bool academic;
	string heading;
	string date;
	string text;
	string file;
public:
	void readFromFile(char *title, bool category);
	void writeToFile();
	void print();
	bool dateCompare(article a,article b);
};

#endif