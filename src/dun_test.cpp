// [dun_test.cpp]
/*
 *	Adapted from Roguelike News Development Article 32,
 *							 Finding Explorable Regions
 *							 by Ross Morgan-Linial [rmorgan@jetcity.com]
 *
 *  Original text and code (C) Copyright 2000, Ross Morgan-Linial
 *
 *  Roughly ported to C++ 2003, Michael Blackney
 *
 *  Original Introduction:
 *	 This program takes a map and divides it into regions that are fully
 *	 connected - that is, not split in half by walls, so the player can
 *	 get anywhere in the region starting from a random point. I wrote it
 *	 to determine if all of a dungeon level can be reached from the
 *	 stairs, but you might find other uses for it; for example, you could
 *	 use the results to find out where corridors need to be added to
 *	 result in a fully connected level. Without further ado, here it is.
 *
 *
 *  GNU NOTICE:
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation; either version 2
 *   of the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 */

#if !defined ( DUN_TEST_CPP )
#define DUN_TEST_CPP

#include "dun_test.hpp"

#include "compiler.hpp"
#include "random.hpp"
#include "rectngle.hpp"

/*
 * This program divides a set of 'grids', which can be either wall or
 * floor, into connected regions. Every grid in a region is connected
 * to every other square in that region by vertical or horizontal (not
 * diagonal) connections, and not connected to grids in any other
 * region.
 *
 * We maintain a map that shows which region each grid is in. To avoid
 * iterating over the entire map every time we discover two candidate
 * regions are joined, we use a table that maps the numbers used on
 * the grid to actual region numbers. At the end of the computation,
 * we renumber the grid using this table.
 */

/* Define "debug" to to see how it works */

//#define __DUN_TEST_HARNESS
//#define debug

#include "enum.hpp"

#define NO_REGION   0   /* 0 is also a valid region */


Dungeon_Tester::Dungeon_Tester( char** cm, int w, int h  ) {

  int x, y;
	width = w+2;
	height = h+2;
	num_regions = 0;

	// Allocate storage for arrays:
	grid = new char*[height];
	region_grid = new int*[height];

	for ( y = 0; y < height; ++y ) {
		grid[y] = new char[width];
		region_grid[y] = new int[width];
		}

	// Clear arrays:
	for ( y = 0; y < height; ++y )
		for ( x = 0; x < width; ++x ) {
			grid[y][x] = 0;
			region_grid[y][x] = 0;
			}

	// Copy map info:
	for ( y = 1; y < height-1; ++y )
		for ( x = 1; x < width-1; ++x )
			grid[y][x] = cm[x-1][y-1];

	}


Dungeon_Tester::~Dungeon_Tester( void ) {

	// De-allocate storage:

	for ( int y = 0; y < height; ++y ) {
		delarr( grid[y] );
		delarr( region_grid[y] );
		}

	delarr( grid );
	delarr( region_grid );

	}



/*
 * To reduce processing time, when two regions are found to connect
 * (they're really the same region), we just mark them as the same
 * without actually renumbering grids. At the end of the computation,
 * or if we run out of region numbers, we use this function to update
 * all grids to true region numbers. It also compacts the region
 * numbers, so that afterwards all region numbers 0..num_regions-1
 * are still used.
 */
void Dungeon_Tester::remap_regions(void)
{
		int region_index[MAX_REGIONS];
		int region_index2[MAX_REGIONS];
		int new_regions;
		int x, y;
		int i;

		new_regions = 0;

		/*
		 * Iterate through the regions, and assign a new number to each
		 * non-remapped region so that they'll end up compacted.
		 */
		for (i = 0; i < num_regions; i++)
		{
				/* Has this region not been mapped to another region? */
				if (region_map[i] == i)
				{
						/* No, it hasn't. Assign it a new region number. */
#						ifdef debug
								printf("Mapping region: #%i -> #%i\n", i,
											 new_regions);
#						endif //debug
						region_index[i] = new_regions++;
				}
				else
				{
						/* It's been renumbered. Check for processing errors. */
						assert (region_map[region_map[i]] == region_map[i]);
						/*
						 * We've encountered a bug: this region has been
						 * mapped to a region that has also been mapped. Give
						 * an error and abort.
						 */

						/* Assign an in-bounds but otherwise meaningless value. */
						region_index[i] = NO_REGION;
				}
		}

		/*
		 * Construct a table of the double-indirection through remapping
		 * and compaction.
		 */
		for (i = 0; i < num_regions; i++)
				region_index2[i] = region_index[region_map[i]];

		/* Renumber the grids, using the table we just computed. */
		for (y = 0; y < height; y++)
				for (x = 0; x < width; x++)
						region_grid[y][x] = region_index2[region_grid[y][x]];

		/* Create a new do-nothing region mapping table. */
		for (i = 0; i < new_regions; i++)
				region_map[i] = i;

		/* Save the new number of regions. */
		num_regions = new_regions;
}

/*
 * Join two candidate regions together. This just involves updating
 * the region remapping table.
 */
void Dungeon_Tester::join_regions(int r1, int r2)
{
		int i;
		int r1_map, r2_map;

		/* We have to operate on primary (unremapped) regions here */
		r1_map = region_map[r1];
		r2_map = region_map[r2];

#		ifdef debug

				printf("Joining regions #%i (%i) and #%i (%i)\n", r1, r1_map,
							 r2, r2_map);
#   endif

		/* Modify the region mapping table. */
		for (i = 0; i < num_regions; i++)
				if (region_map[i] == r2_map)
				{
#						ifdef debug
								printf("Mapping region #%i (%i) -> #%i\n", i,
											 region_map[i], r1_map);

#						endif // debug
						region_map[i] = r1_map;
				}
}

/*
 * Create a new region.
 */
int Dungeon_Tester::new_region(void)
{
		int i;

		/* If we're out of regions, compact them. */
		if (num_regions >= MAX_REGIONS)
		{
#				ifdef debug
						printf("Remapping regions\n");
#				endif // debug
				remap_regions();
		}

		/* If we're still out of regions, we have a problem. */
		assert (num_regions < MAX_REGIONS);

		/* Allocate a new region. */
		i = num_regions++;
		region_map[i] = i;

#		ifdef debug
				printf("New region: #%i\n", i);
#		endif // debug

		return i;
}

void Dungeon_Tester::find_regions(void)
{
		int x, y;
		int i;

#		ifdef debug
				printf("Clearing region grid\n");
#		endif // debug

		/* Clear the region grid to a valid (but meaningless) value. */
		for (y = 0; y < height; y++)
				for (x = 0; x < width; x++)
						region_grid[y][x] = NO_REGION;

		/* Initialize the remapping table to a null map. */
		for (i = 0; i < MAX_REGIONS; i++)
				region_map[i] = i;

		/* Start with no regions. */
		num_regions = 0;

		/* Consider each grid, except the borders (leave them as walls) */
		for (y = 1; y < height - 1; y++)
		{
				for (x = 1; x < width - 1; x++)
				{
						/* Skip wall grids. */
						if (!grid[y][x])
								continue;

#						ifdef debug
								printf("(%i, %i) ", x, y);
#						endif // debug

						/*
						 * Consider the possible combinations of left, above
						 * grids.
						 */
						if (!grid[y - 1][x])
						{
								if (!grid[y][x - 1])
								{
										/* No floor left or above */
										region_grid[y][x] = new_region();
								}
								else
								{
										/* Floor left, but not above */
#        						ifdef debug

												printf("Copying over #%i\n",
															 region_grid[y][x - 1]);
#						        endif // debug

										region_grid[y][x] = region_grid[y][x - 1];
								}
						}
						else
						{
								if (!grid[y][x - 1])
								{
										/* Floor above, but not left */
#	        					ifdef debug
												printf("Copying down #%i\n",
															 region_grid[y - 1][x]);
#						        endif // debug
										region_grid[y][x] = region_grid[y - 1][x];
								}
								else
								{
										/*
										 * Floor both left and above - are they the same
										 * region?
										 */
										if (region_map[region_grid[y - 1][x]] !=
												region_map[region_grid[y][x - 1]])
										{
												/* No, join them. */
												join_regions(region_grid[y - 1][x],
																		 region_grid[y][x - 1]);
										}

										/* They're now the same region, so copy one. */
#		        				ifdef debug
												printf("Copying down #%i\n",
															 region_grid[y - 1][x]);
#						        endif // debug
										region_grid[y][x] = region_grid[y - 1][x];
								}
						}
				}
		}

		/* Do a final remapping, to make the region_grid[][] accurate. */
		remap_regions();
		count_regions();
}

int Dungeon_Tester::get_num_regions( void ) {
	return num_regions;
	}

void Dungeon_Tester::count_regions( void ) {
// Finds the number of points for each region.

	for ( int i = 0; i < num_regions; i++ ) {

		region_total[i] = 0;

		for ( int y = 1; y < height-1; y++ )

			for ( int x = 1; x < width-1; x++ )

				if ( grid[y][x] && region_grid[y][x] == i )

					++region_total[i];

		}


	}

Point Dungeon_Tester::operator []( int index ) {

	// If no part of map exists for this index, return "error" value:
	if (index > num_regions) return Point(-1,-1);

	int x, y;
	int i = Random::randint( region_total[index] ) + 1;

	for ( y = 1; y < height-1; y++ )

		for ( x = 1; x < width-1; x++ )

			if ( grid[y][x] && region_grid[y][x] == index && ! --i)

				return Point(x-1,y-1);


	// Something went wrong; return "error" value:
	assert(0);
  
	return Point(-1,-1);

	}

//#define DIG_FROM_TEST

#ifdef DIG_FROM_TEST
# include "visuals.hpp"
# include "input.hpp"
#endif //DIG_FROM_TEST
List<Point> Dungeon_Tester::dig_from( int index ) {
// Scans out from inital section to find the
// nearest point from a different section.
//
//   aaa   xxx
//   aa12345xx
//   aaa   xxx
//
// IMPORTANT NOTE: When completed, joins the initial
//                 section to the new section!
//
// Returns an empty path if no other points are found.

	int x, y, dx, dy, i;
  Point start, finish;
  List<Point> ret_path;
  
# ifdef DIG_FROM_TEST
  Screen::cls();
# endif //DIG_FROM_TEST
     
	for ( y = 1; y < height-1; y++ )
		for ( x = 1; x < width-1; x++ ) {
      // Make sure that tunnels are not joined through corners:
      int con_diag, con_adj;
      if (grid[y][x] == 0) {
        con_diag = (grid[y-1][x-1]==1)+(grid[y-1][x+1]==1)+(grid[y+1][x-1]==1)+(grid[y+1][x+1]==1);
        con_adj  = (grid[y-1][x]==1)+(grid[y][x-1]==1)+(grid[y+1][x]==1)+(grid[y][x+1]==1);

        if (con_adj == 0 && con_diag == 1) grid[y][x] = 99;
        }
    
      // Initialise the grid
      if (grid[y][x] == 1 && region_grid[y][x] == index) {
        for ( dy = -1; dy < 2; dy++ )
          for ( dx = -1; dx < 2; dx++ )
          if (abs(dx+dy)==1)
          {
            if (grid[y+dy][x+dx]==0)
              grid[y+dy][x+dx] = 2;

#           ifdef DIG_FROM_TEST
            Screen::put(x+dx,y+dy+2,Image(cGrey,'0'+grid[y+dy][x+dx]));
#           endif //DIG_FROM_TEST
            }
            
        if (y+dy < start.gety())
          start.sety(y+dy);
        else
        if (y+dy > finish.gety())
          finish.sety(y+dy);

        if (x+dx < start.getx())
          start.setx(x+dx);
        else
        if (x+dx > finish.getx())
          finish.setx(x+dx);
        }

      
#     ifdef DIG_FROM_TEST
      if (grid[y][x] == 1)
        Screen::put(x,y+2,Image(cGrey,'a'+region_grid[y][x]));
      else if (grid[y][x] == 99)
        Screen::put(x,y+2,Image(cLRed,'X'));
#     endif //DIG_FROM_TEST
      }
      
# ifdef DIG_FROM_TEST
  Keyboard::get();
# endif //DIG_FROM_TEST

  // Floodfill the grid
  Rectangle limits( 1,1,width-2,height-2 ),
            scan_section( start, finish );

  bool found = false;
  int iteration = 1;
  List<Point> end_list;
  
  while (!found && iteration < 80) {
    scan_section += Rectangle(-1,-1,+1,+1);
    scan_section.setintersect(limits);
    ++iteration;

    for (y = scan_section.getya(); y <= scan_section.getyb(); ++y )
      for (x = scan_section.getxa(); x <= scan_section.getxb(); ++x )
      
        if (grid[y][x] == iteration)
          for ( dy = -1; dy < 2; dy++ )
            for ( dx = -1; dx < 2; dx++ )
            if (abs(dx+dy)==1)
            {
              if (grid[y+dy][x+dx]==0) {
                grid[y+dy][x+dx] = iteration+1;

#               ifdef DIG_FROM_TEST
                if (limits.contains(Point(x+dx,y+dy)) && (dx != 0 || dy != 0) )
                  Screen::put(x+dx,y+dy+2,Image(cGrey,'0'+grid[y+dy][x+dx]));
#               endif //DIG_FROM_TEST
                }
              else if (grid[y+dy][x+dx]==1 && region_grid[y+dy][x+dx] != index) {
                found = true;
                end_list.add( Point(x+dx,y+dy) );
#               ifdef DIG_FROM_TEST
                Screen::put(x+dx,y+dy+2,Image(cBlue,'0'+grid[y+dy][x+dx]));
#               endif //DIG_FROM_TEST
                }
                

              }

    }

# ifdef DIG_FROM_TEST
  Keyboard::get();
# endif //DIG_FROM_TEST

  // Make a list of weighted directions:

  assert(end_list.numData());
  // Decide on which end point to use:
  int end_num = Random::randint( end_list.numData() );

  for (i = 0; i < end_num; ++i ) end_list.remove();
  
  Point end_point = end_list.remove();
# ifdef DIG_FROM_TEST
  Screen::put(end_point.getx(),end_point.gety()+2,Image(cRed,'0'+grid[end_point.gety()][end_point.getx()]));
# endif //DIG_FROM_TEST

  join_regions(index, region_grid[end_point.gety()][end_point.getx()]);

  ret_path.add(end_point-Point(1,1));
  grid[end_point.gety()][end_point.getx()] = 1;

  end_list.clr();

# ifdef DIG_FROM_TEST
  Keyboard::get();
# endif //DIG_FROM_TEST

  // Find a path from one of the end points to the initial section:
  while (iteration > 0) {

    // Make a list of possible directions:
    x = end_point.getx();
    y = end_point.gety();
    
    for ( dy = -1; dy < 2; dy++ )
      for ( dx = -1; dx < 2; dx++ )
      if (abs(dx+dy)==1)
      {
        if (grid[y+dy][x+dx]==iteration) {
          end_list.add( Point(x+dx,y+dy) );
#         ifdef DIG_FROM_TEST
          Screen::put(x+dx,y+dy+2,Image(cBlue,'0'+grid[y+dy][x+dx]));
#         endif //DIG_FROM_TEST
          }
      }

    // choose a direction:
    end_num = Random::randint( end_list.numData() );

    for (i = 0; i < end_num; ++i ) end_list.remove();
    end_point = end_list.remove();
#   ifdef DIG_FROM_TEST
    Screen::put(end_point.getx(),end_point.gety()+2,Image(cRed,'0'+grid[end_point.gety()][end_point.getx()]));
#   endif //DIG_FROM_TEST

    ret_path.add(end_point-Point(1,1));
    grid[end_point.gety()][end_point.getx()] = 1;
    end_list.clr();
    
    --iteration;
    }
    
# ifdef DIG_FROM_TEST
  Keyboard::get();
# endif //DIG_FROM_TEST

	for ( y = 0; y < height; y++ )
		for ( x = 0; x < width; x++ )
      // reinitialise the grid
      if (grid[y][x] > 1) grid[y][x] = 0;
      
  remap_regions();

  return ret_path;
  }


#ifdef __DUN_TEST_HARNESS


#ifdef OBSOLETE_INCLUDE
#  include <stdlib.h>
#  include <stdio.h>
#else
#  include <cstdlib>
#  include <cstdio>
#endif

/*
 * Print our results.
 */
void Dungeon_Tester::print_map(void)
{
		int x, y;
		int i;

		for (y = 1; y < height-1; y++)
		{
				for (x = 1; x < width-1; x++)
				{
						if (grid[y][x])
						{
								printf("%c", 'a' + region_grid[y][x]);
						}
						else
								printf(" ");
				}
//				printf("\n");
		}


		printf( "Num Regions: %i\n", num_regions );

		for ( i = 0; i < num_regions; ++i )
			printf( " Total tiles in section %c: %i\n", 'a' + i, region_total[i] );



}


/*
 * Our map - 0 is wall, 1 is floor.
 *
 * We have to have a border of walls, or we may get strange errors.
 */
#define X 1

#include <conio.h>

int main(void)
{

		clrscr();
		Random::randomize();

		//Allocate test grid:
		int x, y;
		char ** testgrid;
		testgrid = new char*[lev_width];

		for ( x = 0; x < lev_width; ++x ) {
			testgrid[x] = new char[lev_height];
			}
		// Initialise test grid:
		for ( y = 0; y < lev_height; ++y )
			for ( x = 0; x < lev_width; ++x )
				testgrid[x][y] = ( ( x ^ 5 ) || (y == 2 ) ) && ( ( y ^ 6 ) || (x == 29) ) && ( x ^ 35 ) && Random::randint(10);



		// Create DT object and test dungeon:
		Dungeon_Tester dt(testgrid,lev_width,lev_height);

		dt.find_regions();


		// Display results of test:
		dt.print_map();
    /*
		int i;
		for ( i = 0; i < dt.get_num_regions(); i++ ) {

			Point p = dt[i];
			gotoxy( p.getx()+1, p.gety()+1 );
			printf("%c", 'A' + i);

			}
    */

		return 0;
}
#endif // __DUN_TEST_HARNESS

#endif // DUN_TEST_CPP
