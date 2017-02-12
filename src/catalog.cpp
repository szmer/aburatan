// [catalog.cpp]
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
#if !defined ( CATALOG_CPP )
#define CATALOG_CPP

#include "catalog.hpp"
#include "visuals.hpp"

List<Catalogue::cat_Handle> Catalogue::catalogue;
# ifdef SOFT_LINK
  List<Catalogue::cat_Handle> Catalogue::links;
# endif // SOFT_LINK

void Catalogue::add( cat_Handle ch ) {
	catalogue.add( ch );
	}

bool Catalogue::remove( cat_Handle ch ) {

	if (  catalogue.find( ch )  ) {
		catalogue.remove();
		return true;
		}

	else
    return false;
//     Error::fatal("Catalogue::remove failed!");

	}

# ifdef SOFT_LINK
  void Catalogue::link( cat_Handle ch ) {
    links.add( ch );
    }
  
  bool Catalogue::unlink( cat_Handle ch ) {

    if ( links.find( ch ) ) {
  		links.remove();
      return true;
      }

    else
      return false;
//     Error::fatal("Catalogue::unlink failed!");
    }
# endif // SOFT_LINK
    
  
#include "message.hpp"
TargetValue *Catalogue::get( long l ) {
	if (!l)
    return NULL;

	assert ( l >= 0 );
  // Error::fatal("Traced error to Catalogue::get!");

	if ( catalogue.toTail() )
	do {
		if ( catalogue.get() == l )
			return catalogue.get().value;
		} while ( catalogue.previous() );

# ifdef SOFT_LINK
	if ( links.toTail() )
	do {
		if ( links.get() == l )
			return links.get().value;
		} while ( links.previous() );
# endif // SOFT_LINK

	return new TargetValue(l);
	}

void Catalogue::clr( void ) {
	catalogue.clr();
# ifdef SOFT_LINK
  links.clr();
# endif // SOFT_LINK
	}

long Catalogue::numitems( void ) {
	return catalogue.numData();
	}
  
# ifdef SOFT_LINK
  long Catalogue::numlinks( void ) {
  	return links.numData();
  	}
# endif // SOFT_LINK


#endif // CATALOG_CPP
