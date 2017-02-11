// [fluid.hpp]
// (C) Copyright 2003 Michael Blackney
#if !defined ( FLUID_HPP )
#define FLUID_HPP

#include "item.hpp"


enum Thirst {

  dr_water = 0x001,
  dr_alco  = 0x002,
  dr_filth = 0x004,
  dr_blood = 0x008,

  dr_milk  = 0x010,
  dr_oil   = 0x020,
  dr_chem  = 0x040,
  dr_alien = 0x080,

  dr_none  = 0x000,
  dr_all   = 0xFFF

  };


class Fluid : public Item {

	public:

    struct Option : public Item::Option {
      materialClass material;
      Option( void ) : material(m_invalid) {}
      };

    METHOD_create_option(Fluid);
    METHOD_create_default(Fluid);
    
		static Fluid *load( std::iostream &ios, TargetValue *TVP ) {
			return new Fluid(ios, TVP);
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

		virtual Attack getAttack(void);
		virtual void takeDamage( const Attack& );

    // Todo: once chemistry has been properly fleshed-out:
    //virtual void identify( IDmask );
    //virtual bool identified( IDmask );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream consuctor and toFile.
		materialClass f_material;

		Fluid( const Option* );
		Fluid( Fluid & );
		Fluid( std::iostream&, TargetValue *TVP );



	};





#endif // FLUID_HPP

