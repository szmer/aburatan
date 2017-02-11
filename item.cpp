// [item.cpp]
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
#if !defined ( ITEM_CPP )
#define ITEM_CPP
#include "item.hpp"
#include "quantity.hpp"
#include "grammar.hpp"
#include "message.hpp"
#include "actor.hpp"
#include "map.hpp"

#define CENTS_PER_DOLLAR 100
#define CP100_PER_CENT 100
long Dollars( long d ) {
  return d * CENTS_PER_DOLLAR * CP100_PER_CENT;
  }
  
long   Cents( long c ) {
  return c * CP100_PER_CENT;
  }

long   CentP100( long cp ) {
  return cp;
  }
  
long WholeDollars( long price ) {
  return price / CP100_PER_CENT / CENTS_PER_DOLLAR;
  }
  
long   WholeCents( long price ) {
  return price / CP100_PER_CENT % CENTS_PER_DOLLAR;
  }
  
long    PartCents( long price ) {
  return price % CP100_PER_CENT;
  }

String PriceStr( long price, bool round_up ) {
// Converts a long to a price string

// e.g. 12300 -> "$1.23"
//       2300 -> "23c"

  int dollars = WholeDollars(price);
  int cents   = WholeCents(price);
  int cp100   = PartCents(price);

  // We round up any fraction of a cent:
  if (cp100 && round_up)
    ++cents;

  String ret_str;
  
  if (dollars || !cents) {
    ret_str = "$";
    ret_str += (long) dollars;
    if (cents) {
      ret_str += ".";
      if (cents < 10)
        ret_str += "0";
      ret_str += (long) cents;
      }
    }
  else {
    ret_str = (long)cents + String("c");
    }

  return ret_str;
    
  }

//--[ class Item ]------------
Item::Item( const Option *o )
: RawTarget(o),
  letter(o->letter),
  still(true),
  //quality(0),
  controller(NULL),
  owner(NULL) {}

Item::Item( Item &copy )
: RawTarget( copy ) {

	letter = copy.letter;
	still = copy.still;
  //quality = copy.quality;

//  controller = copy.controller;
//  owner = copy.owner;

  controller = copy.controller;
  if (!controller)
    controller = copy.getTarget(tg_possessor);
  if (!controller)
    controller = copy.getTarget(tg_controller);
    
  owner = copy.getTarget(tg_owner);
  if (!owner)
    owner = controller;

  //if (!controller && parent != NULL) perform(aSetController,parent.toTarget());
	}

Item::~Item() {

  }


Item::Item( std::iostream &inFile, TargetValue *TVP )
// Read RawTarget Base information:
:	RawTarget( inFile, TVP ) {

  inFile >> controller;

  inFile >> owner;

	inFile.read( (char*) &letter, sizeof(letter) );

	inFile.read( (char*) &still, sizeof(still) );

	//inFile.read( (char*) &quality, sizeof(quality) );
	}

void Item::toFile( std::iostream &outFile ) {

	// Write RawTarget Base information:
	RawTarget::toFile( outFile );

  outFile << controller;
  outFile << owner;

	outFile.write( (char*) &letter, sizeof(letter) );
	outFile.write( (char*) &still, sizeof(still) );
	//outFile.write( (char*) &quality, sizeof(quality) );

	}


Image Item::menuletter() {
  if (letter)
    return Image(cGrey,letter);
  else
  	return getimage();
	}

void Item::setletter( char i ) {
	letter = i;
	}

targetType Item::gettype( void ) const {
	return tg_item;
	}

bool Item::istype( targetType t ) const {
	if ( t == tg_item ) return true;
	else return RawTarget::istype(t);
	}

Target Item::getTarget( targetType type ) {
  switch( type ) {
    case tg_owner:
    	return owner;
      
    case tg_controller: {
      Target deliverer = controller;

      if (!deliverer)
        deliverer = getTarget(tg_possessor);

      if (!deliverer)
        deliverer = THIS;
        
      return deliverer;
      }
      
    default:
      return RawTarget::getTarget(type);
    }
	}

bool Item::capable( Action a, const Target &t ) {

	switch (a) {

    case aSetController:
    case aSetOwner: {
      // Are only capable of setting 't' as controller or owner
      // if we aren't contained within it.
      Target parent = getTarget(tg_parent);
      
      if ( !parent )
        return true;
      if ( parent != t )
        return parent->capable(a,t);
        
      return false;
      }

		case aScatter:
			return true;

		case aCombine:
			if ( equals(t) || t->equals( Target(THIS) ) ) return true;
			return false;

		case aLeave:
			return true;

		case aView:
      // If we are floating detached in limbo, we're not visible to the
      // eye, but likewise we're not *in*visible either.  Assume invisible:
  		//if (location == OFF_MAP)
      if (parent == NULL)
        return false;
        
			//if (!parent.toTarget())
      //  return false;

      // Otherwise, we are only as visible as our surroundings:
			return parent->capable( a, t );

    case aBeAttacked:
		case aBeKicked:
			return true;

		case aBeWielded:
			return true;

		default:
			return RawTarget::capable( a, t );

		}

	}

bool Item::perform( Action a, const Target &t ) {

  bool set_owner = (a == aSetOwner);
  
	switch (a) {

    case aPolymorph:
      draw();
      return true;

    case aSetController: {

      Target parent = getTarget(tg_parent);
      if (!parent || parent->getVal(attrib_itemized))
      // If item isn't owned, own it:
      if ( !getTarget(tg_owner)
      // ATM when someone dies their possessions become free:
      // REMEMBER: changes must be synched with Quantity::perform(aSetController)!
      || ! owner->getVal(attrib_alive) )
        set_owner = true;
        
      // Item is owned, so tell the owner
      else if (owner->getVal(attrib_on_level) && getVal(attrib_on_level) )
        owner->perform(aAlertTrespass,THIS);
      }
      
      // Fall through:
    case aSetOwner: {

      Target parent = getTarget(tg_parent);
      
      if (set_owner && !!t && t->istype(tg_player))
        identify(id_owner);

      Target cont;

      // Set controller to null if t is null
      if (!t)
        cont = Target(NULL);
      // or if we are contained within t
      else if ( parent == t || !capable(a,t) )
            cont = Target(NULL);
           // Otherwise set controller to t
           else
             cont = t;

      if (set_owner)
        owner = cont;
        
      if (a == aSetController)
        controller = cont;

      // If we are owned or controlled, so are our contents:
      if (inventory && inventory->reset() )
      do {
        inventory->get()->perform(a, cont);
        } while (inventory->next());

      return true;
    
      }

    case aFly:
			still = false;
			return true;

		case aLand:
			still = true;
			return true;

		case aCombine:
			if ( equals(t) || t->equals( Target(THIS) ) ) {

				Target parent = getTarget(tg_parent);
				Target me = Target(THIS);

        Quantity::Option q_opt;
        q_opt.target = Target(THIS)->clone();
        q_opt.quantity = 1;

				Target stack = Quantity::create( q_opt );
				detach();

				if (t->quantity() > 1)
					stack->perform ( aCombine, t );
				else {
          q_opt.target = t->clone();
					stack->perform ( aCombine, Quantity::create( q_opt ) );
          }

        assert(!!parent);
				parent->perform( aContain, stack );

				THIS->transform(stack);
				t.transform(stack);

				return true;
				}
        
      assert(0);
      //Error::fatal("Combine inequal items! "+menustring()+" + "+t->menustring());
			return false;

		case aDescribe:
			Message::add(menustring()+".");
			return true;

		case aAttack:
			if ( RawTarget::getVal(attrib_visible) || t->getVal(attrib_visible) ) {
				String subject = Grammar::Subject(THIS,Grammar::det_definite);
				String object = Grammar::Object(t,Grammar::det_definite);
				Message::add( subject + " hit"+( getVal(attrib_plural)?" ":"s " ) + object + "." );
				}

			t->takeDamage( getAttack() );
			return true;


		case aBeEaten: {

      // Food must be eaten by something!
      if (!t)
        return false;

      // MSG: You eat the item.
			String object = Grammar::Object(THIS,Grammar::det_definite);
      // One of us must be visible to let the player know:
      if (t->getVal(attrib_visible) || getVal(attrib_visible))
  			Message::add(Grammar::Subject(t,Grammar::det_definite) +" eat"+(t->getVal(attrib_plural)?" ":"s ")+object+".");

      // Make sure that we pay for the item if required:
      Target deliverer = controller;
      if (!deliverer) {
      
        //Target parent = getTarget(tg_parent);
        //assert(!!parent);
        if ( parent->istype(tg_creature) )
          deliverer = parent.toTarget();
        else
          deliverer = THIS;
        }

      // Whether the flavour is appreciated is defined by the eater:
      Appetite appetite = (Appetite) t->getVal(attrib_appetite),
               flavour  = (Appetite) getVal(attrib_flavour);

      // This part gives the adjective that describes the flavour:
      String str_flavour, str_tastes, str_yuk;
      bool taste;
      
      // Message of the type: Blech!  This cadaver tastes foul!
      //                      str_yuk + name + str_tastes + str_flavour

      // If we like it:
      if ( appetite & flavour )
      // Give the reason why we like the flavour:
        switch ( flavour ) {

          case ap_carnmeat:
          // Yum! Fresh meat!
            taste = true;
            str_flavour = "fresh!"; break;
            
          case ap_herbmeat:
            taste = true;
            str_flavour = "like chicken."; break;
            
          case ap_carrion:
            taste = true;
            str_flavour = "good and dead."; break;
          
          case ap_sweet:
            taste = true;
            str_flavour = "sweet!"; break;
  
          case ap_fruit:
            taste = true;
            str_flavour = "delicious."; break;
            
          case ap_veg:
            taste = true;
            str_flavour = "nutritious."; break;
            
          case ap_grain:
            str_yuk = "Mmm!  ";
            taste = true;
            str_flavour = "wholesome."; break;
            
          case ap_grass:
            taste = true;
            str_flavour = "starchy."; break;

          case ap_fabric:
            taste = true;
            str_flavour = "linty."; break;
            
          case ap_wood:
            taste = true;
            str_flavour = "grainy."; break;
            
          case ap_alien:
            taste = true;
            str_flavour = "foreign."; break;

          // str_flavour won't be cannibal since we aren't yet a creature,
          // so no check for same species:
          // case ap_cannibal:

          case ap_omni:
            taste = false;
            str_flavour = "filling."; break;
          
          default:
          // For mixed str_flavours:
            taste = true;
            // If we like all the flavours:
            if (appetite & flavour == flavour)
              str_flavour = "great!";
            else
              str_flavour = "okay.";
            break;
          }
          
      // Otherwise, we don't like the flavour:
      else
      // Give the reason why we dislike the flavour:
        switch ( flavour ) {

          case ap_carnmeat:
            taste = false;
            str_yuk = "Yuck!  ";
            str_flavour = "tough and inedible"; break;
            
          case ap_herbmeat:
            taste = true;
            str_yuk = "Ew!  ";
            str_flavour = "meaty"; break;
            
          case ap_carrion:
            taste = true;
            str_yuk = "Blech!  ";
            str_flavour = "rancid!"; break;
          
          case ap_sweet:
            taste = true;
            str_flavour = "very rich"; break;
  
          case ap_fruit:
            taste = true;
            str_flavour = "fruity"; break;
            
          case ap_veg:
            taste = true;
            str_flavour = "like rabbit food"; break;
            
          case ap_grain:
            taste = true;
            str_yuk = "Gross!  ";
            str_flavour = "wholesome!"; break;
            
          case ap_grass:
             taste = true;
            str_flavour = "bland"; break;

          case ap_fabric:
            taste = true;
            str_flavour = "dry"; break;
            
          case ap_wood:
            taste = true;
            str_yuk = "Foul!  ";
            str_flavour = "like sawdust"; break;
            
          case ap_alien:
            taste = true;
            str_flavour = "disgusting"; break;

          // flavour won't be cannibal since we aren't yet a creature,
          // so no check for same species:
          // case ap_cannibal:

          default:
          // For mixed flavours:
            taste = true;
            str_flavour = "bad.";
            break;
          }




      str_tastes= String()
                + (taste?" taste":" ")
                + (taste
                  ? (getVal(attrib_plural)? " " : "s " )
                  : (getVal(attrib_plural)? "are " : "is " )
                    );

			if ( t->istype(tg_player) )
				Message::add(str_yuk + String(Grammar::plural(THIS,Grammar::det_demonst_with) +str_tastes+str_flavour).sentence());
      // Questionable: do we give a message: "The orc eats a sandwich.  It looks delicious!"?
      // I think not: does the orc look delicious?  If we give a message,
      //              it should be of the type, "The orc looks happy!" or
      //              "The orc looks sick." since that's what the PC sees.
			//else {
			//	Message::add(str_yuk + String(Grammar::Subject(THIS,Grammar::det_definite) +" look"+(getVal(attrib_plural)?" ":"s ")+str_flavour).sentence());
			//	}

			return true;

			}

    default:
			return RawTarget::perform( a, t );


		}

	}

long Item::getVal( StatType s ) {

	switch (s) {
    case attrib_material:
      return m_stone;

    case attrib_itemized:
      //return true;
      return false;
  
    case attrib_floats:
      return Material::data[ (materialClass)getVal(attrib_material) ].floats;

    case attrib_sunlit:
      //return Map::getTile(location)->getVal(attrib_sunlit);
      if (!parent) return false;
      return parent->getVal(attrib_sunlit);

    case attrib_price:
    case attrib_cash:
    case attrib_sink_weight:
		case attrib_weight: {

      long total = 0;
      if (inventory && inventory->reset())
      do {

        // Only add the price of contents when owned by the same person:
        if ( s != attrib_price
          || inventory->get()->getTarget(tg_owner) == getTarget(tg_owner) )
          
          total += inventory->get()->getVal(s);
        
        } while (inventory->next());
    
			return total;
      
      }

    case attrib_empty_nodoor:
      return false;

		case attrib_person :
			if ( getVal(attrib_pair) ) return Grammar::p_third_pl;
			return Grammar::p_third_n;

		case attrib_discrete:
			return true;

    case attrib_sort_category:
      return (gettype() << TYPE_SORT_SHIFT);

    case attrib_flying:
      //  Items are considered to be incapable of flight
      //  except when kicked or thrown:
      return !still;
      
    case attrib_still:
      //  Is the item at rest:
      // True for creatures at end of turn.
      return still;

		default:
			return RawTarget::getVal(s);
		}

	}

long Item::getSkill( Skill, bool ) {
  // By default, items have no effect on skills:
  return 0;
  }

void Item::takeDamage( const Attack &a ) {

	if ( isCapable(aLeave) ) {

    if (!!owner) a.deliverer->perform(aAddBill,THIS);

    // Convert point3ds to points because we don't want items to
    // fly to other levels!
		Point3d me=getlocation(), attacker=a.instrument->getlocation();
		int str = a.instrument->getVal( stat_str );

    // Todo: make flightpath shorter for heavier objects:
		Point delta( ( me.getx()-attacker.getx() ) * str,
								 ( me.gety()-attacker.gety() ) * str );

		if ( delta != Point(0,0) ) {
      FlightPath::Option flight_opt;
      flight_opt.object = Target(THIS);
      flight_opt.start = me;
      flight_opt.delta = delta;
			FlightPath::create( flight_opt );
			}
		}
	else
		Message::add("Thump.");

	}

Attack Item::getAttack( void ) {
  Target deliverer = controller;
  if (!deliverer) {
  
    //Target parent = getTarget(tg_parent);
    //assert(!!parent);
    if ( parent->istype(tg_creature) )
      deliverer = parent.toTarget();
    else
      deliverer = THIS;
    }
	return Attack( CombatValues(pen_bash,1), deliverer, THIS );
	}


bool Item::equals( const Target &rhs ) {

	bool retVal = RawTarget::equals( rhs );

	if (retVal) {
		// Good so far - same inventories and both of same type.
		if ( letter != ((Item*)rhs.raw() )->letter  ) retVal = false;
		if ( !still || !((Item*)rhs.raw() )->still  ) retVal = false;
		//if ( quality != ((Item*)rhs.raw() )->quality  ) retVal = false;
    if ( owner != ((Item*)rhs.raw() )->owner  ) retVal = false;
		}

	return retVal;

	}

Target Item::detach( int ) {

  if (parent != NULL )
    doPerform( aLeaveLev );

  return RawTarget::detach();
	}

void Item::setParent( TargetValue *t ) {
  RawTarget::setParent(t);
  if ( parent != NULL )
    doPerform( aEnterLev );
	}

/*
void Item::setlocation( const Point3d &p3d ) {

  if ( location != OFF_MAP )
    doPerform( aLeaveLev );

  RawTarget::setlocation(p3d);
  // Because RawTarget::setlocation no longer calls detach():

  if ( location != OFF_MAP )
    doPerform( aEnterLev );

	}
*/

String Item::name( NameOption n_opt ) {
  switch (n_opt) {
    case n_menu:
      if (!!owner && !owner->istype(tg_player) && owner->getVal(attrib_alive)
        && owner->getVal(attrib_trader) )
        return " (unpaid, "+PriceStr(getVal(attrib_price))+")";

    default:
      return "";
    }
  }


/*
void Item::allocInventory( void ) {
  if (!inventory)
    inventory = new Sorted<Target>;
  }
*/
#endif // ITEM_CPP
