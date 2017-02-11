// [money.cpp]
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
#if !defined ( MONEY_CPP )
#define MONEY_CPP

#include "money.hpp"
#include "random.hpp"
#include "grammar.hpp"
#include "actor.hpp"

targetType Money::gettype( void ) const {
	return tg_money;
	}

bool Money::istype( targetType t ) const {

	if ( t == tg_money )
		return true;
	else return Item::istype(t);

	}

bool Money::capable( Action a, const Target &t ) {

	switch( a ) {
		default:
			return Item::capable(a, t);
		}
	}

bool Money::perform( Action a, const Target &t ) {

	switch( a ) {

    case aPolymorph: {
      char rand = Random::randint(5);

      switch (rand) {
    
        case 0:
          material = m_ruby;
          break;
        case 1:
          material = m_emerald;
          break;
        case 2:
          material = m_gold;
          break;
        case 3:
          material = m_silver;
          break;
          
        default:
          material = m_paper;
          break;
      
        }

     	image.val.color = Material::data[ material ].color;
      // Force paper notes to be greenbacks:
      if (material == m_paper)
        image.val.color = cGreen;
      return Item::perform(a,t);
      }
      
		default:
			return Item::perform(a, t);
		}

	}

long Money::getVal( StatType s ) {

	switch( s ) {

    case attrib_denomination:
    case attrib_cash:
    case attrib_price:
      switch (material) {
        case m_paper: return Dollars(1);
        case m_silver:return Cents(1);
        default:
          // Todo: get price from material table:
          return 100;
        }
        
    case attrib_sort_category:
      return Item::getVal(s) + material;
      
    case attrib_weight:
    case attrib_sink_weight:
      // ATM Players can carry as much paper & silver as they wish:
      if (material == m_paper || material == m_silver)
        return 0;
        
      return 1+Item::getVal(attrib_weight);
      
		case attrib_material:
			return material;

		default:
			return Item::getVal(s);
		}

	}

Money::Money( const Option *o )
: Item( o ), material(o->material) {

	if ( material == m_invalid )
    doPerform(aPolymorph);

 	image.val.color = Material::data[ material ].color;

	}

Money::Money( Money &rhs )
: Item(rhs) {

	material = rhs.material;
	}

Money::Money( std::iostream &inFile, TargetValue *TVP )
// Read Item base information:
: Item (inFile, TVP) {

	// Read material
	inFile.read( (char*) &material, sizeof(material) );

	}

void Money::toFile( std::iostream &outFile ) {

	// Write Item base information:
	Item::toFile( outFile );

	// Write material
	outFile.write( (char*) &material, sizeof(material) );

	}

Target Money::clone( void ) {

	Money *newcopy = new Money( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

bool Money::equals( const Target &rhs ) {

	bool retVal = Item::equals( rhs );

	if (retVal) {
		// Good so far - same inventories and both of same type.
		if ( material != ( (Money*) rhs.raw() )->material ) retVal = false;
		}

	return retVal;

	}

String Money::name( NameOption n_opt ) {
  switch (n_opt) {
    default:

      if (material == m_paper)  return String("#dollar~") + Item::name(n_opt);
      if (material == m_silver) return String("#cent~") + Item::name(n_opt);
  
      String retstr = "#";

     	retstr += Material::data[ material ].name;

      if (material == m_gold)
       	retstr += " sovereign~";
      else
       	retstr += " shard~";
  

      return retstr + Item::name(n_opt);
    }

  }

#endif // MONEY_CPP

