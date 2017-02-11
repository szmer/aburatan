// [actor.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( ACTOR_HPP )
#define ACTOR_HPP

#include "rawtargt.hpp"
#include "dir.hpp"
#include "path.hpp"
#include "assert.h"

class Actor : public RawTarget {
	public:

    struct Option : public RawTarget::Option {
      Target object;
      long time;
      Option( void ) : object(NULL), time(0) {}
      };
      
		virtual targetType gettype( void ) const = 0;
		virtual bool  istype( targetType ) const = 0;

		virtual void toFile( std::iostream &);

		virtual String name( NameOption=n_default ) { return String(""); }

		virtual Image menuletter( void ) { return 0; }
		virtual void setletter( char ) {}
		virtual Target clone( void ) { assert(0); /* Attempt to clone an Actor! */ return Target(); };
		virtual bool equals( const Target& ) { return false; } // Never combine actors!
		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );
    virtual Target getowner( void );

	protected:
		Actor( const Option* );
		Actor( std::iostream&, TargetValue *TVP );
		virtual ~Actor(void);

		Target object;

	};


class FlightPath : public Actor {
	public:

    struct Option : public Actor::Option {
      Point3d start;
      Point delta;
      Option( void ) : start(), delta() {}
      };

		static Target create( Option &o );
    

		static FlightPath *load( std::iostream &ios, TargetValue *TVP ) {
			return new FlightPath(ios, TVP);
			}

    virtual bool perform( Action, const Target &t );
		virtual void toFile( std::iostream & );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType ) const;

	protected:
		FlightPath( const Option* );
		FlightPath( std::iostream&, TargetValue *TVP );

		Path line;

	};


class Brain : public Actor {
	public:

    struct Option : public Actor::Option {
      };

		static Target create( Option &o );
    
		static Brain *load( std::iostream &ios, TargetValue *TVP ) {
			return new Brain(ios, TVP);
			}

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

		virtual void toFile( std::iostream & );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType ) const;

		bool moveDir( Point );

	protected:
		Brain( const Option* );
		Brain( std::iostream&, TargetValue *TVP );

		Target enemy;
		Point3d enLoc;
	};

class WalkDir : public Actor {
	public:

    struct Option : public Actor::Option {
      Dir dir;
      };
      
		static Target create( Option & );
    
		virtual bool perform( Action, const Target &t );

		static WalkDir *load( std::iostream &ios, TargetValue *TVP ) {
			return new WalkDir(ios, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType ) const;

		bool moveDir( Point );

	protected:
		WalkDir( const Option* );
		WalkDir( std::iostream&, TargetValue *TVP );

    Dir dir;
    int num_steps;
	};

#endif // ACTOR_HPP

