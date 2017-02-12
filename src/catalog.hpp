// [catalog.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( CATALOG_HPP )
#define CATALOG_HPP


#include "list.hpp"
#include "target.hpp"
#include "config.hpp"

#include "serial.hpp"

class Catalogue {

	public:

		class cat_Handle {
			public:
				cat_Handle( TargetValue *tvp ) : value(tvp) {}
				cat_Handle( const cat_Handle &chr ) : value(chr.value) {}
				~cat_Handle( void ) {}

				bool operator > ( const cat_Handle& ) const { return false; }
				bool operator < ( const cat_Handle& ) const { return false; }

				bool operator == ( const cat_Handle &chr ) const { return (chr.value == value)? true:false; }
				bool operator == ( long l ) const { return (value->getSerial().toLong() == l)? true:false; }
				TargetValue *operator ->( void ) { return value; }
				cat_Handle &operator = ( const cat_Handle &chr ) { value = chr.value; return *this; }

				friend class Catalogue;

			private:
				TargetValue *value;
			};


		static void add( cat_Handle );
		static bool remove( cat_Handle );

#   ifdef SOFT_LINK
		static void link( cat_Handle );
		static bool unlink( cat_Handle );
#   endif // SOFT_LINK

		static TargetValue *get( long );

		static void clr( void );

		static long numitems( void );
#   ifdef SOFT_LINK
		static long numlinks( void );
#   endif // SOFT_LINK

	private:
		static List<cat_Handle> catalogue;
#   ifdef SOFT_LINK
    static List<cat_Handle> links;
#   endif // SOFT_LINK

	};


#endif // CATALOG_HPP
