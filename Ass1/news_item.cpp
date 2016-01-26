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
		path = "Data/Academic/" + this->heading+".txt";
	else
		path = "Data/Non-Academic/" + this->heading+".txt";
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
bool article::dateCompare(article a, article b)
{
	int dd1, dd2, mm1, mm2, yy1, yy2;
	sscanf(a.date.c_str(), "%d/%d/%d", &dd1, &mm1, &yy1);
	sscanf(b.date.c_str(), "%d/%d/%d", &dd1, &mm1, &yy1);
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

