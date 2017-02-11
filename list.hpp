/* [list.hpp]
 * (C) Copyright 2000 Michael Blackney
 *
 * Defines a template class List.
 * Classes to be contained in a List should derive from the class Listable.
 * Such classes must have the method int type::Compare( type * )
 *
 */
#if !defined ( LIST_HPP )
#define LIST_HPP

#include "compiler.hpp"
#include "assert.h"

template <class ListClass>
class ListNode {
	public:
		ListNode( const ListClass& );
		ListClass &getData( void );
		ListNode<ListClass>* getNext( void ) const;
		ListNode<ListClass>* getPrevious( void ) const;
		void setNext( ListNode<ListClass> * );
		void setPrevious( ListNode<ListClass> * );
	private:
		ListNode<ListClass>* next;
		ListNode<ListClass>* previous;
		ListClass data;
	};

template <class ListClass>
class List {
	public:
		List( void );
		List( const List<ListClass> & );
		virtual ~List( void );      // calls clr();
		void clr( void );

		virtual void add( const ListClass& );
		virtual void add( const List<ListClass> & );

		ListClass &get( void ) const;
    bool canGet( void )    const;
		bool next( void )      const;
		bool previous( void )  const;
		bool reset( void )     const;
		bool toTail( void )    const;
    bool atHead( void )    const;
    bool atTail( void )    const;

		bool find( const ListClass& );

		long lock( void ); // Remember the 'current' position
		void load( long ); // Return to last locked position

		int numData( void ) const;
		ListClass remove( void ); // Removes & returns data currently pointed to.

		List<ListClass> &operator =( const List<ListClass> &);

	protected:
		ListNode<ListClass>
			*head,
			*tail;
		MUTABLE ListNode<ListClass>
			*current;

		int num_objs;
	};



// A macro to aid in freeing up pointer lists;
// removes all the items from the list and deletes them.
#define FreeList(l) {   \
	while ((l).reset())   \
		delptr( (l).remove() );\
	}


//--[ template class List implementation ]--//

#include "enum.hpp"
#include "standard.hpp"

template <class ListClass>
ListNode<ListClass>::ListNode (const ListClass &newList) : data(newList)	{
	next = previous = NULL;
	}
template <class ListClass>
ListClass &ListNode<ListClass>::getData( void ) {
	return data;
	}
template <class ListClass>
ListNode<ListClass>* ListNode<ListClass>::getNext( void ) const	{
	return next;
	}
template <class ListClass>
ListNode<ListClass>* ListNode<ListClass>::getPrevious( void ) const	{
	return previous;
	}
template <class ListClass>
void ListNode<ListClass>::setNext (ListNode<ListClass>* tmp)	{
	next = tmp;
	}
template <class ListClass>
void ListNode<ListClass>::setPrevious (ListNode<ListClass>* tmp)	{
	previous = tmp;
	}

//--------------------------------

template <class ListClass>
List<ListClass>::List( void )
: head(NULL), tail(NULL), current(NULL), num_objs(0)	{
	}

template <class ListClass>
List<ListClass>::List( const List<ListClass> &original )
: head(NULL), tail(NULL), current(NULL), num_objs(0){

	add( original );

	}

template <class ListClass>
void List<ListClass>::add( const List<ListClass> &original ) {

	if ( original.reset() )
	do {

		add( original.get() );

		} while ( original.next() );


	}

template <class ListClass>
List<ListClass> &List<ListClass>::operator =( const List<ListClass> &original) {

	clr();

	add( original );

	return *this;

	}

template <class ListClass>
List<ListClass>::~List( void )	{
	clr();
	}

template <class ListClass>
void List<ListClass>::clr( void )	{
	reset();
	while ( current )
			remove();
	}


template <class ListClass>
void List<ListClass>::add (const ListClass &data) {
		ListNode<ListClass>* tempNode;
		tempNode = new ListNode<ListClass> (data);
		if (tail)
		{
			tail->setNext (tempNode);
			tempNode->setPrevious (tail);
		}
		tail = tempNode;
		if (!head)
			head = current = tempNode;
		num_objs++;
	}
template <class ListClass>
ListClass &List<ListClass>::get( void ) const {

  assert(current);
	return current->getData ();
	}

template <class ListClass>
bool List<ListClass>::canGet( void ) const {
  return current;
  }

template <class ListClass>
bool List<ListClass>::next( void ) const	{
		List<ListClass> * const fakeThis = (List<ListClass> * const)this;
		if (current)
			fakeThis->current = current->getNext();
		if (current)
			return true;

		toTail();
		return false;
	}
template <class ListClass>
bool List<ListClass>::previous( void ) const	{
		List<ListClass> * const fakeThis = (List<ListClass> * const)this;
		if (current)
			fakeThis->current = current->getPrevious();
		if (current)
			return true;

		reset();
		return false;
	}
template <class ListClass>
bool List<ListClass>::reset( void )	const {
		List<ListClass> * const fakeThis = (List<ListClass> * const)this;
		fakeThis->current = head;
		return (current)?true:false;
	}
template <class ListClass>
bool List<ListClass>::toTail( void ) const	{
		List<ListClass> * const fakeThis = (List<ListClass> * const)this;
		fakeThis->current = tail;
		return (current)?true:false;
	}

template <class ListClass>
bool List<ListClass>::atHead( void ) const {
  return (current == head);
  }

template <class ListClass>
bool List<ListClass>::atTail( void ) const {
  return (current == tail);
  }


template <class ListClass>
long List<ListClass>::lock( void ) {
  ListNode<ListClass> *correct = current;

  long ret_val = -1;
  while (current) {
    current = current->getPrevious();
    ++ret_val;
    }

  current = correct;
  
  return ret_val;
	}
  
template <class ListClass>
void List<ListClass>::load( long key ) {

  current = head;
  long count = key;

  while ( count > 0 && current ) {
    current = current->getNext();
    --count;
    }

	}

template <class ListClass>
int List<ListClass>::numData( void ) const	{
		return num_objs;
	}
template <class ListClass>
ListClass List<ListClass>::remove( void )	{

		ListClass tempData = get();
		ListNode<ListClass> *nextData, *previousData;

		nextData = current->getNext();
		previousData=current->getPrevious();

		if (nextData)
				nextData->setPrevious( previousData );
		else
				tail = previousData;
		if (previousData)
				previousData->setNext( nextData );
		else
				head = nextData;
		num_objs--;
		delptr(current);
		current=nextData;
		if (!current) current = tail;
		return (tempData);
	}

template <class ListClass>
bool List<ListClass>::find( const ListClass &data ) {

	current = head;

	while ( current ) {
		if (current->getData() == data) return true;
		current = current->getNext();
		}

	return false;
	}

#endif // LIST_HPP
