// [rectngle.hpp]
// (C) Copyright 2000 Michael Blackney
#if !defined ( RECTNGLE_HPP )
#define RECTNGLE_HPP
#include "point.hpp"

class Rectangle {
	public:
	 // Constructors:
	 Rectangle( void );
	 Rectangle( const Rectangle& );
	 Rectangle( const Point&, const Point& );
	 Rectangle( int, int, int, int );
	 // Public interface:
	 Rectangle &setintersect(const Rectangle&);
	 Rectangle &setunion(const Rectangle&);
	 INLINE void set ( int, int, int, int );
	 INLINE void set ( Point, Point );
	 INLINE const Point& geta( void ) const;
	 INLINE const Point& getb( void ) const;
	 INLINE Point& geta( void );
	 INLINE Point& getb( void );
	 INLINE void seta( const Point& );
	 INLINE void setb( const Point& );
	 INLINE int getheight( void ) const;
	 INLINE int getwidth( void ) const;
	 INLINE int getxa( void ) const;
	 INLINE int getxb( void ) const;
	 INLINE int getya( void ) const;
	 INLINE int getyb( void ) const;
	 INLINE int &getxa( void );
	 INLINE int &getxb( void );
	 INLINE int &getya( void );
	 INLINE int &getyb( void );
	 INLINE void setxa( int );
	 INLINE void setxb( int );
	 INLINE void setya( int );
	 INLINE void setyb( int );
	 bool contains( const Point& ) const;
	 bool contains( const Point3d& ) const;
	 bool edgePoint( const Point& ) const;
	 bool edgePoint( const Point3d& ) const;
	 bool edgePointX( const Point& ) const;
	 bool edgePointX( const Point3d& ) const;
	 bool edgePointY( const Point& ) const;
	 bool edgePointY( const Point3d& ) const;
	 Point getMid( void ) const;
	 Rectangle &operator +=(const Rectangle &);
	 Rectangle &operator -=(const Rectangle &);
	 Rectangle &operator +=(const Point &);
	 Rectangle &operator -=(const Point &);

	 Rectangle quadrant( int ) const;
   void validatePoints( void );
	protected:
	 // Data members:
		Point a, b;
	};

bool operator ==(const Rectangle &, const Rectangle &);
bool operator !=(const Rectangle &, const Rectangle &);
Rectangle operator +(const Rectangle &, const Rectangle &);
Rectangle operator -(const Rectangle &, const Rectangle &);


#endif // RECTNGLE_HPP

