// [temp.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( TEMP_HPP )
#define TEMP_HPP

#include "rawtargt.hpp"

class Temp : public RawTarget {

	public:

    struct Option : public RawTarget::Option {
      String str1;
      String str2;
      long temp_val;
      Target target;
      
      Option( void ) : str1(), str2(), temp_val(0), target() {}
      Option( Image i, String s ) : str1(s), str2(), temp_val(0), target() {
        image = i;
        }
      };

    METHOD_create_option(Temp);
    
		virtual void toFile( std::iostream & );

		virtual Target clone( void );
		virtual bool equals( const Target& );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

		virtual String menustring( char len = 80 );
		virtual String name( NameOption=n_default );

		virtual Image menuletter( void );
		virtual void setletter( char );

    virtual List<Target> *itemList( void );

		virtual long getVal( StatType );

    virtual void setParent( TargetValue * );
    //virtual void   setlocation( const Point3d& );
    //virtual Target findTarget( targetType, const Target& );
    virtual Target getTarget( targetType );
    //virtual Target getowner( void );
		//virtual Target getparent( void );
		virtual void add( const Target & );
		virtual bool remove( const Target & );

		virtual int quantity( void );
		virtual Target detach( int = -1 );

    long getSkill( Skill, bool = true );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		String str1, str2;
    long temp_val;
    
    Target targ;

		Temp( const Option *o );
		Temp( Temp & );
		Temp( std::iostream&, TargetValue *TVP );



	};

#endif // TEMP_HPP
