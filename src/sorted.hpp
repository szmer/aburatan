// [sorted.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( SORTED_HPP )
#define SORTED_HPP

#include "list.hpp"

template <class ListClass>
class Sorted : public List<ListClass> {
	public:
		Sorted( void );
		Sorted( const List<ListClass> & );
		virtual void add( const ListClass& );
		virtual void add( const List<ListClass> & );
		virtual void add( const Sorted<ListClass> & );

		Sorted<ListClass> &operator =( const List<ListClass> &);
    // To avoid stupid synthesis issues:
		Sorted<ListClass> &operator =( const Sorted<ListClass> &);
	};

//--[ template class Sorted implementation ]--//
//#define SORTED__TEST

template <class ListClass>
Sorted<ListClass>::Sorted( void )
: List<ListClass>() {
	}


template <class ListClass>
Sorted<ListClass>::Sorted( const List<ListClass> &original )
: List<ListClass>() {

	add( original );

	}

template <class ListClass>
void Sorted<ListClass>::add( const List<ListClass> &original ) {

	if ( original.reset() )
	do {

		add( original.get() );

		} while ( original.next() );

	}

template <class ListClass>
void Sorted<ListClass>::add( const Sorted<ListClass> &original ) {

	if ( original.reset() )
	do {

		add( original.get() );

		} while ( original.next() );

	}

template <class ListClass>
Sorted<ListClass> &Sorted<ListClass>::operator =( const List<ListClass> &original) {

	List<ListClass>::clr();

	add( original );

	return *this;
	}

template <class ListClass>
Sorted<ListClass> &Sorted<ListClass>::operator =( const Sorted<ListClass> &original) {

	List<ListClass>::clr();

	add( original );

	return *this;
	}


template <class ListClass>
void Sorted<ListClass>::add( const ListClass &data ) {

#ifdef SORTED__TEST
	Screen::write( String("Adding ") + data);
#endif // SORTED__TEST

	ListNode<ListClass>* tempNode, *previous_node = NULL, *next_node = NULL;
	tempNode = new ListNode<ListClass> (data);

	if ( List<ListClass>::reset() ) {

		while ( List<ListClass>::current && !( List<ListClass>::current->getData() > data ) )
			List<ListClass>::current = List<ListClass>::current->getNext();

		if (List<ListClass>::current) previous_node = List<ListClass>::current->getPrevious();
		else         previous_node = List<ListClass>::tail;
		next_node = List<ListClass>::current;

		}

#ifdef SORTED__TEST
	if (previous_node) {

		Screen::write( String(" after " ) + previous_node->getData());
		if (next_node) Screen::write(" and");
		else Screen::write(".\n\r");
		} else if (!next_node) Screen::write(".\n\r");

	if (next_node)
		Screen::write( String(" before ") + next_node->getData() + ".\n\r");
#endif // SORTED__TEST


	if (previous_node)		previous_node->setNext( tempNode );
	else            List<ListClass>::head = tempNode;

	tempNode->setPrevious( previous_node );
	tempNode->setNext( next_node );

	if (next_node) next_node->setPrevious( tempNode );
	else      List<ListClass>::tail = tempNode;


	List<ListClass>::current = tempNode;
	List<ListClass>::num_objs++;

	}


#endif // SORTED_HPP
