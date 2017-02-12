// [bagbox.cpp]
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
#if !defined ( BAGBOX_CPP )
#define BAGBOX_CPP

#include "bagbox.hpp"
#include "grammar.hpp"
#include "misc.hpp"
#include "random.hpp"
#include "message.hpp"
#include "ammo.hpp"

//--[ class Container ]------//

targetType Container::gettype( void ) const {
	return tg_container;
	}

bool Container::istype( targetType t ) const {

	if ( t == tg_container )
		return true;
	else return Item::istype(t);

	}

bool Container::capable( Action a, const Target &t ) {

	switch( a ) {
		case aLoad:
			// Can always load; no capacity, but cannot hold fluids:
			if ( !t || ( t != Target(THIS) && !t->istype(tg_fluid) ) )
				return true;
			return false;

		case aUnload:
			// Can unload if the container has an inventory:
			if (inventory && inventory->reset())
				return true;
			return false;

		default:
			return Item::capable(a, t);
		}
	}

bool Container::perform( Action a, const Target &t ) {

	switch( a ) {
		case aLoad:

			if (t != Target(THIS))
				perform(aContain,t->detach());

			return true;

		case aUnload:
			if (inventory && inventory->reset()) {
				if (inventory->find(t)) {
          Target my_parent = getTarget(tg_parent);
          assert(!!my_parent);
					my_parent->perform(aContain,t);
          }
				return true;
				}
			return false;

		default:
			return Item::perform(a, t);
		}

	}

String Container::name( NameOption n_opt ) {

  switch (n_opt) {

    default:
      return Item::name(n_opt);
    }
  }
    

long Container::getVal( StatType s ) {

	switch( s ) {

    case attrib_sort_category:
      return Item::getVal(s) + ((inventory&&inventory->reset())?0:255);

    case attrib_itemized:
       return false;
      
		default:
			return Item::getVal(s);
		}

	}

Container::Container( const Option *o )
: Item( o ) {
	}

Container::Container( Container &rhs )
: Item(rhs) {
	}

Container::Container( std::iostream &inFile, TargetValue *TVP )
// Read Item base information:
: Item (inFile, TVP) {

	}

void Container::toFile( std::iostream &outFile ) {

	// Write Item base information:
	Item::toFile( outFile );


	}

bool Container::equals( const Target &rhs ) {

	bool retVal = Item::equals( rhs );

	return retVal;

	}

//--[ class Bag ]------//


targetType Bag::gettype( void ) const {
	return tg_bag;
	}

bool Bag::istype( targetType t ) const {

	if ( t == tg_bag )
		return true;
	else return Container::istype(t);

	}

bool Bag::capable( Action a, const Target &t ) {

	switch( a ) {
		default:
			return Container::capable(a, t);
		}
	}

bool Bag::perform( Action a, const Target &t ) {

	switch( a ) {
		default:
			return Container::perform(a, t);
		}

	}

long Bag::getVal( StatType s ) {

	switch( s ) {

    case attrib_price:
      return Cents(20);
      
    case attrib_weight:
    case attrib_sink_weight:
      return Material::data[m_leather].weight / 10 +Item::getVal(attrib_weight);

		default:
			return Container::getVal(s);
		}

	}

Bag::Bag( const Option *o )
: Container(o) {

	}

Bag::Bag( Bag &rhs )
: Container(rhs) {

	}

Bag::Bag( std::iostream &inFile, TargetValue *TVP )
// Read Container base information:
: Container (inFile, TVP) {

	}

void Bag::toFile( std::iostream &outFile ) {

	// Write Container base information:
	Container::toFile( outFile );

	}

Target Bag::clone( void ) {

	Bag *newcopy = new Bag( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

bool Bag::equals( const Target &rhs ) {

	bool retVal = Container::equals( rhs );

	return retVal;

	}


String Bag::name( NameOption n_opt ) {
  switch (n_opt) {
    case n_menu_noprice:
    case n_menu: {
    	/* This is the function that determines the contents
    	 * of the bag and gives a little summary string,
    	 *
    	 * like: "a bag {35 bullets}"
    	 *       "a bag {empty}"
    	 *       "a bag {guns and bullets}"
    	 *
    	 *
    	 */

    	String retstr = "#bag~";

    	if (!inventory || !inventory->get())
    		retstr += " [empty]";

    	else {
    		// If only one item in the bag:
    		if (inventory->numData() == 1)
    			retstr += " [contains " + inventory->get()->menustring() + "]";

    		// More than one item so summarise:
    		else
    			retstr += " [contains " + describeList(*inventory) + "]";

    		}

     	return retstr + Container::name(n_opt);
    	}

    default:
    	return String( "#bag~" ) + Container::name(n_opt);
    }
    
  }


//--[ class Bottle ]------//


targetType Bottle::gettype( void ) const {
	return tg_bottle;
	}

bool Bottle::istype( targetType t ) const {

	if ( t == tg_bottle )
		return true;
	else return Container::istype(t);

	}

bool Bottle::capable( Action a, const Target &t ) {

	switch( a ) {
		case aBeQuaffed:
			if (inventory && inventory->reset())
				if ( inventory->get()->isCapable(aBeQuaffed) )
					return true;

		case aLoad:
			// Bottles can only contain fluids:
			if (!t || t->istype(tg_fluid))
				// Bottles can presently only contain a single fluid:
				if ( !inventory || !inventory->reset() )
					return true;

			return false;

		default:
			return Container::capable(a, t);
		}
	}

bool Bottle::perform( Action a, const Target &t ) {

	switch( a ) {
    case aPolymorph:
    	switch( Random::randint( 2 ) ) {
    		//case 0:
    		//  b_material = m_clay;
    		//  break;
    		case 1:
    			b_material = m_leather;
    			break;

    		default:
    			b_material = m_glass;
    		}
    	image.val.color = Material::data[ b_material ].color;

      if (inventory && inventory->reset())
        inventory->get()->doPerform(aPolymorph);
      
      return Container::perform(a,t);
      
		case aBeQuaffed: {

      if (t->getVal(attrib_visible)) {
  			String object = Grammar::Object(THIS,Grammar::det_definite);
  			Message::add(Grammar::Subject(t,Grammar::det_definite) +" drink"+(t->getVal(attrib_plural)?" ":"s ")+"from "+object+".");
        }

			if (inventory && inventory->reset()) {
				Target liquid = inventory->get();
				liquid->perform(aBeQuaffed,t);
				return true;
				}
			return false;
      }

		default:
			return Container::perform(a, t);
		}

	}

long Bottle::getVal( StatType s ) {

	switch( s ) {

    case attrib_price:
      return Cents(50)+Container::getVal(s);

    case attrib_quaff_contents_only:
      return true;
      
    case attrib_sort_category:
      return Container::getVal(s) +
             ((inventory&&inventory->reset())?inventory->get()->getVal(attrib_material):0);
      
    case attrib_weight:
      return Material::data[ b_material ].weight / 10 +Item::getVal(attrib_weight);
      
    case attrib_refreshment:
      if (!inventory || !inventory->reset())
        return 0;
        
      return inventory->get()->getVal(attrib_refreshment);

    case attrib_material:
      return b_material;

		default:
			return Container::getVal(s);
		}

	}

Image Bottle::getimage( void ) {
  // Bottle image colour is that of fluid contents if any:
  Image ret_val = image;
  if ( inventory && inventory->reset() ) {
    ret_val.val.color = inventory->get()->getimage().val.color;
    }
  return ret_val;
  }

Bottle::Bottle( const Option *o )
: Container(o) {

	image.val.appearance = '!';

	b_material = o->material;
	if ( b_material == m_invalid )
    doPerform(aPolymorph);
    
	image.val.color = Material::data[ b_material ].color;

	}

Bottle::Bottle( Bottle &rhs )
: Container(rhs) {

	b_material = rhs.b_material;
	}

Bottle::Bottle( std::iostream &inFile, TargetValue *TVP )
// Read Container base information:
: Container (inFile, TVP) {

	// Read materialClass b_material
	inFile.read( (char*) &b_material, sizeof(b_material) );
	}

void Bottle::toFile( std::iostream &outFile ) {

	// Write Container base information:
	Container::toFile( outFile );

	// Write materialClass b_material
	outFile.write( (char*) &b_material, sizeof(b_material) );
	}

Target Bottle::clone( void ) {

	Bottle *newcopy = new Bottle( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

bool Bottle::equals( const Target &rhs ) {

	bool retVal = Container::equals( rhs );

	if (retVal) {
		// Good so far - same inventories and both of same type.
		if ( b_material != ( (Bottle*) rhs.raw() )->b_material ) retVal = false;
		}

	return retVal;

	}


String Bottle::name( NameOption n_opt ) {
  switch (n_opt) {
    default:
     	/* This is the function that determines the contents
    	 * of the bottle and gives a little summary string,
    	 *
    	 * like: "an empty bottle"
    	 *       "a bottle of water"
    	 *
    	 */

    	String botName;

    	switch (b_material) {
    		case m_glass:
    			botName = "vial~";
    			break;

    		case m_leather:
    			botName = "waterskin~";
    			break;

    		case m_clay:
    			botName = "gourd~";
    			break;

    		default:
    			botName = "bottle~";
    			break;
    		}


    	String retstr = "#";

    	if (!inventory || !inventory->get()) {
    		retstr += "empty " + botName;
    		}

    	else {
    		retstr += botName;
    		// If only one item in the bottle:
    		if (inventory->numData() == 1)
    			retstr += " of " + Grammar::plural(inventory->get()/*,Grammar::det_possessive*/);

    		// More than one item so summarise:
    		else
    			retstr += " of " + describeList(*inventory);

    		}

    	return retstr + Container::name(n_opt);

    }
    
  }


//--[ class Quiver ]------//

struct QuiverStats {
  const char *name;

  // How to see if an item will fit in the quiver:
  targetType type;
  StatType stat;
  long value;
  bool result;

  Volume capacity;

  int max_types;
  
  };

#define NUM_QUIVER 2
const QuiverStats quiverStats [ NUM_QUIVER ] = {
  { "#quiver~",        tg_ammo,    attrib_ammotype, bul_arrow, true,   v_long, 1 },
  { "#bandolier~",     tg_ammo,    attrib_ammotype, bul_arrow, false,  v_long, 1 },
//  { "#gunbelt~",       tg_ranged,  attrib_ammotype, bul_arrow, false,  v_long, 1 },
//  { "#heavy gunbelt~", tg_ranged,  attrib_ammotype, bul_arrow, false,  v_long, 2 },
//  { "#scabbard~",      tg_melee,   attrib_meleetype, mel_sword, true,  v_long, 1 },
  
  };

targetType Quiver::gettype( void ) const {
	return tg_quiver;
	}

bool Quiver::istype( targetType t ) const {

	if ( t == tg_quiver )
		return true;
	else return Container::istype(t);

	}

bool Quiver::canCarry( int type, Target t ) {

  return ( t->istype( quiverStats[type].type )
        && ( t->getVal( quiverStats[type].stat ) == quiverStats[type].value )
             == quiverStats[type].result );
  }


bool Quiver::capable( Action a, const Target &t ) {

	switch( a ) {
		case aBeWorn:
			return true;

		case aLoad:

      if (!t) {
        // Can only fit more in if current contents are less
        // than the capacity:
        int bulk = 0;
        if ( inventory && inventory->reset() )
        do {
          bulk += inventory->get()->getVal( attrib_volume );
          } while ( inventory->next() );

        if ( bulk < quiverStats[form].capacity )
          return true;

        return false;
        }

      // If the maximum number of item types is reached, can
      // only add more items if they will stack:
      if ( inventory && inventory->numData() >= quiverStats[form].max_types ) {
        bool can_stack = false;

        inventory->reset();
        do {
          if ( inventory->get()->equals(t) )
            can_stack = true;
          } while (inventory->next());

        if (! can_stack )
          return false;
        }
        
			// Check to see that the ammo fills the requirements:
			if ( canCarry(form, t) ) {
      
				if (  !inventory || !inventory->reset() )
					return true;

        // Can only fit more in if current+new contents
        // are less than the capacity:
        int bulk = 0;
        if ( inventory && inventory->reset() )
        do {
          bulk += inventory->get()->getVal( attrib_volume );
          } while ( inventory->next() );

        if ( bulk+t->getVal(attrib_volume) < quiverStats[form].capacity )
          return true;
        }

			return false;

		default:
			return Container::capable(a, t);
		}
	}

bool Quiver::perform( Action a, const Target &t ) {

	switch( a ) {

    // Todo: poly quivers and poly ammunition to something appropriate
    case aPolymorph:

      if (inventory && inventory->reset()) {
        // Remove contents (if any)
        // Polymorph the quiver
        // Then add something appropriate (if contained something before)
        }
      else {

        form = NUM_QUIVER;
        
        if (!t)
          form = Random::randint(NUM_QUIVER);
        else
          for (int i = 0; i < NUM_QUIVER; ++i)
            if ( canCarry(i,t) )
              form = i;

        assert(form != NUM_QUIVER);
        //Error::fatal("Invalid Quiver polymorph!");
        }
        
      return Container::perform(a,t);

		default:
			return Container::perform(a, t);
		}

	}

long Quiver::getVal( StatType s ) {

	switch( s ) {

    case attrib_price:
      return Cents(150);
      
    case attrib_weight:
      return Material::data[ m_leather ].weight / 10 +Item::getVal(attrib_weight);
      
		case attrib_slot:
			return s_pack;

    case attrib_filled_quiver:
      // Assumption: Ammo holders only contain ammo!
      return ( quiverStats[form].type == tg_ammo && inventory && inventory->reset() );
  

		default:
			return Container::getVal(s);
		}

	}

Quiver::Quiver( const Option *o )
: Container(o) {

	image.val.appearance = '[';
  image.val.color = cBrown;

  perform( aPolymorph, o->contents );

	}

Quiver::Quiver( Quiver &rhs )
: Container(rhs) {

  form = rhs.form;
	}

Quiver::Quiver( std::iostream &inFile, TargetValue *TVP )
// Read Container base information:
: Container (inFile, TVP) {

	// Read int form
	inFile.read( (char*) &form, sizeof(form) );
	}

void Quiver::toFile( std::iostream &outFile ) {

	// Write Container base information:
	Container::toFile( outFile );

	// Write int form
	outFile.write( (char*) &form, sizeof(form) );
	}

Target Quiver::clone( void ) {

	Quiver *newcopy = new Quiver( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

bool Quiver::equals( const Target &rhs ) {

	bool retVal = Container::equals( rhs );

	if (retVal) {
		// Good so far - same inventories and both of same type.
		if ( form != ( (Quiver*) rhs.raw() )->form ) retVal = false;
		}

	return retVal;

	}


String Quiver::name( NameOption n_opt ) {
  String retstr = quiverStats[form].name;
  
  switch (n_opt) {
    case n_menu_noprice:
    case n_menu:

    	if (inventory && inventory->reset())
      do {
        retstr += " [";
        retstr += inventory->get()->menustring();
        retstr += "]";
        } while( inventory->next() );


      return retstr + Container::name(n_opt);

    default:
      return retstr + Container::name(n_opt);
    }
    
  }


Image Quiver::getimage( void ) {

	char c = image.val.color;

	if ( inventory && inventory->reset() )
	do {
		c = inventory->get()->getimage().val.color;
		}	while ( inventory->next() );

	return Image(c,image.val.appearance);
	}


#endif // BAGBOX_CPP
