// [stairs.cpp]
// (C) Copyright 2003 Michael Blackney
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
#if !defined ( STAIRS_CPP )
#define STAIRS_CPP

#include "stairs.hpp"
#include "message.hpp"
#include "grammar.hpp"
#include "level.hpp"
#include "global.hpp"
#include "input.hpp"

targetType Stairs::gettype( void ) const {
	return tg_stairs;
	}

bool Stairs::istype( targetType t ) const {

	if ( t == tg_stairs )
		return true;
	else return Item::istype(t);

	}

bool Stairs::capable( Action a, const Target &t ) {

	switch( a ) {
		case aBlock:
			return false;

    case aBeAttacked:
		case aBeKicked:
			return false;

		case aSpMessage:
			// Special message:
			return true;

		case aLeave:
			// As of yet, Stairs are fixed to the ground:
			return false;

		case aBeAscended:
			return ( image.val.appearance == '<')?true:false;

		case aBeDescended:
			return ( image.val.appearance == '>')?true:false;

		default:
			return Item::capable(a, t);
		}
	}

bool Stairs::perform( Action a, const Target &t ) {

	switch( a ) {


		case aSpMessage: {
			// Special message:
			String message = "Stairs here lead ";
			if (connecting > getlevel().loc().getz() ) {
				message += "upwards.";
				if ( global::isNew.upstairs ) {
					message += String("  <?Press '\\") + Keyboard::keytostr(keyMap[kAscend].dkey) + "' to ascend them.>";
					global::isNew.upstairs = false;
					}
				}
			else {
				message += "downwards.";
				if ( global::isNew.downstairs ) {
					message += String("  <?Press '\\") + Keyboard::keytostr(keyMap[kDescend].dkey) + "' to descend them.>";
					global::isNew.downstairs = false;
					}
				}
			Message::add( message );

			return true;
		  }
		default:
			return Item::perform(a, t);
		}

	}

long Stairs::getVal( StatType s ) {
	switch(s) {
		case attrib_connecting:
			return connecting;
    case attrib_has_stairs:
      return true;
		default:
			return Item::getVal(s);
		}
	}

Stairs::Stairs( const Option *o )
: Item(o) {

	connecting = o->connecting;

	}

Stairs::Stairs( Stairs &rhs )
: Item(rhs) {

	connecting = rhs.connecting;
	}

Stairs::Stairs( std::iostream &inFile, TargetValue *TVP )
// Read Item base information:
: Item (inFile, TVP) {

	// Read char connecting
	inFile.read( (char*) &connecting, sizeof(connecting) );


	}

void Stairs::toFile( std::iostream &outFile ) {

	// Write Item base information:
	Item::toFile( outFile );

	// Write char connecting
	outFile.write( (char*) &connecting, sizeof(connecting) );


	}

Target Stairs::clone( void ) {

	Stairs *newcopy = new Stairs( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

bool Stairs::equals( const Target & ) {

	return false; // Stairs never stack!

	}


String Stairs::name( NameOption n_opt ) {
  switch (n_opt) {
    default:
    	return "#staircase~" + Item::name(n_opt);
  	}
  }

//void Stairs::setlocation( const Point3d &p3d ) {

	//Item::setlocation(p3d);
void Stairs::setParent( TargetValue *p ) {
	Item::setParent(p);

	char levHeight = getlocation().getz();

	if (connecting > levHeight) {
		image = Image( cGrey, '<' );
		letter = '<';
		}
	else if (connecting < levHeight) {
		image = Image( cGrey, '>' );
		letter = '>';
		}
	else {
		// Attempt to make an intra-level staircase!
    assert(0);
    }

	}

#endif // STAIRS_CPP
