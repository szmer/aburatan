// [fluid.cpp]
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
#if !defined ( FLUID_CPP )
#define FLUID_CPP

#include "fluid.hpp"
#include "random.hpp"
#include "grammar.hpp"
#include "message.hpp"
#include "cloud.hpp"

targetType Fluid::gettype( void ) const {
	return tg_fluid;
	}

bool Fluid::istype( targetType t ) const {

	if ( t == tg_fluid )
		return true;
	else return Item::istype(t);

	}

bool Fluid::capable( Action a, const Target &t ) {

	switch( a ) {

		case aScatter:
			return false;

		case aBeQuaffed:
			return (Material::data[ f_material ].state == liquid ||
							Material::data[ f_material ].state == ooze)?true:false;

		case aSpMessage:
			return true;

		default:
			return Item::capable(a, t);
		}
	}

bool Fluid::perform( Action a, const Target &t ) {

	switch( a ) {

    case aPolymorph:
    	do {
    		f_material = (materialClass) Random::randint( m_total );
    		} while ( Material::data[ f_material ].state != powder
   	  				 && Material::data[ f_material ].state != ooze
   	  				 && Material::data[ f_material ].state != liquid);
    	image.val.color = Material::data[ f_material ].color;
      return Item::perform(a,t);


		case aSpMessage: {
      String thisplace = "This place";
      Target parent = getTarget(tg_parent);
      if (!!parent) thisplace = Grammar::plural(parent,Grammar::det_definite) + " here";
			Message::add(thisplace+" is covered in " + menustring() + ".");
			return true;
      }

		case aCombine:
			if ( equals(t) || t->equals( Target(THIS) ) )
				return true;

			return false;


		case aBeQuaffed: {

      if (t->getVal(attrib_visible)) {
    		String adj;
  			switch (f_material) {
    				case m_blood: adj = "metallic"; break;
    				case m_water: adj = "refreshing"; break;
    				default: adj = "revolting"; break;
    				}

  			if ( t->istype(tg_player) )
  				Message::add(Grammar::Subject(THIS,Grammar::det_definite) +" taste"+(getVal(attrib_plural)?" ":"s ")+adj+".");
  			else
  				Message::add(Grammar::Subject(THIS,Grammar::det_definite) +" look"+(getVal(attrib_plural)?" ":"s ")+adj+".");

        }

      getTarget(tg_controller)->perform(aAddBill,THIS);

      detach();
      
			return true;
      }
      
		default:
			return Item::perform(a, t);
		}

	}

long Fluid::getVal( StatType s ) {

	switch( s ) {

    case attrib_price:
      return Dollars(1);
      
		case attrib_discrete:
			return false;

		case attrib_abstract:
			return true;

		case attrib_need_container:
			// Fluid must be kept in a container
  			return true;

		case attrib_material:
			return f_material;

    case attrib_refreshment:
      // Todo:  change this to a sensible (ie calculated) value:
      return 1000;

    case attrib_weight:
      // Todo:  scale fluid weight based on quantity
      return Material::data[ f_material ].weight / 10;
      
    case attrib_sink_weight:
      // Liquid will not add to weight:
      return 0;

		default:
			return Item::getVal(s);
		}

	}

Fluid::Fluid( const Option *o )
: Item(o) {

	f_material = o->material;


	if ( f_material == m_invalid )
    RawTarget::doPerform(aPolymorph);

	image.val.color = Material::data[ f_material ].color;
	image.val.appearance = ',';
	}

Fluid::Fluid( Fluid &rhs )
: Item(rhs) {

	f_material = rhs.f_material;
	}

Fluid::Fluid( std::iostream &inFile, TargetValue *TVP )
// Read Item base information:
: Item (inFile, TVP) {

	// Read materialClass f_material
	inFile.read( (char*) &f_material, sizeof(f_material) );


	}

void Fluid::toFile( std::iostream &outFile ) {

	// Write Item base information:
	Item::toFile( outFile );

	// Write materialClass f_material
	outFile.write( (char*) &f_material, sizeof(f_material) );

	}

Target Fluid::clone( void ) {

	Fluid *newcopy = new Fluid( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

bool Fluid::equals( const Target &rhs ) {

	bool retVal = Item::equals( rhs );

	if (retVal) {
		// Good so far - same inventories and both of same type.
		if ( f_material != ( (Fluid*) rhs.raw() )->f_material ) retVal = false;
		}

	return retVal;


	}

Attack Fluid::getAttack( void ) {
	return Item::getAttack();
	}

void Fluid::takeDamage( const Attack &a ) {

  // Puddles splash when kicked:
	if (Material::data[ f_material ].state == liquid && a.c_values.penetration[pen_bash] > 2 )
    // Todo: check to see that the player can hear
		Message::add("Splash!");

  // Powder forms a cloud when kicked:
  if (Material::data[ f_material ].state == powder && a.c_values.penetration[pen_bash] > 2 ) {
    Cloud::Option cloud_opt;
    cloud_opt.material = f_material;
    Target parent = getTarget(tg_parent);
    if (!!parent)
      parent->perform( aContain, Cloud::create(cloud_opt) );
    detach();
    }

	}


String Fluid::name( NameOption n_opt ) {
  switch (n_opt) {
    default:
    	String retstr = "#" + String( Material::data[ f_material ].name )
                    + Item::name(n_opt);
    	return retstr;

  	}
    
  }

/*
void Fluid::identify( IDmask id_mask ) {

  if (id_mask & id_form)
    Material::data[form].known = true;

  Item::identify(id_mask);
  }
  
bool Fluid::identified( IDmask id_mask ) {
  IDmask override = id_none;

  if (Material::data[form].known)
    (int) override |= id_form;

  return Item::identified( (IDmask) (id_mask & ~override) );
  }
*/


#endif // FLUID_CPP
