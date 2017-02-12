// [trap.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( TRAP_HPP )
#define TRAP_HPP

#include "item.hpp"

class Trap : public Item {

	public:
    struct Option : public Item::Option {
      bool shut;
      Option( void ) :shut(false) {
        image=Image(cRed, '^');
        }
      };

    METHOD_create_option(Trap);
    METHOD_create_default(Trap);

		static Trap *load( std::iostream &ios, TargetValue *TVP ) {
			return new Trap(ios, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual Target clone( void );
		virtual bool equals( const Target& );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

		virtual Action autoAction( void );
		virtual Action exitAction( void );

		virtual Image menuletter();
		virtual String name( NameOption=n_default );

		virtual Image getimage( void );

		virtual long getVal( StatType );
		virtual void takeDamage( const Attack& );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		bool shut;

		Trap( const Option *o );
		Trap( Trap & );
		Trap( std::iostream&, TargetValue *TVP );



	};


#endif // TRAP_HPP
