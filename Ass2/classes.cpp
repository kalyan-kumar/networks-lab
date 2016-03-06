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

Ticket Train::getSeats(bool flagger, Booking y)
{
	vector<Coach> x;
	if(flagger)
		x = this->ac_coaches;
	else
		x = this->nonac_coaches;
	int num_coaches = x.size(), i, prob = -1, num_seats, j, here=0, tmp, t0, t1, t2, t3, t4, t5;
	Ticket final;
	final.s1.clear();
	for(i=0;i<num_coaches;i++)
	{
		if(((x[i].total)-(x[i].reserved)) >= y.berths)
		{
			tmp = 0;
			t0 = y.prefers[0];
			t1 = (y.prefers[1]-x[i].num_LB);
			t2 = (y.prefers[2]-x[i].num_MB);
			t3 = (y.prefers[3]-x[i].num_UB);
			t4 = (y.prefers[4]-x[i].num_SL);
			t5 = (y.prefers[5]-x[i].num_SU);
			if(t1 <= 0)
				tmp += y.prefers[1];
			else
				tmp += x[i].num_LB;
			if(t2 <= 0)
				tmp += y.prefers[2];
			else
				tmp += x[i].num_MB;
			if(t3 <= 0)
				tmp += y.prefers[3];
			else
				tmp += x[i].num_UB;
			if(t4 <= 0)
				tmp += y.prefers[4];
			else
				tmp += x[i].num_SL;
			if(t5 <= 0)
				tmp += y.prefers[5];
			else
				tmp += x[i].num_SU;
			if(tmp >= here)
			{
				here = tmp;
				prob = i;
			}
			if(t1<=0 && t2<=0 && t3<=0 && t4<=0 && t5<=0)
			{
				t0 = y.prefers[0];
				t1 = y.prefers[1];
				t2 = y.prefers[2];
				t3 = y.prefers[3];
				t4 = y.prefers[4];
				t5 = y.prefers[5];
				num_seats = x[i].seats.size();
				for(j=0;j<num_seats;j++)
				{
					if(x[i].seats[j].booked)
						continue;
					switch (x[i].seats[j].b)
					{
						case LB:
						{
							if(t1 > 0)
							{
								final.s1.push_back(make_pair(i+1, j+1));
								x[i].seats[j].booked = true;
								x[i].reserved++;
								x[i].num_LB--;
								t1--;
							}
							break;
						}
						case MB:
						{
							if(t2 > 0)
							{
								final.s1.push_back(make_pair(i+1, j+1));
								x[i].seats[j].booked = true;
								x[i].reserved++;
								x[i].num_MB--;
								t2--;
							}
							break;
						}
						case UB:
						{
							if(t3 > 0)
							{
								final.s1.push_back(make_pair(i+1, j+1));
								x[i].seats[j].booked = true;
								x[i].reserved++;
								x[i].num_UB--;
								t3--;
							}
							break;
						}
						case SL:
						{
							if(t4 > 0)
							{
								final.s1.push_back(make_pair(i+1, j+1));
								x[i].seats[j].booked = true;
								x[i].reserved++;
								x[i].num_SL--;
								t4--;
							}
							break;
						}
						case SU:
						{
							if(t5 > 0)
							{
								final.s1.push_back(make_pair(i+1, j+1));
								x[i].seats[j].booked = true;
								x[i].reserved++;
								x[i].num_SU--;
								t5--;
							}
							break;
						}
					}
				}
				for(j=0;j<num_seats && t0>0;j++)
				{
					if(x[i].seats[j].booked)
						continue;
					final.s1.push_back(make_pair(i+1, j+1));
					x[i].seats[j].booked = true;
					x[i].reserved++;
					switch (x[i].seats[j].b)
					{
						case LB:
							x[i].num_LB--;
							break;
						case MB:
							x[i].num_MB--;
							break;
						case UB:
							x[i].num_UB--;
							break;
						case SL:
							x[i].num_SL--;
							break;
						case SU:
							x[i].num_SU--;
							break;
					}
					t0--;
				}
				if(flagger)
				{
					final.ticket_type = 1;
					this->ac_coaches = x;
				}
				else
				{
					final.ticket_type = 0;
					this->nonac_coaches = x;
				}
				return final;
			}
		}
	}
	if(prob!=-1)
	{
		i = prob;
		num_seats = x[i].seats.size();
		t0 = y.prefers[0];
		t1 = y.prefers[1];
		t2 = y.prefers[2];
		t3 = y.prefers[3];
		t4 = y.prefers[4];
		t5 = y.prefers[5];		
		for(j=0;j<num_seats;j++)
		{
			if(x[i].seats[j].booked)
				continue;
			switch (x[i].seats[j].b)
			{
				case LB:
				{
					if(t1 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_LB--;
						t1--;
					}
					break;
				}
				case MB:
				{
					if(t2 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_MB--;
						t2--;
					}
					break;
				}
				case UB:
				{
					if(t3 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_UB--;
						t3--;
					}
					break;
				}
				case SL:
				{
					if(t4 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_SL--;
						t4--;
					}
					break;
				}
				case SU:
				{
					if(t5 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_SU--;
						t5--;
					}
					break;
				}
			}
		}
		tmp = t0+t1+t2+t3+t4+t5;
		for(j=0;j<num_seats && tmp>0;j++)
		{
			if(x[i].seats[j].booked)
				continue;
			final.s1.push_back(make_pair(i+1, j+1));
			x[i].seats[j].booked = true;
			x[i].reserved++;
			switch (x[i].seats[j].b)
			{
				case LB:
					x[i].num_LB--;
					break;
				case MB:
					x[i].num_MB--;
					break;
				case UB:
					x[i].num_UB--;
					break;
				case SL:
					x[i].num_SL--;
					break;
				case SU:
					x[i].num_SU--;
					break;
			}
			tmp--;
		}
		if(flagger)
		{
			final.ticket_type = 1;
			this->ac_coaches = x;
		}
		else
		{
			final.ticket_type = 0;
			this->nonac_coaches = x;
		}
		return final;
	}
	t0 = y.prefers[0];
	t1 = y.prefers[1];
	t2 = y.prefers[2];
	t3 = y.prefers[3];
	t4 = y.prefers[4];
	t5 = y.prefers[5];
	for(i=0;i<num_coaches;i++)
	{
		if(x[i].reserved==x[i].total)
			continue;
		num_seats = x[i].seats.size();
		for(j=0;j<num_seats;j++)
		{
			if(x[i].seats[j].booked)
				continue;
			switch (x[i].seats[j].b)
			{
				case LB:
				{
					if(t1 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_LB--;
						t1--;
					}
					break;
				}
				case MB:
				{
					if(t2 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_MB--;
						t2--;
					}
					break;
				}
				case UB:
				{
					if(t3 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_UB--;
						t3--;
					}
					break;
				}
				case SL:
				{
					if(t4 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_SL--;
						t4--;
					}
					break;
				}
				case SU:
				{
					if(t5 > 0)
					{
						final.s1.push_back(make_pair(i+1, j+1));
						x[i].seats[j].booked = true;
						x[i].reserved++;
						x[i].num_SU--;
						t5--;
					}
					break;
				}
			}
		}
	}
	tmp = t0+t1+t2+t3+t4+t5;
	for(i=0;i<num_coaches && tmp>0;i++)
	{
		if(x[i].reserved==x[i].total)
			continue;
		num_seats = x[i].seats.size();
		for(j=0;j<num_seats && tmp>0;j++)
		{
			if(x[i].seats[j].booked)
				continue;
			final.s1.push_back(make_pair(i+1, j+1));
			x[i].seats[j].booked = true;
			x[i].reserved++;
			switch (x[i].seats[j].b)
			{
				case LB:
					x[i].num_LB--;
					break;
				case MB:
					x[i].num_MB--;
					break;
				case UB:
					x[i].num_UB--;
					break;
				case SL:
					x[i].num_SL--;
					break;
				case SU:
					x[i].num_SU--;
					break;
			}
			tmp--;
		}
	}
	if(flagger)
	{
		final.ticket_type = 1;
		this->ac_coaches = x;
	}
	else
	{
		final.ticket_type = 0;
		this->nonac_coaches = x;
	}
	return final;
}

void Train::assignSeats(Booking x, Ticket &ret)
{
	//Ticket ret;
	ret.ticket_id = x.id;
	if(x.coach)
	{
		if(ac_avail < x.berths)
		{
			printf("Not enough seats available\n");
			return ;
			//return ret;
		}
		ac_avail -= x.berths;
		ac_reserved += x.berths;
		ret = this->getSeats(true, x);
	}
	else
	{
		if(nonac_avail < x.berths)
		{
			printf("Not enough seats available\n");
			return ;
			//return ret;
		}
		nonac_avail -= x.berths;
		nonac_reserved += x.berths;
		ret = this->getSeats(false, x);
	}
	//return ret;
}

Coach::Coach(int tier, int num_cabin)
{
	int i;
	if(tier==2)
		total = 6*num_cabin;
	else
		total = 8*num_cabin;
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

// Ticket::Ticket(const Ticket &t)
// {
// 	this->ticket_number = t.ticket_number;
// 	this->ticket_type = t.ticket_type;
// 	this->ticket_id = t.ticket_id;
// 	this->s1 = t.s1;
// }