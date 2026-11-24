//Cristian Victoria
//COP 3330 Section 15
//Homework 3
//The class definition for Time

#include <iostream>
#include "time.h"

using namespace std;

Time::Time()
{
	days = 0;
	hours = 0;
	minutes = 0;
	seconds = 0;
}

Time::Time(int s)
{
	if(s>0){
		minutes = s / 60;
		seconds = s % 60;
		if(minutes>59){
			hours = minutes / 60;
			minutes = minutes % 60;
			if(hours>23){
				days = hours / 24;
				hours = hours % 24;
			}
		}
	}else{
		days = 0;
        	hours = 0;
        	minutes = 0;
        	seconds = 0;
	}
}

Time::Time(int d, int h, int m, int s)
{
	if((d>0)&&(h>0)&&(m>0)&&(s>0)){
		days = d;
		hours = h;
		minutes = m;
		seconds = s;
		if(seconds > 59){
                        minutes = minutes + (seconds/60);
                        seconds = seconds % 60;
                }
		if(minutes > 59){
                        hours = hours + (minutes/60);
                        minutes = minutes % 60;
                }
		if(hours > 23){
			days = days + (hours/24);
			hours = hours % 24;
		}
	}else{
		days = 0;
                hours = 0;
                minutes = 0;
                seconds = 0;
	}
}

ostream& operator<<(ostream& os, const Time& t)
{
	if(t.hours<10){
		if(t.minutes<10){
			if(t.seconds<10){
				os<<t.days<<"~0"<<t.hours<<":0"<<t.minutes<<":0"<<t.seconds;
			}else{
				os<<t.days<<"~0"<<t.hours<<":0"<<t.minutes<<":"<<t.seconds;
			}
		}else if(t.seconds<10){
			os<<t.days<<"~0"<<t.hours<<":"<<t.minutes<<":0"<<t.seconds;
		}else{
			os<<t.days<<"~0"<<t.hours<<":"<<t.minutes<<":"<<t.seconds;
		}
	}else if(t.minutes<10){
		if(t.seconds<10){
			os<<t.days<<"~"<<t.hours<<":0"<<t.minutes<<":0"<<t.seconds;
		}else{
			os<<t.days<<"~"<<t.hours<<":0"<<t.minutes<<":"<<t.seconds;
		}
	}else if(t.seconds<10){
		os<<t.days<<"~"<<t.hours<<":"<<t.minutes<<":0"<<t.seconds;
	}else{
		os<<t.days<<"~"<<t.hours<<":"<<t.minutes<<":"<<t.seconds;
	}

	return os;
}

istream& operator>>(istream& is, Time& t)
{
	char dash, semicolonOne, semicolonTwo;
	is>>t.days>>dash>>t.hours>>semicolonOne>>t.minutes>>semicolonTwo>>t.seconds;
		
	if((t.days>0)&&(t.hours>0)&&(t.minutes>0)&&(t.seconds>0)){
		if(t.seconds > 59){
                        t.minutes = t.minutes + (t.seconds/60);
                        t.seconds = t.seconds % 60;
                }
		if(t.minutes > 59){
                        t.hours = t.hours + (t.minutes/60);
                        t.minutes = t.minutes % 60;
                }
                if(t.hours > 23){
                        t.days = t.days + (t.hours/24);
                        t.hours = t.hours % 24;
                }
        }else{
                t.days = 0;
                t.hours = 0;
                t.minutes = 0;
                t.seconds = 0;
        }

	return is;
}

Time operator+(const Time& t1, const Time& t2)
{
	return Time(t1.days+t2.days, t1.hours+t2.hours, t1.minutes+t2.minutes, t1.seconds+t2.seconds);
}

Time operator-(const Time& t1, const Time& t2)
{
	return Time(t1.days-t2.days, t1.hours-t2.hours, t1.minutes-t2.minutes, t1.seconds-t2.seconds);
}

Time operator*(const Time& t, int num)
{
	return Time(t.days*num, t.hours*num, t.minutes*num, t.seconds*num);
}

bool operator<(const Time& t1, const Time& t2)
{
	bool result = false;
	if(t1.days<t2.days){
		result = true;
	}else if(t1.days==t2.days){
		if(t1.hours<t2.hours){
			result = true;
		}else if(t1.hours==t2.hours){
			if(t1.minutes<t2.minutes){
				result = true;
			}else if(t1.seconds<t2.seconds){
				result = true;
			}
		}
	}
	return result;			
}

bool operator>(const Time& t1, const Time& t2)
{
	return (t2<t1);
}

bool operator<=(const Time&t1, const Time& t2)
{
	return !(t1>t2);
}

bool operator>=(const Time&t1, const Time& t2)
{
	return !(t1<t2);
}

bool operator==(const Time&t1, const Time& t2)
{
	if((t1<=t2)&&(t1>=t2)){
		return true;
	}else{
		return false;
	}
}

bool operator!=(const Time&t1, const Time& t2)
{
	return !(t1==t2);
}

Time& Time::operator++()	//prefix increment
{
	seconds = seconds + 1;
	if(seconds>59){
		minutes = minutes + 1;
		seconds = 0;
		if(minutes > 59){
			hours = hours + 1;
			minutes = 0;
			if(hours>24){
				days = days + 1;
				hours = 0;
			}
		}
	}
	return *this;
}

Time Time::operator++(int y)	//postfix increment
{
	Time temp = *this;
	seconds = seconds + 1;
	if(seconds>59){
                minutes = minutes + 1;
                seconds = 0;
                if(minutes > 59){
                        hours = hours + 1;
                        minutes = 0;
                        if(hours>24){
                                days = days + 1;
                                hours = 0;
                        }
                }
        }
	return temp;
}

Time& Time::operator--()
{
	if(seconds>0){
		seconds = seconds - 1;
	}else if(minutes>0){
		minutes = minutes - 1;
		seconds = 59;
	}else if(hours>0){
		hours = hours - 1;
		minutes = 59;
		seconds = 59;
	}else if(days>0){
		days = days - 1;
		hours = 23;
		minutes = 59;
		seconds = 59;
	}
	return *this;		
}

Time Time::operator--(int y)
{
	Time temp = *this;
	if(seconds>0){
                seconds = seconds - 1;
        }else if(minutes>0){
                minutes = minutes - 1;
                seconds = 59;
        }else if(hours>0){
                hours = hours - 1;
                minutes = 59;
                seconds = 59;
        }else if(days>0){
                days = days - 1;
                hours = 23;
                minutes = 59;
                seconds = 59;
        }
	return temp;
}
