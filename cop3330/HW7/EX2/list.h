//Cristian Victoria
//COP 3330 Section 15
//Homework 7, Exercise 2
//Template List class definition with the following functions made by myself:
//insertMiddle, removeMiddle, copy constructor, assignment operator

#ifndef LIST_H
#define LIST_H

#include <iostream>

using std::cout;

#include <new>
#include "listnode.h"  // ListNode class definition

template< class NODETYPE >
class List {

public:
	List();      // constructor
	~List();     // destructor
	void insertAtFront( const NODETYPE & );
	void insertAtBack( const NODETYPE & );
	bool removeFromFront( NODETYPE & );
	bool removeFromBack( NODETYPE & );
	bool isEmpty() const;
	void print() const;

	//The following functions are made by Cristian Victoria
	void insertMiddle(const NODETYPE & , int );
	bool removeMiddle( NODETYPE & , int );
	List(const List & );
	List& operator= (const List & );
	
private:
	ListNode< NODETYPE > *firstPtr;  // pointer to first node
   	ListNode< NODETYPE > *lastPtr;   // pointer to last node

   	// utility function to allocate new node
   	ListNode< NODETYPE > *getNewNode( const NODETYPE & );

}; // end class List

// default constructor
template< class NODETYPE >
List< NODETYPE >::List() 
   : firstPtr( 0 ), 
     lastPtr( 0 ) 
{ 
   // empty body

} // end List constructor

// destructor
template< class NODETYPE >
List< NODETYPE >::~List()
{
   if ( !isEmpty() ) {    // List is not empty
//      cout << "Destroying nodes ...\n";

      ListNode< NODETYPE > *currentPtr = firstPtr;
      ListNode< NODETYPE > *tempPtr;

      while ( currentPtr != 0 )         // delete remaining nodes
      {  
         tempPtr = currentPtr;

// commented out the output -- no need to print what we are deallocating
//         cout << tempPtr->data << '\n';  

         currentPtr = currentPtr->nextPtr;
         delete tempPtr;

      }

   }

//   cout << "All nodes destroyed\n\n";

} // end List destructor

// insert node at front of list
template< class NODETYPE >
void List< NODETYPE >::insertAtFront( const NODETYPE &value )
{
   ListNode< NODETYPE > *newPtr = getNewNode( value );

   if ( isEmpty() )  // List is empty
      firstPtr = lastPtr = newPtr;

   else {  // List is not empty
      newPtr->nextPtr = firstPtr;
      firstPtr = newPtr;

   } // end else

} // end function insertAtFront

// insert node at back of list
template< class NODETYPE >
void List< NODETYPE >::insertAtBack( const NODETYPE &value )
{
   ListNode< NODETYPE > *newPtr = getNewNode( value );

   if ( isEmpty() )  // List is empty
      firstPtr = lastPtr = newPtr;

   else {  // List is not empty
      lastPtr->nextPtr = newPtr;
      lastPtr = newPtr;

   } // end else

} // end function insertAtBack

// delete node from front of list
template< class NODETYPE >
bool List< NODETYPE >::removeFromFront( NODETYPE &value )
{
   if ( isEmpty() )  // List is empty
      return false;  // delete unsuccessful

   else {  
      ListNode< NODETYPE > *tempPtr = firstPtr;

      if ( firstPtr == lastPtr )
         firstPtr = lastPtr = 0;
      else
         firstPtr = firstPtr->nextPtr;

      value = tempPtr->data;  // data being removed
      delete tempPtr;

      return true;  // delete successful

   } // end else

} // end function removeFromFront

// delete node from back of list
template< class NODETYPE >
bool List< NODETYPE >::removeFromBack( NODETYPE &value )
{
   if ( isEmpty() )
      return false;  // delete unsuccessful

   else {
      ListNode< NODETYPE > *tempPtr = lastPtr;

      if ( firstPtr == lastPtr )
         firstPtr = lastPtr = 0;
      else {
         ListNode< NODETYPE > *currentPtr = firstPtr;

         // locate second-to-last element
         while ( currentPtr->nextPtr != lastPtr )
            currentPtr = currentPtr->nextPtr;

         lastPtr = currentPtr;
         currentPtr->nextPtr = 0;

      } // end else

      value = tempPtr->data;
      delete tempPtr;

      return true;  // delete successful

   } // end else

} // end function removeFromBack

// is List empty?
template< class NODETYPE > 
bool List< NODETYPE >::isEmpty() const 
{ 
   return firstPtr == 0; 
   
} // end function isEmpty

// return pointer to newly allocated node
template< class NODETYPE >
ListNode< NODETYPE > *List< NODETYPE >::getNewNode( 
   const NODETYPE &value )
{
   return new ListNode< NODETYPE >( value );

} // end function getNewNode

// display contents of List
template< class NODETYPE >
void List< NODETYPE >::print() const
{
   if ( isEmpty() ) {
      cout << "The list is empty\n\n";
      return;

   } // end if

   ListNode< NODETYPE > *currentPtr = firstPtr;

   cout << "The list is: ";

   while ( currentPtr != 0 ) {
      cout << currentPtr->data << ' ';
      currentPtr = currentPtr->nextPtr;

   } // end while

   cout << "\n\n";

} // end function print

//-------------------------------------------
//------- MY FUNCTION DEFINITIONS -----------
//-------------------------------------------

template< class NODETYPE >
void List< NODETYPE >::insertMiddle(const NODETYPE &value, int pos)
{
	ListNode< NODETYPE > *newPtr = getNewNode( value );
	if( isEmpty() ){
		firstPtr = lastPtr = newPtr;
	}else if(pos < 1){
		insertAtFront(value);
	}else{
		ListNode< NODETYPE > *previousPtr = firstPtr;
		ListNode< NODETYPE > *postPtr = firstPtr->nextPtr;
		
		int count = 1;
		while((count!=pos-1)&&(previousPtr!=lastPtr)){
			previousPtr = previousPtr->nextPtr;
			count++;
		}
		if(previousPtr == lastPtr){
			insertAtBack(value);
		}else{
			postPtr=previousPtr;
			postPtr=postPtr->nextPtr;
			newPtr->nextPtr=postPtr;
			previousPtr->nextPtr=newPtr;
		}
		
	}

}

template< class NODETYPE >
bool List< NODETYPE >::removeMiddle( NODETYPE &value, int pos)
{
	if(( isEmpty() )||(pos<1)){
		return false;
	}else if(pos == 1){
		removeFromFront(value);
		return true;
	}else{
		ListNode< NODETYPE > *tempPtr = firstPtr;

		if(firstPtr==lastPtr){
			firstPtr = lastPtr = 0;
		}else{
			ListNode< NODETYPE > *previousPtr = firstPtr;
			ListNode< NODETYPE > *postPtr = firstPtr->nextPtr;
			int count = 1;
			while((count!=pos-1)&&(previousPtr!=lastPtr)){
				previousPtr=previousPtr->nextPtr;
				count++;
			}
			if(previousPtr == lastPtr){
				if(count<pos){
					return false;
				}else{
					removeFromBack(value);
					return true;
				}
			}else{
	                        tempPtr=previousPtr->nextPtr;
				postPtr=tempPtr->nextPtr;
				previousPtr->nextPtr=postPtr;
			}
		}
		value = tempPtr->data;
		delete tempPtr;

		return true;
	}
}

template< class NODETYPE >
List< NODETYPE >::List(const List &L )
{
	if( !L.isEmpty() ){
		ListNode< NODETYPE > *LPtr = L.firstPtr;
		insertAtFront(LPtr->getData());

		LPtr=LPtr->nextPtr;

		while(LPtr!=0){
			insertAtBack(LPtr->getData());
			LPtr=LPtr->nextPtr;
		}
	}else{
		firstPtr = lastPtr = 0;	
	}
}

template< class NODETYPE >
List< NODETYPE >& List< NODETYPE >::operator= (const List &L )
{
	if(this != &L){
		ListNode< NODETYPE > *currentPtr = firstPtr;
		ListNode< NODETYPE > *tempPtr;
		while ( currentPtr != 0 ){
			tempPtr = currentPtr;
			currentPtr = currentPtr->nextPtr;
			delete tempPtr;
		}
		if( !L.isEmpty() ){
			ListNode< NODETYPE > *LPtr = L.firstPtr;
                	insertAtFront(LPtr->getData());

                	LPtr=LPtr->nextPtr;

                	while(LPtr!=0){
                        	insertAtBack(LPtr->getData());
                        	LPtr=LPtr->nextPtr;
                	}
		}else{
			firstPtr = lastPtr = 0;
		}
	}
}

#endif
