// [door.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( DOOR_HPP )
#define DOOR_HPP

#include "item.hpp"

class Door : public Item {

	public:

    struct Option : public Item::Option {
      materialClass material;
      bool open;
      bool hidden;
      bool gate;
      Option( void ) : material(m_wood), open(false),
                       hidden(false), gate(false) {
        image = Image( Material::data[material].color, (open)?'/':'+' );
        }
      };
      

    METHOD_create_option(Door);
    METHOD_create_default(Door);

		static Door *load( std::iostream &ios, TargetValue *TVP ) {
			return new Door(ios, TVP);
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
		materialClass material;
		bool open, hidden, gate;

		Door( const Option* );
		Door( Door & );
		Door( std::iostream&, TargetValue *TVP );



	};


#endif // DOOR_HPP
