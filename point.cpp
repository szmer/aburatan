// [point.cpp]
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
#if !defined ( POINT_CPP )
#define POINT_CPP
#include "point.hpp"

//-[ class Point - x,y ]----

Point::Point( void ):	x(0), y(0) {}
Point::Point( const Point& p): x( p.getx() ), y( p.gety() ) {}
Point::Point( int a, int b ) : x(a), y(b) {}

	 // Public interface:
int &Point::getx( void ) {
	return x;
	}
int &Point::gety( void ) {
	return y;
	}
const int &Point::getx( void ) const {
	return x;
	}
const int &Point::gety( void ) const {
	return y;
	}
void Point::setx( int a ) {
	x = a;
	}
void Point::sety( int b ) {
	y = b;
	}
void Point::set( int a, int b ) {
	x = a; y = b;
	}

Point &Point::incx( int d ) {
	x += d;
	return *this;
	}
Point &Point::incy( int d ) {
	y += d;
	return *this;
	}
Point &Point::decx( int d ) {
	x -= d;
	return *this;
	}
Point &Point::decy( int d ) {
	y -= d;
	return *this;
	}


bool operator ==(const Point &lhs, const Point &rhs) {
	return (  lhs.getx() == rhs.getx() && lhs.gety() == rhs.gety()  ) ? true : false;
	}

bool operator !=(const Point &lhs, const Point &rhs) {
	return (lhs==rhs) ? false : true;
	}


Point &Point::operator +=(const Point &rhs) {
	x += rhs.getx();
	y += rhs.gety();
	return *this;
	}

Point &Point::operator -=(const Point &rhs) {
	x -= rhs.getx();
	y -= rhs.gety();
	return *this;
	}


Point operator +(const Point &lhs, const Point &rhs) {
	Point p = lhs;
	return p += rhs;
	}

Point operator -(const Point &lhs, const Point &rhs) {
	Point p = lhs;
	return p -= rhs;
	}

Point operator -(const Point &unary) {
	Point p;
	return p -= unary;
	}

long Point::X_plus_Y_times(long mult) const {
  return x + y * mult;
  }

long Point::axisSum( void ) const {
  return x + y;
  }

//-[ class Point3d - x,y ]----

Point3d::Point3d( void ):	x(0), y(0), z(0) {}
Point3d::Point3d( const Point3d& p)
: x( p.getx() ), y( p.gety() ), z( p.getz() ) {}

Point3d::Point3d( const Point& p, int c)
: x( p.getx() ), y( p.gety() ), z( c ) {}

Point3d::Point3d( int a, int b, int c ) : x(a), y(b), z(c) {}

	 // Public interface:
int &Point3d::getx( void ) {
	return x;
	}
int &Point3d::gety( void ) {
	return y;
	}
int &Point3d::getz( void ) {
	return z;
	}
const int &Point3d::getx( void ) const {
	return x;
	}
const int &Point3d::gety( void ) const {
	return y;
	}
const int &Point3d::getz( void ) const {
	return z;
	}
void Point3d::setx( int a ) {
	x = a;
	}
void Point3d::sety( int b ) {
	y = b;
	}
void Point3d::setz( int c ) {
	z = c;
	}
void Point3d::set( int a, int b, int c ) {
	x = a; y = b; z = c;
	}

Point3d &Point3d::incx( int d ) {
	x += d;
	return *this;
	}
Point3d &Point3d::incy( int d ) {
	y += d;
	return *this;
	}
Point3d &Point3d::incz( int d ) {
	z += d;
	return *this;
	}
Point3d &Point3d::decx( int d ) {
	x -= d;
	return *this;
	}
Point3d &Point3d::decy( int d ) {
	y -= d;
	return *this;
	}
Point3d &Point3d::decz( int d ) {
	z -= d;
	return *this;
	}


bool operator ==(const Point3d &lhs, const Point3d &rhs) {
	return lhs.getx() == rhs.getx()
      && lhs.gety() == rhs.gety()
      && lhs.getz() == rhs.getz();
	}

bool operator !=(const Point3d &lhs, const Point3d &rhs) {
	return !(lhs==rhs);
	}


Point3d &Point3d::operator +=(const Point3d &rhs) {
	x += rhs.getx();
	y += rhs.gety();
	z += rhs.getz();
	return *this;
	}

Point3d &Point3d::operator -=(const Point3d &rhs) {
	x -= rhs.getx();
	y -= rhs.gety();
	z -= rhs.getz();
	return *this;
	}


Point3d operator +(const Point3d &lhs, const Point3d &rhs) {
	Point3d p = lhs;
	return p += rhs;
	}

Point3d operator -(const Point3d &lhs, const Point3d &rhs) {
	Point3d p = lhs;
	return p -= rhs;
	}

Point3d operator -(const Point3d &unary) {
	Point3d p;
	return p -= unary;
	}
  
Point Point3d::toPoint( void ) const {
  return Point(x,y);
  }

long Point3d::X_plus_Y_times(long mult) const {
  return x + y * mult;
  }

long Point3d::axisSum( void ) const {
  return x + y + z;
  }

Point3d Point3d::abs( void ) const {
  Point3d ret_pt = *this;

  if (ret_pt.x < 0) ret_pt.x = 0 - ret_pt.x;
  if (ret_pt.y < 0) ret_pt.y = 0 - ret_pt.y;
  if (ret_pt.z < 0) ret_pt.z = 0 - ret_pt.z;

  return ret_pt;
  }
  
Point3d Point3d::delta( const Point3d &rhs ) const {
  
  return *this - rhs;
  }

#endif // POINT_CPP
