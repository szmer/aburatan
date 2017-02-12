// [creature.cpp]
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
#if !defined ( CREATURE_CPP )
#define CREATURE_CPP

#include "creature.hpp"
#include "visuals.hpp"
#include "enum.hpp"
#include "level.hpp"
#include "crlist.hpp"
#include "standard.hpp"
#include "message.hpp"
#include "event.hpp"
#include "grammar.hpp"
#include "random.hpp"
#include "file.hpp"
#include "fluid.hpp"
#include "global.hpp"
#include "input.hpp"
#include "skill.hpp"
#include "quantity.hpp"
#include "money.hpp"
#include "weapon.hpp"
#include "player.hpp"

#include <assert.h>

// Swimming multiplier (multiplied by athletics skill
//    to determine maximum weight that can be carried
//    when swimming:
#define SWIM_MULT 40

//--[ struct Notoriety ]--//
Impression Notoriety::impressionOf( const Notoriety &other ) const {

  // Todo: return an impression based on fame and demeanor.

  // ATM returns despised when one is lawless and other isn't
  if ((demeanor < 0) != (other.demeanor < 0))
    return imp_assault;
    
  return imp_tolerate;
  }
  

#define FAME_BANDS 6
const char* note_str[FAME_BANDS][3] = {
 { "Unknown",    "Unknown",    "Unknown" },
 { "Servile",    "Lowly",      "Modest" },
 { "Shady",      "Obscure",    "Humble" },
 { "Scandalous", "Noted",      "Eminent" },
 { "Dishonored", "Popular",    "Honored" },
 { "Infamous",   "Notorious",  "Famous" },
 };

const char fame_min[FAME_BANDS] = { 0, 5, 10, 35, 60, 85 };

String Notoriety::toString( void ) const {
// Grabs one of the strings from the above table, note_str.

  String ret_str;
  char el_fame = 0, el_demeanor = 1;

  if (demeanor < -DEMEANOR_MID_BAND)
    el_demeanor = 0;
  else if (demeanor > DEMEANOR_MID_BAND)
    el_demeanor = 2;

  for ( int i = 1; i < FAME_BANDS; ++i )
    if ( fame >= fame_min[i] )
      el_fame = i;
  
  return String( note_str[el_fame][el_demeanor] );
  }



//--[ class Creature ]--//
Creature::Creature( const Option *o )
: Item ( o ),
  gender( o->gender ),
  c_name(),
	brain(NULL),
	equipped(),
	primary(NULL),
	secondary(NULL),
  mount(NULL),
  thrown(NULL){

  form = crInvalid;

  current.reset();
  maximum.reset();
  current_xp = 0;
  current_lev = 1;
  
	}

Creature::Creature( Creature &copy )
: Item(copy),
  gender(copy.gender),
  c_name(),
	brain(NULL),
	equipped(),
	primary(NULL),
	secondary(NULL),
  mount(NULL),
  thrown(NULL){

	form = copy.form;
	maximum = copy.maximum;
	current = copy.current;
  current_xp = copy.current_xp;
  current_lev = copy.current_lev;

	c_name = copy.c_name; // Should clone names be the same?
  // Maybe, "A Copy of James"? :)
  

	if ( (copy.equipped.reset() )  )
	do {
		equipped.add( copy.equipped.get()->clone() );
		} while ( copy.inventory->next() );

	if ( !!copy.primary )
		primary = copy.primary->clone();

	if ( !!copy.secondary )
		secondary = copy.secondary->clone();

	if ( !!copy.mount )
		mount = copy.mount->clone();

	if ( !!copy.brain )
  	brain = copy.brain->clone();

	}

Creature::~Creature( void ) {

	if (equipped.reset())
	do {
		equipped.get().save();
		} while ( equipped.next() );

	if (!!primary) primary.save();
	if (!!secondary) secondary.save();
	if (!!mount) mount.save();
  
  if (!!brain) brain.save();

	}

Creature::Creature( std::iostream &inFile, TargetValue *TVP )
: Item( inFile, TVP ),
  gender(sex_any),
  c_name(),
	brain(NULL),
	equipped(),
	primary(NULL),
	secondary(NULL),
  mount(NULL) {

	using namespace std;
	long  saveLoc;

  inFile >> c_name;
	inFile >> brain;

	inFile >> primary;
	saveLoc = inFile.tellg();
  if (!!primary) primary->parent = THIS;
	inFile.seekg(saveLoc);

	inFile >> secondary;
	saveLoc = inFile.tellg();
  if (!!secondary) secondary->parent = THIS;
	inFile.seekg(saveLoc);

	inFile >> mount;
	saveLoc = inFile.tellg();
  if (!!mount) mount->parent = THIS;
	inFile.seekg(saveLoc);

	inFile.read( (char*) &form, sizeof(form) );
	inFile.read( (char*) &gender, sizeof(gender) );

	inFile.read( (char*) &maximum, sizeof(maximum) );
	inFile.read( (char*) &current, sizeof(current) );

	inFile.read( (char*) &current_xp, sizeof(current_xp) );
	inFile.read( (char*) &current_lev, sizeof(current_lev) );
  
	bool hasequip;

	inFile.read( (char*) &hasequip, sizeof(hasequip) );
	saveLoc = inFile.tellg();

	if (hasequip) {

		std::fstream myFile;

		String filename = SAVEDIR;
		filename = filename + THIS->getSerial().toFileName() + ".CRE"; // '.CRE' == Creature Equipped

		myFile.open( filename.toChar(), ios::in | ios::binary );

		assert (!! myFile);
    // Error::fatal(  "Creature load error: Error opening file " + filename);

		// Load Number of equipped items:
		int numitems;
		myFile.read( (char*) &numitems, sizeof(numitems) );

		for ( int count = 0; count < numitems; count++ ) {
			Target t;
			myFile >> t;
			equipped.add(t);
			t->parent = THIS;
			}

		myFile.close();
		}

  // If the creature is damaged and alive, start to heal:
  if ( ascending( 0, current.hp, maximum.hp ) )
	  ActionEvent::create( Target(THIS), aHeal, 500 / getSkill(sk_heal) );

  // If damaged and not alive, strt to rot:
  if (current.hp <= 0)
    ActionEvent::create( Target(THIS), aRot, TIME_ROT );
  
	inFile.seekg(saveLoc);
	}

void Creature::toFile( std::iostream &outFile ) {

  using namespace std;

	Item::toFile( outFile );

  outFile << c_name;
	outFile << brain;
	outFile << primary;
	outFile << secondary;
  outFile << mount;

	outFile.write( (char*) &form, sizeof(form) );
	outFile.write( (char*) &gender, sizeof(gender) );
	outFile.write( (char*) &maximum, sizeof(maximum) );
	outFile.write( (char*) &current, sizeof(current) );

	outFile.write( (char*) &current_xp, sizeof(current_xp) );
	outFile.write( (char*) &current_lev, sizeof(current_lev) );

	bool hasequip = (equipped.reset() )?true:false;

	outFile.write( (char*) &hasequip, sizeof(hasequip) );

# ifndef NDEBUG
	long  saveLoc = outFile.tellp();
# endif // NDEBUG

	// If this object has equipped items, save them to a separate file:
	if (hasequip) {

		std::fstream *myFile;

		String filename = SAVEDIR;
		filename = filename + THIS->getSerial().toFileName() + ".CRE"; // '.CRE' == Creature Equipped

    myFile = new std::fstream;
    myFile->open( filename.toChar(), ios::out | ios::binary );

		assert (!! (*myFile));
    //Error::fatal(  "Creature save error: Error opening file " + filename);

		// Save Number of equipped items:
		int numitems = equipped.numData();
		myFile->write( (char*) &numitems, sizeof(numitems) );


		do {
			*myFile << equipped.get();
			} while ( equipped.next() );

    delptr(myFile);
    
		}

	assert (saveLoc == outFile.tellp());
  // Error::fatal("Eureka!");
  
	}


/*
void Creature::setlocation( const Point3d &p3d ) {

	if ( equipped.reset() ) //
	do {
		equipped.get()->setlocation(p3d);
		} while ( equipped.next() );

  if (!! primary) primary->setlocation(p3d);
  if (!! secondary) secondary->setlocation(p3d);

  Item::setlocation(p3d);

	}
*/

/*
Target Creature::findTarget( targetType type, const Target& t ) {

  switch( type ) {
    case tg_parent:
    case tg_possessor: {
      //assert( location == t->getlocation() );
      if (location != t->getlocation()) {
        String msg;
        msg << Grammar::plural(THIS);
        msg << "(" << location.getx() << "," << location.gety() << ") != ";
        msg << Grammar::plural(t);
        msg << "(" << t->getlocation().getx() << "," << t->getlocation().gety() << ")";

        Message::add(msg);
        //Message::add(String("Invalid group! p(")+(long)location.getx()+","+(long)location.gety()+")");
        return Target();
        }

      Target found;

      // Check equipped items:
    	if ( equipped.reset() ) {
        long key = equipped.lock();
        
      	do {
      		if (equipped.get() == t) {
            equipped.load(key);
            return THIS;
            }

          found = equipped.get()->findTarget(type,t);
          if (!!found) {
            equipped.load(key);
            return found;
            }

      		} while ( equipped.next() );
          
        equipped.load(key);
        }

      // Check wielded weapons:
      if ( primary == t ) return THIS;
      if ( secondary == t) return THIS;

      if (!!primary) {
        found = primary->findTarget(type,t);
        if (!!found) return found;
        }

      if (!!secondary) {
        found = secondary->findTarget(type,t);
        if (!!found) return found;
        }

      // Look through inventory
      if ( (inventory) && (inventory->reset() )  ) {
        long key = inventory->lock();

        do {
          // if  an item in the inventory is the item we look for,
          if (inventory->get() == t) {
              
            inventory->load(key);
            // we're the parent / possessor
            return THIS;
            }

          // otherwise, ask the item if it knows who the parent/possessor is:
          found = inventory->get()->findTarget(type,t);
          // If the item knows,
          if (!!found) {
            inventory->load(key);
            // we're the possesor:
            if (type == tg_possessor)
              return THIS;
            // and it's the parent:
            return found;

          } while ( inventory->next() );

        inventory->load(key);
        } //

      // We don't know.  Return NULL:
      return Target(NULL);
      } //
      
    default:
      return Item::findTarget(type,t);
    } //
  } //
*/

void Creature::setTHIS( TargetValue *TVP ) {
	THIS = TVP;
  
	if ( equipped.reset() )
	do {
		equipped.get()->setParent(THIS);
		} while ( equipped.next() );
    
  if (!! primary) primary->setParent(THIS);
  if (!! secondary) secondary->setParent(THIS);
  if (!! mount) mount->setParent(THIS);
 
  Item::setTHIS(TVP);
  }

char Creature::polymorph( CreatureType f ) {

  // Must be careful here: if creature is dead,
  // we must not polymorph into a creature that
  // doesn't leave a corpse - and we must stay dead.
  bool is_dead = (current.hp == 0 && form != crInvalid);
  
  if (is_dead && creature::List[f].corpse_pc == 0) {
    return 0;
    }
    
  // Remove current form effects
  if (form != crInvalid) {
  	maximum -= creature::List[form].stats;
  	current -= creature::List[form].stats;
    }

  // Change form
	form = f;
	maximum += creature::List[f].stats;
	current += creature::List[f].stats;
	image = creature::List[f].image;

  // No change of level on polymorph.

  if (is_dead)
    current.hp = 0;

  if ( current.hp == 0 )
    image.val.appearance = '%';

	// To do: Parse equipped & remove illegal items

	return 1;
	}

const String Creature::getname( void ) const {
	return "Creature";
	}
/*CreatureType Creature::getform( void ) const {
	return form;
	}*/
targetType Creature::gettype( void ) const {
	return tg_creature;
	}
bool Creature::istype( targetType t ) const {
	if (t == tg_creature)
		return true;
	else
		return RawTarget::istype(t);
	}

String Creature::name( NameOption n_opt ) {
  switch (n_opt) {
    default: return getname() + Item::name(n_opt);
    }
  }

Image Creature::menuletter() {
	return image;
	}
bool Creature::equals( const Target & ) {
// Each creature is different until Mike fixes the
// "Error opening file G$A:q3@R09wqA#JKA" bug that
// occurs when dead monsters stack.

	return false;

/*	bool retVal = Item::equals( rhs );

	if (retVal) {
		// Good so far - same inventories and both of same type.
		// Creature form must be same:
		if ( form != ( (Creature*) rhs.raw() )->form ) retVal = false;
		// Both creatures must be dead:
		if ( !(current.hp <= 0) || !( (Creature*) rhs.raw() )->current.hp <= 0 ) retVal = false;
		}

	return retVal;
  */
	}

compval Creature::compare( RawTarget &rt ) {

	if ( rt.istype( gettype() ) ) {

		if ( current.hp < rt.getVal( stat_hp ) ) return k_larger;
		if ( current.hp > rt.getVal( stat_hp ) ) return k_smaller;
		return k_same;

 		}

	return Item::compare(rt);

	}

Action Creature::autoAction( void ) {

	return aBlock;
	}

bool Creature::capable( Action a, const Target &t ) {

	switch( a ) {

    case aThrow:
      // ATM all creatures can throw all items:
      return true;

    case aSearch:
      // Todo: only intelligent creatures can search?
      if (!t)
        return true;
      return t->isCapable(aFind);

    case aSee:
    case aHear:
    case aSpeak:
    case aTaste:
    case aTouch:
      return true;

    case aOpen:
      //Todo: ask the crList if this creature can open/close
      // arbitrary items
      if (!t)
        return true;
      return t->isCapable(aBeOpened);
    case aClose:
      //Todo: ask the crList if this creature can open/close
      // arbitrary items
      if (!t)
        return true;
      return t->isCapable(aBeClosed);

    case aDisarm:
      //Todo: ask the crList if this creature can manipulate
      // complex items like traps
      if (!t)
        return true;
      return t->isCapable(aBeDisarmed);

    case aKick:
      // Todo: ask the crList if this creature can kick
      if (!t)
        return true;
      return t->isCapable(aBeKicked);
        
    case aAttack:
      // Every creature can attack:
      if (!t)
        return true;
      return t->isCapable(aBeAttacked);

    case aAscend:
    case aDescend:
      // Every creature can currently climb/descend.
      // Todo: Some creature types should be restricted from climbing trees, etc.
      if (!t)
        return true;
      if (a == aAscend)
        return t->isCapable(aBeAscended);
      return t->isCapable(aBeDescended);
      
    case aAssault: {
      // If we're dead, we won't assault anything:
      if (!brain)
        return false;

      // If no target is given, assume they mean the player:
      Target target = t;
      
      if (!target)
        target = Player::handle();

      // Is the creature capable of an unprovoked assault on Target t?
      // By default the only reason to attack is hunger:
			// If the creature who is not ourself:
			if ( Target(THIS) != target
      // and is not our creature type:
      && ( getVal(attrib_form) != target->getVal(attrib_form)
      // (unless we are a cannibal)
       || (getVal(attrib_appetite)&ap_cannibal) )
      // and it fills our appetite:
      &&  (getVal(attrib_appetite) & target->getVal(attrib_flavour))  )

        return true;

      return brain->capable(aAssault,target);
      }

    case aTellTime: {
      // Can tell the time if wearing or wielding an item
      // that can tell the time:
      if (!!primary && primary->isCapable( aTellTime )) return true;
      if (!!secondary && secondary->isCapable( aTellTime )) return true;

      long key = equipped.lock();
      if (equipped.reset())
      do {

        if (equipped.get()->isCapable( aTellTime )) {
          equipped.load(key);
          return true;
          }

        } while( equipped.next() );
      equipped.load( key );
      return false;
      }
    
    case aChat:
    
      // Cannot chat if mute:
//      if (creature::List[form].voice == vo_none)
//        return false;
      
      if (!t)
        return true;
      return t->capable( aRespond, THIS );

    case aRespond:
      // Cannot chat if mute:
//      if (creature::List[form].voice == vo_none)
//        return false;
      // Todo: Cannot chat if asleep, etc.
      return true;
      
		case aBlock:
			// As of yet living creatures exist one-per location:
			return (current.hp > 0)?true:false;

		case aContain:
		// Todo:
		// Check weight and size restrictions
			return true;

		case aSwitch:
      if (!!primary && primary->getVal(attrib_two_handed))
        return false;
			return ( !primary && !secondary )?false:true;

		case aWear: {
      // All creatures can wear something:
      if ( !t ) return true;
      
			if ( t->isCapable(aBeWorn) )
				return true;

      // If item is not wearable in the traditional (clothing) sense,
      // it might be able to be loaded into a worn item:
      long key = equipped.lock();

      if ( equipped.reset() )
      do {
        if ( equipped.get()->capable(aLoad,t) ) {
          equipped.load(key);
          return true;
          }
        } while ( equipped.next() );

      equipped.load(key);
      
			return false;

      }
		case aEat:
      if ( !t )
        return getVal(attrib_appetite);

      // Can't eat yourself...
      if ( Target(THIS) == t ) return false;

      // Can't eat anything that refuses to be eaten:
      if ( !t->isCapable(aBeEaten) ) return false;
      
      // Can eat anything we like to eat:
      if ( getVal(attrib_appetite) & t->getVal(attrib_flavour) )
				return true;
        
      // Can also eat anything edible but not made of wood/fabric:
      if ( t->getVal(attrib_flavour) & ~(ap_wood | ap_fabric) )
        return true;

			return false;

		case aRead:
			if ( t->isCapable(aBeRead) ) {
				return true;
				}
			return false;

		case aQuaff:
			if ( t->capable(aBeQuaffed,Target(THIS)) ) {
				return true;
				}
			return false;

		case aApply:
			if ( t->capable(aBeApplied,Target(THIS)) ) {
				return true;
				}
			return false;

		case aWield:
			if ( t->capable(aBeWielded,Target(THIS)) ) {
				return true;
				}
			return false;

		case aBeEaten:
			return (current.hp <= 0)?true:false;

		case aSpMessage:
			return (current.hp > 0)?true:false;

    case aQuickReload: {

      // Look through equipped items for a quiver w/ammo:
      long e_key, q_key;

      e_key = equipped.lock();

       if (equipped.reset())
      do {

        if ( equipped.get()->getVal(attrib_filled_quiver) ) {

          List<Target> *quivContents = equipped.get()->itemList();
          q_key = quivContents->lock();

          if (quivContents->reset())
          do {

            if (!!primary && primary->capable(aLoad, quivContents->get() )) {
              quivContents->load(q_key);
              equipped.load(e_key);
              return true;
              }

            if (!!secondary && secondary->capable(aLoad, quivContents->get() )) {
              quivContents->load(q_key);
              equipped.load(e_key);
              return true;
              }

            } while (quivContents->next());
          
          quivContents->load(q_key);
        
          }
      
        } while (equipped.next());

      equipped.load(e_key);
    
      return false;

      }

    default:
			return Item::capable(a, t);
		}

	}

#define DROWN_TIME (GRANULARITY/5)
bool Creature::perform( Action a, const Target &t) {

	switch( a ) {

    case aAscend:
    case aDescend:
      // if we're *scending a mount:
      if (t->istype( tg_mount )) {

        if (a == aAscend) {

          // Can't mount more than one thing at a time:
          if (!!mount ) {
            Message::add("You are already mounted.");
            return false;
            }
          
          // Mount up:
          mount = t->detach(1);
          // if we're visible,
          if (getVal(attrib_visible)) {
            // give a message:
            String subject = Grammar::Subject(THIS,Grammar::det_definite);
            Message::add( subject << " mount"
                                  << ( (getVal(attrib_plural))?" ":"s " )
                                  << Grammar::Object(t, Grammar::det_definite)
                                  << ".");
            }
          }
        else {

          // Must be mounted to dismount:
          if (!mount ) {
            Message::add("You are not mounted.");
            return false;
            }

          // Dismount:
          Target old_mount = mount;
          if (perform(aRelease, mount))
            mount = Target(NULL);

          // if we're visible,
          if (getVal(attrib_visible)) {
            // give a message:
            String subject = Grammar::Subject(THIS,Grammar::det_definite);
            Message::add( subject << " dismount"
                                  << ( (getVal(attrib_plural))?" ":"s " )
                                  << Grammar::Object(t, Grammar::det_definite)
                                  << ".");
            }

          }
          
        }
        
      return Item::perform(a,t);

    case aSwim: {
      // --- Called periodically when creature is submerged:
      
      // Don't bother swimming if we're dead:
      if ( !getVal(attrib_alive) )
        return false;

      // If the object isn't swimming, it takes damage:
      int strength = 20 * ( !getVal(attrib_canswim) );

      Target tile = getTarget(tg_parent);
      
      // Are we still submerged?
      // If so, create another periodic swim event:
      if ( tile->getVal(attrib_pool) ) {
        if (!ActionEvent::exists(THIS, aSwim) )
          ActionEvent::create( THIS, aSwim, DROWN_TIME );
        }
      else
        // No longer submerged:
        return true;

      
  		Attack wet( CombatValues(pen_vapor,strength,dam_drown,10), tile, tile, "drowned" );
    
      if (strength) {
        // We're sinking:
        takeDamage( wet );
        // This message should be given in takeDamage
        // - non-breathers shouldn't get it
        if (istype(tg_player)) {
          Message::add(Grammar::Subject(THIS,Grammar::det_definite)+(getVal(attrib_plural)?" are":" is")+" drowning!");
          //Message::add( String("SinkWeight: ")
          //              << getVal(attrib_sink_weight) << " "
          //              << String("SwimStrength: ")
          //              << getSkill(sk_athl) * SWIM_MULT);
          }
        }
      else if (istype(tg_player))
        Message::add(Grammar::Subject(THIS,Grammar::det_definite)+" swim"+(getVal(attrib_plural)?"":"s")+".");

      // We successfully swam:
      return true;
      }

    case aParry: {
      // can only parry if adjacent:
      Point3d delta = getlocation() - t->getlocation();
      if (max( abs(delta.getx()), abs(delta.gety()) ) > 1)
        return false;

      // can only parry if intelligent:
      if ( getVal(stat_kno) < stat_low )
        return false;

      // Adjacent.  Attempt a parry:
      // DC is 10+ skill of weapon + skill of attacker
      switch ( test( sk_figh, 10+t->getSkill(sk_figh,true)
                                +t->getTarget(tg_controller)->getSkill(sk_figh,true), t ) ) {
        case result_pass:
        case result_perfect: {
          // Todo:  perfect means a bonus retaliatory attack
          String subject = Grammar::Subject(THIS, Grammar::det_definite);
          Message::add( subject << " block"
                                << ( (getVal(attrib_plural))?" ":"s " )
                                << Grammar::Object(t, Grammar::det_definite)
                                << ".");
          return true;
          }
        default:
          return false;
        }
      }

    case aRot:
      if ( RawTarget::getVal(attrib_visible) )
        Message::add( Grammar::Subject( Target(THIS), Grammar::det_definite) + " rots away." );

      ActionEvent::remove( Target(THIS), aAll );
      detach();

      return true;

    case aSetController:

      // Set controller to null if t is null
      if (!t)
        controller = Target(NULL);
        
      // or if we are contained within t
      else if ( getTarget(tg_parent) == t || !capable(a,t) )
        controller = Target(NULL);
        
      // Otherwise set controller to t
      else
        controller = t;
        
      return Item::perform(a,t);
      

    case aPayFor:{

      // Item must exist:
      if (!t) return false;
        
      // Item must be owned:
      Target t_owner = t->getTarget(tg_owner);
      if (!t_owner) return false;

      // Creature must have sufficient funds:
      long price = t->getVal(attrib_price);
      if (getVal(attrib_cash) < price)
        return false;

      // Look through inventory for cash:
      List<Target> cash_list;
      long cash_list_total = 0;

      if (inventory && inventory->reset() )
      do {

        if ( inventory->get()->istype(tg_money) ) {
          // Found cash - add to cash list:
          cash_list.add(inventory->get());
          cash_list_total += inventory->get()->getVal(attrib_cash);
          }
      
        } while ( inventory->next() );

      // Can only pay if we have enough money in general inventory:
      if (cash_list_total < price)
        return false;
        
      // We now have a list of all of the cash carried.
      long money_taken = 0;

      // Take money, smallest denominations first:
      while (money_taken < price) {
        if (cash_list.reset()) {
          Target smallest_bill;
          if (!smallest_bill)
            smallest_bill = cash_list.get();
            
          // Find smallest bill:
          do {
            if (smallest_bill->getVal(attrib_cash)
              > cash_list.get()->getVal(attrib_cash) )
              smallest_bill = cash_list.get();
               
            } while (cash_list.next());
          //
          int det_quant = -1;
          
          if ( smallest_bill->getVal(attrib_cash) > (price-money_taken) )
            det_quant = (price-money_taken) / smallest_bill->getVal(attrib_denomination)
                      +((price-money_taken) % smallest_bill->getVal(attrib_denomination) > 0);

          smallest_bill = smallest_bill->detach(det_quant);
            
          money_taken += smallest_bill->getVal(attrib_cash);
            
          cash_list.remove();
          }
        else {
          Message::add("Error taking money! "
                       "Wanted "+PriceStr(price)+" but took "+PriceStr(money_taken));
          break;
          }
        }
      
      // If necessary, give change:
      if (money_taken > price) {
        long change = money_taken - price;
        
        Quantity::Option q_opt;
        Money::Option m_opt;
        int dollars = WholeDollars(change),
            cents = WholeCents(change);
        
        // Dollars:
        m_opt.material = m_paper;

        if (dollars == 1) {
          perform(aContain, Money::create(m_opt));
          }
        else if (dollars) {

          q_opt.target = Money::create(m_opt);
          q_opt.quantity = dollars;
          
          perform(aContain, Quantity::create(q_opt));
          }
          
        // Cents:
        m_opt.material = m_silver;

        if (cents == 1) {
          perform(aContain, Money::create(m_opt));
          }
        else if (cents) {

          q_opt.target = Money::create(m_opt);
          q_opt.quantity = cents;
          
          perform(aContain, Quantity::create(q_opt));
          }
        }

      return true;
      }

    case aGainExp:{
      assert(!!t);
      //Error::fatal("Cannot gain Exp for something that doesn't exist!");

      // Actual xp gained is scaled up for creatures of a greater level
      long real_xp = t->getVal( attrib_xp ),
           lev_diff = t->getVal(attrib_level)-getVal(attrib_level)+1,
           xp = real_xp;

      while (lev_diff-- > 0)
        xp += real_xp / (lev_diff+1);

        //  Lev diff:  0   1   2   3   4   5   6   7   8    9
        //  fraction:  - 1/2 1/3 1/4 1/5 1/6 1/7 1/8 1/9 1/10
        //  xp %     100 150 183 208 228 245 259 272 283  293 etc.

      // NOTE: When making changes to this algorithm,
      //       please synchronise Monster::name() function in 'monster.cpp'
        
      current_xp += xp;
      return true;
      }
      
    case aThrowAt: {

      if (!thrown || !t)
        return false;

      Target single_thrown;
      if ( thrown->quantity() == 1 ) {
        single_thrown = thrown;
        thrown = Target(NULL);
        single_thrown->detach();
        }
      else
        single_thrown = thrown->detach(1);
        
  		Point3d me = getlocation();

      // Todo: make flightpath shorter for heavier objects:
  		Point3d delta = t->getlocation() - me;

 			single_thrown->doPerform(aFly);
      single_thrown->perform(aSetController,Target(THIS));

 			Map::getTile(me)->perform(aContain,single_thrown);

        
      FlightPath::Option flight_opt;
      flight_opt.object = single_thrown;
      flight_opt.start = me;
      flight_opt.delta = delta.toPoint();
			FlightPath::create( flight_opt );

      return true;
      }

    case aThrow:
      if (!!t)
        thrown = t;
      return true;
    
    case aPolymorph: {

      CreatureType f;
      CreatureType old = form;
   		f = (CreatureType) Random::randint( crNum );

      String subject = Grammar::Subject(Target(THIS),Grammar::det_definite);

      bool success = polymorph(f);

      if (RawTarget::getVal(attrib_visible) && getVal(attrib_on_level))
        if (f == old || !success)
    			Message::add(subject+(istype(tg_player)?" feel":" look")+(getVal(attrib_plural)?"":"s")+" like a new "+creature::List[form].name+"!");
        else
    			Message::add(subject+" turn"+(getVal(attrib_plural)?"":"s")+" into a "+creature::List[form].name+"!");
    
      // Allow for death by system shock:
    	Attack shock = Attack( CombatValues(), Target(THIS), Target(THIS), "mutated" );
    	takeDamage( shock );
  
      return Item::perform(a,t);
      }

    case aEnterLev:
    case aLeaveLev: {
      Target tTHIS = THIS;

      if (a == aEnterLev)
        //getlevel().addCreature( tTHIS );
        Map::addCreature( tTHIS );
      else
        //getlevel().removeCreature( tTHIS );
        Map::removeCreature( tTHIS );

      if ( equipped.reset() )
      do {
        equipped.get()->perform(a,t);
        } while (equipped.next());

      if (!!primary) primary->perform(a,t);
      if (!!secondary) secondary->perform(a,t);
      if (!!mount) mount->perform(a,t);
      
      return Item::perform(a,t);
      }

		case aWarn:

			if ( current.hp > 0 && Target(THIS) != t) {
				if (!brain)
					Message::add(Grammar::Subject(THIS,Grammar::det_definite)+" has no brain!");
				else
					return brain->perform(aWarn, t);
				}


      // Why is this here?!
			//if ( ascending( 0, current.hp, maximum.hp ) )
			//	ActionEvent::create( Target(THIS), aHeal, 500 / getSkill(sk_heal) );

			if (!!brain) {
				return brain->doPerform(aWake);
				}
			else {
				return false;
				}

    case aWake:
      if (!!brain)
        if (brain->doPerform(aWake)) {
          Map::addCreature(THIS);
          return true;
          }

      return false;

		case aSleep:

      Map::removeCreature( THIS );

			ActionEvent::remove( Target(THIS), aHeal );

      if (!! brain && current.hp <= 0)
        Message::add("Why won't you die?!");

			if (!!brain)
				return brain->doPerform(aSleep);
			else {
        if (current.hp <= 0 && !global::save) {
          Target tTHIS = THIS;
          //detach();
          ActionEvent::remove( tTHIS, aAll );
          return true;
          }
				return false;
        }

    case aQuickReload: {
      // Look through equipped items for a quiver w/ammo:
      long e_key, q_key;

      e_key = equipped.lock();

      if (equipped.reset())
      do {

        if ( equipped.get()->getVal(attrib_filled_quiver) ) {

          List<Target> *quivContents = equipped.get()->itemList();
          q_key = quivContents->lock();

          if (quivContents->reset())
          do {

            bool reloaded_p = false, reloaded_s = false;
            if (!!primary && primary->capable(aLoad, quivContents->get() )) {
              primary->perform(aLoad, quivContents->get() );
              reloaded_p = true;
              quivContents = equipped.get()->itemList();
              }

            if (!!secondary && quivContents && quivContents->numData() && secondary->capable(aLoad, quivContents->get() )) {
              secondary->perform(aLoad, quivContents->get() );
              reloaded_s = true;
              }


            if ( (reloaded_p || !primary || !primary->isCapable(aLoad))
               &&(reloaded_s || !secondary || !secondary->isCapable(aLoad)) ) {
              quivContents = equipped.get()->itemList();
              if (quivContents) quivContents->load(q_key);
              equipped.load(e_key);
              
              return true;
              }
              
            quivContents = equipped.get()->itemList();
          
            } while (quivContents && quivContents->next());
          
          quivContents->load(q_key);
        
          }
      
        } while (equipped.next());

      equipped.load(e_key);
    
      return false;
      }
    case aPickUp:
      if (RawTarget::getVal(attrib_visible)) {
        String subject = Grammar::Subject(Target(THIS),Grammar::det_definite),
               object = Grammar::Object(t);
  			Message::add(subject +" pick"+(getVal(attrib_plural)?" ":"s ")+(Grammar::Pronoun()?"":"up ")+object+(Grammar::Pronoun()?" up":"")+".");
        }
		case aContain: {
			// a.k.a Pick Up
      if (!t) {
        Message::add("Creatures can only contain non-NULL items.");
        return false;
        }

			if (Item::perform(aContain,t)) {
        t->perform(aSetController,THIS);
        return true;
        }
      return false;
      }

		case aRelease: {
			// a.k.a Drop
      assert (!!t);
      //Error::fatal("Creatures can only drop non-NULL items.");
      
      Target tRetain = t;
			//t->detach();
      Target p = parent.toTarget();

			if (!!parent)
				parent->perform( aContain, t );
        
      t->perform(aSetController,THIS);
			return true;
      }

		case aSwitch: {

      if (!!primary && primary->getVal(attrib_two_handed))
        return false;

			swap (primary, secondary);

			if ( parent != NULL && RawTarget::getVal(attrib_visible) && !t ) {
 			  String message;
			  message = Grammar::plural(THIS) + " now wield" + ( getVal(attrib_plural)?" ":"s " );

			  if (!! primary ) {
				  message += Grammar::plural(primary);
          if ( !! secondary ) message +=  " and ";
				  }

        if ( !! secondary ) {
          message += Grammar::plural(secondary);
          if ( ! primary ) message +=  " as a secondary weapon";
          }

        message += ".";
                          
			  Message::add(message);
        }


			return true;

       }

		case aEat:
      assert (!!t);
      //Error::fatal("Creatures can only eat non-NULL items.");
      t->perform(aSetController,THIS);
      
			t->perform( aBeEaten, Target(THIS) );
      // Todo:  replace this clumsy detach with a more OO one,
      //        ie. objects should detach themselves when eaten.
			perform( aAddBill, t->detach(1) );
			return true;

		case aRead:
      assert (!!t);
      //Error::fatal("Creatures can only read non-NULL items.");
      
      t->perform(aSetController,THIS);
      
			t->perform( aBeRead, Target(THIS) );
			return true;

		case aApply: {

      assert (!!t);
      //Error::fatal("Creatures can only apply non-NULL items.");
      
      t->perform(aSetController,THIS);
      
      Target p = t->getTarget(tg_parent), targ = t;
      bool restack = false;

      if (t->quantity() > 1) {
        restack = true;
        targ = t->detach(1);
        }
      
			targ->perform( aBeApplied, Target(THIS) );
      
      if (restack) p->perform(aContain,targ);
			return true;

      }

		case aBeEaten: {

      // Food must be eaten by *something*
      if (!t)
        return false;
        
      // We're being eaten, so cancel all appointments:
      ActionEvent::remove( Target(THIS), aAll );

      // Whether the flavour is appreciated is defined by the eater:
      Appetite appetite = (Appetite) t->getVal(attrib_appetite);
               
      String special_case;

      // Special case for cannibalism:
      if (getVal(attrib_form) == t->getVal(attrib_form)) {

        if (appetite & ap_cannibal)
          special_case = "You relish the flavour of familiar flesh.";
        else
          special_case = "Cannibal!  "
                       + String( Grammar::plural(THIS,Grammar::det_demonst_with)
                       + " tastes awful." ).sentence();
        }

      // Other special cases:
      if (appetite & ap_herbmeat)
      switch (form) {
        case crCow:
          special_case = "Mmm, beef!"; break;
        case crHorse:
          special_case = "This meat tastes dry."; break;
        case crPig:
          special_case = "Mmm, pork!"; break;
        case crChicken:
          special_case = "Mmm, chicken!"; break;
        case crSheep:
          special_case = "Mmm, lamb!"; break;
        case crDeer:
          special_case = "Mmm, venison!"; break;
        case crDuck:
          special_case = "Mmm, duck!"; break;
        case crGoat:
          special_case = "Mmm, goat!"; break;
        default: break;
          
        }
      

      // Nobody likes eating triffids - even cattle
      if (form == crTriffid )
          special_case = "This cattle food tastes awful.";

      // If there's a special case message, print it then return:
      if ( special_case != "" && t->istype( tg_player ) ) {
        // MSG: You eat the item.
  			String object = Grammar::Object(THIS,Grammar::det_definite);
  			Message::add(Grammar::Subject(t,Grammar::det_definite) +" eat"+(t->getVal(attrib_plural)?" ":"s ")+object+".");
        // MSG: Special case string:
        Message::add(special_case);
        return true;
        }

      // Todo: poisonous meat
      
      // If the creature's not a special case, the response
      // is no different from other items:
      return Item::perform(a,t);
      }

		case aQuaff: {
      assert (!!t);
      //Error::fatal("Creatures can only quaff non-NULL items.");
      
      t->perform(aSetController,THIS);
      
			Target bottle = t,
      bottleAt = t->getTarget(tg_parent); // < in case the bottle is on the floor
			bool reattach = false;

			if (bottle->quantity() > 1) {
				bottle = bottle->detach(1);
				reattach = true;
				}

			bottle->perform(aBeQuaffed, Target(THIS) );

			if (bottle->getVal(attrib_quaff_contents_only)) {
				if (reattach) bottleAt->perform(aContain,bottle);
  			bottle->doPerform(aRestack);
        }


			return true;
      }
		case aWear: {

      assert (!!t);
      // Error::fatal("Creatures can only wear non-NULL items.");
      
			Target solid = t; // <- Make sure detach doesn't delete t!

      if ( t->isCapable(aBeWorn) ) {
  			Target targ = t->detach(1);

				Slot slot = (Slot) targ->getVal( attrib_slot );

 				if (inventory && inventory->find(targ)) {
 					inventory->remove();
 					}
 				else {
 					targ->detach();
 					//targ->setlocation( location );
          targ->setParent( THIS );
 					}

        // Find and remove worn items of the same slot:
        List<Target> removed;
				if ( equipped.reset() ) {
					do {

						if ( equipped.get()->getVal(attrib_slot) & slot ) {
              removed.add( equipped.get() );
//							perform(aTakeOff, equipped.get() );
							}

						} while( equipped.next() );

					}
          
 				equipped.add(targ);
        // Give the player feedback if any clothes are removed:
        String msg;
        if (THIS) msg = Grammar::Subject(THIS,Grammar::det_definite)
                     + " swap" + (getVal(attrib_plural)?" ":"s ");
        
        if (removed.reset()) {

          do {
            msg += Grammar::Subject(removed.get());
           	perform(aTakeOff, removed.remove() );
            if (removed.numData() > 1)
              msg += ", ";
            else if (removed.numData() == 1)
              msg += " and ";
            
            } while (removed.numData());

          msg += " for " + Grammar::Object(targ) + ".";
          if (THIS) Message::add(msg);
          }

        t->perform(aSetController,THIS);
 				return true;

        }
        
      else {
        // If item is not wearable in the traditional (clothing) sense,
        // it might be able to be loaded into a worn item:
        long key = equipped.lock();

        if ( equipped.reset() )
        do {
          if ( equipped.get()->capable(aLoad,t) ) {
            equipped.get()->perform(aLoad,t);
            equipped.load(key);
            t->perform(aSetController,THIS);
            return true;
            }
          } while ( equipped.next() );

        equipped.load(key);
      
        }
        
			return false;
      }
		case aWield: {

      if (!t) {
        // Creatures can only wield non-NULL items but wield with
        // no parameters should select the most appropriate weapon.
        return false;
        }
      
      // If we're already wielding the item, don't bother:
      if (primary == t || secondary == t)
        return false;
      
      t->perform(aSetController,THIS);
      
			String message = Grammar::Subject(THIS,Grammar::det_definite);
      bool packP = false, packS = false, secSame = (!!primary && !!secondary);

      String pack1 = " pack";
			String away1 = " away ";
      String pack2 = pack1, away2 = away1;
  		String blank = " ";
      
			if (!! primary ) {
				String subject;
				if ( RawTarget::getVal(attrib_visible) ) {
          
          if (primary->istype(tg_ranged)) {
            pack1 = " holster";
            away1 = " ";
            if (!secondary || secondary->istype(tg_ranged))
              secSame = false;
            }
          else if (primary->istype(tg_weapon) &&
            ( primary->getVal(attrib_meleetype) == mel_sword || primary->getVal(attrib_meleetype) == mel_knife )) {
            pack1 = " sheathe";
            away1 = " ";
             if (!secondary || !secondary->istype(tg_weapon) ||
                !( secondary->getVal(attrib_meleetype) == mel_sword || secondary->getVal(attrib_meleetype) == mel_knife ))
                  secSame = false;
            }
					subject = Grammar::Subject(primary,Grammar::det_definite);
          if (!Grammar::Pronoun() && secSame && (t->getVal(attrib_two_handed)) ) away1 += "both ";
					message += pack1 + ( getVal(attrib_plural)?"":"s" ) + (Grammar::Pronoun()?blank:(away1)) + subject + (Grammar::Pronoun()?away1:blank);
          if (!secondary || !t->getVal(attrib_two_handed) ) message+="to";
					}
          
        packP = true;

				}
        
      if (!!secondary && t->getVal(attrib_two_handed) ) {
        if (!!primary) message += "and";
        
				String subject;
				if ( RawTarget::getVal(attrib_visible) ) {
          if (secSame) {
            pack2 = "";
            away2 = " ";
            }
          else{
            if ( Grammar::Pronoun() ) {
              pack2 = " pack";
    					away2 = " away ";
              }
            if (secondary->istype(tg_ranged)) {
              pack2 = " holster";
              away2 = " ";
              }
            else if (secondary->istype(tg_weapon) &&
              ( secondary->getVal(attrib_meleetype) == mel_sword || secondary->getVal(attrib_meleetype) == mel_knife )) {
              pack2 = " sheathe";
              away2 = " ";
              }
            }
					subject = Grammar::Subject(secondary,Grammar::det_definite);
					message += pack2 + ( getVal(attrib_plural)?"":"s" ) + (Grammar::Pronoun()?blank:(away2)) + subject + (Grammar::Pronoun()?away2:blank)+ "to";
					}
          
        packS = true;
      
        }

      Target pack;
      if (packP) {
        pack = primary->detach();
        perform(aContain,pack);
        }
      if (packS) {
        pack = secondary->detach();
        perform(aContain,pack);
        }

			Target wield = t->detach(1);
			//wield->setlocation( location );
      wield->setParent(THIS);
			primary = wield;

      String wield_str = " wield";
      if ( primary->getVal(attrib_draw) )
        wield_str = " draw";

			if ( RawTarget::getVal(attrib_visible) ) {
				message += wield_str + ( (packP||packS)||getVal(attrib_plural)?" ":"s " ) + Grammar::Object(primary,Grammar::det_possessive) + ".";
				Message::add(message);
				}
      // Changed to return true 26-4-04... any reason this was set to
      // return false?
			return true;
      }
      
		case aTakeOff:
			if ( ( equipped.find(t) )
					 || ( primary==t )
					 || ( secondary==t ) ) {

				RawTarget::perform(aContain,t);
				return true;
				}

			return false;

		case aOpen:
			// Test to open:
      assert (!!t);
      // Error::fatal("Creatures can only open non-NULL items.");
      
			switch ( test(sk_athl, t->getVal( DC_open ), t) ) {
      
        case result_perfect:
        case result_pass:
   				if ( RawTarget::getVal(attrib_visible) ) {
  					String s = Grammar::Subject(THIS,Grammar::det_definite);
  					Message::add( s + " open"+( getVal(attrib_plural)?" ":"s " ) + Grammar::Object(t,Grammar::det_definite) + "." );
  					}
          else if ( t->getVal(attrib_visible) )
  					Message::add( Grammar::Object(t,Grammar::det_definite) + " open"+( t->getVal(attrib_plural)?"":"s" ) + "." );
        //  t->perform(aSetController,THIS);  // Needed for open?
  		 		t->perform(aBeOpened, Target(THIS) );
				  return true;
          
        default:
  				if (RawTarget::getVal(attrib_visible) ) {
  					String s = Grammar::Subject(THIS,Grammar::det_definite);
  					Message::add( s + " fail"+( getVal(attrib_plural)?"":"s" ) +" to open " + Grammar::Object(t,Grammar::det_definite) + "." );
  					}
          return false;
				}

		case aDisarm:
			switch ( test(sk_mech, t->getVal( DC_untrap ), t) ) {
      
        case result_perfect:
        case result_pass:
  				if ( t->getVal(attrib_visible) ) {
  					String s = Grammar::Subject(THIS,Grammar::det_definite);
  					Message::add( s + " disarm"+( getVal(attrib_plural)?" ":"s " ) + Grammar::Object(t,Grammar::det_definite) + "." );
  					}
          t->perform(aSetController,THIS);
  				t->perform(aBeDisarmed, Target(THIS) );
  				return true;
          
        default:
  				if ( t->getVal(attrib_visible) ) {
  					String s = Grammar::Subject(THIS,Grammar::det_definite);
  					Message::add( s + " fail"+( getVal(attrib_plural)?"":"s" ) +" to disarm " + Grammar::Object(t,Grammar::det_definite) + "." );
  					}
          return false;
				}

		case aClose:
			// Test to close:
			switch ( test(sk_athl, t->getVal( DC_close ), t) ) {
      
        case result_perfect:
        case result_pass:
  				if ( RawTarget::getVal(attrib_visible) || t->getVal(attrib_visible)  ) {
  					String s = Grammar::Subject(THIS,Grammar::det_definite);
  					Message::add( s + " close"+( getVal(attrib_plural)?" ":"s " ) + Grammar::Object(t,Grammar::det_definite) + "." );
  					}
        //  t->perform(aSetController,THIS);  // Needed for close?
  				t->perform(aBeClosed, Target(THIS) );
  				return true;
        default:
  				if ( RawTarget::getVal(attrib_visible) ) {
  					String s = Grammar::Subject(THIS,Grammar::det_definite);
  					Message::add( s + " fail"+( getVal(attrib_plural)?"":"s" ) +" to close " + Grammar::Object(t,Grammar::det_definite) + "." );
  					}
          return false;
				}

		case aSearch:

			switch ( test(sk_awar, t->getVal( DC_spot ), t) ) {
      
        case result_perfect:
        case result_pass:
  				if (t->perform( aFind, Target(THIS) )  )
  					if ( t->getVal(attrib_visible) ) {
  						String s = Grammar::Subject(THIS,Grammar::det_definite);
  						Message::add(  s + " find"+( getVal(attrib_plural)?" ":"s " ) + Grammar::plural(t) + "." );

  						if (t->istype(tg_trap) && global::isNew.untrap ) {
  							Message::add( String("<?Press '\\") + Keyboard::keytostr(keyMap[kDisarm].dkey) + "' to disarm traps.>" );
  							global::isNew.untrap = false;
  							}
  						if (global::isNew.search) {
  							Message::add( String("<?Press '\\") + Keyboard::keytostr(keyMap[kSearch].dkey) + "' to search actively for hidden doors and traps.>" );
  							global::isNew.search = false;
  							}

            }
            
        default:
          return false;
        }

		case aKick:
		case aAttack: {

      // We must be attacking something:
      if (!t) return false;

      // Keep track of whether the target was initially alive
      // and whether it still is:
      bool t_was_alive = t->getVal(attrib_alive),
           t_is_alive  = t_was_alive;
      
      // Value for time taken to attack with weapons:
      int fight_time = 0;
      
      // Weapon & creature offense values:
      CombatValues cv_primary, cv_secondary, cv_innate;
      
      // Find out if we're wielding weapons while getting offense values:
      bool mel_primary  = (!!primary   && primary->GET_STRUCT(s_offence_cv,cv_primary) ),
           mel_secondary= (!!secondary && secondary->GET_STRUCT(s_offence_cv,cv_secondary) ),
      // and whether we have an innate attack:
           mel_innate   = GET_STRUCT(s_offence_cv, cv_innate);

      // If we're kicking, ignore our weapons:
      if (a == aKick) {
        mel_primary = false;
        mel_secondary = false;
        }

      // Get target's defence values:
      CombatValues cv_defender, cv_result;
      t->GET_STRUCT( s_defence_cv, cv_defender );

      // For feedback strings:
 			String subject = Grammar::Subject(THIS,Grammar::det_definite);
 			String object = Grammar::Object(t,Grammar::det_definite);
      String hit_str;

      if (mel_primary) {
        // primary weapon attack:
        cv_primary += DiceRoll(1,20).roll()
                    + getSkill(sk_figh);

        // If we're backstabbing,
        if ( t->getVal(attrib_vis_mon) != t->getVal(attrib_vis_enemy)
          // and we have the skill,
          && ( getVal(attrib_feat)&f_stab ) )
          // get the bonus:
          cv_primary += BACKSTAB;

        // When twoweaponing,
        if (mel_secondary
          // and not a twoweapon master,
          && ( getVal(attrib_feat)&f_2hcm )==0 )
          //  we get a -TWO_WEAPON modifier to hit:
          cv_primary += -TWO_WEAPON;
                    
        // Add time taken:
        fight_time = primary->getVal(stat_spd);

        // ROLL THE DICE!
        cv_result = cv_primary.combatResult(cv_defender);

        // GIVE DEBUG MESSAGES:
        #ifdef DEBUG_DICE
        DiceRoll damage = cv_result.damageRoll();
        Message::add( String("<_HIT:") + cv_result.toString() );
        Message::add( String("<_DICEROLL:") + damage.toString());
        #endif // DEBUG_DICE

        // Give the message and attack:
        AttackPenetration pen_primary = cv_result.penetrated();
        if (pen_primary != pen_invalid ) {
        
          if (cv_primary.hit_str)
            hit_str = cv_primary.hit_str;
          else
            hit_str = DamStringArr[pen_primary].hit;

          // Use the values after defence/resistance:
          Attack attack( cv_result, THIS, primary );
          t->takeDamage( attack );

          // If target was just killed, note it:
          t_is_alive = t->getVal(attrib_alive);
          if (t_is_alive != t_was_alive)
            if (t->getVal(attrib_flesh_blood))
              hit_str = "kill~";
            else
              hit_str = "destroy~";
          }
        else
          hit_str = "miss~";

        if ( RawTarget::getVal(attrib_visible) || t->getVal(attrib_visible) )
  				Message::add( subject + " "+Grammar::plural(hit_str,2-getVal(attrib_plural),true,false)+" "+ object + "." );

        }

      if (t_is_alive == t_was_alive && mel_secondary) {
        // secondary weapon attack
        cv_secondary += DiceRoll(1,20).roll()
                      + getSkill(sk_figh);
        
        // If we're backstabbing,
        if ( t->getVal(attrib_vis_mon) != t->getVal(attrib_vis_enemy)
          // and we have the skill,
          && ( getVal(attrib_feat)&f_stab ) )
          // get the bonus:
          cv_secondary += BACKSTAB;

        // When twoweaponing,
        if (mel_primary
          // and not a twoweapon master,
          && ( getVal(attrib_feat)&f_2hcm )==0 )
          //  we get a -TWO_WEAPON modifier to hit:
          cv_secondary += -TWO_WEAPON;

        // and a further -UNBALANCED_2ND if we're not ambidexterous
        // or not using a balanced weapon:
        if (!secondary->getVal(attrib_balanced)
          && ( getVal(attrib_feat)&f_ambi )==0 )
          cv_secondary += -UNBALANCED_2ND;

        // Add time taken:
        if (fight_time > 0)
          // If wielding two weapons, fight time for
          // second weapon is only non-zero when time
          // taken is different from time taken for
          // primary weapon.  So using two similar
          // weapons is same as using one of them:
          fight_time += abs( secondary->getVal(stat_spd)
                             - primary->getVal(stat_spd) );
        else
          fight_time = secondary->getVal(stat_spd);

        // ROLL THE DICE!
        cv_result = cv_secondary.combatResult(cv_defender);

        // GIVE DEBUG MESSAGES:
        #ifdef DEBUG_DICE
        DiceRoll damage = cv_result.damageRoll();
        Message::add( String("<_HIT:") + cv_result.toString() );
        Message::add( String("<_DICEROLL:") + damage.toString());
        #endif // DEBUG_DICE
        
        // Give the message and attack:
        AttackPenetration pen_secondary = cv_result.penetrated();
        if (pen_secondary != pen_invalid ) {
        
          if (cv_secondary.hit_str)
            hit_str = cv_secondary.hit_str;
          else
            hit_str = DamStringArr[pen_secondary].hit;

          // Use the values after defence/resistance:
          Attack attack( cv_result, THIS, secondary );
          t->takeDamage( attack );

          // If target was just killed, note it:
          t_is_alive = t->getVal(attrib_alive);
          if (t_is_alive != t_was_alive)
            if (t->getVal(attrib_flesh_blood))
              hit_str = "kill~";
            else
              hit_str = "destroy~";
          }
        else
          hit_str = "miss~";

        if ( RawTarget::getVal(attrib_visible) || t->getVal(attrib_visible) )
  				Message::add( subject + " "+Grammar::plural(hit_str,2-getVal(attrib_plural),true,false)+" "+ object + "." );

        }

      if (t_is_alive == t_was_alive  && !mel_primary && !mel_secondary && mel_innate) {
        // innate attack
        cv_innate += DiceRoll(1,20).roll()
                   + getSkill(sk_figh);
        
        // If we're explicitly kicking, kick values are calculated like so:
        if (a == aKick) {
          cv_innate.penetration[pen_bash] = getVal(stat_str);
          cv_innate.hit_str = "kick~";
          }
          
        // ROLL THE DICE!
        cv_result = cv_innate.combatResult(cv_defender);

        // GIVE DEBUG MESSAGES:
        #ifdef DEBUG_DICE
        DiceRoll damage = cv_result.damageRoll();
        Message::add( String("<_HIT:") + cv_result.toString() );
        Message::add( String("<_DICEROLL:") + damage.toString());
        #endif // DEBUG_DICE

        // Give the message and attack:
        AttackPenetration pen_innate = cv_result.penetrated();
        if (pen_innate != pen_invalid ) {
        
          if (cv_innate.hit_str)
            hit_str = cv_innate.hit_str;
          else
            hit_str = DamStringArr[pen_innate].hit;

          // Use the values after defence/resistance:
          Attack attack( cv_result, THIS, THIS );
          t->takeDamage( attack );

          // If target was just killed, note it:
          t_is_alive = t->getVal(attrib_alive);
          if (t_is_alive != t_was_alive)
            if (t->getVal(attrib_flesh_blood))
              hit_str = "kill~";
            else
              hit_str = "destroy~";
          }
        else
          hit_str = "miss~";

        if ( RawTarget::getVal(attrib_visible) || t->getVal(attrib_visible) )
  				Message::add( subject + " "+Grammar::plural(hit_str,2-getVal(attrib_plural),true,false)+" "+ object + "." );

        }

      // Create an act event for the brain at a time
      // determined by the speeds of melee weapons:
      if ( fight_time > 0 && !!brain )
        ActionEvent::create( Target(brain), aAct,
         // Time taken is standard turn time per speed:
            GRANULARITY / getVal(stat_spd)
         // multiplied by weapon_speed / 10
            * fight_time / 10
            );


      return true;

      }


    case aFire : {

      // We must be attacking something:
      if (!t) return false;

      // Keep track of whether the target was initially alive
      // and whether it still is:
      bool t_was_alive = t->getVal(attrib_alive);

      // Value for time taken to fire weapons:
      int fire_time = 0;
      
      // Weapon & creature offense values:
      CombatValues cv_primary, cv_secondary, cv_innate;
      
      // Find out if we're wielding weapons while getting offense values:
      bool fire_primary  = (!!primary
                            && primary->isCapable(aFire)
                            && primary->GET_STRUCT(s_shoot_cv,cv_primary) ),
           fire_secondary= (!!secondary
                            && secondary->isCapable(aFire)
                            && secondary->GET_STRUCT(s_shoot_cv,cv_secondary) );
      // Get our innate ranged skill:
      GET_STRUCT(s_shoot_cv, cv_innate);

      // Structure for target's defence values:
      CombatValues cv_defender, cv_result;

      // For feedback strings:
 			String subject = Grammar::Subject(THIS,Grammar::det_definite);
 			String object;
      String hit_str;

      if (fire_primary) {
        // primary weapon attack:
        cv_primary += DiceRoll(1,20).roll()
                    + getSkill(sk_figh);

        // When twoweaponing,
        if (fire_secondary
          // and not a twoweapon master,
          && ( getVal(attrib_feat)&f_2hcm )==0 )
          //  we get a -TWO_WEAPON modifier to hit:
          cv_primary += -TWO_WEAPON;

        // Add a Blam!/Boom! messages for loud weapons,
        // and Todo: alert nearby alertables.
        int sound = primary->getVal(attrib_boom);
        if (sound > 1)
          Message::add("BOOM!");
        else if (sound == 1)
    			Message::add("Blam!");
        // else no sound message

        // Weapon is fired; add time taken:
        fire_time = primary->getVal(stat_spd);

        // Get the missile object:
        Target missile = primary->getTarget(tg_ammo);
        assert(!!missile);

        // Add score for ammo:
        CombatValues cv_ammo;
        missile->GET_STRUCT(s_shoot_cv, cv_ammo);
        cv_primary += cv_ammo;

        // Find the first target (i.e. the first blocker in the path)
        Point3d me = getlocation(), target = t->getlocation();
        Path path(me, target);
        path.calculate();
        // Bullet is finished if all slugs have hit something (missile)
        // or it travels its maximum range (beam)
        // or it reaches the target destination (all)
        bool bullet_done = false;
        int num_slugs = missile->getVal(attrib_num_slugs);

        // bullet is dropped if it is not a gunpowder shell,
        // and it didn't hit:
        bool bullet_drop
          = primary->getVal(attrib_ammotype)==bul_arrow
         || primary->getVal(attrib_ammotype)==bul_harpoon;

        // distance meter (for beams)
        int dist_travelled = 0;
        int range_unit = primary->getVal(attrib_range_unit);
        
        // If creature has the far shot feat,
        if ( getVal(attrib_feat)&f_shot )
          // increase range:
          range_unit = range_unit * RANGE_BONUS;

        // Check all tiles in the path:
        if ( path.reset() )
        do {
          Target toAttack = Map::getTile( path.get() );

          // Deduct range modifier:
          // For each range unit from the player,
          if (dist_travelled % range_unit == 0)
            // bullet gets -RANGE_MOD to hit:
            cv_primary += -RANGE_MOD;

          int num_hits = 0;
          // See if the path contains a blocker:
          if ( toAttack->capable(aBlock,missile) ) {
            toAttack = toAttack->getTarget(tg_top_blocker);
            assert(!!toAttack);

            // Keep track of target's death progress:
            bool to_attack_alive;
            bool to_attack_was_alive
                 = to_attack_alive
                 = toAttack->getVal(attrib_alive);
            
            // Now roll to hit:
            //num_hits = Random::randint(num_slugs) + 1;

            // Opponent gets a chance to parry:
            // since they try to *wrestle* the gun, athletics is appropriate... :)
            if ( toAttack->perform(aParry,primary) )
              // Yes, this *does* stack.  Not likely that there will
              // be situations where 2 mons can parry the same bullet...
              cv_primary += max(
                            (int)(getSkill(sk_athl)-toAttack->getSkill(sk_athl))*2,
                            (int)0 );

            // Get opponent's defence:
            toAttack->GET_STRUCT( s_dodge_cv, cv_defender );

            // Assume miss until we get a hit:
            hit_str = "miss~";
            
            // But if we get a hit, we *are* shooting:
            cv_primary.hit_str = "shoot~";

            // Try to hit num_slug times:
            int count = 0;
            while (count++ < num_slugs && to_attack_alive == to_attack_was_alive) {
              // ROLL THE DICE!
              cv_result = cv_primary.combatResult(cv_defender);

              // GIVE DEBUG MESSAGES:
              #ifdef DEBUG_DICE
              DiceRoll damage = cv_result.damageRoll();
              Message::add( String("<_HIT:") + cv_result.toString() );
              Message::add( String("<_DICEROLL:") + damage.toString());
              #endif // DEBUG_DICE

              object = Grammar::Object(toAttack,Grammar::det_definite);
              // Give the message and attack:
              AttackPenetration pen_primary = cv_result.penetrated();
              if (pen_primary != pen_invalid) {

                num_hits++;
              
                if (cv_primary.hit_str)
                  hit_str = cv_primary.hit_str;
                else
                  hit_str = DamStringArr[pen_primary].hit;

                // Use the values after defence/resistance:
                Attack attack( cv_result, THIS, primary );
                toAttack->takeDamage( attack );

                // If target was just killed, note it:
                to_attack_alive = toAttack->getVal(attrib_alive);
                if (to_attack_alive != to_attack_was_alive)
                  if (toAttack->getVal(attrib_flesh_blood))
                    hit_str = "kill~";
                  else
                    hit_str = "destroy~";
                }
              }

            // Give a single message per monster:
            if ( RawTarget::getVal(attrib_visible) || toAttack->getVal(attrib_visible) )
      				Message::add( subject + " "+Grammar::plural(hit_str,2-getVal(attrib_plural),true,false)+" "+ object + "." );

            }

          // Give a multi-slug message:
          if (num_hits > 1)
            Message::add(String() << num_hits << " hit" << ((num_hits != 1)?"s!":"!"));

          while (num_hits--) {
            // we've hit, so no dropping the shell:
            bullet_drop = false;
            // but it now has extra damage potential:
            missile->doPerform(aFire);

            // If it's not a beam weapon and slugs have all hit:
            if (!getVal(attrib_beam) && (num_slugs--) <= 0)
              bullet_done = true;
            }

          if (getVal(attrib_beam) && dist_travelled++ >= getVal(attrib_beam_range) )
            bullet_done = true;

          } while (path.next() && !bullet_done);

        // the missile may land at the target's feet:
        if (bullet_drop)
    			Map::getTile(target)->perform(aContain,missile);
        }


      if (t->getVal(attrib_alive) == t_was_alive && fire_secondary) {
        // primary weapon attack:
        cv_primary += DiceRoll(1,20).roll()
                    + getSkill(sk_figh);

        // When twoweaponing,
        if (fire_primary
          // and not a twoweapon master,
          && ( getVal(attrib_feat)&f_2hcm )==0 )
          //  we get a -TWO_WEAPON modifier to hit:
          cv_secondary += -TWO_WEAPON;

        // and a further -UNBALANCED_2ND if we're not ambidexterous
        // or not using a balanced weapon:
        if (!secondary->getVal(attrib_balanced)
          && ( getVal(attrib_feat)&f_ambi )==0 )
          cv_secondary += -UNBALANCED_2ND;

        // Add a Blam!/Boom! messages for loud weapons,
        // and Todo: alert nearby alertables.
        int sound = secondary->getVal(attrib_boom);
        if (sound > 1)
          Message::add("BOOM!");
        else if (sound == 1)
    			Message::add("Blam!");
        // else no sound message

        // Weapon is fired; add time taken:
        if (fire_time > 0)
          // If wielding two weapons, fire time for
          // second weapon is only non-zero when time
          // taken is different from time taken for
          // primary weapon.  So firing two similar
          // weapons is same as firing one of them:
          fire_time += abs( secondary->getVal(stat_spd)
                            - primary->getVal(stat_spd) );
        else
          fire_time =  secondary->getVal(stat_spd);

        // Get the missile object:
        Target missile = secondary->getTarget(tg_ammo);
        assert(!!missile);

        // Add score for ammo:
        CombatValues cv_ammo;
        missile->GET_STRUCT(s_shoot_cv, cv_ammo);
        cv_secondary += cv_ammo;

        // Find the first target (i.e. the first blocker in the path)
        Point3d me = getlocation(), target = t->getlocation();
        Path path(me, target);
        path.calculate();
        // Bullet is finished if all slugs have hit something (missile)
        // or it travels its maximum range (beam)
        // or it reaches the target destination (all)
        bool bullet_done = false;
        int num_slugs = missile->getVal(attrib_num_slugs);

        // bullet is dropped if it is an arrow and it didn't hit:
        bool bullet_drop = secondary->getVal(attrib_ammotype)==bul_arrow;

        // distance meter (for beams)
        int dist_travelled = 0;
        int range_unit = secondary->getVal(attrib_range_unit);

        // If creature has the far shot feat,
        if ( getVal(attrib_feat)&f_shot )
          // increase range:
          range_unit = range_unit * RANGE_BONUS;

        // Check all tiles in the path:
        if ( path.reset() )
        do {
          Target toAttack = Map::getTile( path.get() );

          // Deduct range modifier:
          // For each range unit from the player,
          if (dist_travelled % range_unit == 0)
            // bullet gets -RANGE_MOD to hit:
            cv_secondary += -RANGE_MOD;

          int num_hits = 0;
          // See if the path contains a blocker:
          if ( toAttack->capable(aBlock,missile) ) {
            toAttack = toAttack->getTarget(tg_top_blocker);
            assert(!!toAttack);

            // Keep track of target's death progress:
            bool to_attack_alive;
            bool to_attack_was_alive
                 = to_attack_alive
                 = toAttack->getVal(attrib_alive);
            
            // Now roll to hit:
            //num_hits = Random::randint(num_slugs) + 1;

            // Opponent gets a chance to parry:
            // since they try to *wrestle* the gun, athletics is appropriate... :)
            if ( toAttack->perform(aParry,secondary) )
              // Yes, this *does* stack.  Not likely that there will
              // be situations where 2 mons can parry the same bullet...
              cv_secondary += max(
                            (int)(getSkill(sk_athl)-toAttack->getSkill(sk_athl))*2,
                            (int)0 );

            // Get opponent's defence:
            toAttack->GET_STRUCT( s_dodge_cv, cv_defender );

            // Assume miss until we get a hit:
            hit_str = "miss~";
            
            // But if we get a hit, we *are* shooting:
            cv_secondary.hit_str = "shoot~";

            // Try to hit num_slug times:
            int count = 0;
            while (count++ < num_slugs && to_attack_alive == to_attack_was_alive) {
              // ROLL THE DICE!
              cv_result = cv_secondary.combatResult(cv_defender);

              // GIVE DEBUG MESSAGES:
              #ifdef DEBUG_DICE
              DiceRoll damage = cv_result.damageRoll();
              Message::add( String("<_HIT:") + cv_result.toString() );
              Message::add( String("<_DICEROLL:") + damage.toString());
              #endif // DEBUG_DICE


              object = Grammar::Object(toAttack,Grammar::det_definite);
              // Give the message and attack:
              AttackPenetration pen_secondary = cv_result.penetrated();
              if (pen_secondary != pen_invalid) {

                num_hits++;
              
                if (cv_secondary.hit_str)
                  hit_str = cv_secondary.hit_str;
                else
                  hit_str = DamStringArr[pen_secondary].hit;

                // Use the values after defence/resistance:
                Attack attack( cv_result, THIS, secondary );
                toAttack->takeDamage( attack );

                // If target was just killed, note it:
                to_attack_alive = toAttack->getVal(attrib_alive);
                if (to_attack_alive != to_attack_was_alive)
                  if (toAttack->getVal(attrib_flesh_blood))
                    hit_str = "kill~";
                  else
                    hit_str = "destroy~";
                }
              }

            // Give a single message per monster:
            if ( RawTarget::getVal(attrib_visible) || toAttack->getVal(attrib_visible) )
      				Message::add( subject + " "+Grammar::plural(hit_str,2-getVal(attrib_plural),true,false)+" "+ object + "." );

            }

          // Give a multi-slug message:
          if (num_hits > 1)
            Message::add(String() << num_hits << " hit" << ((num_hits != 1)?"s!":"!"));

          while (num_hits--) {
            // we've hit, so no dropping the shell:
            bullet_drop = false;
            // but it now has extra damage potential:
            missile->doPerform(aFire);

            // If it's not a beam weapon and slugs have all hit:
            if (!getVal(attrib_beam) && (num_slugs-=num_hits) <= 0)
              bullet_done = true;
            }

          if (getVal(attrib_beam) && dist_travelled++ >= getVal(attrib_beam_range) )
            bullet_done = true;

          } while (path.next() && !bullet_done);

        // the missile may land at the target's feet:
        if (bullet_drop)
    			Map::getTile(target)->perform(aContain,missile);

        }

      // For consideration: do I want innate ranged attacks?
      // Answer: probably.  Think about how and who should have them.

      // Create an act event for the brain at a time
      // determined by the speeds of fired weapons:
      if ( fire_time > 0 && !!brain )
        ActionEvent::create( Target(brain), aAct,
         // Time taken is standard turn time per speed:
            GRANULARITY / getVal(stat_spd)
         // multiplied by weapon_speed / 10
            * fire_time / 10
            );

      return true;

      }
      
		case aBlock:
      if (t->capable(aAssault,THIS))
  			return t->perform( aAttack, Target(THIS) );
      else
        return t->perform( aChat, Target(THIS) );

		case aHeal:

			//When you're dead, you stay dead:
			if ( current.hp <= 0 ) return true;

			current.hp ++;//current.tuf;
			if ( ascending( 0, current.hp, maximum.hp ) )
				ActionEvent::create( Target(THIS), aHeal, 500 / getSkill(sk_heal) );
			else if (current.hp > maximum.hp)
				current.hp = maximum.hp;
			return true;


		case aSpMessage:
			Message::add( Grammar::plural(Target(THIS)) + ( getVal(attrib_plural)?" are":" is" ) + " here!" );
			return true;

		default:
			return Item::perform(a, t);
		}

	return false;

	}

long Creature::getVal( StatType s ) {

	switch(s) {

    case attrib_ranged_spd:
      return ( (( !!primary && primary->getVal(attrib_combat_style)&2 )
                ? primary->getVal(stat_spd)
                : 0 )
              +(( !!secondary && secondary->getVal(attrib_combat_style)&2 )
                ? (( !!primary && primary->getVal(attrib_combat_style)&2 )
                   ? abs( secondary->getVal(stat_spd)
                          - primary->getVal(stat_spd) )
                   : secondary->getVal(stat_spd) )
                : 0 )
                );
                
    case attrib_melee_spd:
      return ( (( !!primary && primary->getVal(attrib_combat_style)&1 )
                ? primary->getVal(stat_spd)
                : 0 )
              +(( !!secondary && secondary->getVal(attrib_combat_style)&1 )
                ? (( !!primary && primary->getVal(attrib_combat_style)&1 )
                   ? abs( secondary->getVal(stat_spd)
                          - primary->getVal(stat_spd) )
                   : secondary->getVal(stat_spd) )
                : 0 )
                );

    case attrib_floats:
      return Material::data[ creature::List[form].element ].floats;

    case attrib_canswim: {
      // Whether a creature can float depends on the swimming
      // strength of the creature vs. its total non-buoyant weight

      // The average person weighs 125 when naked, and with a str 5
      // and 0 athletics, can only support 125 'pounds'.
      int swim_strength = getSkill(sk_athl) * SWIM_MULT;
      long sink_weight = getVal(attrib_sink_weight);

      return (swim_strength > sink_weight);
      }

    case attrib_volume:
      return creature::List[form].volume;
      
    case attrib_flesh_blood:
      return creature::List[form].element == m_flesh
          && creature::List[form].blood   == m_blood;
    
    case attrib_itemized:
      return true;
      
    case attrib_cash: {
      long cash = 0;
      
      cash += Item::getVal(attrib_cash);

      if (!!primary)   cash += primary->getVal(attrib_cash);
      if (!!secondary) cash += secondary->getVal(attrib_cash);
      if (!!mount) cash += mount->getVal(attrib_cash);
      
      if (equipped.reset())
      do {

        cash += equipped.get()->getVal(attrib_cash);
        
        } while (equipped.next());
    
			return cash;
      }

    case attrib_auto_throw:
      return !!thrown;
      
    case attrib_invisible:
      // Do not draw to the screen unless in LOS and lit
      // so that there aren't creature 'ghosts' displayed
      // all over the out of sight portions of the level.
      // Draw always when dead.
      return (current.hp)?!RawTarget::getVal(attrib_visible):false;
      
    case attrib_gender:
      return gender;
      
    case attrib_form:
      return form;

    case attrib_material:
      return creature::List[form].element;

    case attrib_appetite:
      return creature::List[form].appetite;

    case attrib_flavour:
      // It we're made of meat, our flavour depends on our appetite:
      if ( Material::data[ creature::List[form].element ].flavour
        == ap_allmeat ) {

        // Todo: If we're dead & putrid, we are carrion:
        
        // If we eat meat, we're carnivorous meat:
        if ( creature::List[form].appetite & ap_allmeat )
          return ap_carnmeat;

        // Otherwise, we're herbivorous meat:
        return ap_herbmeat;
        }
       
      return Material::data[ creature::List[form].element ].flavour;

    case attrib_sink_weight:
		case attrib_weight: {

      long weight = VolumeStat::array[creature::List[form].volume].volume
             * Material::data[ creature::List[form].element ].weight
             / 100;

      // If the creature floats, only 1/5th of its weight is non-buoyant:
      if (s == attrib_sink_weight && getVal(attrib_floats) )
        weight /= 5;

      weight += Item::getVal(s);

      if ( !!primary)   weight += primary->getVal(s);
      if ( !!secondary) weight += secondary->getVal(s);
      // Mounted creatures shouldn't add to stamdard weight:
      if ( !!mount
        && s != attrib_weight ) weight += mount->getVal(s);
      
      if (equipped.reset())
      do {

        weight += equipped.get()->getVal(s);
        
        } while (equipped.next());
    
			return weight;
      
      }


    case attrib_nutrition:
      return (getVal(stat_tuf) * getVal(stat_str) * 150);

		case stat_crd :	return current.crd;
		case stat_per :	return current.per;
		case stat_str :	return current.str;
		case stat_tuf :	return current.tuf;
		case stat_wil :	return current.wil;
		case stat_kno :	return current.kno;
		case stat_con :	return current.con;
    case stat_spd : return current.spd;

		case stat_hp :
			return current.hp;

    case DC_hitmelee:
      // Todo:  add visibility, other combat factors
      return getSkill(sk_figh) + getSkill(sk_acro,false) + 10;

		case attrib_vis_mon :
			return current.hp > 0;

    case attrib_vis_enemy :
      return (current.hp > 0) && isCapable(aAssault);
      
    case attrib_alive :
			return current.hp > 0;

    case attrib_sort_category:
      return ( ( (current.hp>0)?gettype():tg_food ) << TYPE_SORT_SHIFT )
             + form;

    case attrib_price:
      // Todo: If dead, a creature is worth its materials:
      if ( !getVal(attrib_alive) )
        return 10;

      // Fall through; a living creature is worth its xp in Dollars:
    case attrib_xp: {
      // Calculate xp value for player:
      long xp;
      xp = maximum.str+maximum.crd+maximum.spd+current_lev;
      xp += VolumeStat::array[getVal(attrib_volume)].bonus;

      CombatValues cv_defence;
      GET_STRUCT( s_defence_cv, cv_defence );

      // Give a bonus for armour:
      long armour_bonus;
      long a_max = 0, a_min = -1, a_total = 0;

      // Bonus applies only to: pierce, bash, cut & chop:
      for (int i = pen_pierce; i <= pen_chop; ++i ) {

        if (cv_defence.defence[i] > a_max)
          a_max = cv_defence.defence[i];
          
        if (cv_defence.defence[i] < a_min || a_min < 0)
          a_min = cv_defence.defence[i];

        a_total += cv_defence.defence[i];
        }

#     define MAX_ARMOUR_BONUS 50
#     define MAX_ARMOUR_MULT  5

      armour_bonus = ( (a_total-a_min) + (a_total-a_max) ) / 2;
      armour_bonus = max( armour_bonus, static_cast<long>(MAX_ARMOUR_BONUS) );

      if (armour_bonus > 1)
        xp += (xp * MAX_ARMOUR_MULT) / (MAX_ARMOUR_BONUS - armour_bonus + 1);
      
      // Give a bonus for resistances:
      long resist_bonus = 0;

      for (int i = dam_physical+1; i < dam_num; ++i)
        resist_bonus += cv_defence.resist[i];

      // Resist bonus is only non-zero when resists physical damage:
      resist_bonus *= cv_defence.resist[dam_physical];

      if (resist_bonus > 0)
        xp += resist_bonus/100;

      // Combat stats bonus:
      xp *= ( creature::List[form].innate_penetration
            + maximum.hp )
             * creature::List[form].innate_lethality / 100;

      // Scale down the xp:
      xp /= 10;
      
      if (s == attrib_price)
        return Dollars(xp);
        
      return xp;
      }

    case attrib_level:
      return current_lev;

		default:
			return Item::getVal(s);
		}

	}

long Creature::getSkill( Skill skill, bool get_total ) {

  int equipped_bonus = 0, size_bonus = 0;

  // We should add equipped bonus for non-fighting skills:
  if (skill != sk_figh && skill != sk_mark) {
    // Now attacks have individual to-hit rolls, so we must take
    // account of the values individually (so not here.
    
    // Otherwise, ask worn/wielded for their contributions:
  
    if (!!primary)   equipped_bonus += primary->getSkill(skill);
    if (!!secondary) equipped_bonus += secondary->getSkill(skill);

    if (equipped.reset())
    do {
      equipped_bonus += equipped.get()->getSkill(skill);
      } while (equipped.next());
    }
   
  else
    // When fighting we add the creature's size bonus:
    size_bonus = VolumeStat::array[getVal(attrib_volume)].bonus;


  // Special return value for diplomacy skill...
  // Add up all clothing appearance values and give
  // a fraction of the absolute total.  Therefore
  // wearing a lot of mean clothing is as visually
  // effective as wearing a lot of dignified clothing.
  // Wearing an outfit that is neither here nor there
  // is as ineffectual as wearing all bland clothing.
  // The idea: Give the player incentive to pick
  // an outfit that all goes together.
  int theme_bonus = 0;

  if ( skill == sk_dipl ) {
    // Todo: fill this in!
    }

  // If not adding the stat, just return the equipped bonus + size bonus:
  if (!get_total) return equipped_bonus + size_bonus + theme_bonus;
  
  return equipped_bonus + size_bonus
       + theme_bonus + getVal(Skills[skill].stat);

  }
  

Result Creature::test( Skill skill, int DC, const Target & ) {

//  StatType stat = Skills[s].stat;
  int roll = DiceRoll(1,20).roll();

  if ( roll + getSkill(skill) < DC ) {

    if (roll == 1) return result_fumble;
    return result_fail;
  
    }
  else {

    if (roll == 20) return result_perfect;
    return result_pass;

    }

  }

void Creature::takeDamage( const Attack &a ) {

  Target retain = THIS;

	// If attack is not very strong, creature has a
	// chance to shrug off the attack:
  // Now taken care of with combatvalues.
	//if ( a.strength + Random::randint( a.strength ) < current.tuf && current.hp>0 ) {
	//	return;
	//	}

  // Some attacks allow an armour save.
  // Does not apply for drowning damage
  // Todo: armour saves

	//int damage = a.damage.roll();
  int damage = a.c_values.rollDamage();
  
//	String wound = "wounded.";
/*
  if (a.type == dam_drown)
    wound = "drowning!";
  else if (damage > 0) {
  	if (current.hp / damage < 4)
  	 wound = "badly wounded.";
  	if (current.hp / damage < 3)
  	 wound = "heavily wounded.";
   	if (current.hp / damage < 2) {
   	 wound = "horribly wounded.";
     
	 // If the hit was critical, possibly bleed:
   
   // Todo: make dead bodies lose some nutrition (and rot faster) when bleeding
   // so the player won't kick corpses around to scum for nutrition.
   
  	 if (parent != NULL && Random::randint(100) <= creature::List[form].blood_pc ) {
       Fluid::Option fluid_opt;
       fluid_opt.material = creature::List[form].blood;
  		 Target blood = Fluid::create( fluid_opt );
       Target p = parent.toTarget();
  		 parent->perform( aContain,blood );
   		 }
   	 }
   }
  */

//	if ( RawTarget::getVal(attrib_visible) && damage > 0 && current.hp-damage > 0 )
//		Message::add( Grammar::Subject(THIS,Grammar::det_definite) + (getVal(attrib_plural)?" are ":" is ") + wound);

  // Set the controller to the deliverer of the damage:
  perform(aSetController, a.deliverer);

	// If fully healthy, begin to heal:
	if (current.hp == maximum.hp && damage > 0)
		ActionEvent::create( Target(THIS), aHeal, 1000 / getSkill(sk_heal) );

	// If not dead, damage:
	if (current.hp > 0) {
		current.hp -= damage;

    // Give damage experience to the attacking weapon/creature:
    a.instrument->gainExp( damage );
    
    // If we just died, drop our inventory:
    if (current.hp <= 0 && !istype(tg_player)) {
      //assert (location != OFF_MAP);

      assert( !!parent );
      //  Message::add("Critical Error: Monster died when in transit.");
      //  return;
      //  }

      //Target parent = getTarget(tg_parent);
      //assert(!!parent);

      Target drop;

      if (!!primary) {
        drop = primary;
        perform( aRelease, drop );
        }
      if (!!secondary) {
        drop = secondary;
        perform( aRelease, drop );
        }
      if (!!mount) {
        drop = mount;
        perform( aRelease, drop );
        }

      while (equipped.reset()) {
        drop = equipped.get();
        perform( aRelease, drop );
        }

      if (inventory)
      while (inventory && inventory->reset()) {
        drop = inventory->get();
        perform( aRelease, drop );
        }

      // And tell the brain we're done thinking:
 		  if (!!brain)
  			brain->doPerform(aSleep);
      brain = Target(NULL);

      }
    }
	else {
		Item::takeDamage(a);
		if (!!brain)
			brain->doPerform(aSleep);
    brain = Target(NULL);
    }

	if (current.hp > 0 && (Target(THIS)!=a.deliverer ) && !!a.deliverer )
	// Alert the brain that we have an enemy:
		perform(aWarn, a.deliverer);

	}

bool Creature::remove( const Target &t ) {

	// A rewrite of the Rawtarget version
	// that forbids deletion of the inventory
	// list.  Hack for UI menus.
  long i_key = 0, e_key = 0;

  // Make sure that we are the controller:
  t->perform( aSetController, THIS );

  // If we have assigned the item as to-be-thrown,
  // unassign:
  if (thrown == t) thrown = Target(NULL);
  
	if ( inventory ) i_key = inventory->lock();
  if ( inventory && inventory->reset() ) {

 		if ( inventory->find(t) ) {
 			inventory->remove();

      inventory->load(i_key);
 			return true;
 			}

    inventory->load(i_key);
    }

	// If item is equipped, take it off:
  e_key = equipped.lock();
	if ( equipped.reset() ) {
		if ( equipped.find(t) ) {
			equipped.remove();

      equipped.load(e_key);
			return true;
			}

    equipped.load(e_key);
		}

	// If item is wielded, remove it:
	if ( t == primary ) {
		primary = Target(NULL);
		return true;
		}
	if ( t == secondary ) {
		secondary = Target(NULL);
		return true;
		}

  // If item is mount, detach it:
	if ( t == mount ) {
		mount = Target(NULL);
		return true;
		}

	return false;

	}

Target Creature::getTarget( targetType type ) {
  switch( type ) {
    case tg_possessor:
      // All creatures possess themself and all possessions:
      return Target(THIS);
      
    case tg_mount:
      return mount;
    
    default:
      return Item::getTarget(type);
    }
  }

bool Creature::getStruct( structType type, void *buffer, size_t size ) {

  switch (type) {
    case s_shoot_cv: {
      // Ranged attack:
      // Make sure that there's the exact amount of space:
      assert( size == sizeof(CombatValues) );
      CombatValues cv;

      // STEP 2
      // ... (Todo: step 2)


      // Copy our CombatValues to the buffer:
      memcpy( buffer, (const void*) &cv, size );
      
      return true;
      }

    case s_offence_cv: {
      // Melee attack:
      // Make sure that there's the exact amount of space:
      assert( size == sizeof(CombatValues) );
      CombatValues cv;

      // The creature's innate attack: kick, punch, spit, etc.
      InnateAttack attack_type = creature::List[form].innate_attack;
      AttackDamage damage = creature::List[form].innate_damage;
      int penetration = creature::List[form].innate_penetration,
          lethality = creature::List[form].innate_lethality;
      
      // Fill cv with combat value details calculated from creature list:

      // Penetration:
      // If there is more than one attack type, only use one:
      attack_type = (InnateAttack)Random::randbit(attack_type);
      switch (attack_type) {
        case ia_hit  :
          cv.penetration[pen_bash] = penetration;
          cv.hit_str = "hit~";
          break;
        case ia_kick :
          cv.penetration[pen_bash] = penetration;
          cv.hit_str = "kick~";
          break;
        case ia_claw :
          cv.penetration[pen_cut] = penetration;
          cv.hit_str = "claw~";
          break;
        case ia_bite :
          cv.penetration[pen_pierce] = penetration;
          cv.hit_str = "bite~";
          break;
        case ia_peck :
          cv.penetration[pen_pierce] = penetration;
          cv.hit_str = "peck~";
          break;
        case ia_butt :
          cv.penetration[pen_bash] = penetration;
          cv.hit_str = "butt~";
          break;
        case ia_gore :
          cv.penetration[pen_pierce] = penetration;
          cv.hit_str = "gore~";
          break;
        case ia_spit :
          cv.penetration[pen_splash] = penetration;
          cv.hit_str = "spit~";
          break;
        case ia_sting: // Sting is splash since most clothing will protect
          cv.penetration[pen_splash] = penetration;
          cv.hit_str = "sting~";
          break;
        case ia_choke:
          cv.penetration[pen_crush] = penetration;
          cv.hit_str = "choke~";
          break;
        case ia_crush:
          cv.penetration[pen_crush] = penetration;
          cv.hit_str = "crush~";
          break;
        case ia_touch :
          cv.penetration[pen_splash] = penetration;
          cv.hit_str = "touch~";
          break;
        case ia_spray :
          cv.penetration[pen_vapor] = penetration;
          cv.hit_str = "spray~";
          break;
        default:
          Message::add(String("Damage type ")+(long)attack_type+" is not implemented.");
        };
      
      // Damage:
      // ATM the creature list only supports one damage type,
      // so (for example) snakes can bite and spit poison but
      // each attack only does poison damage and not physical.
      // So you couldn't implement a unicorn that has a gore-purity
      // and a kick-physical attack.
      // This should be remedied in a later version.
      cv.damage[damage] = lethality;

      // Copy our CombatValues to the buffer:
      memcpy( buffer, (const void*) &cv, size );
      
      return true;
      }
      
    case s_defence_cv:
    case s_dodge_cv: {
    
      
      // Make sure that there's the exact amount of space:
      assert( size == sizeof(CombatValues) );
      CombatValues cv, temp;

      // Fill cv with combat value totals of all worn/wielded items:
      
      // Innate:
      // Base AC of 10
      cv += 10;
      if (type == s_defence_cv)
        // in melee, either fighting skill or acrobatics (dodge) skill:
        cv += max( (int)getSkill(sk_acro), (int)getSkill(sk_figh) );
      else
        // when dodging you only get the acrobatics skill:
        cv += getSkill(sk_acro);

      // Racial:
      if (creature::List[form].cv_pointer)
        cv += (*creature::List[form].cv_pointer);

      // Wielded:
      if (!!primary && primary->GET_STRUCT(type,temp) )
        cv += temp;
      if (!!secondary && secondary->GET_STRUCT(type,temp) )
        cv += temp;

      // Worn:
      if (equipped.reset())
      do {

        if (equipped.get()->GET_STRUCT(type,temp) )
          cv += temp;
      
        } while (equipped.next());

      // Copy our CombatValues to the buffer:
      memcpy( buffer, (const void*) &cv, size );
      
      return true;
      }

    default:
      return false;
    }

  }
  

void Creature::gainExp( long xp ) {
// Gain xp experience points
// called when we dodge a blow without armour
// protection, or when we land a blow without
// a wielded weapon:
  current_xp += xp;

  }
  
#endif // CREATURE_CPP
