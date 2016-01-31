#include "classes.h"

Train::Train(int ac, int nonac, int type, int cabins)
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
	num_LB = num_cabin*2;
	num_MB = ((tier == 3) ? num_cabin*2 : 0);
	num_UB = num_cabin*2;
	num_SL = num_cabin;
	num_SU = num_cabin;
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
				b = LB;
				break;
			case 2:
				b = UB;
				break;
			case 3:
				b = LB;
				break;
			case 4:
				b = UB;
				break;
			case 5:
				b = SL;
				break;
			case 0:
				b = SU;
				break;
		}
	}
	else
	{
		switch(pos%8)
		{
			case 1:
				b = LB;
				break;
			case 2:
				b = MB;
				break;
			case 3:
				b = UB;
				break;
			case 4:
				b = LB;
				break;
			case 5:
				b = MB;
				break;
			case 6:
				b = UB;
				break;
			case 7:
				b = SL;
				break;
			case 0:
				b = SU;
				break;
		}
	}
}