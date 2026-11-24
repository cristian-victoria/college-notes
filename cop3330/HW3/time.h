//Cristian Victoria
//COP 3330 Section 15
//Homework 3
//Header file for time.h

#include <iostream>
using namespace std;

class Time
{
	//Operators for add and subtract
	friend Time operator+(const Time& t1, const Time& t2);
	friend Time operator-(const Time& t1, const Time& t2);
	
	//Operator for multiply with an int
	friend Time operator*(const Time& t, int num);

	//Comparison Operators
	friend bool operator<(const Time& t1, const Time& t2);
	friend bool operator>(const Time& t1, const Time& t2);
	friend bool operator<=(const Time& t1, const Time& t2);
	friend bool operator>=(const Time& t1, const Time& t2);
	friend bool operator==(const Time& t1, const Time& t2);
	friend bool operator!=(const Time& t1, const Time& t2);

	//Operators for extraction and insertion
	friend ostream& operator<<(ostream& os, const Time& t);
	friend istream& operator>>(istream& is, Time& t);

public:
	//Constructors
	Time();
	Time(int s);
	Time(int d, int h, int m, int s);

	//Operators for increment and decrement
	Time& operator++();	//prefix increment
	Time operator++(int y);	//postfix increment
	Time& operator--();	//prefix decrement
	Time operator--(int y);	//postfix increment

private:
	int days;
	int hours;
	int minutes;
	int seconds;
};
