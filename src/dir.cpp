// [dir.cpp]
// (C) Copyright 2000-2002 Michael Blackney
/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 **/
#if !defined ( DIR_CPP )
#define DIR_CPP

#include "dir.hpp"
#include "point.hpp"

//--[ class Dir ]--
Dir::Dir( void ) {
	reset();
	}

void Dir::reset( bool v ) {
	nw=n=ne=
	w=ground=e=
	sw=s=se=self= v;
	}

bfProxy<Dir> Dir::operator[]( Point p ) {

	if ( p.gety() < 0 )
		if ( p.getx() < 0 )
			return bfProxy<Dir>(d_northwest,*this);
		else if ( p.getx() == 0)
			return bfProxy<Dir>(d_north,*this);
		else // ( p.getx() > 0 )
			return bfProxy<Dir>(d_northeast,*this);

	else if ( p.gety() == 0)
		if ( p.getx() < 0 )
			return bfProxy<Dir>(d_west,*this);
		else if ( p.getx() == 0)
			return bfProxy<Dir>(d_ground,*this);
		else // ( p.getx() > 0 )
			return bfProxy<Dir>(d_east,*this);

	else // ( p.gety() > 0 )
		if ( p.getx() < 0 )
			return bfProxy<Dir>(d_southwest,*this);
		else if ( p.getx() == 0)
			return bfProxy<Dir>(d_south,*this);
		else // ( p.getx() > 0 )
			return bfProxy<Dir>(d_southeast,*this);

	}

char Dir::total( void ) const {

	return abs(nw+n+ne+w+ground+e+sw+s+se+self);

	}

char Dir::totalOrthog( void ) const {

	return abs(n+w+e+s);

	}

char Dir::totalDiag( void ) const {

	return abs(nw+ne+sw+se);

	}

Point Dir::toPoint( void ) const {

	if (nw ) return Point(-1, -1);
	if ( n ) return Point( 0, -1);
	if (ne ) return Point( 1, -1);

	if ( w ) return Point(-1,  0);
	if ( e ) return Point( 1,  0);

	if (sw ) return Point(-1,  1);
	if ( s ) return Point( 0,  1);
	if (se ) return Point( 1,  1);

	return Point( 0,  0);
	}

List<Point> Dir::allPoints( void ) const {

  List<Point> ret_list;
  
	if (nw ) ret_list.add( Point(-1, -1) );
	if ( n ) ret_list.add( Point( 0, -1) );
	if (ne ) ret_list.add( Point( 1, -1) );

	if ( w ) ret_list.add( Point(-1,  0) );
	if ( e ) ret_list.add( Point( 1,  0) );

	if (sw ) ret_list.add( Point(-1,  1) );
	if ( s ) ret_list.add( Point( 0,  1) );
	if (se ) ret_list.add( Point( 1,  1) );

  return ret_list;
  }


void Dir::set( direction d, char c ) {

  switch (d) {

    case d_northwest: nw = c; break;
    case d_north:     n  = c; break;
    case d_northeast: ne = c; break;
    case d_west:      w  = c; break;
    case d_east:      e  = c; break;
    case d_southwest: sw = c; break;
    case d_south:     s  = c; break;
    case d_southeast: se = c; break;
    default:
      ground = c;
      self = c;
      break;

    }

  }
  
char Dir::get( direction d ) const {

  switch (d) {

    case d_northwest: return nw;
    case d_north:     return n ;
    case d_northeast: return ne;
    case d_west:      return w ;
    case d_east:      return e ;
    case d_southwest: return sw;
    case d_south:     return s ;
    case d_southeast: return se;
    default:
      return ground;

    }

  }

bool Dir::operator !( void ) const {
  return total()==0;
  }

Dir Dir::invert( void ) const {
  Dir ret_dir = *this;
  
	ret_dir.nw = 1 - ret_dir.nw;
  ret_dir.n  = 1 - ret_dir.n;
  ret_dir.ne = 1 - ret_dir.ne;
	ret_dir.w  = 1 - ret_dir.w;
  ret_dir.e  = 1 - ret_dir.e;
	ret_dir.sw = 1 - ret_dir.sw;
  ret_dir.s  = 1 - ret_dir.s;
  ret_dir.se = 1 - ret_dir.se;
  ret_dir.ground = 1 - ret_dir.ground;
  ret_dir.self   = 1 - ret_dir.self;

  return ret_dir;
  }
  
//--[ class Orthog ]--
Orthog::Orthog( void ) {
	reset();
	}

void Orthog::reset( bool v ) {
	n=
	w=e=
	s=self=ground= v;
	}

bfProxy<Orthog> Orthog::operator[]( Point p ) {

	if ( p.gety() < 0 )
		if ( p.getx() < 0 )
			return bfProxy<Orthog>(d_northwest,*this);
		else if ( p.getx() == 0)
			return bfProxy<Orthog>(d_north,*this);
		else // ( p.getx() > 0 )
			return bfProxy<Orthog>(d_northeast,*this);

	else if ( p.gety() == 0)
		if ( p.getx() < 0 )
			return bfProxy<Orthog>(d_west,*this);
		else if ( p.getx() == 0)
			return bfProxy<Orthog>(d_ground,*this);
		else // ( p.getx() > 0 )
			return bfProxy<Orthog>(d_east,*this);

	else // ( p.gety() > 0 )
		if ( p.getx() < 0 )
			return bfProxy<Orthog>(d_southwest,*this);
		else if ( p.getx() == 0)
			return bfProxy<Orthog>(d_south,*this);
		else // ( p.getx() > 0 )
			return bfProxy<Orthog>(d_southeast,*this);

	}

char Orthog::total( void ) const {

	return abs(n+w+ground+e+s+self);

	}

Point Orthog::toPoint( void ) const {

	if ( n ) return Point( 0, -1);
	if ( w ) return Point(-1,  0);
	if ( e ) return Point( 1,  0);
	if ( s ) return Point( 0,  1);

	return Point( 0,  0);
	}

void Orthog::set( direction d, char c ) {

  switch (d) {

    case d_northwest: n = c; w = c; break;
    case d_north:     n  = c; break;
    case d_northeast: n = c; e = c; break;
    case d_west:      w  = c; break;
    case d_east:      e  = c; break;
    case d_southwest: s = c; w = c; break;
    case d_south:     s  = c; break;
    case d_southeast: s = c; e = c; break;
    default:
      ground = c;
      self = c;
      break;

    }

  }
  
char Orthog::get( direction d ) const {

  switch (d) {

    case d_northwest: return n&w;
    case d_north:     return n ;
    case d_northeast: return n&e;
    case d_west:      return w ;
    case d_east:      return e ;
    case d_southwest: return s&w;
    case d_south:     return s ;
    case d_southeast: return s&e;
    default:
      return ground;

    }

  }

bool Orthog::operator !( void ) const {
  return total()==0;
  }

bool Orthog::operator ==( const Orthog &rhs ) const {
  return (n == rhs.n && s == rhs.s  && w == rhs.w  && e == rhs.e
          && ground == rhs.ground && self == rhs.self );
  }
  
bool Orthog::operator !=( const Orthog &rhs ) const {
  return (n != rhs.n || s != rhs.s  || w != rhs.w  || e != rhs.e
          || ground != rhs.ground || self != rhs.self );
  }

#endif // DIR_CPP
