// [rectngle.cpp]
// (C) Copyright 2000 Michael Blackney
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
#if !defined ( RECTNGLE_CPP )
#define RECTNGLE_CPP

#include "rectngle.hpp"
#include "standard.hpp"
#include "assert.h"

Rectangle::Rectangle( void ) : a(), b() {}
Rectangle::Rectangle( const Rectangle& r )
: a( r.geta() ), b( r.getb() ) { }
Rectangle::Rectangle( const Point& p1, const Point& p2 )
: a(p1), b(p2) { }
Rectangle::Rectangle( int x1, int y1, int x2, int y2 )
: a(x1,y1), b(x2,y2) { }

// Public interface:
Rectangle &Rectangle::setintersect(const Rectangle &r ) {
	setxa( max( r.getxa(), getxa() ) );
	setya( max( r.getya(), getya() ) );
	setyb( min( r.getyb(), getyb() ) );
	setxb( min( r.getxb(), getxb() ) );
  
	return *this;
	}

Rectangle &Rectangle::setunion(const Rectangle&) {
	return *this;
	}

void Rectangle::set ( int ax, int ay, int bx, int by ) {
	a.set(ax, ay);
	b.set(bx, by);

	}
void Rectangle::set ( Point ap, Point bp ) {
	a = ap;
	b = bp;

	}
Point& Rectangle::geta( void ) {
	return a;
	}
Point& Rectangle::getb( void ) {
	return b;
	}
const Point& Rectangle::geta( void )const  {
	return a;
	}
const Point& Rectangle::getb( void )const  {
	return b;
	}
int Rectangle::getheight( void ) const {
	return abs(b.gety() - a.gety()) + 1;
	}
int Rectangle::getwidth( void ) const {
	return abs(b.getx() - a.getx()) + 1;
	}
void Rectangle::seta( const Point& p ) {
	a = p;
	}
void Rectangle::setb( const Point& p ) {
	b = p;
	}
int Rectangle::getxa( void ) const {
	return a.getx();
	}
int Rectangle::getxb( void ) const {
	return b.getx();
	}
int Rectangle::getya( void ) const {
	return a.gety();
	}
int Rectangle::getyb( void ) const {
	return b.gety();
	}
int &Rectangle::getxa( void ) {
	return a.getx();
	}
int &Rectangle::getxb( void ) {
	return b.getx();
	}
int &Rectangle::getya( void ) {
	return a.gety();
	}
int &Rectangle::getyb( void ) {
	return b.gety();
	}
void Rectangle::setxa( int x ) {
	a.setx(x);
	}
void Rectangle::setxb( int x ) {
	b.setx(x);
	}
void Rectangle::setya( int y ) {
	a.sety(y);
	}
void Rectangle::setyb( int y ) {
	b.sety(y);
	}

bool Rectangle::contains( const Point &p ) const {
	return ( ascending( a.getx() - 1, p.getx(), b.getx() + 1)
				&& ascending( a.gety() - 1, p.gety(), b.gety() + 1) );

	}

bool Rectangle::contains( const Point3d &p3d ) const {
  return contains(p3d.toPoint());
  }

bool Rectangle::edgePoint( const Point &p ) const {
// Point is an edge point if its x or y values
// match either of the rectangle's x or y values
	return p.getx() == a.getx()
      || p.getx() == b.getx()
      || p.gety() == a.gety()
      || p.gety() == b.gety();

	}

bool Rectangle::edgePoint( const Point3d &p3d ) const {
  return edgePoint(p3d.toPoint());
  }

bool Rectangle::edgePointX( const Point &p ) const {
// Point is an edge point x if its x values
// match either of the rectangle's x values
	return p.getx() == a.getx()
      || p.getx() == b.getx();

	}

bool Rectangle::edgePointX( const Point3d &p3d ) const {
  return edgePointX(p3d.toPoint());
  }
bool Rectangle::edgePointY( const Point &p ) const {
// Point is an edge point y if its y values
// match either of the rectangle's y values
	return p.gety() == a.gety()
      || p.gety() == b.gety();

	}

bool Rectangle::edgePointY( const Point3d &p3d ) const {
  return edgePointY(p3d.toPoint());
  }


Point Rectangle::getMid( void ) const {

	return Point( (a.getx()+b.getx())/2,(a.gety()+b.gety())/2 );

	}


void Rectangle::validatePoints( void ) {
	int ax, bx, ay, by;
	ax = min( a.getx(), b.getx() );
	bx = max( a.getx(), b.getx() );
	ay = min( a.gety(), b.gety() );
	by = max( a.gety(), b.gety() );

	a.set(ax, ay);
	b.set(bx, by);
	}


Rectangle &Rectangle::operator +=(const Rectangle &rhs) {
	a += rhs.geta();
	b += rhs.getb();
	return *this;
	}

Rectangle &Rectangle::operator -=(const Rectangle &rhs) {
	a -= rhs.geta();
	b -= rhs.getb();
	return *this;
	}

Rectangle &Rectangle::operator +=(const Point &rhs) {
	a += rhs;
	b += rhs;
	return *this;
	}

Rectangle &Rectangle::operator -=(const Point &rhs) {
	a -= rhs;
	b -= rhs;
	return *this;
	}

Rectangle operator +(const Rectangle &lhs, const Rectangle &rhs) {
	Rectangle r = lhs;
	return r += rhs;
	}

Rectangle operator -(const Rectangle &lhs, const Rectangle &rhs) {
	Rectangle r = lhs;
	return r -= rhs;
	}

bool operator ==(const Rectangle &lhs, const Rectangle &rhs) {
	return ( ( lhs.geta() == rhs.geta() ) && ( lhs.getb() == rhs.getb() ) )?true:false;
	}

bool operator !=(const Rectangle &lhs, const Rectangle &rhs) {
	return ( ( lhs.geta() != rhs.geta() ) || ( lhs.getb() != rhs.getb() ) )?true:false;
	}

Rectangle Rectangle::quadrant( int q ) const {

	q %= 4;

	int qWidth = getwidth() / 2 - (getwidth()%2?0:1);
	int qHeight = getheight() / 2 - (getheight()%2?0:1);

	switch (q) {

		case 0:  return Rectangle( a.getx(), a.gety(), a.getx()+qWidth, a.gety()+qHeight );

		case 1:  return Rectangle( b.getx()-qWidth, a.gety(), b.getx(), a.gety()+qHeight );

		case 2:  return Rectangle( a.getx(), b.gety()-qHeight, a.getx()+qWidth, b.gety() );

		case 3:  return Rectangle( b.getx()-qWidth, b.gety()-qHeight, b.getx(), b.gety() );

		}

  // Error getting rectangle quadrant:
  assert(0);
	return Rectangle();

	}

#endif // RECTNGLE_CPP

