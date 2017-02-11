// [temp.cpp]
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
#if !defined ( TEMP_CPP )
#define TEMP_CPP

#include "temp.hpp"
#include "grammar.hpp"


targetType Temp::gettype( void ) const {
	return tg_temp;
	}

bool Temp::istype( targetType t ) const {

	if ( t == tg_temp )
		return true;
	else if (!!targ && targ->istype(t))
    return true;
  else return RawTarget::istype(t);

	}

bool Temp::capable( Action a, const Target &t ) {

	if ( a != aCombine && a != aContain && a != aLoad && a != aUnload
     && a != aBeWorn )
		return true;

  if (!!targ) return targ->capable(a,t);

  // Temp objects without a targ reference can always be worn:
  if ( a == aBeWorn ) return true;

	return false;

	}

bool Temp::perform( Action a, const Target &t ) {

	switch( a ) {

    case aContain: {
      //Target parent = getTarget(tg_parent);
      if (!!parent)
        return parent->perform(a,t);
      return false;
      }

		default:
      if (!!targ) return targ->perform(a,t);
			return RawTarget::perform(a, t);
		}

	}

long Temp::getVal( StatType s ) {

	switch( s ) {

    case attrib_temp:
      return temp_val;

		default:
      if (!!targ) return targ->getVal(s);
			return RawTarget::getVal(s);
		}

	}

Temp::Temp( const Option *o )
: RawTarget( o ), str1(o->str1), str2(o->str2), temp_val(o->temp_val), targ(o->target) {}

Temp::Temp( Temp &rhs )
: RawTarget(rhs) {

	str1 = rhs.str1;
	str2 = rhs.str2;
  targ = rhs.targ->clone();
	}

Temp::Temp( std::iostream &inFile, TargetValue *TVP )
: RawTarget( inFile, TVP ) {

	// Attempt to load object of type temp:
  assert(0);
	}

void Temp::toFile( std::iostream & ) {

	// Attempt to save object of type temp:
  assert(0);

	}

Target Temp::clone( void ) {

	Temp *newcopy = new Temp( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

Target Temp::getTarget( targetType type ) {
//
  if (!!targ)
    return targ->getTarget(type);
  else
    return Target();
    //
  } //
  
/*
void Temp::setlocation( const Point3d &p3d ) {
//
  // Can be called with NULL targ:
  if (!!targ)
    targ->setlocation(p3d);
//
  RawTarget::setlocation(p3d);
  }//
*/
//
/*
Target Temp::findTarget( targetType type, const Target& t ) {
//
  if (!!targ)
    return targ->findTarget(type,t);
    return Target();
  }//
*/
void Temp::setParent( TargetValue *tvp ) {
  // Calls to Temp::setparent() are now forbidden.  Call Temp::detach()!
  assert (!targ);

  RawTarget::setParent(tvp);
  }

List<Target> *Temp::itemList( void ) {
  if (!!targ) return targ->itemList();
  
  return NULL;
	}


void Temp::add( const Target & ) {
  assert(0);
  }
  
bool Temp::remove( const Target & ) {
  assert(0);
  return false;
  }

int Temp::quantity( void ) {
  if (!!targ) return targ->quantity();
  return 1;
  }
  
Target Temp::detach( int num ) {
  if (!!targ)
    return targ->detach(num);
  return RawTarget::detach(num);
  }
 
bool Temp::equals( const Target & ) {

	return false; // Temp items don't stack

	}

/*String Temp::menustring( char len ) {

	return Grammar::plural( THIS ).abbrev(len);
	}

String Temp::name( void ) {
*/

String Temp::name( NameOption n_opt ) {
  switch (n_opt) {
    case n_menu_noprice:
    case n_menu:
      if (!!targ)
      	return str1+targ->name(n_opt)+str2;
      else
        return str1+str2;
    default:
      if (!!targ)
        return targ->name(n_opt);
      else
        return str1+str2;
    }
	}

String Temp::menustring( char len ) {

  if (!!targ)
   	return String(str1+targ->menustring()+str2).abbrev(len);
  else
    return String(str1+str2).abbrev(len);

  }

Image Temp::menuletter( void ) {
	return image;
	}

void Temp::setletter( char c ) {
	image.val.appearance = c;
	}


long Temp::getSkill( Skill skill, bool get_total ) {

  if (!!targ)
   	return targ->getSkill(skill,get_total);
  else
    return 0;
  }

#endif // TEMP_CPP
