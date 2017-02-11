// [path.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( PATH_HPP )
#define PATH_HPP

#include "point.hpp"
#include "list.hpp"
#include "types.hpp"

class Path {

	public:

		enum type {
		//pt pathtype
      pt_straight,           // Direct route
			pt_shortest,           // Path of least resistance - for AI
			pt_flow                // Curved, flowing path - road, river

			};

		Path( void );
		Path( Point3d, Point3d );
		~Path( void );
		void setFinish( Point3d );
		void setStart( Point3d );

		const Point3d &getFinish( void ) const;
		const Point3d &getStart( void ) const;

		void set( Point3d, Point3d );
		bool calculate( char **map_arr = NULL, type t = pt_straight );

		void display( void );
		void hide( void );
		bool hasLOS( void );

		bool reset( void );
		bool next( void );
		bool previous( void );
		bool toTail( void );
		Point3d get( void );

	private:

		Point3d start, finish;

		List<Point3d> points;
		List<Image> images;


	};


#endif // PATH_HPP
