#ifndef CLASSES_H
#define CLASSES_H

#include <string>
#include <utility>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

using namespace std;

enum BERTH_TYPE{
	NA = 0,
	LB,
	MB,
	UB,
	SL,
	SU
};

class Booking
{
public:
	int route, berths, seniors, cli_fd;
	bool coach;
	vector<int> prefers;
	vector<int> ages;
};

class Ticket
{
public:
	vector<pair<int,int> > s1;
};

class Seat
{
public:
	bool booked;
	BERTH_TYPE b;
	Seat(int pos, int type);
};

class Coach
{
public:
	int total, reserved, num_LB, num_MB, num_UB, num_SL, num_SU;
	std::vector<Seat> seats;
	Coach(int tier, int num_cabin);
};

class Train
{
public:
	int num_ac_coach, num_nonac_coach, tier, num_cabin;
	int ac_avail, nonac_avail, ac_reserved, nonac_reserved;
	vector<Coach> ac_coaches, nonac_coaches;
	Train(int ac, int nonac, int type, int cabins);
	Ticket getSeats(bool flagger, Booking y);
	Ticket assignSeats(Booking x);
};

#endif