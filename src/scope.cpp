// [scope.cpp]
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
#if !defined ( SCOPE_CPP )
#define SCOPE_CPP

#include "scope.hpp"
#include "random.hpp"
#include "grammar.hpp"
#include "actor.hpp"
#include "level.hpp"
#include "global.hpp"


struct scopeStats {

  const char *rand_name;
	const char *name;

  ScopeFlags flags;

  bool known;

	static scopeStats *ScopeStats;
	static int numScope;

	} ScopeStorage [] = {

/* -Scopes-
 *
 *  Necroscope   - seeing invisible creatures
 *
 *  Telescope    - seeing object forms at distance
 *
 *  Microscope   - identifying substance composition
 *
 *  Periscope    - seeing around corners
 *
 *  Kaleidoscope - for tripping out
 *
 */
 
	{ "thin",   "#necroscope~",    sc_necro,  false },
	{ "bulky",  "#microscope~",    sc_magnif, false },
	{ "gilded", "#telescope~",     sc_telesc, false },
	{ "black",  "#periscope~",     sc_none,   false },
	{ "cloudy", "#kaleidoscope~",  sc_trippy, false }


  };

scopeStats *scopeStats::ScopeStats = ScopeStorage;
int scopeStats::numScope = 4;


targetType Scope::gettype( void ) const {
	return tg_scope;
	}

bool Scope::istype( targetType t ) const {

	if ( t == tg_scope )
		return true;
	else return Item::istype(t);

	}

bool Scope::capable( Action a, const Target &t ) {

	switch( a ) {

    case aBeApplied:
      return true;
      
		default:
			return Item::capable(a, t);
		}
	}


bool Scope::perform( Action a, const Target &t ) {

	switch( a ) {

    case aPolymorph:
     	form = Random::randint( scopeStats::numScope );
      return Item::perform(a,t);

    case aBeApplied: {

      if (!t) return false;
      
      ScopeFlags flags = scopeStats::ScopeStats[form].flags;

      if (flags & sc_trippy) {
        // Kaleidoscopes polymorph:
        t->doPerform(aPolymorph);
        identify(id_form);
        }

      if (flags & sc_telesc) {
        // Telescopes identify the item form:
        t->identify(id_form);
        identify(id_form);
        }

      if (flags & sc_magnif) {
        // Microscopes identify the substance composition,
        // but only work at short distance:
        Point3d pt_me = getlocation(), pt_target = t->getlocation();
        if ( abs(pt_me.getx() - pt_target.getx()) <= 1 &&
             abs(pt_me.gety() - pt_target.gety()) <= 1 &&
             abs(pt_me.getz() - pt_target.getz()) < 1 )
          t->identify(id_recipe);
        }

      if (flags & sc_necro) {
        // Necroscopes do nothing ATM :)
        }

      return true;
      }

		default:
			return Item::perform(a, t);
		}

	}

long Scope::getVal( StatType s ) {

	switch( s ) {

    case attrib_sort_category:
      return Item::getVal(s) + form;
      
    case attrib_weight:
    case attrib_sink_weight:
      return 25;
		case attrib_form:
			return form;
    //case attrib_targeting:
    //  return scopeStats::ScopeStats[form].targeting;
    case attrib_scope_flags:
      return scopeStats::ScopeStats[form].flags;

		default:
			return Item::getVal(s);
		}

	}

Scope::Scope( const Option *o )
: Item(o) {

  doPerform(aPolymorph);
  
	}

Scope::Scope( Scope &rhs )
: Item(rhs) {

	form = rhs.form;
	}

Scope::Scope( std::iostream &inFile, TargetValue *TVP )
// Read Item base information:
: Item (inFile, TVP) {

	// Read char form
	inFile.read( (char*) &form, sizeof(form) );

	}

void Scope::toFile( std::iostream &outFile ) {

	// Write Item base information:
	Item::toFile( outFile );

	// Write char form
	outFile.write( (char*) &form, sizeof(form) );

	}

Target Scope::clone( void ) {

	Scope *newcopy = new Scope( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

bool Scope::equals( const Target &rhs ) {

  // Ascope scopesources must not stack!
//  return false;


  bool retVal = Item::equals( rhs );

	if (retVal) {
		// Good so far - same inventories and both of same type.
		if ( form != ( (Scope*) rhs.raw() )->form ) retVal = false;
		}

	return retVal;

	}


String Scope::name( NameOption n_opt ) {
  switch (n_opt) {
    default:
      if ( identified(id_form) )
        return String( scopeStats::ScopeStats[form].name ) + Item::name(n_opt);
      else
        // If the scope type isn't yet ID'd, give a random description
        // of the sort "a xxxx spyglass"
        return String() << '#' << scopeStats::ScopeStats[form].rand_name
                        << " spyglass~" << Item::name(n_opt);
      
    }
    
	}


void Scope::identify( IDmask id_mask ) {

  if (id_mask & id_form)
    scopeStats::ScopeStats[form].known = true;

  Item::identify(id_mask);
  }
  
bool Scope::identified( IDmask id_mask ) {
  IDmask override = id_none;

  if (scopeStats::ScopeStats[form].known)
    (int&) override |= (int) id_form;

  return Item::identified( (IDmask) (id_mask & ~override) );
  }

#endif // SCOPE_CPP
