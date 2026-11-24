//Cristian Victoria
//COP 3330 Section 15
//Homework 4
//Header file for store.h

#include <iostream>
#include "book.h"
using namespace std;

class Store
{
public:
	Store();			//Initiate cashTotal and empty inventory of books
	~Store();			//Deallocate the entry list of books
	void InsertBook(const char* t, const char* a, Genre g, double p); //Insert book into inventory
	void SellBook(const char* t);	//Sell a book from inventory
	void DisplayInventory() const;	//Display the current book inventory
	void DisplayGenre(Genre g) const;//Display the curreny book inventory matching a genre 
	void LookupBook(const char* s) const;	//Display book searched
		
private:
	double cashTotal;	//Total amount of money in cash register
	
	int maxSize;		//Maximum allowable number of entries
	int currentSize;	//Current number of entries
	Book* bookInventory;	//Pointer to the list of entries	
	void Grow();		//Increase maximum size, when required.
	
	int FindBook(const char* t) const;	//Return index of an entry, given the title of book
};
