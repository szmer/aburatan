// [money.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( MONEY_HPP )
#define MONEY_HPP

#include "enum.hpp"
#include "diceroll.hpp"
#include "item.hpp"
#include "string.hpp"

class Money : public Item {

	public:

    struct Option : public Item::Option {
      materialClass material;
      Option( void ) : material(m_gold) {
        image.val.appearance='$';
        }
      };

    METHOD_create_option(Money);
    METHOD_create_default(Money);
    
		static Money *load( std::iostream &io, TargetValue *TVP ) {
			return new Money(io, TVP);
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

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		materialClass material;

		Money( const Option* );
		Money( Money & );
		Money( std::iostream&, TargetValue *TVP );



	};

#endif // MONEY_HPP

