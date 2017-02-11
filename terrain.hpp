// [terrain.hpp]
// (C) Copyright 2003 Michael Blackney
#if !defined ( TERRAIN_HPP )
#define TERRAIN_HPP

#include "item.hpp"
#include "name.hpp"

class Grave : public Item {

	public:

    struct Option : public Item::Option {
      bool open;
      Option( void ) : open(false) {
        image=Image( cBrown, '-' );
        }
      };

    METHOD_create_option(Grave);
    METHOD_create_default(Grave);
    
		static Grave *load( std::iostream &ios, TargetValue *TVP ) {
			return new Grave(ios, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual Target clone( void );
		virtual bool equals( const Target& );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

		virtual String name( NameOption=n_default );

		virtual long getVal( StatType );
		virtual void takeDamage( const Attack& );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		bool open;

		Grave( const Option *o );
		Grave( Grave & );
		Grave( std::iostream&, TargetValue *TVP );

	};


class Headstone : public Item {

	public:

    struct Option : public Item::Option {
      Option( void ) {
        image=Image( cGrey, '+' );
        }
      };

    METHOD_create_option(Headstone);
    METHOD_create_default(Headstone);

		static Headstone *load( std::iostream &ios, TargetValue *TVP ) {
			return new Headstone(ios, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual Target clone( void );
		virtual bool equals( const Target& );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );
		virtual Action autoAction( void );

		virtual String name( NameOption=n_default );

		virtual long getVal( StatType );
		virtual void takeDamage( const Attack& );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
    Name message;

		Headstone( const Option * );
		Headstone( Headstone & );
		Headstone( std::iostream&, TargetValue *TVP );

	};


#endif // TERRAIN_HPP
