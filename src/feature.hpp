// [feature.hpp]
// (C) Copyright 2000 Michael Blackney
#if !defined ( FEATURE_HPP )
#define FEATURE_HPP

#include "rawtargt.hpp"
#include "rectngle.hpp"
#include "level.hpp"
#include "name.hpp"

class Feature : public RawTarget {

	public:

    struct Option : public RawTarget::Option {
      Rectangle border;
      Level *level;
      Target owner;
      Option( void ) : border(0,0,0,0), level(NULL), owner(NULL) {}
      };

		virtual void toFile( std::iostream & );

		virtual Target clone( void );
		virtual bool equals( const Target& );

		virtual targetType gettype( void ) const = 0;
		virtual bool  istype( targetType t ) const = 0;

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

		virtual Image menuletter();
		virtual void setletter( char i );

		virtual Level &getlevel( void );

		virtual Point3d getlocation( void );
		virtual Rectangle getborder( void );

    virtual Target getTarget( targetType );
    
		virtual long getVal( StatType );
    virtual bool getStruct( structType, void*, size_t );

    virtual void connectAll( Level * );
    
	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		Rectangle border;
		char letter;
    Name f_name;

		Point3d levelloc;

    Target owner;

		Feature( const Option* );
		Feature( Feature & );
		Feature( std::iostream&, TargetValue *TVP );



	};




#endif // FEATURE_HPP
