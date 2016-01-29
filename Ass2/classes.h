#include <string>
#include <stdlio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

enum BERTH_TYPE{
	NA = 0,
	LB,
	MB,
	UB,
	SL,
	SU
};

class Seat
{
private:
	bool booked;
	BERTH_TYPE b;
public:
	Seat(int pos, int type);
};

class Coach
{
private:
	int total, reserved;
	std::vector<Seat> seats;
public:
	Coach();
};

class Train
{
public:
	int num_ac_coach, num_nonac_coach, tier, num_cabin;
	int ac_avail, nonac_avail, ac_reserved, nonac_reserved;
	vector<Coach> ac_coaches, nonac_coaches;
	Train(int ac, nonac, type, cabins);
};

class Booking
{
	int route, berths, seniors;
	bool coach;
	vector<BERTH_TYPE> prefers;
	vector<int> ages;
};



