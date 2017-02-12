// [stairs.hpp]
// (C) Copyright 2003 Michael Blackney
#if !defined ( STAIRS_HPP )
#define STAIRS_HPP

#include "item.hpp"

class Stairs : public Item {

	public:

    struct Option : public Item::Option {
      char connecting;
      Option( void ) : connecting(0) {
        image=Image(cRed,'?');
        }
      };

    METHOD_create_option(Stairs);
    
		static Stairs *load( std::iostream &ios, TargetValue *TVP ) {
			return new Stairs(ios, TVP);
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
    virtual void setParent( TargetValue * );
    //virtual void   setlocation( const Point3d& );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		char connecting;

		Stairs( const Option * );
		Stairs( Stairs & );
		Stairs( std::iostream&, TargetValue *TVP );

	};

#endif // STAIRS_HPP
