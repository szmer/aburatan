// [point.hpp]
// (C) Copyright 2000 Michael Blackney
#if !defined ( POINT_HPP )
#define POINT_HPP

#include "enum.hpp"

//-[ class Point - x,y ] ---


class Point {
	public:
	 // Constructors:
	 Point( void );
	 Point( const Point& );
	 Point( int, int );
	 // Public interface:
	 INLINE int &getx( void );
	 INLINE int &gety( void );
	 INLINE const int &getx( void ) const;
	 INLINE const int &gety( void ) const;
	 INLINE void setx( int );
	 INLINE void sety( int );
	 INLINE void set( int, int );
	 INLINE Point &incx( int d = 1 );
	 INLINE Point &incy( int d = 1 );
	 INLINE Point &decx( int d = 1 );
	 INLINE Point &decy( int d = 1 );
	 Point &operator +=(const Point &);
	 Point &operator -=(const Point &);

   long X_plus_Y_times(long) const;
   long axisSum( void ) const;

  private:
	 // Data members:
	 int x, y;
	};

bool operator ==(const Point &, const Point &);
bool operator !=(const Point &, const Point &);
Point operator +(const Point &, const Point &);
Point operator -(const Point &, const Point &);
Point operator -(const Point &);

//-[ class Point3d - x,y,z ] ---

class Point3d {
	public:
	 // Constructors:
	 Point3d( void );
	 Point3d( const Point3d& );
	 Point3d( int, int, int = 0 );

	 explicit
   Point3d( const Point&, int = 0 );
	 // Public interface:
	 INLINE int &getx( void );
	 INLINE int &gety( void );
	 INLINE int &getz( void );
	 INLINE const int &getx( void ) const;
	 INLINE const int &gety( void ) const;
	 INLINE const int &getz( void ) const;
	 INLINE void setx( int );
	 INLINE void sety( int );
	 INLINE void setz( int );
	 INLINE void set( int, int, int );
	 INLINE Point3d &incx( int d = 1 );
	 INLINE Point3d &incy( int d = 1 );
	 INLINE Point3d &incz( int d = 1 );
	 INLINE Point3d &decx( int d = 1 );
	 INLINE Point3d &decy( int d = 1 );
	 INLINE Point3d &decz( int d = 1 );
	 Point3d &operator +=(const Point3d &);
	 Point3d &operator -=(const Point3d &);
   Point toPoint( void ) const;

   long X_plus_Y_times(long) const;
   long axisSum( void ) const;
   
   Point3d abs( void ) const;
   Point3d delta( const Point3d& ) const;

  private:
	 // Data members:
	 int x, y, z;
	};

bool operator ==(const Point3d &, const Point3d &);
bool operator !=(const Point3d &, const Point3d &);
Point3d operator +(const Point3d &, const Point3d &);
Point3d operator -(const Point3d &, const Point3d &);
Point3d operator -(const Point3d &);

#endif // POINT_HPP
