//Cristian Victoria
//COP 3330 Section 15
//Homework 4
//The class definition for Book

#include <iostream>
#include <iomanip>
#include <cstring>
#include "book.h"

using namespace std;

Book::Book(){
	strcpy(title, " ");
	strcpy(author, " ");
	price = 0.0;
}

void Book::Set(const char* t, const char* a, Genre g, double p){
	strcpy(title, t);
	strcpy(author, a);
	type = g;
	price = p;
}

const char* Book::GetTitle() const{
	return title;
}

const char* Book::GetAuthor() const{
	return author;
}

double Book::GetPrice() const{
	return price;
}

Genre Book::GetGenre() const{
	return type;
}

void Book::Display() const{
	const int Width = 20;	//Set width to 20 to display in table format
	char genre[20];		//C-string used to display the genre
	switch(GetGenre()){
		case 0:
			strcpy(genre, "Fiction");
			break;
		case 1:
                        strcpy(genre, "Mystery");
                        break;
		case 2:
                        strcpy(genre, "Sci-fi");
                        break;
		case 3:
                        strcpy(genre, "Computer");
                        break;
		default:
			strcpy(genre, " ");
			break;	
	}
	cout<<left<<setw(Width)<<GetTitle()		//Display title
	<<left<<setw(Width)<<GetAuthor()		//Display author
	<<left<<setw(Width)<<genre			//Display genre
	<<"$"<<fixed<<setprecision(2)<<GetPrice()<<endl;//Display price
}


