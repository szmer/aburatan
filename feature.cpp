// [feature.cpp]
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
#if !defined ( FEATURE_CPP )
#define FEATURE_CPP

#include "feature.hpp"
#include "grammar.hpp"
#include "map.hpp"
#include "random.hpp"
#include "timeline.hpp"
#include "rectngle.hpp"
#include "dun_test.hpp"
#include "message.hpp"

targetType Feature::gettype( void ) const {
	return tg_feature;
	}

bool Feature::istype( targetType t ) const {

	if ( t == tg_feature )
		return true;
	else return RawTarget::istype(t);

	}

bool Feature::capable( Action a, const Target &t ) {

	switch( a ) {
    case aSetOwner:
      return true;

		default:
			return RawTarget::capable(a, t);
		}
	}

bool Feature::perform( Action a, const Target &t ) {

	switch( a ) {
    case aSetOwner:
      owner = t;
      return true;
  
    // case aEnterLev:  // <- why did I start this?
    
		default:
			return RawTarget::perform( a, t );
		}

	}

Point3d Feature::getlocation( void ) {
	return Point3d( Random::randint(border.getwidth())  + border.getxa(),
								Random::randint(border.getheight()) + border.getya(),
                levelloc.getz());

	}

Target Feature::getTarget( targetType type ) {
  switch( type ) {
    case tg_owner:
      return owner;
      
    default:
      return RawTarget::getTarget(type);
    }
  }

Rectangle Feature::getborder( void ) {
  return border;
  }

Feature::Feature( const Option *o )
: RawTarget( o ) {

	border = o->border;
  owner = o->owner;
	letter = image.val.appearance;

  // If level option is not set, we assume that this is a
  // multi-level feature:
  if (o->level)
  	levelloc = o->level->loc();

	}

Feature::Feature( Feature &rhs )
: RawTarget(rhs) {

	border = rhs.border;
	letter = rhs.letter;
	levelloc = rhs.levelloc;

  owner = rhs.owner;
	}

Feature::Feature( std::iostream &inFile, TargetValue *TVP )
// Read RawTarget base information:
: RawTarget (inFile, TVP) {

  inFile >> owner;

	// Read Rectangle border
	inFile.read( (char*) &border, sizeof(border) );

	// Read char letter
	inFile.read( (char*) &letter, sizeof(letter) );

	// Read Point3d levelloc
	inFile.read( (char*) &levelloc, sizeof(levelloc) );

  // Read name
  inFile >> f_name;

	}

void Feature::toFile( std::iostream &outFile ) {

	// Write RawTarget base information:
	RawTarget::toFile( outFile );

  outFile << owner;
  
	// Write Rectangle border
	outFile.write( (char*) &border, sizeof(border) );

	// Write char letter
	outFile.write( (char*) &letter, sizeof(letter) );

	// Write Point3d levelloc
	outFile.write( (char*) &levelloc, sizeof(levelloc) );

  // Write name
  outFile << f_name;

	}

Target Feature::clone( void ) {

  assert(0);
	return Target();

	}

bool Feature::equals( const Target & ) {

	return false;

	}


Image Feature::menuletter() {
	return letter;
	}

void Feature::setletter( char i ) {
	letter = i;
	}

Level &Feature::getlevel( void ) {

	return *Map::get( levelloc );
	}


long Feature::getVal( StatType s ) {

  switch(s) {

    //case attrib_sort_category:
      // By default, features are sorted in ascending order
      // of interest.
    //  return getVal(attrib_ooo_ness);

    case attrib_sunlit:
      return true;
      //return (levelloc.getz() == 0 && Timeline::timeOfDay(Timeline::now()) != tod_night);

    default:
      return RawTarget::getVal(s);
    }

  }

bool Feature::getStruct( structType type, void *buffer, size_t size ) {

  switch (type) {
    case s_border: {
      // Get the feature's border...
      
      // Make sure that there's the exact amount of space:
      assert( size == sizeof(Rectangle) );

      // Copy our border to the buffer:
      memcpy( buffer, (const void*) &border, size );
      
      return true;
      }

    default:
      return false;
    }

  }


void Feature::connectAll( Level *level ) {
// Checks that all parts of the level are connected to each other:

  int x, z = level->loc().getz();
  Rectangle lev_border = level->getBorder();
  
	// Check for inaccessible parts:
	char ** testgrid;
	testgrid = new char*[lev_width];

	for ( x = 0; x < lev_width; ++x )
		testgrid[x] = new char[lev_height];

	// Initialise test grid:
	for (int y = lev_border.getya(); y <= lev_border.getyb(); ++y)
		for (x = lev_border.getxa(); x <= lev_border.getxb(); ++x) {
			testgrid[x%lev_width][y%lev_height] = ( level->getTile( Point3d(x,y,z))->getVal(attrib_non_wall) );
      //Screen::put(x+1,y+3,Image(cGrey,'0'+testgrid[x][y]));
      }

	Dungeon_Tester dt(testgrid,lev_width,lev_height);
	dt.find_regions();
  Tile::Option tile_opt;
  tile_opt.floor = m_stone;
  tile_opt.type = tt_ground;
  Point origin = lev_border.geta();

  while (dt.get_num_regions() > 1) {
    List<Point> route = dt.dig_from(0);

    if (route.reset())
    do {

			if ( ! level->getTile( Point3d( route.get()+origin, z ) )->istype( tg_floor ) ) {

        tile_opt.location = Point3d( route.get()+origin, z );
        tile_opt.feature = THIS;
        level->tileCreate( tile_opt );

        }

      } while ( route.next() );

    else Message::add("Error digging path in Basement::build()");
      
    }

	// Dealloc test grid:
	for ( x = 0; x < lev_width; ++x )
		delarr( testgrid[x]);

	delarr( testgrid );

  }

/*#include "enum.hpp"

Feature::Feature( Level* l, char x1, char y1, char x2, char y2 )
: Rectangle(x1,y1,x2,y2), onlevel(l), name("") { }
Feature::~Feature( void ) {
	}

char Feature::gettype( void ) const {
	return tg_feature;
	}
char Feature::istype( char t ) const {
	if (t == tg_feature)
		return true;
	else
		return false;
	}

*/

#endif // FEATURE_CPP
