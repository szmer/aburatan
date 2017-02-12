// [levelel.cpp]
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
#if !defined ( LEVELEL_CPP )
#define LEVELEL_CPP

#include "tile.hpp"
#include "standard.hpp"
#include "visuals.hpp"
#include "define.hpp"
#include "grammar.hpp"
#include "message.hpp"
#include "global.hpp"
#include "level.hpp"

// The following macro will be required too often for us to
// want to make it a function:
#   define assertlev() \
    if (!level) {\
      level = &*Map::get( Point3d( location.getx() / lev_width, \
                                   location.gety() / lev_height, \
                                   location.getz() ) \
                                   );\
      }


Tile::Tile( const TempOption *o )
: RawTarget(o),
  level( o->level ),
  location( o->location )
  {};

Tile::Tile( Tile &copy )
: RawTarget( copy ),
  level( copy.level ),
  location( copy.location )
  {};


bool Tile::equals( const Target & ) {
	return false; // Can not stack level tiles!
	}

Target Tile::clone( void ) {
// Functionality for this, even though we probably
// shouldn't allow cloning of tiles:
	Tile *newcopy = new Tile( *this );
	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);
	return Target(NEWTHIS);
  }

Tile::~Tile( void ) {
  //location = OFF_MAP;
	}


/*void Tile::setparent( TargetValue *t ) {
	// Check that t is a feature:
//	assert ( Target(t)->istype(tg_feature) );
//
//	feature = Target(t);
//  if (!lit) lit = feature->getVal(attrib_sunlit);
	}*/

Target Tile::getTarget( targetType type ) {
  switch( type ) {
    case tg_parent:
    case tg_feature:
    case tg_top_blocker:
      assertlev();
      return level->tileGetTarget(location, type);
      
    case tg_possessor:
    case tg_owner:
      assertlev();
      return level->tileGetTarget(location, tg_feature)->getTarget(tg_owner);
    
    default:
      return RawTarget::getTarget(type);
    }
  }

/*
Target Tile::getparent( void ) {
  return getfeature();
	}

Target Tile::getowner( void ) {
	return getparent()->getowner();
	}

Target Tile::getfeature( void ) {
  assertlev();
  return level->tileGetFeature(location);
	}


Target Tile::getpossessor( void ) {
  return getowner();
  }
*/

Level &Tile::getlevel( void ) {
  assertlev();
  return *level;
	}

Point3d Tile::getlocation( void ) {
  return location;
  }

Tile::Tile( std::iostream &inFile, TargetValue *TVP )
// Read RawTarget Base information:
: RawTarget( inFile, TVP ), level(NULL), location(OFF_MAP) {

  inFile.read( (char*) &location, sizeof(location) );

	}

void Tile::toFile( std::iostream &outFile ) {

	// Write RawTarget Base information:
	RawTarget::toFile( outFile );

  outFile.write( (char*) &location, sizeof(location) );
	}


void Tile::add( const Target &t ) {

  assertlev();
  level->tileAdd( THIS, t );
  }

bool Tile::remove( const Target &t ) {

  assertlev();
	return level->tileRemove( location, t );
	}

Image Tile::getimage( void ) {

  assertlev();
	return level->tileImage( location );
	}

void Tile::draw( void ) {

  assertlev();
	level->tileDraw( location );
	}

void Tile::takeDamage( const Attack &a ) {

  assertlev();
	level->tileTakeDamage( location, a );

  }

long Tile::getVal( StatType s ) {
  assertlev();
	return level->tileVal( location, s );

	}


targetType Tile::gettype( void ) const {
	return tg_tile;
	}
bool Tile::istype( targetType t ) const {
	if (t == tg_tile)
		return true;
	else
		return RawTarget::istype(t);
	}


String Tile::name( NameOption n_opt ) {

  assertlev();
	return level->tileName( location, n_opt );
  }


Image Tile::menuletter() {
	return getimage();
	}


bool Tile::capable( Action a, const Target &t ) {

  assertlev();
	return level->tileCapable( location, a, t );

	}

bool Tile::perform( Action a, const Target &t ) {

  assertlev();

  // Special cases to redirect add and remove items:
  switch (a) {

    case aContain:
      return level->tileAdd( THIS, t );
      
    case aRelease:
      return level->tileRemove( location, t );

    default:
    	return level->tilePerform( location, a, t );
    
    }
  
	}

/*
void Tile::unlight( void ) {
  assertlev();
	level->tileUnlight( location );
  }
*/
void Tile::unview( void ) {
  assertlev();
	level->tileUnview( location );
  }


void Tile::view( int visible, int hypotenuse ) {

  assertlev();
	level->tileView( location, visible, hypotenuse );
  
  }

/*void Tile::light( const Orthog &dir ) {

  assertlev();
	level->tileLight( location, dir );
  }
*/

Action Tile::autoAction( void ) {
  assertlev();
	return level->tileAutoAction( location );
	}

List<Target> *Tile::itemList( void ) {
  assertlev();
	return level->tileItemList( location );
  }

/*
Target Tile::findTarget( targetType type, const Target& t ) {

  assertlev();
  return level->findTarget( location, type, t );
  }
*/

void Tile::setTHIS( TargetValue *tvp ) {
  assertlev();

  THIS = tvp;

  //assert (0);

  level->tileSetTHIS( location, tvp );
  }


#endif // LEVELEL_CPP
