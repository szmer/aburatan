// [random.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( RANDOM_HPP )
#define RANDOM_HPP

#include "enum.hpp"
#include "string.hpp"
#include "target.hpp"
#include "rectngle.hpp"

class Random {
	public:
		static void randomize( void ); // Seed the RNG
		static int randint( int n ); // returns a random number 0 < r < n
		static bool randbool( void ); // returns a true/false
    static long randbit( long ); // Given a bitmask, returns a single random bit
    static Point randpoint( Rectangle ); // a random point from within the rect (border inc.)
    static Point borderpoint( Rectangle ); // a random point from the edge
    static String name( bool, Theme=th_none ); // returns a random, thematic name
    static String surname( Theme=th_none ); // returns a random, thematic surname
    static String townname( Theme=th_none ); // returns a random, thematic town name
    static String fortune( void ); // Returns a random fortune
    static String epitaph( void ); // Returns a random epitaph
    static String graffiti( void ); // Graffiti text
    static Target shopitem( Shop );
    static Target questitem( void );
    static Target themeitem( Theme=th_all );
  private:
    static String pickfrom( const char** ); // returns a random string from a list
	};


#endif // RANDOM_HPP
