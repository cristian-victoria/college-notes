//Cristian Victoria
//COP 3330 Section 15
//Homework 1
//The class definition for diamonds 

#include <iostream>
#include <cmath>
#include <iomanip>
#include "diamond.h"

using namespace std;

Diamond::Diamond()
//Default constructor. Initiliazes diamond.
{
	size = 1;
	border = '#';
	fill = '*';
}

Diamond::Diamond(int s, char b, char f)
//Initializes diamond to s, b, and f
//Validates input of size
{
	size = s;
	border = b;
	fill = f;
	if(s<1){
		size = 1;
	}else if(s>39){
		size = 39;
	}
}

int Diamond::GetSize() const
{
	return size;
}

int Diamond::Perimeter() const
{
	return size*4;
}

double Diamond::Area() const
{
	return size*size*sqrt(3)/2;
}

void Diamond::Grow()
//Increments size
{
	if(size<39){
		size = size + 1;
	}
}

void Diamond::Shrink()
//Decrements size
{
	if(size>1){
                size = size - 1;
        }
}

void Diamond::SetBorder(char b)
//Set border to b and validates whether ASCII value in between 33 and 126
{
	if((b<33)||(b>126)){
		border = '#';
	}else{
		border = b; 
	}
}

void Diamond::SetFill(char f)
//Set fill to f and validates whether ASCII value in between 33 and 126
{	
	if((f<33)||(f>126)){
                fill = '*';
        }else{
                fill = f;  
        }
}

void Diamond::Draw() const
{
	int i, j, space;
	space = size - 1;
	for(i = 1; i<=size; i++){		//First half of the diamond
		for(j = 1; j<=space; j++){
			cout<<" ";	
		}
     		space--;	//Space decreases by 1 every line
		cout<<border<<" ";
        	for(j = 1; j<=i-2; j++){
		//Num of fill characters reduced by 2 to account for border characters
            		cout<<fill<<" ";
        	}
		if(i>1){
			cout<<border;		//First character does not require border ends
		}
		cout<<endl;
	}
	space = 1;
	for(i = 1; i<=size - 1; i++){ 		//Second half of the diamond
	        for(j = 1; j<=space; j++){
			cout<<" ";
		}
        	space++;	//Space increases by 1 every line
		cout<<border<<" ";
        	for(j = 1; j<=size-i-2; j++){
            		cout<<fill<<" ";
		}
		if(i!=size-1){
			cout<<border;		//Last character does not require border ends
		}
        	cout<<endl;
    	}
}

void Diamond::Summary() const
//Outputs all of the values of the diamond
{
	cout<<"Size of diamond's side = "<<GetSize()<<" units."<<endl;
	cout<<"Perimeter of diamond = "<<Perimeter()<<" units."<<endl;
	cout<<"Area of diamond = "<<fixed<<setprecision(2)<<Area()<<" units."<<endl;
	cout<<"Diamond looks like: "<<endl;
	Draw();
}
