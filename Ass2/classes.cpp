#include "classes.h"

Train::Train(int ac, nonac, type, cabins)
{
	num_ac_coach = ac;
	num_nonac_coach = nonac;
	tier = type;
	num_cabin = cabins;
	if(tier==2)
	{
		ac_avail = num_ac_coach*6*num_cabin;
		nonac_avail = num_nonac_coach*8*num_cabin;
	}
	else
	{
		ac_avail = num_ac_coach*8*num_cabin;
		nonac_avail = num_nonac_coach*8*num_cabin;
	}
	ac_reserved = 0;
	nonac_reserved = 0;
	ac_coaches.assign(ac, Coach(tier, num_cabin));
	nonac_coaches.assign(nonac, Coach(tier, num_cabin));
}

Coach::Coach(int tier, int num_cabin)
{
	int i;
	total = tier*num_cabin;
	reserved = 0;
	seats.clear();
	for(i=0;i<total;i++)
		seats.push_back(Seat(i+1, tier));
}

Seat::Seat(int pos, int type)
{
	booked = false;
	if(type==2)
	{
		switch(pos%6)
		{
			case 1:
				b = 1;
				break;
			case 2:
				b = 3;
				break;
			case 3:
				b = 1;
				break;
			case 4:
				b = 3;
				break;
			case 5:
				b = 4;
				break;
			case 0:
				b = 5;
				break;
		}
	}
	else
	{
		switch(pos%8)
		{
			case 1:
				b = 1;
				break;
			case 2:
				b = 2;
				break;
			case 3:
				b = 3;
				break;
			case 4:
				b = 1;
				break;
			case 5:
				b = 2;
				break;
			case 6:
				b = 3;
				break;
			case 7:
				b = 4;
				break;
			case 0:
				b = 5;
				break;
		}
	}
}