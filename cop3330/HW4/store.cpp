//Cristian Victoria
//COP 3330 Section 15
//Homework 4
//The class definition for Store

#include <iostream>
#include <iomanip>
#include <cstring>
#include "book.h"
#include "store.h"

using namespace std;

Store::Store()			
// Set up empty inventory of books and initiate total cash to 0
{ 
   	maxSize = 5; 
	currentSize = 0; 
   	bookInventory = new Book[maxSize];
	cashTotal = 0.0;
}

Store::~Store()
//Deallocate the inventory of book objects
{
	delete [] bookInventory;
}

void Store::Grow()
//Duble the size of the inventory by creating larger array
{
	maxSize = currentSize + 5;
	Book* newBookInventory = new Book[maxSize];
	for(int i=0; i<currentSize; i++){
		newBookInventory[i] = bookInventory[i];
	}
	delete [] bookInventory;
	bookInventory = newBookInventory;
}

void Store::InsertBook(const char* t, const char* a, Genre g, double p)
{
	if(currentSize == maxSize){
		Grow();
	}
	bookInventory[currentSize++].Set(t, a, g, p);
}

int Store::FindBook(const char* s) const
//Return index of book if found, return -1 if not
{
	int found = -1;
	for(int i=0; i<currentSize; i++){
		if((strcmp(bookInventory[i].GetTitle(), s)==0)){
			found = i;
		}
	}
	return found;
}


void Store::SellBook(const char* t)
//Increase total cash by price and shift inventory one position to the left to remove the book
{
	int thisBook = FindBook(t);
	if(thisBook == -1){
		cout<< t <<" not found in inventory."<<endl;
	}else{
		for(int i=thisBook+1; i<currentSize; i++){	//Shift array, thereby deleting element
			bookInventory[i-1] = bookInventory[i];
		}
		currentSize--;
		cashTotal += bookInventory[thisBook].GetPrice();
		cout<<"Book successfully sold. Your new cash balance is: "<<cashTotal<<endl;
	}
}

void Store::DisplayInventory() const
//Display the current inventory of books
{
	if(currentSize == 0){
		cout<<"Current inventory is empty."<<endl;
	}else{
		for(int i=0; i<currentSize; i++){
			bookInventory[i].Display();
		}
		cout<<"Total number of books: "<<currentSize<<endl;
	}
}

void Store::DisplayGenre(Genre g) const
//Display the current inventory of books based on genre
{
	bool isEmpty = 0;
	if(currentSize == 0){
                cout<<"Current inventory is empty."<<endl;
        }else{
                for(int i=0; i<currentSize; i++){
			if(bookInventory[i].GetGenre() == g){
				bookInventory[i].Display();
				isEmpty = 1;
			}
                }
        }
	if(isEmpty == 0){
		cout<<"There are no books under that genre."<<endl;
	}
}

void Store::LookupBook(const char* s) const
//Displays book
{
	bool match = 0;
	for(int i=0; i<currentSize; i++){
        	if((strcmp(bookInventory[i].GetTitle(), s)==0)||(strcmp(bookInventory[i].GetAuthor(), s)==0)){
			bookInventory[i].Display();
			match = 1;
                }
        }
	if(match == 0){
		cout<<"There are no books matching your input."<<endl;
	}
}
