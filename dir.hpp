// [dir.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( DIR_HPP )
#define DIR_HPP

#include "point.hpp"
#include "list.hpp"

enum direction {
  d_northwest,
  d_north,
  d_northeast,
  d_west,
  d_ground,
  d_east,
  d_southwest,
  d_south,
  d_southeast,
  d_self
  };

template <class T>
struct bfProxy {
// Bitfield proxy struct.

  bfProxy( direction, T& );
  bfProxy &operator =( bool );
  operator bool();

  direction dir;
  T &ref;
  
  };

struct Dir {

	char nw:1, n:1, ne:1, w:1, ground:1, e:1, sw:1, s:1, se:1, self:1;

	Dir( void );
	void reset( bool v = false );

  bfProxy<Dir> operator[]( Point );

  void set( direction, char );
  char get( direction ) const;
  
	char total( void ) const;
  char totalOrthog( void ) const;
  char totalDiag( void ) const;

	Point toPoint( void ) const;
  List<Point> allPoints( void ) const;

  bool operator !( void ) const;
  Dir  invert( void ) const;

	};

struct Orthog {
  char n:1,w:1,e:1,s:1,ground:1,self:1;
  
  Orthog( void );
	void reset( bool v = false );

  bfProxy<Orthog> operator[]( Point );

  void set( direction, char );
  char get( direction ) const;
  
	char total( void ) const;

	Point toPoint( void ) const;

  bool operator !( void ) const;
  bool operator ==( const Orthog & ) const;
  bool operator !=( const Orthog & ) const;
  
  };


//--[ template class bfProxy ]--
template <class T>
bfProxy<T>::bfProxy<T>( direction d, T &r )
: dir(d), ref(r) { }

template <class T>
bfProxy<T> &bfProxy<T>::operator =( bool b ) {

  ref.set(dir, (char)b);
  return *this;
  }

template <class T>
bfProxy<T>::operator bool() {

  return (bool) ref.get(dir);
  }

#endif // DIR_HPP
