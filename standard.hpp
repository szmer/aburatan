// [standard.hpp]
// (C) Copyright 2000 Michael Blackney
// Contains template functions and pointer macros.
#if !defined ( STANDARD_HPP )
#define STANDARD_HPP

#include <cstdlib>

template <class T>
char inbounds( const T &test,  const T &lowValue, const T &hiValue ) {
	return (test >= lowValue) && (test <= hiValue);
	}

template <class A, class B, class C>
int ascending( const A &a,  const B &b, const C &c ) {
	return (a < b) && (b < c);
	}

template <class A, class B, class C>
int descending( const A &a,  const B &b, const C &c ) {
	return (a > b) && (b > c);
	}



template <class T>
void swap( T& a, T& b ) {
	T c = a;
	a = b;
	b = c;
	}

template <class T>
T min( T a, T b ) {
	if (a < b) return a;
	return b;
	}

template <class T>
T max( T a, T b ) {
	if (a > b) return a;
	return b;
	}

template <class T>
T abs( T a ) {
	if (a < 0) return -a;
	return a;
	}


#define COUNT_DELETES

int printTotalDeletes( void );

#ifdef COUNT_DELETES

#  include <string>
   void * operator new(size_t size) throw (std::bad_alloc);
   void operator delete(void * mem) throw ();
   void incTotalAllocs( void );
   void incTotalAllocsArr( void );
   void incTotalDeletes( void );
   void incTotalDeletesArr( void );
#  define delthis(myptr)	{delete myptr;}
#  define delptr(myptr)	{delete myptr;myptr=NULL;}
#  define delarr(myarr)	{delete [] myarr;myarr=NULL;}
//#  define delthis(myptr)	{delete myptr;incTotalDeletes();}
//#  define delptr(myptr)	  {delete myptr;myptr=NULL;incTotalDeletes();}
//#  define delarr(myarr)	  {delete [] myarr;myarr=NULL;incTotalDeletesArr();}
#else
#  define incTotalAllocs();
#  define incTotalAllocsArr();
#  define delthis(myptr)	{delete myptr;}
#  define delptr(myptr)	{delete myptr;myptr=NULL;}
#  define delarr(myarr)	{delete [] myarr;myarr=NULL;}
#endif


#endif // STANDARD_HPP

