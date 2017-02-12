// [dun_test.hpp]
// (C) Copyright 2003-2004 Michael Blackney
// An interface for C language methods that
// are (C) Copyright 2000, Ross Morgan-Linial

#if !defined ( DUN_TEST_HPP )
#define DUN_TEST_HPP

#include "enum.hpp"
#include "point.hpp"
#include "list.hpp"

#define MAX_REGIONS 40
//#define __DUN_TEST_HARNESS

class Dungeon_Tester {

	public:

		Dungeon_Tester( char**, int, int );
		~Dungeon_Tester( void );

		void find_regions( void );
#       ifdef __DUN_TEST_HARNESS
		void print_map( void );
#       endif

		int get_num_regions( void );

		Point operator []( int );

    List<Point> dig_from( int );

	private:

		char** grid;
		int** region_grid;
		int region_map[MAX_REGIONS];
		int region_total[MAX_REGIONS];
		int num_regions;

		int height, width;

		int  new_region( void );
		void remap_regions( void );
		void join_regions( int r1, int r2 );
		void count_regions( void );


	};



#endif // DUN_TEST_HPP
