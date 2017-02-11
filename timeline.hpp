// [timeline.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( TIMELINE_HPP )
#define TIMELINE_HPP

#include "event.hpp"
#include "sorted.hpp"

enum TimeOfDay {
  tod_night,
  tod_dawn,
  tod_day,
  tod_dusk,

  tod_invalid
  };

#define SEC_PER_TURN 1
#define SEC_PER_MINUTE 60
#define MIN_PER_HOUR 60
#define HOUR_PER_DAY 24

#define TURN_PER_MINUTE (SEC_PER_MINUTE/SEC_PER_TURN)
#define TURN_PER_HOUR (TURN_PER_MINUTE*MIN_PER_HOUR)
#define TURN_PER_DAY (TURN_PER_HOUR*HOUR_PER_DAY)

#define START_TIME 10
#define START_DAY 0

class Timeline {
	public:
		static void run( void );
		static void load( void );
		static void save( void );
		static void reset( void );

		static unsigned long now( void );

    static String timeAt( unsigned long );
    static String dateAt( unsigned long );

    static TimeOfDay timeOfDay( unsigned long );

	friend class Event;

	private:
		static void add(ev_Handle e);
		static Sorted<ev_Handle> ev_list;

		static unsigned long time;
	};


#endif // TIMELINE_HPP
