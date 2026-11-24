//Cristian Victoria
//COP 3330 Section 15
//Homework 7, Exercise 1
//Uses a Stack object to determine if a string is a palindrome

#include <iostream>
#include <cstring>
#include <cctype>
#include "stack.h"

using namespace std;

int main(){
	Stack< char > stringStack;
	char string[101], stringCopy[101], stringReverse[101];
	cout<<"Please enter a string:\n";
	cin.getline(string, 100);
	
        for(int i=0; i<strlen(string); i++){
                if(isalpha(string[i])!=0){
			stringCopy[i] = toupper(string[i]);
        		stringStack.push(stringCopy[i]);
		}
	}

	for(int i=0; i<strlen(stringCopy); i++){
		stringStack.pop(stringReverse[i]);
	}

	if(strcmp(stringCopy, stringReverse)==0){
		cout<<"\""<<string<<"\" IS a palindrome."<<endl;
	}else{
		cout<<"\""<<string<<"\" is NOT a palindrome."<<endl;
	}

	return 0;
}
