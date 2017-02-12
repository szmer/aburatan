// [trap.cpp]
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
#if !defined ( TRAP_CPP )
#define TRAP_CPP

#include "trap.hpp"
#include "message.hpp"
#include "grammar.hpp"
#include "input.hpp"
#include "global.hpp"
#include "player.hpp"

targetType Trap::gettype( void ) const {
	return tg_trap;
	}

bool Trap::istype( targetType t ) const {

	if ( t == tg_trap )
		return true;
	else return Item::istype(t);

	}

bool Trap::capable( Action a, const Target &t ) {

	switch( a ) {
    case aBeAttacked:
		case aBeKicked:
			return capable( aView, t );
		case aFind:
			return ( capable( aView, t ) )?false:true;
		case aRelease:
			return (shut)?false:true;
		case aBeDisarmed:
			return (image.i && ( !shut || inventory ) )?true:false;
		case aBeOpened:
			return (shut)?true:false;
		case aSeeInv:
			return true;
		case aBlock:
			if (inventory && inventory->reset())
				return inventory->get()->capable( a, t );
			return false;
		case aSpMessage:
			return true;
		default:
			return Item::capable(a, t);
		}

	}

bool Trap::perform( Action a, const Target &t ) {

	switch( a ) {
		case aAlertShareSquare:
			if ( !shut && t->getlocation() == getlocation() ) {
				// Todo: check that t item weight is over sensitivity threshold
				// Todo: check to see if t can avoid the trap

				// Hack: do not set off if t is ammo, a trap or liquid:
				if ( t->istype(tg_ammo) || t->istype(tg_trap) || t->istype(tg_fluid) ) return true;

				bool isPlayer = t->istype(tg_player);

  			switch ( t->test(sk_awar, getVal( DC_avoid ), THIS) ) {
      
          case result_perfect:
          case result_pass:
  					if ( RawTarget::getVal(attrib_visible) ) {
  						String s = Grammar::Subject(t,Grammar::det_definite);
  						Message::add( s + " avoid"+( t->getVal(attrib_plural)?" ":"s " ) + Grammar::Object(THIS) +".");

  						if (global::isNew.untrap ) {
  							Message::add( String("<?Press '\\") + Keyboard::keytostr(keyMap[kDisarm].dkey) + "' to disarm traps.>" );
  							global::isNew.untrap = false;
  							}
  						if (global::isNew.search) {
  							Message::add( String("<?Press '\\") + Keyboard::keytostr(keyMap[kSearch].dkey) + "' to search actively for hidden doors and traps.>" );
  							global::isNew.search = false;
  							}

  						}
  					image = menuletter();

					  return true;
            
          default:
            break;
					}

				image = menuletter();

				if ( RawTarget::getVal(attrib_visible) ) {
					String subject = Grammar::Subject(t,Grammar::det_definite);
					String object = Grammar::Object(THIS);
					Message::add( ((isPlayer)?"<-":"<+" ) + subject + " set"+( (isPlayer || t->quantity() > 1)?" ":"s " ) + (Grammar::Pronoun()?"":"off ") + object + (Grammar::Pronoun()?" off":"")  +"." );

					if (global::isNew.untrap ) {
						Message::add( String("<?Press '\\") + Keyboard::keytostr(keyMap[kDisarm].dkey) + "' to disarm traps.>");
						global::isNew.untrap = false;
						}
					if (global::isNew.search) {
						Message::add( String("<?Press '\\") + Keyboard::keytostr(keyMap[kSearch].dkey) + "' to search actively for hidden doors and traps.>");
						global::isNew.search = false;
						}
					}

				Item::perform( aContain, t);
        
        Target deliverer = controller;
        if (!deliverer)
            deliverer = getTarget(tg_controller);

				Attack caught( CombatValues(pen_chop,3), deliverer, THIS, "trapped" );
				t->takeDamage( caught );

				shut = true;
				return true;
				}
			return false;
		case aBeDisarmed: {
			image = menuletter();
			shut = true;
			if ( inventory && inventory->reset() ) {
        Target parent = getTarget(tg_parent);
        assert(!!parent);
        Target contents = inventory->get();
				parent->perform( aContain,  contents );
        }

			draw();
			return true;
      }
		case aBeOpened:
			if ( inventory && inventory->reset() ) {
        Target parent = getTarget(tg_parent);
        assert(!!parent);
        Target contents = inventory->get();
				parent->perform( aContain, contents );
        }

			shut = false;
			image = menuletter();
			draw();
			return true;

		case aContain:
			if (inventory && inventory->reset()) {
        Target parent = getTarget(tg_parent);
        assert(!!parent);
  			return parent->perform( a, t );
        }
			else
				return Item::perform( a, t );

		case aFind:
			image = menuletter();
			draw();
			return true;

		case aBlock:
			if (inventory && inventory->reset())
				return inventory->get()->perform( a, t );

			return false;

		case aSpMessage:
			if (shut && (!inventory||!inventory->reset()) ) Message::add("A disarmed trap is here.");
			if (shut && inventory && inventory->reset() && !inventory->get()->istype(tg_player)) {
        if (Player::get().capable(aSee,Target()))
          Message::add("You see here "+Grammar::plural(THIS)+".");
        else if (Player::get().capable(aTouch,Target()))
          Message::add("You feel here "+Grammar::plural(THIS)+".");
        }
			return true;

		default:
			return Item::perform(a, t);
		}

	}

Action Trap::autoAction( void ) {
// Not only called if item in trap is a blocker:
	if (inventory && inventory->reset())
    return inventory->get()->autoAction();

  return aNone;
	}
  
Action Trap::exitAction( void ) {

	return aOpen;
	}

Trap::Trap( const Option *o )
: Item(o) {
	image = 0;
	shut = false;
	}

Trap::Trap( Trap &rhs )
: Item(rhs) {

	shut = rhs.shut;
	}

Image Trap::getimage( void ) {
	if ( inventory && inventory->reset() ) {
		return inventory->get()->getimage();
		}

	return menuletter();
	}

Trap::Trap( std::iostream &inFile, TargetValue *TVP )
// Read Item base information:
: Item (inFile, TVP) {

	// Read char shut
	inFile.read( (char*) &shut, sizeof(shut) );


	}

void Trap::toFile( std::iostream &outFile ) {

	// Write Item base information:
	Item::toFile( outFile );

	// Write char shut
	outFile.write( (char*) &shut, sizeof(shut) );

	}

Target Trap::clone( void ) {

	Trap *newcopy = new Trap( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

bool Trap::equals( const Target & ) {

	return false; // Traps never stack.
	}

Image Trap::menuletter() {
	return Image( (!shut)?cRed:cGrey, '^');
	}



String Trap::name( NameOption n_opt ) {
  switch (n_opt) {
    default:
    	String retstr = "#bear trap~";

    	if ( inventory && inventory->reset() ) {
    		if ( ! inventory->get()->istype(tg_creature) ||
    					 inventory->get()->getVal(stat_hp) <= 0 &&
    					 ! inventory->get()->istype(tg_player) )
    			retstr += " containing " + Grammar::plural(inventory->get());
    		}

    	return retstr + Item::name(n_opt);
    }
    
  }


long Trap::getVal( StatType s ) {

	switch(s) {

    case attrib_sort_category:
      return Item::getVal(s) + ((inventory&&inventory->reset())?0:1);
      
    case attrib_weight:
    case attrib_sink_weight:
      return Material::data[ getVal(attrib_material) ].weight/5
           + Item::getVal(attrib_weight);

    case attrib_material:
      return m_iron;
      
		case DC_open :
			return 10;

		case DC_untrap :
			return 12;

		case DC_avoid :
			return 8 + ( (image.i == 0)?8:0 );

		case DC_spot :
			return 12;

    case attrib_invisible :
			return !image.i;

		default:
			return Item::getVal(s);
		}

	}


void Trap::takeDamage( const Attack &a ) {

  // Todo: trap can be destroyed.
	Item::takeDamage(a);

	}

#endif // TRAP_CPP
