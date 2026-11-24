//Cristian Victoria
//COP 3330 Section 15
//Homework 4

#include <iostream>
#include <iomanip>
#include <cstring>
#include <cctype>
#include "store.h"
#include "book.h"

using namespace std;

void ShowMenu()
//Display the main program menu
{
        cout << "\t*** STORE MENU ***"<<endl;
        cout << "A: \tAdd a book to the inventory"<<endl;
        cout << "F: \tFind a book from inventory"<<endl;
        cout << "S: \tSell a book"<<endl;
        cout << "D: \tDisplay the inventory list"<<endl;
        cout << "G: \tGenre summary"<<endl;
        cout << "M: \tShow this Menu"<<endl;
        cout << "X: \tExit the program"<<endl;
}

bool isLegalCommand(char c)
//Determine if the character is legal
{
        bool result = 0;
        if((c=='A')||(c=='F')||(c=='S')||(c=='D')||(c=='G')||(c=='M')||(c=='X')){
                result = 1;
        }
        return result;
}

char GetCommand()
//Prompts user for a menu command until a legal character is entered
//Returns the command character
{
        char cmd;
        cout<<"Please enter a command."<<endl;
        cin>>cmd;
        cmd = toupper(cmd);
        while(isLegalCommand(cmd) == 0){
                cout<<"Invalid command. Please try again."<<endl;
                cin>>cmd;
                cmd = toupper(cmd);
        }

        return cmd;
}

bool isLegalGenre(char g)
//Determine if the genre character is legal
{
        bool result = 0;
        if((g=='F')||(g=='M')||(g=='S')||(g=='S')){
                result = 1;
        }
        return result;
}

Genre AskGenre()
//Insert character corresponding to a genre. Error check validation.
{
	Genre type;
	char g;
	cin>>g;
	g = toupper(g);
	while(isLegalGenre(g)==0){
		cout<<"Invalid genre. Please try again."<<endl;
		cin>>g;
		g = toupper(g);
	}
	switch(g){
		case 'F':
			type = FICTION;
			break;
		case 'M':
                        type = MYSTERY;
                        break;
		case 'S':
                        type = SCIFI;
                        break;
		case 'C':
                        type = COMPUTER;
                        break;	
	}
	return type;
}

double AskPrice()
//Insert price. Must be above 0.
{
	double p;
	cin>>p;
	while(p<0){
		cout<<"Invalid price. Please try again."<<endl;
		cin>>p;
	}
	return p;
}

int main(){
        char bookTitle[31], bookAuthor[21], bookTitleOrAuthor[31];
	Genre bookGenre;
        double bookPrice;

        Store s;

        ShowMenu();

        char command;
        do{
                command = GetCommand();
                if(command == 'A'){
			cout<<"Please enter the title of the book."<<endl;
			cin.ignore();
			cin.getline(bookTitle, 31);
			cout<<"Please enter the author of the book."<<endl;
                        cin.getline(bookAuthor, 31);
			cout<<"Please enter the genre of the book."<<endl;
			bookGenre = AskGenre();
			cout<<"Please enter the price of the book."<<endl;
                        bookPrice = AskPrice();
			s.InsertBook(bookTitle, bookAuthor, bookGenre, bookPrice);
                }else if(command == 'F'){
			cout<<"Please enter the title/author of the book."<<endl;
                        cin.ignore();
			cin.getline(bookTitleOrAuthor, 31);
			s.LookupBook(bookTitleOrAuthor);
                }else if(command == 'S'){
                        cout<<"Please enter the title of the book."<<endl;
                        cin.ignore();
			cin.getline(bookTitle, 31);
                        s.SellBook(bookTitle);
                }else if(command == 'D'){
                        s.DisplayInventory();
                }else if(command == 'G'){
			cout<<"Please enter the genre of the book."<<endl;
                        bookGenre = AskGenre();
                        s.DisplayGenre(bookGenre);
                }else if(command == 'M'){
                        ShowMenu();
                }
        }while(command != 'X');

        return 0;
}
