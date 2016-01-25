#include "news_header.h"

void article::readFromFile(char *title, bool category)
{
	ifstream myfile;
	char path[100];
	if(category)
	{
		strcpy(path, "Data/Academic/");
		strcat(path, title);
	}
	else
	{
		strcpy(path, "Data/Non-Academic/");
		strcat(path, title);
	}
	myfile.open(path);
	if(myfile.is_open())
	{
		getline(myfile, this->heading);
		getline(myfile, this->date);
		getline(myfile, this->text);
		myfile.close();
	}
	else
		cout << "Unable to open file.\n";
}

void article::writeToFile()
{
	ofstream myfile;
	string path;
	if(this->academic)
		path = "Data/Academic/" + this->heading;
	else
		path = "Data/Non-Academic/" + this->heading;
	myfile.open(path.c_str());
	if(myfile.is_open())
	{
		myfile << this->heading << "\n";
		myfile << this->date << "\n";
		myfile << this->text << "\n";
		myfile.close();
	}
	else 
		cout << "Unable write to file.\n";
}

void article::print()
{
	cout << "Heading - " << this->heading << "\n";
	cout << "Date - " << this->date << "\n";
	cout << "Content - " << this->text << "\n";
}
