// [event.cpp]
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
#if !defined ( EVENT_CPP )
#define EVENT_CPP

#include "event.hpp"
#include "timeline.hpp"
#include "player.hpp"
#include "message.hpp"

//#define EVENT__HARNESS

std::iostream &operator << ( std::iostream &ios, Event &e ) {

	e.save(ios);
	return ios;
	}


//[ class Event ]--------------

Event::Event( long t )
: time(t + Timeline::now() ) {
	Timeline::add( this );
	}

Event::Event( std::iostream &ios ) {

	ios.read( (char*) &time, sizeof(time) );

	Timeline::add( this );
	}

void Event::save( std::iostream &ios ) {

	ios.write( (char*) &time, sizeof(time) );
	}


bool Event::operator > ( const Event &e ) {
	return ( time > e.time )? true:false;
	}
bool Event::operator < ( const Event &e ) {
	return ( time < e.time )? true:false;
	}

long Event::gettime( void ) {
	return time;
	}

Sorted<ev_Handle> &Event::getTimeline( void ) {
	return Timeline::ev_list;
	}


//[ class ActionEvent ]--------------

ActionEvent::ActionEvent( const Target &t, Action a, long t_v )
: Event(t_v),
	action(a),target(t) { }

ActionEvent::ActionEvent( std::iostream &ios )
:	Event(ios) {

	// Read target
	ios >> target;

	// Read actor
	ios.read( (char*) &action, sizeof( action ) );


	}

void ActionEvent::create( const Target &t, Action a, long time ) {
	new ActionEvent( t, a, time );
	}

long ActionEvent::remove( const Target &t, Action a ) {
// Removes all actionevents with same parameters as given,
// returns the event time of the last removed event:

	long retval = 0;
  
	Sorted<ev_Handle> &timeLine = Event::getTimeline();

	if (timeLine.reset())
	do {

		Event *ev = &timeLine.get();
		if ( ev->type() == ev_action ) {
			// type is ok; right to cast:
			if (  ((ActionEvent*)ev)->target == t
				 && (  a == aAll || ((ActionEvent*)ev)->action == a  ) ) {

				 retval = timeLine.remove()->gettime();
				 }
			}

		} while ( timeLine.next() );

	return retval;
	}

bool ActionEvent::exists( const Target &t, Action a ) {
// Returns true if actionevent exists for given parameters:

	Sorted<ev_Handle> &timeLine = Event::getTimeline();

	if (timeLine.reset())
	do {

		Event *ev = &timeLine.get();
		if ( ev->type() == ev_action ) {
			// type is ok; right to cast:
			if (  ((ActionEvent*)ev)->target == t
				 && (  a == aAll || ((ActionEvent*)ev)->action == a  ) ) {

				 return true;
				 }
			}

		} while ( timeLine.next() );

	return false;
	}

void ActionEvent::load( std::iostream &ios ) {
	new ActionEvent( ios );
	}

bool ActionEvent::occur( void ) {
  assert (!!target);
	return target->doPerform( action );
	}

void ActionEvent::save( std::iostream &ios ) {

	Event::save(ios);

	// Save target
	ios << target;

	// Save actor
	ios.write( (char*) &action, sizeof( action ) );
	}

EventType ActionEvent::type( void ) {
	return ev_action;
	}

//[ class SkillLevelEvent ]--------------

SkillLevelEvent::SkillLevelEvent( Skill sk, int b, long t_v )
: Event(t_v),
	bonus(b),skill(sk) { }

SkillLevelEvent::SkillLevelEvent( std::iostream &ios )
:	Event(ios) {

	// Read bonus
	ios.read( (char*) &bonus, sizeof( bonus ) );

	// Read skill
	ios.read( (char*) &skill, sizeof( skill ) );


	}

void SkillLevelEvent::create( Skill sk, int b, long time ) {
	new SkillLevelEvent( sk, b, time );
	}


void SkillLevelEvent::load( std::iostream &ios ) {
	new SkillLevelEvent( ios );
	}

bool SkillLevelEvent::occur( void ) {
  Player::get().modSkill( skill,bonus );
  int cur_lev = Player::get().getSkill(skill,false);
  String quantity;
  
  quantity = "some";
  if (cur_lev == 0) quantity = "all";
  else if (bonus > cur_lev) quantity = "much";
  
  Message::add("You forget "+quantity+" of your knowledge of "+String(Skills[skill].name)+".");
	return true;
	}

void SkillLevelEvent::save( std::iostream &ios ) {

	Event::save(ios);

	// Save bonus
	ios.write( (char*) &bonus, sizeof( bonus ) );

	// Save skill
	ios.write( (char*) &skill, sizeof( skill ) );
	}

EventType SkillLevelEvent::type( void ) {
	return ev_skill;
	}

//[ class ev_Handle ]--------------

ev_Handle::ev_Handle( Event *ev )
: obj(ev), master(true) {}

ev_Handle::ev_Handle( const ev_Handle &eh )
: obj(eh.obj), master(true) {
	((ev_Handle&)eh).master = false;
	}

ev_Handle::~ev_Handle( void ) {
	if (master) delptr(obj);
	}

bool ev_Handle::operator > ( const ev_Handle &eh ) const {
	return *obj > *eh.obj;
	}

bool ev_Handle::operator < ( const ev_Handle &eh ) const {
	return *obj < *eh.obj;
	}

bool ev_Handle::operator == ( const ev_Handle &eh ) const {
	return (obj == eh.obj)?true:false;
	}

Event *ev_Handle::operator ->() {
  // Invalid use of null-pointer:
	assert (!!obj);
	return obj;
	}

Event &ev_Handle::operator  *() {
  // Invalid use of null-pointer:
	assert (!!obj);
	return *obj;
	}

Event *ev_Handle::operator  &() {
  // Invalid use of null-pointer:
	assert (!!obj);
	return obj;
	}

ev_Handle &ev_Handle::operator = ( const ev_Handle &eh ) {

	if (master) delptr(obj);

	eh.master = false;
	master = true;

	obj = eh.obj;

	return *this;
	}


#ifdef EVENT__HARNESS

#include "visuals.hpp"
#include "point.hpp"
#include "rectngle.hpp"
#include "enum.hpp"
#include "string.hpp"
#include "counted.hpp"
#include "error.hpp"
#include "random.hpp"
#include "input.hpp"
#include "timeline.hpp"
#include "sorted.hpp"

#include "alloc.h"

class CursorEvent : public Event {
	public:
		virtual bool occur( void ) {
			char key = (char) getch();

			switch ( key ) {
				case 'H' : Screen::locate( Screen::cursorat().decy() ); break;
				case 'K' : Screen::locate( Screen::cursorat().decx() ); break;
				case 'M' : Screen::locate( Screen::cursorat().incx() ); break;
				case 'P' : Screen::locate( Screen::cursorat().incy() ); break;

				}

			return false; // exit == false;
			}

		static void create( long t = 0 ) {
			new CursorEvent(t);
			}

	protected:
		CursorEvent( long t ) : Event(t) {}

	};


class KeyEvent : public Event {
	public:
		virtual bool occur( void ) {

			char key = (char) getch();


			if (key != '\033') KeyEvent::create();

			if (key == '\0') CursorEvent::create();
			else Screen::write( key );

			return false; // exit == false;
			}

		static void create( long t = 1 ) {
			new KeyEvent(t);
			}

	protected:
		KeyEvent( long t ) : Event(t) {}

	};

void mainroutine() {
	KeyEvent::create();

	Timeline::run();
	}

int main() {
	long initialMem = coreleft();

	Random::randomize();

	Screen::clr();

	mainroutine();

	Screen::locate(50, 5);
	Screen::write( "Initial Mem: " );
	Screen::write( (long) initialMem );
	Screen::locate(50, 6);
	Screen::write( "Current Mem: " );
	Screen::write( (long) coreleft() );
	Screen::write("\r\n");

	return 0;
	}

#endif // EVENT__HARNESS

#endif // EVENT_CPP
