// [quantity.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( QUANTITY_HPP )
#define QUANTITY_HPP

#include "rawtargt.hpp"

class Quantity : public RawTarget {

	public:

    struct Option : public RawTarget::Option {
      int quantity;
      Target target;
      Option( void ) : quantity(2), target() {}
      };

    METHOD_create_option(Quantity);
    
		static Quantity *load( std::iostream &ios, TargetValue *TVP ) {
			return new Quantity(ios, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual compval compare( RawTarget& );
		virtual bool equals( const Target& );
		virtual Target clone( void );
		virtual int quantity( void );
		virtual Target detach( int = -1 );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

    virtual void identify( IDmask );
    virtual bool identified( IDmask );

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

		virtual String name( NameOption=n_default );
		virtual Image menuletter( void );
		virtual void setletter( char );
		virtual Image getimage( void );

		virtual long getVal( StatType );
		virtual void takeDamage( const Attack& );

    //virtual Target getowner( void );
    //virtual Target getdeliverer( void );
    virtual Target getTarget( targetType );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		int number;

		Quantity( const Option* );
		Quantity( Quantity & );
		Quantity( std::iostream&, TargetValue *TVP );



	};


#endif // QUANTITY_HPP
