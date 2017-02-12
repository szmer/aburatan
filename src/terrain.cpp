// [terrain.cpp]
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
#if !defined ( TERRAIN_CPP )
#define TERRAIN_CPP

#include "terrain.hpp"
#include "message.hpp"
#include "random.hpp"
#include "grammar.hpp"
#include "player.hpp"
#include "level.hpp"


//--[ class Grave ]--//

targetType Grave::gettype( void ) const {
	return tg_grave;
	}

bool Grave::istype( targetType t ) const {

	if ( t == tg_grave )
		return true;
	else return Item::istype(t);

	}

bool Grave::capable( Action a, const Target &t ) {

	switch( a ) {

		case aLeave:
			// Graves are fixed to the ground:
			return false;

		case aSpMessage:
			return true;

		default:
			return Item::capable(a, t);
		}
	}

bool Grave::perform( Action a, const Target &t ) {

	switch( a ) {

		case aSpMessage:
			// Special message:
			if (open) Message::add( "An open grave is here." );
//      else Message::add( "A"+(age_str)+" grave is here.");

			return true;

		case aBlock:
			return t->perform( aOpen, Target(THIS) );


		default:
			return Item::perform(a, t);
		}

	}

void Grave::takeDamage( const Attack & ) {
  // Graves do not take damage.
	}

long Grave::getVal( StatType s ) {
	switch(s) {
    case attrib_empty_nodoor:
    case attrib_non_wall:
      return true;
		default:
			return Item::getVal(s);
		}
	}

Grave::Grave( const Option *o )
: Item(o) {
	open = o->open;
	}

Grave::Grave( Grave &rhs )
: Item(rhs) {

	open = rhs.open;
	}

Grave::Grave( std::iostream &inFile, TargetValue *TVP )
// Read Item base information:
: Item (inFile, TVP) {

	// Read bool open
	inFile.read( (char*) &open, sizeof(open) );

	}

void Grave::toFile( std::iostream &outFile ) {

	// Write Item base information:
	Item::toFile( outFile );

	// Write bool open
	outFile.write( (char*) &open, sizeof(open) );

	}

Target Grave::clone( void ) {

	Grave *newcopy = new Grave( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

bool Grave::equals( const Target &rhs ) {

	bool retVal = Item::equals( rhs );

	if (retVal) {
		// Good so far - same inventories and both of same type.
		if ( open     != ( (Grave*) rhs.raw() )->open )     retVal = false;
		}

	return retVal;

	}


String Grave::name( NameOption n_opt ) {
  switch (n_opt) {
    default:
    	return String( "#") + ( open?"open ":"" ) +"grave~" + Item::name(n_opt);
    }
    
	}

//--[ class Headstone ]--//
targetType Headstone::gettype( void ) const {
	return tg_headstone;
	}

bool Headstone::istype( targetType t ) const {

	if ( t == tg_headstone )
		return true;
	else return Item::istype(t);

	}

bool Headstone::capable( Action a, const Target &t ) {

	switch( a ) {
		case aBlock:
			// Headstones block the passage of *everything*
			// Later to check for non-corporeal/flying item types.
			return true;

		case aLeave:
			// As of yet, Headstones are fixed to the ground:
			return false;

		case aSpMessage:
			return true;

		default:
			return Item::capable(a, t);
		}
	}

bool Headstone::perform( Action a, const Target &t ) {

	switch( a ) {

		case aSpMessage:
			// Special message:
			Message::add( "A headstone is here." );
			return true;

    case aBeRead:
      // Give a message regarding the writing:
      if (!message.generated())
        message = Random::epitaph();
      if (t->istype(tg_player))
        Message::add("The headstone reads: \""+message.toString()+"\"");
      return true;

		case aBlock:
			return t->perform( aRead, Target(THIS) );


		default:
			return Item::perform(a, t);
		}

	}

void Headstone::takeDamage( const Attack & ) {
  // As of yet, headstones cannot be destroyed.
	}

long Headstone::getVal( StatType s ) {
	switch(s) {
		case attrib_opaque:
			return 5;

    case attrib_empty_nodoor:
    case attrib_non_wall:
      return true;
		default:
			return Item::getVal(s);
		}
	}


Action Headstone::autoAction( void ) {
  return aBlock;
	}


Headstone::Headstone( const Option *o )
: Item(o) {
  }

Headstone::Headstone( Headstone &rhs )
: Item(rhs) {
  message = rhs.message;
  }

Headstone::Headstone( std::iostream &inFile, TargetValue *TVP )
// Read Item base information:
: Item (inFile, TVP) {

  inFile >> message;

  }

void Headstone::toFile( std::iostream &outFile ) {

	// Write Item base information:
	Item::toFile( outFile );

  outFile << message;
	}

Target Headstone::clone( void ) {

	Headstone *newcopy = new Headstone( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

bool Headstone::equals( const Target & ) {
  // Headstones don't stack
  return false;
	}

String Headstone::name( NameOption n_opt ) {
  switch (n_opt) {
    default:
    	return String( "#headstone~" ) + Item::name(n_opt);
    }
	}


#endif // TERRAIN_CPP
