// [timeline.cpp]
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
#if !defined ( TIMELINE_CPP )
#define TIMELINE_CPP

#include "timeline.hpp"
#include "file.hpp"
#include "visuals.hpp"
#include "global.hpp"
#include "player.hpp"

/*
#include "compiler.hpp"
#ifdef OBSOLETE_INCLUDE
#  include <dos.h>
#else
#  include <dos>
#endif
*/

unsigned long Timeline::time = START_TIME;

Sorted<ev_Handle> Timeline::ev_list;

void Timeline::reset( void ) {
	time = START_TIME;
	}

#define HOUR_DAWN   6  // Dawn  6am - 7am
#define HOUR_DAY    7  // Day   7am - 5pm
#define HOUR_DUSK  17  // Dusk  5pm - 6pm
#define HOUR_NIGHT 18  // Night 6pm - 6am

TimeOfDay Timeline::timeOfDay( unsigned long l ) {
// Return a vague indication of the time of day
  long todayPart = l % TURN_PER_DAY,
       hour = (todayPart / TURN_PER_HOUR);

  if ( hour < HOUR_DAWN || hour >= HOUR_NIGHT )
    return tod_night;
  if ( hour < HOUR_DAY )
    return tod_dawn;
  if ( hour < HOUR_DUSK )
    return tod_day;

  return tod_dusk;

  }


String Timeline::timeAt( unsigned long l ) {
// Convert a long int (number of turns) into a time string

  String timeStr;

  long todayPart = l % TURN_PER_DAY,
       minute = (todayPart % TURN_PER_HOUR) / TURN_PER_MINUTE,
       hour = (todayPart / TURN_PER_HOUR);

  // Todo: Option for 24hr military time
  // Todo: Should soldiers always use military time?
//  if (Options::hr24)
//    timeStr = String(hour<10?"0":"") + (long)hour + (minute<10?"0":"") + String(minute) + "hrs";
//  else
    timeStr = String((hour%12)?hour%12:12) + (minute<10?":0":":") + String(minute) + (hour<12?"am":"pm");

  return timeStr;

  }

struct DateRec {
  const char *month;
  int numDays;
  } months [12] = {

  {"January",   31 },
  {"February",  28 },
  {"March",     31 },
  {"April",     30 },
  {"May",       31 },
  {"June",      30 },
  {"July",      31 },
  {"August",    31 },
  {"September", 30 },
  {"October",   31 },
  {"November",  30 },
  {"December",  31 } };

String Timeline::dateAt( unsigned long l ) {
// Convert a long int (number of turns) into a date string

  String dateStr;

  long gameDay = l / TURN_PER_DAY + START_DAY,
       month = 0;

  bool done = false;

  do {

    if (gameDay < months[month].numDays)
      done = true;
    else {
      gameDay -= months[month].numDays;
      if (++month > 11)
        month = 0;
      
      }
  
    } while (!done);

  dateStr = String(months[month].month) + " " + String(gameDay+1);
  
  return dateStr;
  }

void Timeline::run( void ) {
	unsigned long current;
  TimeOfDay current_tod = tod_invalid;

	while ( ev_list.reset() ) {
		ev_Handle event = ev_list.remove();
		current = event->gettime();
		if (time < current) {
			time = current;
      // Check to see if the sun must rise/set:
      if (current_tod != timeOfDay(time)) {
        if (current_tod != tod_invalid)
          Player::get().setDayTime(timeOfDay(time));
        current_tod = timeOfDay(time);
        }
      }

    Screen::hidecursor();
		Screen::locate(60,map_bottom+3);
    String tod_str;
    if (Player::get().capable(aTellTime,Target()))
	  	tod_str = timeAt(time);
    else switch ( timeOfDay(time) ) {
      case tod_night: tod_str = "Night"; break;
      case tod_dawn:  tod_str = "Dawn"; break;
      case tod_day:   tod_str = "Day";  break;
      case tod_dusk:  tod_str = "Dusk"; break;
      case tod_invalid: { assert(0); }
      }
 		Screen::write( dateAt(time)+"  "+tod_str );
    //Screen::write( String("t:")+(long)time );
    Screen::clearline();
    Screen::showcursor();

    event->occur();
		if ( global::quit != global::quit_false ) break;
		}
	}

void Timeline::load( void ) {

	File file;

	String filename = SAVEDIR+"timeline.dat";

	std::fstream &myFile = file[filename];

  myFile.read( (char*) &time, sizeof(time) );

	int numitems;
	myFile.read( (char*) &numitems, sizeof(numitems) );

	for ( int count = 0; count < numitems; count++ ) {

		EventType type;
		myFile.read( (char*) &type, sizeof( type ) );

		switch (type) {
			case ev_action: ActionEvent::load( myFile ); break;
			case ev_skill: SkillLevelEvent::load( myFile ); break;

			default: { assert(0); }
			}

		}
	}

void Timeline::save( void ) {

	File file;

	String filename = SAVEDIR+"timeline.dat";

	std::fstream &myFile = file[filename];

  myFile.write( (char*) &time, sizeof(time) );

	int numitems = ev_list.numData();
	myFile.write( (char*) &numitems, sizeof(numitems) );

	while ( ev_list.reset() ) {
		ev_Handle temp = ev_list.remove();

		EventType type = temp->type();
		myFile.write( (char*) &type, sizeof( type ) );
		temp->save( myFile );

		}

	}


void Timeline::add(ev_Handle e) {
	ev_list.add(e);
	}

unsigned long Timeline::now( void ) {
	return time;
	}


#endif // TIMELINE_CPP
