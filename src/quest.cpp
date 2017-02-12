// [quest.cpp]
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
#if !defined ( QUEST_CPP )
#define QUEST_CPP

#include "quest.hpp"
#include "level.hpp"
#include "event.hpp"
#include "enum.hpp"
#include "message.hpp"
#include "input.hpp"
#include "menu.hpp"
#include "target.hpp"
#include "dialog.hpp"
#include "global.hpp"
#include "player.hpp"
#include "random.hpp"
#include "grammar.hpp"
#include "path.hpp"
#include "temp.hpp"
#include "misc.hpp"
#include "fluid.hpp"
#include "visuals.hpp"
#include "catalog.hpp"

//-[ class Quest ]----------

Quest::Quest( const Option *o )
: RawTarget(o), object(o->object), stage(qs_briefing) {
  }

Quest::~Quest( void ) {
  //if (!!object) object.save();
  }


Quest::Quest( std::iostream &inFile, TargetValue *TVP )
: RawTarget( inFile, TVP ) {

	inFile >> object;
  inFile.read( (char*)&stage, sizeof(stage) );
  
	}

void Quest::toFile( std::iostream &outFile ) {
	RawTarget::toFile( outFile );

	outFile << object;
  outFile.write( (char*)&stage, sizeof(stage) );

	}


targetType Quest::gettype( void ) const {
	return tg_quest;
	}

bool  Quest::istype( targetType t ) const {
	if ( t == tg_quest ) return true;
	else return RawTarget::istype(t);
	}

bool Quest::capable( Action a, const Target &t ) {

	switch (a) {

    case aDeductBill:
      return (stage == qs_briefed && t == object);

		default:
			return RawTarget::capable( a, t );
		}
	}

bool Quest::perform( Action a, const Target &t ) {

	switch (a) {

    case aRespond: {

      //Target parent = getTarget(tg_parent);
      assert(!!parent);
      
      switch (stage) {
        
        case qs_briefing: {
          // Brief the player on the quest:

          // Make absolutely sure that we've created an item:
          //RawTarget::perform(aEnterLev);

          // Now we know who owns the item:
          object->identify( id_owner );

          Grammar::Subject(parent.toTarget());
            
          Message::add("\"Please help retrieve "+Grammar::plural(object,Grammar::det_demonst_wout)+".\"");
          Message::add("\"I lost it somewhere deep underground.\"");
          stage = qs_briefed;
          break;
          }
        case qs_briefed: {
          // Remind the player what the quest was:
          Grammar::Subject(parent.toTarget());
          Message::add("\"Please bring me "+Grammar::plural(object,Grammar::det_demonst_wout)+" soon!\"");
          break;
          }
          
        case qs_reward: {
          // Reward the player for completing the quest:
          Target reward = Random::questitem();
          
          Message::add("\"Thank you!  Please accept this small token as a reward.\"");
          Message::add(Grammar::Subject(parent.toTarget(),Grammar::det_definite)+" gives you "+Grammar::Object(reward)+"." );

          if (!!t)
            t->perform(aContain, reward);
          
          stage = qs_complete;
          break;
          }

        case qs_complete:
          // No more rewards, buddy!
          Message::add("\"Thank you!\"");
          break;

        default:
          break;
        
        }

      
      return true;
      }

    case aDeductBill: {

      //Target parent = getTarget(tg_parent);
      assert(!!parent);
      Grammar::Subject(parent.toTarget());
      Message::add( "\""+Grammar::plural(object,Grammar::det_demonst_with)+"!  You found it!\"" );
      stage = qs_reward;
      return true;
      }


    case aEnterLev:
      // If we haven't made an object yet, make one now:
      if (!object && getVal(attrib_on_level)) {
        // This should occur the first time we are placed on a level *only*:
        //Target parent = getTarget(tg_parent);
        if (!!parent) {
          object = Random::questitem();
          //Map::get( getlevel().loc() - Point3d(0,0,Map::getTile(location)->getVal(attrib_level)) ).addRequirement( object );
          Map::get( getlevel().loc() - Point3d(0,0,parent->getVal(attrib_level)) ).addRequirement( Req(object) );
          object->perform( aSetOwner, parent.toTarget() );
          }
        }
      return true;


    case aSleep:
      // Called when a questbearer dies:
      object = Target();
      return true;

    default:
			return RawTarget::perform( a, t );
		}
	}

String Quest::name( NameOption ) {
  return String("#quest~");
  }

Image Quest::menuletter( void ) {
  return Image(cGrey, letter);
  }
  
void Quest::setletter( char l ) {
  letter = l;
  }

/*
void Quest::setlocation( const Point3d &p3d ) {
// This function could be inherited from Item, but we
// don't want quest to have a lot of the same functionality
// and quests aren't tangible (& shouldn't be placed on map)

  if ( location != OFF_MAP )
    RawTarget::perform( aLeaveLev );

  RawTarget::setlocation(p3d);
  // Because RawTarget::setlocation no longer calls detach():

  if ( location != OFF_MAP )
    RawTarget::perform( aEnterLev );

	}
*/
  

#endif // QUEST_CPP

