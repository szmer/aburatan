// [clothing.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( CLOTHING_HPP )
#define CLOTHING_HPP

#include "enum.hpp"
#include "diceroll.hpp"
#include "item.hpp"
#include "widget.hpp"

//  Special clothing traits:
#define  cl_none  0x00
    
#define  cl_nose     0x08 // protects from inhaling gas
#define  cl_eyes     0x10 // protects eyes
#define  cl_warm     0x20 // keeps warm
#define  cl_time     0x40 // tells the time
#define  cl_feet     0x80 // covers feet

#define  cl_all  0xFF
  
class Clothing : public Buildable {

	public:

    struct Option : public Item::Option {
      Slot slot;
      Theme theme;
      Gender gender;
      Option( void ) : slot(s_none), theme(th_all), gender(sex_any) {
        image=Image( cGrey, ']' );
        }
      };
      
    METHOD_create_option(Clothing);
    METHOD_create_default(Clothing);

		static Clothing *load( std::iostream &ios, TargetValue *TVP ) {
			return new Clothing(ios, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual Target clone( void );
		virtual bool equals( const Target& );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

		virtual String name( NameOption=n_default );

		virtual compval compare( RawTarget& );

		virtual long getVal( StatType );
    virtual bool getStruct( structType, void*, size_t );
    virtual long getSkill( Skill, bool = true );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		char form;

		Clothing( const Option* );
		Clothing( Clothing& );
		Clothing( std::iostream&, TargetValue *TVP );



	};

#endif // CLOTHING_HPP

