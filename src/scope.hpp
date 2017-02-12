// [scope.hpp]
// (C) Copyright 2003 Michael Blackney
#if !defined ( SCOPE_HPP )
#define SCOPE_HPP


#include "item.hpp"


enum ScopeFlags {

  sc_magnif = 0x01,
  sc_telesc = 0x02,
  sc_necro  = 0x04,
  sc_trippy = 0x08,

  sc_all  = 0xFF,
  sc_none = 0x00

  };


class Scope : public Item {

	public:

    struct Option : public Item::Option {
      Option( void ) {
        image=Image(cWhite,'[');
        }
      };

    METHOD_create_option(Scope);
    METHOD_create_default(Scope);

		static Scope *load( std::iostream &io, TargetValue *TVP ) {
			return new Scope(io, TVP);
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

    virtual void identify( IDmask );
    virtual bool identified( IDmask );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		char form;

		Scope( const Option* );
		Scope( Scope & );
		Scope( std::iostream&, TargetValue *TVP );



	};



#endif // SCOPE_HPP
