// [path.cpp]
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
#if !defined ( PATH_CPP )
#define PATH_CPP

#include "path.hpp"
#include "list.hpp"
#include "map.hpp"
#include "visuals.hpp"
#include "rawtargt.hpp"

Path::Path( void ) {
	}

Path::Path( Point3d s, Point3d f ) {
	start = s;
	finish = f;
	}

Path::~Path( void ) {
	hide();
	}

void Path::set( Point3d s, Point3d f ) {
	hide();

	start = s;
	finish = f;
	points.clr();
	images.clr();
	}

void Path::setFinish( Point3d p ) {
	hide();

	finish = p;
	points.clr();
	images.clr();
	}

void Path::setStart( Point3d p ) {
	hide();

	start = p;
	points.clr();
	images.clr();
	}

const Point3d &Path::getFinish( void ) const {
	return finish;
	}

const Point3d &Path::getStart( void ) const {
	return start;
	}


#include "input.hpp"

bool Path::calculate( char **map_arr, type t ) {

	hide();

	points.clr();
	images.clr();

	Point3d current = start;

  // With no input map, we can only calculate a straight line:
  if (!map_arr) t = pt_straight;

  switch (t) {
    case pt_straight: {
      int	longestLength,
    			endX = finish.getx(), startX = start.getx(),
    			endY = finish.gety(), startY = start.gety(),
          // We only support paths that stay on the same level:
          z = start.getz();

    	int lengthX = (endX - startX)*MULT,
    			lengthY = (endY - startY)*MULT,
    			divX = 0, divY = 0,
    			posX = 0,
    			posY = 0,
    			posXdiv, posYdiv;

    	char signX = 0, signY = 0;

    	char plusX = 0, plusY = 0;

    	if (abs(lengthX) > abs(lengthY))
    		longestLength = abs (endX - startX);
    	else
    		longestLength = abs (endY - startY);


    	if (longestLength != 0) {
    		divX = lengthX / longestLength;
    		divY = lengthY / longestLength;
    		}

    	signX = (divX < 0)?-1:1;
    	signY = (divY < 0)?-1:1;

    	divX = abs(divX);
    	divY = abs(divY);

    	for (char count = 1; count < longestLength; count++) {

    		posX += divX; plusX = (posX % 256 > 127);
    		posY += divY; plusY = (posY % 256 > 127);

    		posXdiv = posX/MULT;
    		posYdiv = posY/MULT;

	    	current.set(startX+(posXdiv+plusX)*signX,startY+(posYdiv+plusY)*signY, z);
 				points.add( current );
				}

			points.add( finish );

      // straight paths always succeed:
			return true;
      }


  // Incomplete.  To finish only if required.
  /*
    case pt_shortest: {
    // Given a char** array like so:
    //
    // 000000000000000000
    // 011001100000011000
    // 011001100000011000
    // 011000000110011000
    // 011000000110011000
    // 000000000000000000
    //
    // will create a path between two sections.

      Dungeon_Tester dt(testgrid,lev_width,lev_height);
    	dt.find_regions();

    	int regions = dt.get_num_regions();
      
      // Can only join regions when there are at least two:
      if (regions < 2)
        return false;



    	for ( y = 0; y < lev_height; ++y )
    		for ( x = 0; x < lev_width; ++x ) {
          Screen::put(x+1,y+3,Image(cGrey,'0'+testgrid[x][y]));
          }
      Keyboard::get();
    
      break;
      }
    */
    
		default:
			// Undefined path type:
      assert(0);
      return false;
		}
	}


void Path::display( void ) {

  // LOS to finish is blocked:
	bool blocked = false;
  // LOS to finish may be blocked, ie. squares in path are unlit
  bool questionable = false;
	int opacity = 0;

  Screen::hidecursor();

	if ( points.reset() )
	do {

		Point3d p = points.get();

  	images.add( LEV_GET(p.getx(),p.gety()) );

    Target tile = Map::getTile(p);

    if (!tile->getVal(attrib_visible)) questionable = true;

		LEV_DRAW( p.getx(), p.gety(), Image( (blocked?cRed:(questionable?cGreen:cGrey)), '+' ) );

		if (tile->getVal(attrib_visible)) opacity += tile->getVal(attrib_opaque);
		if (opacity > OPACITY_BLOCK) blocked = true;
		else blocked = false;

		} while ( points.next() );


	LEV_DRAW( finish.getx(), finish.gety(), Image( (blocked?cYellow:(questionable?cLGreen:cWhite)), 'X' ) );

  Screen::showcursor();

  Screen::update();

	}


void Path::hide( void ) {

  
	if ( points.reset() && images.reset() ) {

    Screen::hidecursor();
  	do {

  		Point3d p = points.get();
  			LEV_DRAW( p.getx(),p.gety(), images.get() );
  		} while (images.next() && points.next());

  	if (images.toTail())
  			LEV_DRAW( finish.getx(),finish.gety(), images.get() );

    Screen::showcursor();

    Screen::update();
    }
    
	}

bool Path::hasLOS( void ) {

	bool blocked = false;
	int opacity = 0;

	if ( points.reset() )
	do {

		Point3d p = points.get();

		opacity += Map::getTile(p)->getVal(attrib_opaque);
		if (opacity > OPACITY_BLOCK) blocked = true;
		else blocked = false;

		} while ( points.next() );

	return (blocked)?false:true;
	}


bool Path::reset( void ) {
	return points.reset();
	}

bool Path::next( void ) {
	return points.next();
	}
bool Path::previous( void ) {
	return points.previous();
	}
bool Path::toTail( void ) {
	return points.toTail();
	}
Point3d Path::get( void ) {
	return points.get();
	}

#endif // PATH_CPP
