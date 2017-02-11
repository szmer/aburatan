// [actor.cpp]
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
#if !defined ( ACTOR_CPP )
#define ACTOR_CPP

#include "actor.hpp"
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

//-[ pure virtual class Actor ]----------

Actor::Actor( const Option *o )
: RawTarget(o), object(o->object) {
	}

Actor::~Actor( void ) {

	}


Actor::Actor( std::iostream &inFile, TargetValue *TVP )
: RawTarget( inFile, TVP ) {

	inFile >> object;
	}

void Actor::toFile( std::iostream &outFile ) {
	RawTarget::toFile( outFile );

	outFile << object;
	}


Target Actor::getowner( void ) {
  return Target(NULL);
  }

targetType Actor::gettype( void ) const {
	return tg_actor;
	}

bool  Actor::istype( targetType t ) const {
	if ( t == tg_actor ) return true;
	else return RawTarget::istype(t);
	}

bool Actor::capable( Action a, const Target &t ) {

	switch (a) {
		case aAct:
			return true;

		case aWarn:
			return true;

		default:
			return RawTarget::capable( a, t );
		}
	}

bool Actor::perform( Action a, const Target &t ) {

	switch (a) {
		case aWake:
			ActionEvent::create( Target(THIS), aAct, GRANULARITY / object->getVal(stat_spd) );
			return true;

		case aSleep:
			ActionEvent::remove( Target(THIS), aAct );
			return true;

		case aAct:
			return true;

		default:
			return RawTarget::perform( a, t );
		}
	}


//-[ class FlightPath ]----------


FlightPath::FlightPath( const Option *o )
: Actor( o ), line(o->start,o->start+Point3d(o->delta)) {

	line.calculate();
	line.reset();

	o->object->doPerform(aFly);

	}

Target FlightPath::create( Option &o ) {

	FlightPath *flight = new FlightPath( &o );

	TargetValue *THIS = new TargetValue( flight );

	flight->setTHIS(THIS);

	ActionEvent::create( Target(THIS), aAct, o.time );

	return Target( THIS );

	}

targetType FlightPath::gettype( void ) const {
	return tg_flight;
	}

bool  FlightPath::istype( targetType t ) const {
	if ( t == tg_flight ) return true;
	else return Actor::istype(t);
	}

bool FlightPath::perform( Action a, const Target &t ) {

	if ( a != aAct ) return Actor::perform( a, t );

  // Todo: Pause for animation effect if option set:
  // Screen::nextFrame();

	// Can object leave current location?
	if ( !!object->getTarget(tg_parent) )
	if ( ! object->getTarget(tg_parent)->capable(aRelease,object) ) {
		Message::add( Grammar::Subject(object,Grammar::det_definite) + ((object->quantity()>1)?" are":" is")+" stuck!" );
		object->doPerform(aLand);
		object->doPerform(aRestack);
		return true;
		}

	// Find next point in line:
	Point3d newpoint( line.get() );

	// If newlocation is outside the map, stop:
	if ( !Map::limitsRect().contains( newpoint.toPoint() ) ) {
		object->doPerform(aLand);
		object->doPerform(aRestack);
		return true;
		}


	Target newLoc = Map::getTile( newpoint );
	if ( newLoc->capable( aContain,object) ) {

		// If we can enter the square, do so:
		if (line.next()) {
      Screen::hidecursor();
			newLoc->perform(aContain,object );
			ActionEvent::create( Target(THIS), aAct, -1 );
      Screen::showcursor();
			}
		else {

			// Following lines were removed until Mike gets round to
			// fixing up the "eternal loop when shooting at item stacks" bug.
      // Later it was found that the bug had disappeared...
      // Could be volatile code.
      //
      // 12-3-04: Found to be volatile.  Attmepts to repeatedly kick items
      // from a stack results in errors.

			// If end of the line, attack the location:

      /*
			Target target = newLoc;
			List<Target> *items = newLoc->itemList();
			if ( items && items->reset())
			do {
				if ( items->get() != object )
					target = items->get();
				} while ( items->get() == object && items->next() );

			object->perform( aAttack, target );
      */
      //-[ end volatile code ]------

			newLoc->perform(aContain,object );
			object->doPerform(aLand);
			object->doPerform(aRestack);

			}
		return true;
		}
	else {
		// If item cannot enter this square, attack it:
		Target target = newLoc;
		List<Target> *items = newLoc->itemList();
		if ( items ) {
			items->reset();
			target = items->get();
			}
		object->perform( aAttack, target );
		object->doPerform(aLand);
		object->doPerform(aRestack);
		return true;
		}
	}

FlightPath::FlightPath( std::iostream &inFile, TargetValue *TVP )
: Actor( inFile, TVP ) {

	// todo:  add path to load/save!
  assert(0);
  // Must add flightpath load/save!
	}

void FlightPath::toFile( std::iostream &outFile ) {
	Actor::toFile( outFile );

	// todo:  add path to load/save!
	}


//-[ class Brain ]----------


Brain::Brain( const Option *o )
: Actor( o ), enemy(), enLoc(-1,-1,-1) {
	}

Target Brain::create( Option &o ) {

	Brain *brain = new Brain( &o );

	TargetValue *THIS = new TargetValue( brain );

	brain->setTHIS(THIS);

	ActionEvent::create( Target(THIS), aAct, o.time );

	return Target( THIS );

	}

targetType Brain::gettype( void ) const {
	return tg_brain;
	}

bool  Brain::istype( targetType t ) const {
	if ( t == tg_brain ) return true;
	else return Actor::istype(t);
	}

bool Brain::capable( Action a, const Target &t ) {

	switch (a) {
    case aAssault:
      return (enemy == t);

		case aChat: {
      bool ret_val = true;
      // Can only chat when we aren't angry at 't':
      if (enemy == t) ret_val = false;

			return ret_val;
      }

		default:
			return Actor::capable( a, t );
		}
	}


bool Brain::perform( Action a, const Target &t ) {

  switch (a) {
  
    case aWarn:
    // Somebody is threatening us or our property, so we hate them:
   	if ( a == aWarn && !enemy && !!t && t->istype(tg_creature) ) {
   		enemy = t;
   		enLoc = enemy->getlocation();
   		return true;
   		}
    return Actor::perform(a,t);

    case aRespond:
    // Somebody is talking to us, so notice them:
    if ( !enemy && !!t && object->capable(aAssault,t) ) {
   		enemy = t;
   		enLoc = enemy->getlocation();
   		return true;
      }
    return Actor::perform(a,t);
    
   case aAct: {
   	// Todo: If creature is not on level, save & return

    // Check for strangeness:
    assert(!!object);
    
    // Get rid of all scheduled act events for this brain:
    ActionEvent::remove( Target(THIS), aAct );

    // If the creature somehow died without telling us,
    if (!object->getVal(attrib_alive)) {
      // Debug String:
      // //Message::add(Grammar::Subject(object)+" has "+object->getVal(stat_hp)+" hit points!");

      // Forget about the object, since we're not being called again:
      object = Target();
      
      // make sure we don't act!
      return true;
      }

   	if ( !object->getTarget(tg_parent) ) {
   		object.save();
   		return true;
   		}
   	// If creature is dead, don't give it another turn:
   	if ( ! object->getVal(stat_hp) ) return true;

   	// Creature is alive and well...
   	Point dir;
   	//Level &lev = object->getlevel();
   	Point3d myLoc = object->getlocation();
   	Path p;

   	// If no current, living target, look about for an enemy:
   	if (!enemy || !enemy->getVal(attrib_alive) ) {
   		//List<Target> &mons = object->getlevel().creatureList();
      List<Target> &mons = Map::creatureList();

   		// Search the level for a player:
   		if (mons.reset())
   		do {

         // If we can assault it unprovoked:
         if (object->capable(aAssault,mons.get())
         // and we notice it: (Todo: take into account visibility of target)
   			&& object->test(sk_awar,
                        // Difficulty is mon's stealth + distance from mon
                        mons.get()->getSkill(sk_stea)
                        + object->getlocation().delta( mons.get()->getlocation() ).abs().axisSum(),
                        mons.get() ) >= result_pass ) {
   				// find out if we can see him:
   				p.set( myLoc, mons.get()->getlocation() );
   				p.calculate();

   				if ( p.hasLOS() ) {

   					enemy = mons.get();
             // change to a check for tile illumination only:
             if (object->getVal(attrib_visible))
     					enemy->perform( aWarn, Target(THIS) );

               object->perform(aAssault,enemy);
   //          if (object->getVal(attrib_visible)) {
   //  	  			String subject = Grammar::Subject(object,Grammar::det_definite);
   //	    			Message::add( subject + " hungrily look"+( getVal(attrib_plural)?"":"s" ) +" at "+ Grammar::Object(enemy,Grammar::det_definite) + "." );
   //            }

   					enLoc = enemy->getlocation();
   					break;
   					}

   				}

   			} while (mons.next());

   		}

   	else {
      // We have a target.  Try to wield a weapon:
      if (object->doPerform(aWield)) {
        // Wield was successful, so our turn is over:
  			ActionEvent::create( Target(THIS), aAct, GRANULARITY / object->getVal(stat_spd) );
        return true;
        }
      
      // First, ensure that if we point to an old target that the
      // target is on a loaded level:
      if ( Map::get( Map::LevScale( enemy->getlocation() ) ).inMemory() ) {
        // If we already had a target and it is still visible,
     		// remember the square it is in as the most recent location.
     		p.set( myLoc, enemy->getlocation() );
     		p.calculate();
     		if ( p.hasLOS() ) enLoc = enemy->getlocation();
        }
        
   		}

   	// If we have an appropriate target, approach it:
   	if (p.reset()) {

   		dir = Point3d( p.get()-myLoc ).toPoint();
      // approach if possible:
   		if (dir != Point(0,0))
   			return moveDir(dir);
      // otherwise we're in the same location
      else {
        // Find out if we want to eat the target:
        if (object->capable(aEat,enemy))
          object->perform(aEat,enemy);

        }


   		enemy = Target(NULL);

   		}

   	// Otherwise, stumble randomly:

    int tries = 10;
   	do {
   		dir = Point( Random::randint(3)-1, Random::randint(3)-1 );
   		} while ( ( (dir.getx()==0 && dir.gety()==0)
      // but ensure that we don't stumble out of loaded levels:
          || (! Map::limitsRect().contains(Point3d(dir) + myLoc)
               || !Map::get( Map::LevScale(Point3d(dir) + myLoc) ).inMemory())
      // and if we're a shopkeeper, don't stumble out of the shop:
          || ( object->getVal( attrib_shk )
               && Map::getTile(Point3d(dir) + myLoc)->getTarget(tg_owner) != object  )
      // and don't loop infinitely
        ) && --tries);

    if (tries == 0) return true;

    return moveDir(dir);


    }

    default:
  	  return Actor::perform( a, t );
    }

	}


bool Brain::moveDir( Point delta ) {

	// Can object leave current location?
	if ( ! object->getTarget(tg_parent)->capable(aRelease,object) ) {
		if (object->getTarget(tg_parent)->istype( tg_trap )) {
			object->perform( aOpen, object->getTarget(tg_parent) );
			ActionEvent::create( Target(THIS), aAct, GRANULARITY / object->getVal(stat_spd) );
			return true;
			}

		Message::add( Grammar::Subject(object,Grammar::det_definite) + ((object->quantity()>1)?" are":" is")+" stuck!" );
		ActionEvent::create( Target(THIS), aAct, GRANULARITY / object->getVal(stat_spd) );
		return true;
		}


	Point3d loc = object->getlocation(), dir = Point3d(delta);
	int i;

	for (i = 0; i < NUM_ABORT/5; ++i)

//	if ( object->getlevel().getBorder().contains( loc + dir )
	if ( Map::limitsRect().contains( loc + dir )
    && dir != Point3d(0,0,0)) {
		Target newLoc = Map::getTile( loc+dir );
		if ( newLoc->capable(aContain,object) ){
			newLoc->perform( aContain, object );
			ActionEvent::create( Target(THIS), aAct, GRANULARITY / object->getVal(stat_spd) );
			return true;
			}
		else {
			Action autoAct = newLoc->autoAction();

			if (autoAct != aNone) {
				newLoc->perform( autoAct, object );
        // If no event yet exists for the brain's next turn:
        if ( ! ActionEvent::exists( Target(THIS), aAct ) )
          // Create another event for the next movement turn:
				  ActionEvent::create( Target(THIS), aAct, GRANULARITY / object->getVal(stat_spd) );
				return true;
				}
			}

		if ( Random::randbool() )
			if (delta.getx() == 0) dir.setx( (Random::randint(2)?-1:1) );
			else dir.setx(0);

		else
			if (delta.gety() == 0) dir.sety( (Random::randint(2)?-1:1) );
			else dir.sety(0);

		}

	ActionEvent::create( Target(THIS), aAct, GRANULARITY / object->getVal(stat_spd) );
	return true;

	}

Brain::Brain( std::iostream &inFile, TargetValue *TVP )
: Actor( inFile, TVP ) {

	// read Target enemy
	inFile >> enemy;

	// read Point enLoc
	inFile.read( (char*) &enLoc, sizeof(enLoc) );
	}

void Brain::toFile( std::iostream &outFile ) {
	Actor::toFile( outFile );

	// write Target enemy
	outFile << enemy;

	// write Point enLoc
	outFile.write( (char*) &enLoc, sizeof(enLoc) );

	}


//-[ class WalkDir ]----------


WalkDir::WalkDir( const Option *o )
: Actor( o ), dir(o->dir), num_steps(0) {
	}

Target WalkDir::create( Option &o ) {

	WalkDir *walkDir = new WalkDir( &o );

	TargetValue *THIS = new TargetValue( walkDir );

	walkDir->setTHIS(THIS);

	return Target( THIS );

	}

targetType WalkDir::gettype( void ) const {
	return tg_walkdir;
	}

bool  WalkDir::istype( targetType t ) const {
	if ( t == tg_walkdir ) return true;
	else return Actor::istype(t);
	}

bool WalkDir::perform( Action a, const Target &t ) {

  Target retain = Target(THIS);

	if ( a == aWarn ) {

    // Todo: Stop running
    detach();
    return true;
		}

	else if ( a != aAct ) return Actor::perform( a, t );

  // Run in direction 'dir':

  //Target parent = getTarget(tg_parent);
  assert( parent != NULL );
  // Error::fatal("WalkDir must always have active parent!");
    
	ActionEvent::create( parent.toTarget(), aAct, GRANULARITY / object->getVal(stat_spd) );

  if ( ! moveDir(dir.toPoint()) )
    detach();

  return true;

	}


bool WalkDir::moveDir( Point delta ) {

  // Will stop moving if:
  //  - target cell is not empty
  bool empty = true;
  //  - we've moved enough steps
  if (num_steps++ >= global::options.run_max)
    return false;
  //  - a wall we are walking beside becomes wider
  //
  //         ####
  //     #####
  //       ->@ stop!
  //
  //  - a passageway opens up in a wall we are walking beside
  //
  //          # #
  //     ###### ####
  //        ->@ stop!
  //
  // My theory:  After each turn, check the square immediately
  //   in front of the player for number of orthogonal exits.
  //
  //      e       @ - Player
  //   ->@fe      f - front square
  //      e       e - exits to count
  //
  //   If the number of orthogonal exits differs between the current
  //   square and the next square, stop running.
  

	// Can object leave current location?
	if ( ! object->getTarget(tg_parent)->capable(aRelease,object) ) {
		Message::add( Grammar::Subject(object,Grammar::det_definite) + (object->getVal(attrib_plural)?" are":" is")+" stuck!" );
		return false;
		}

	Point3d loc = object->getlocation();

	if ( Map::limitsRect().contains( loc + Point3d(delta) ) ) {
		Target newLoc = Map::getTile( loc + Point3d(delta) );

    // If the target tile blocks entry, check to see if either
    // left 45deg or right 45deg square is empty:
    //
    //  ######? < check me
    //    -> @#
    //  ######? < and me
    //
    // Mainly used for skipping the corner square in
    // a right-angle corner:
    //
    //   ##### When @ gets to loc 4, the square
    //   @23 # directly in front,
    //   ###4#
    //     #5#<- ie. this one, blocks us so we check
    //
    // the 45deg left and 45deg right squares.
    //

    if ( !newLoc->capable(aContain,object) ) {

      int newx = delta.getx(), newy = delta.gety();
      Point3d leftDelta, rightDelta;

      if (delta.getx()!=delta.gety())
        newx += delta.gety();
      if (delta.getx()!=-delta.gety())
        newy -= delta.getx();
      leftDelta.set( newx, newy, 0 );

      newx = delta.getx(); newy = delta.gety();

      if (delta.getx()!=-delta.gety())
        newx -= delta.gety();
      if (delta.getx()!=delta.gety())
        newy += delta.getx();
      rightDelta.set( newx, newy, 0 );

      bool lEmpt = false, rEmpt = false,
           coor = (
                  Map::limitsRect().contains( Point3d(delta.gety(),-delta.getx(),0)+loc )&&
                  Map::limitsRect().contains( Point3d(-delta.gety(),delta.getx(),0)+loc )&&
                  !Map::getTile( Point3d(delta.gety(),-delta.getx(),0)+loc)->capable(aContain,object) &&
                  !Map::getTile( Point3d(-delta.gety(),delta.getx(),0)+loc)->capable(aContain,object)
                  );

      if (coor) {

        if (Map::limitsRect().contains( loc + leftDelta )) {
          if ( Map::getTile( loc+leftDelta )->capable(aContain,object))
            lEmpt = true;
          }

        if (Map::limitsRect().contains( loc + rightDelta )) {
          if ( Map::getTile( loc+rightDelta )->capable(aContain,object))
            rEmpt = true;
          }

        if (!(rEmpt&&lEmpt)) {

          if (rEmpt) {
            newLoc = Map::getTile( loc+rightDelta );
            dir.reset();
            dir[rightDelta.toPoint()] = true;
            }
        
          if (lEmpt) {
            newLoc = Map::getTile( loc+leftDelta );
            dir.reset();
            dir[leftDelta.toPoint()] = true;
           }

          }
      
        }
    
      }

    empty = newLoc->getVal(attrib_empty_nodoor);
    
		if ( newLoc->capable(aContain,object) ){
			newLoc->perform( aContain, object );
			if (empty) {

        //Level &lev = object->getlevel();
        //Rectangle Map::limitsRect() = Map::getMap::limitsRect()();
        loc += Point3d(delta);
        int currExit = 0, nextExit = 0;

        //count current orthogonal exits:

        Point3d pForward = Point3d(delta)+loc,
                pLeft =  Point3d(delta.gety(),-delta.getx(),0)+loc,
                pRight = Point3d(-delta.gety(),delta.getx(),0)+loc;

        currExit = (Map::limitsRect().contains(pForward)?Map::getTile(pForward)->getVal(attrib_non_wall):0)
                 + (Map::limitsRect().contains(pRight)?Map::getTile(pRight)->getVal(attrib_non_wall):0)
                 + (Map::limitsRect().contains(pLeft)?Map::getTile(pLeft)->getVal(attrib_non_wall):0);
        
        //count future orthogonal exits:

        pForward += Point3d(delta);
        pLeft += Point3d(delta);
        pRight += Point3d(delta);

        nextExit = (Map::limitsRect().contains(pForward)&&Map::getTile(pForward-Point3d(delta))->getVal(attrib_non_wall)?Map::getTile(pForward)->getVal(attrib_non_wall):0)
                 + (Map::limitsRect().contains(pRight)?Map::getTile(pRight)->getVal(attrib_non_wall):0)
                 + (Map::limitsRect().contains(pLeft)?Map::getTile(pLeft)->getVal(attrib_non_wall):0);

        // Run around corners: (incomplete, Todo: finish it)
        // This section deals with skipping the corner square.
        //
        //  ##### <- This corridor    #####
        //  @   # will be taken       123 #
        //  ### # in this sequence -> ###4#
        //    # #                       #5#
        
        if (nextExit == 1
        && (Map::limitsRect().contains(Point3d(delta.gety(),-delta.getx(),0)+loc)&&!Map::getTile(Point3d(delta.gety(),-delta.getx(),0)+loc)->getVal(attrib_non_wall) )
        && (Map::limitsRect().contains(Point3d(-delta.gety(),delta.getx(),0)+loc)&&!Map::getTile(Point3d(-delta.gety(),delta.getx(),0)+loc)->getVal(attrib_non_wall) ) ) {
          Point newdir = delta;
          if (Map::limitsRect().contains( pLeft)?Map::getTile( pLeft)->getVal(attrib_non_wall):0)
            newdir += Point(delta.gety(),-delta.getx());
          else
          if (Map::limitsRect().contains(pRight)?Map::getTile(pRight)->getVal(attrib_non_wall):0)
            newdir += Point(-delta.gety(),delta.getx());

          dir.reset();
          dir[newdir] = true;
          
          }

        if (nextExit <= currExit || currExit == 0)
          return true;

        return false;

        }
      else return false;
			}
		else
			return false;
		}

	return false;

	}

WalkDir::WalkDir( std::iostream &inFile, TargetValue *TVP )
: Actor( inFile, TVP ) {

	// read Dir dir
	inFile.read( (char*) &dir, sizeof(dir) );
	}

void WalkDir::toFile( std::iostream &outFile ) {
	Actor::toFile( outFile );

	// write Dir dir
	outFile.write( (char*) &dir, sizeof(dir) );

	}


#endif // ACTOR_CPP

