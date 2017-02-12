// [quantity.cpp]
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
#if !defined ( QUANTITY_CPP )
#define QUANTITY_CPP

#include "quantity.hpp"
#include "message.hpp"
#include "grammar.hpp"
#include "random.hpp"
#include "actor.hpp"
#include "item.hpp"

int Quantity::quantity( void ) {
	return number;
	}

Target Quantity::detach( int detnum ) {

	Target me = THIS;

	if ( (detnum >= number) || (detnum < 1) ) return RawTarget::detach();

  Target ret_targ;

	assert( inventory && inventory->reset() );
  inventory->reset();

		Target myClone = inventory->get()->clone(),
           owner = inventory->get()->getTarget(tg_owner),
           deliverer = inventory->get()->getTarget(tg_controller);

    bool add_bill = false;
    // Code for shops/trading:
    if (!!owner && !!deliverer && deliverer->capable(aDeductBill,THIS) )
      add_bill = true;

		if ( (number -= detnum) == 1) {
      // Note: after this block, we cannot access *any*
      //       members of this object - THIS, inventory, etc.
			Target parent = getTarget(tg_parent);
			RawTarget::detach();
      //       because of the next line deletes 'this' :
			THIS->transform( myClone );
			if (!!parent)
        parent->perform( aContain, myClone );
			}

		if ( detnum == 1 )
      ret_targ = myClone->clone();
    else {

      Option q_opt;
      q_opt.target = myClone->clone();
      q_opt.quantity = detnum;
  		ret_targ = Quantity::create( q_opt );
      }

    if (add_bill)
      deliverer->perform(aAddBill,ret_targ);


    return ret_targ;

	}

void Quantity::identify( IDmask mask ) {
	assert( inventory && inventory->reset() );

  inventory->reset();
  inventory->get()->identify(mask);
  return;
  }
  
bool Quantity::identified( IDmask mask ) {
	assert( inventory && inventory->reset() );
  inventory->reset();
	return inventory->get()->identified(mask);
  }

/*
Target Quantity::getowner( void ) {

	assert( inventory && inventory->reset() );

  inventory->reset();
  return inventory->get()->getTarget(tg_owner);
  }

Target Quantity::getdeliverer( void ) {

	assert( inventory && inventory->reset() );

  inventory->reset();
  return inventory->get()->getTarget(tg_controller);
  }
*/

Target Quantity::getTarget( targetType type ) {
  switch( type ) {
    case tg_owner:
    case tg_controller:
    	assert( inventory && inventory->reset() );

      inventory->reset();
      return inventory->get()->getTarget(type);

    default:
      return RawTarget::getTarget(type);
    }
  }


targetType Quantity::gettype( void ) const {

	return tg_quantity;
	}

bool Quantity::istype( targetType t ) const {

	assert( inventory && inventory->reset() );
  inventory->reset();

  if   (t == tg_quantity)
		return true;
	else if ( inventory->get()->istype(t) ) return true;
	else return RawTarget::istype(t);

	}

compval Quantity::compare( RawTarget &rt ) {

	assert( inventory && inventory->reset() );
  inventory->reset();

  return inventory->get()->compare( rt );
	}


bool Quantity::capable( Action a, const Target &t ) {

	switch (a) {

    case aSetController:
    case aSetOwner: {
      // Are only capable of setting 't' as controller or owner
      // if we aren't contained within it.
      //Target parent = getTarget(tg_parent);

      if ( !parent )
        return true;
      if ( parent.toTarget() != t )
        return parent->capable(a,t);

      }

		case aCombine:
			if ( equals(t) ) return true;
			break;

		case aContain: {
			// Check for child:
    	assert( inventory && inventory->reset() );
      inventory->reset();
			// Try to add 't' to child Target:
			if ( inventory->get()->capable( aContain, t ) )
				return true;
			// If we can't, add 't' to parent:
      //Target parent = getTarget(tg_parent);
			if (!!parent)
        return false;
        
			return true;
      }

		case aView: {

      //Target parent = getTarget(tg_parent);
			if (!parent) return true;
			return parent->capable( a, t );
      }

		default:
			if ( RawTarget::capable(a, t) ) return true;

    	assert( inventory && inventory->reset() );
      inventory->reset();
			return inventory->get()->capable( a, t );
		}

	return false;
	}

bool Quantity::perform( Action a, const Target &t ) {

	switch (a) {

		case aDescribe:
			Message::add(menustring()+".");
			return true;

		case aCombine:
			if ( equals(t) ) {
				number += t->quantity();

        // Code for shops/trading:
//        if (!!getTarget(tg_owner) )
//        if (!!getTarget(tg_controller))
//          getTarget(tg_controller)->perform(aDeductBill,t);
         
				t.transform( Target(THIS) );
				return true;
				}
			break;

    case aSetController: {

      Target parent = getTarget(tg_parent);
      if (!parent || parent->getVal(attrib_itemized))

      // If item isn't owned, own it:
      if ( !getTarget(tg_owner)
      // ATM when someone dies their possessions become free:
      // REMEMBER: changes must be synched with Item::perform(aSetController)!
      || ! getTarget(tg_owner)->getVal(attrib_alive) )
        perform(aSetOwner, t);
        //set_owner = true;
        
      // Else Item is owned, so tell the owner
      else if (getTarget(tg_owner)->getVal(attrib_on_level) && getVal(attrib_on_level) )
        getTarget(tg_owner)->perform(aAlertTrespass,THIS);

    	assert( inventory && inventory->reset() );
      inventory->reset();
			return inventory->get()->perform( a, t );
      }

		case aContain: {
			// Check for child:
    	assert( inventory && inventory->reset() );
      inventory->reset();
			// Try to add 't' to child Target:
			if ( inventory->get()->perform( aContain, t ) )
				return true;
			// If we can't, add 't' to parent:
      Target p = getTarget(tg_parent);
			assert (!!parent);
			return parent->perform( aContain, t );
      }

		default:
			if ( RawTarget::perform( a, t ) )
				return true;
    	assert( inventory && inventory->reset() );
      inventory->reset();
			return inventory->get()->perform( a, t );
		}

	return false;
	}


Quantity::Quantity( const Option *o )
: RawTarget( o ), number(o->quantity) {

  assert (!! o->target );

	if (! RawTarget::perform(aContain,o->target) ) {
    assert(0);
    }

	}

Quantity::Quantity( Quantity &rhs )
: RawTarget(rhs) {

	number = rhs.number;
	}

Quantity::Quantity( std::iostream &inFile, TargetValue *TVP )
// Read RawTarget base information:
: RawTarget (inFile, TVP) {

	// Read char form
	inFile.read( (char*) &number, sizeof(number) );


	}

void Quantity::toFile( std::iostream &outFile ) {

	// Write RawTarget base information:
	RawTarget::toFile( outFile );

	// Write char number
	outFile.write( (char*) &number, sizeof(number) );


	}

Target Quantity::clone( void ) {

	Quantity *newcopy = new Quantity( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}


bool Quantity::equals( const Target &rhs ) {

	// For a stack to be equal, it has to be a stack of the same thing
	// or be compared with a single item of the same thing.
	bool retVal = RawTarget::equals( rhs );

	if (!retVal) {
    assert( inventory && inventory->reset() );
    inventory->reset();
		retVal = inventory->get()->equals(rhs);
    }

	return retVal;

	}


String Quantity::name( NameOption n_opt ) {

	assert( inventory && inventory->reset() );
  inventory->reset();

  String ret_str;

  if ( n_opt == n_menu
    || n_opt == n_menu_noprice
    || n_opt == n_default) {
    
    ret_str = inventory->get()->name(n_menu_noprice);
      
    Target owner = getTarget(tg_owner);
    if (!!owner && !owner->istype(tg_player))
      ret_str += " (unpaid, "+PriceStr(getVal(attrib_price)) +
                 "; "+PriceStr(inventory->get()->getVal(attrib_price))+" each)";
    }
  else
    ret_str = inventory->get()->name(n_opt);
      
  return ret_str;
	}

Image Quantity::menuletter( void ) {

	assert( inventory && inventory->reset() );
  inventory->reset();
  return inventory->get()->menuletter();
	}

void Quantity::setletter( char l ) {

	assert( inventory && inventory->reset() );
  inventory->reset();
	inventory->get()->setletter(l);
	return;
	}

Image Quantity::getimage( void ) {
	assert( inventory && inventory->reset() );
  inventory->reset();

	return inventory->get()->getimage();
	}

long Quantity::getVal( StatType s ) {

	switch(s) {

    case attrib_on_level:
      return RawTarget::getVal(s);

		case attrib_plural:
			if (RawTarget::getVal(attrib_visible))
			return true;
			return false;

		case attrib_person :
			return Grammar::p_third_pl;

    case attrib_itemized :
      return false;

    case attrib_cash :
    case attrib_price :
    case attrib_weight :
    case attrib_volume :
    case attrib_sink_weight:
     	assert( inventory && inventory->reset() );
      inventory->reset();
			return inventory->get()->getVal(s) * number;

		default:
    	assert( inventory && inventory->reset() );
      inventory->reset();
			return inventory->get()->getVal(s);
		}

	}

void Quantity::takeDamage( const Attack &a ) {

	assert( inventory && inventory->reset() );
  inventory->reset();

	if ( isCapable(aLeave) && inventory->get()->isCapable(aScatter) ) {

		Point3d start = getlocation();
    Target p = getTarget(tg_parent);
    if (!parent) return;
    if (a.instrument->getlocation() == start) return;
		Target ground = THIS;

		// Can scatter: select about half of the stack:
		int num = number / 4 + ( Random::randint(number/2) );
		if (num < 1) num = 1;
		if (num > number) num = number - 1;

		Target half = detach(num);

		half->doPerform(aFly);
		parent->perform(aContain,half);
    
    if (num > 1)
  		Message::add( Grammar::Subject(half,Grammar::det_definite) + " scatter"+(half->getVal(attrib_plural)?"":"s")+"!");

		Point me=ground->getlocation().toPoint(), attacker=a.instrument->getlocation().toPoint();
		int str = a.instrument->getVal( stat_str );

		Point finish( ( me.getx()-attacker.getx() ) * str,
									( me.gety()-attacker.gety() ) * str );

		while (half->quantity() > 1) {
			Target single = half->detach(1);

      if (!!single->getTarget(tg_owner)) a.deliverer->perform(aAddBill,single);
    
			Point delta( Random::randint(3)-1,Random::randint(3)-1 );
			single->doPerform(aFly);
			parent->perform(aContain,single);
			Target tSingle = Target(single);
      FlightPath::Option flight_opt;
      flight_opt.object = tSingle;
      flight_opt.start = start;
      flight_opt.delta = finish+delta;
			FlightPath::create( flight_opt );
			}

		half->doPerform(aFly);
    if (!!half->getTarget(tg_owner)) a.deliverer->perform(aAddBill,half);
		parent->perform(aContain,half);
    Target tHalf = Target(half);
    FlightPath::Option flight_opt;
    flight_opt.object = tHalf;
    flight_opt.start = start;
    flight_opt.delta = finish;
		FlightPath::create( flight_opt );
		}
//	else
//		Message::add( Grammar::Subject(THIS,Grammar::det_definite) + " lie still.");

	}


#endif // QUANTITY_CPP
