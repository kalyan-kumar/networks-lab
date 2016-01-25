#include <string>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

class article
{
private:
	bool academic;
	string heading;
	string date;
	string text;
public:
	void readFromFile(string title, bool category);
	void writeToFile();
	void print();
};