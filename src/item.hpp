// [item.hpp]
// (C) Copyright 2002 Michael Blackney
#if !defined ( ITEM_HPP )
#define ITEM_HPP

#include "string.hpp"
#include "types.hpp"
#include "enum.hpp"
#include "target.hpp"
#include "rawtargt.hpp"
#include "visuals.hpp"

long    Dollars( long );
long      Cents( long );
long   CentP100( long );

long WholeDollars( long );
long   WholeCents( long );
long    PartCents( long );
String   PriceStr( long, bool round_up=true );

enum Appetite {

  // Bitflags:
  ap_herbmeat= 0x001, // Eats herbivore meat - humans often like to
  ap_carnmeat= 0x002, // Eats carnivore meat - humans don't often like to
  ap_carrion = 0x004, // Eats putrifying meat, eg Vultures & insects like to
  ap_sweet   = 0x008, // Eats sweets & sweet foods
  
  ap_fruit   = 0x010,
  ap_veg     = 0x020,
  ap_grain   = 0x040,
  ap_grass   = 0x080, // Cattle
  
  ap_fabric  = 0x100, // Moths
  ap_wood    = 0x200, // Termites
  ap_alien   = 0x400,
  ap_cannibal= 0x800,

  // Bitmasks:
  ap_none    = 0x000,
  
  ap_allmeat = ap_carnmeat|ap_herbmeat|ap_carrion,
  ap_carn    = ap_carnmeat|ap_herbmeat,
  ap_herb    = ap_fruit|ap_veg|ap_grain,

  ap_omni    = ap_herbmeat|ap_herb|ap_sweet,
  
  ap_bird    = ap_grain,
  
  ap_all     = 0xFFF

  };


class Item : public RawTarget {

	public:

    struct Option : public RawTarget::Option {
      char letter;
      Option( void ) : letter(0) {}
      };
      
		virtual ~Item();
		virtual void toFile( std::iostream & ) = 0;

		virtual Image menuletter();
		virtual void setletter( char i );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

		virtual long getVal( StatType );
    virtual long getSkill( Skill, bool = true );

		virtual Attack getAttack( void );
		virtual void takeDamage( const Attack& );

		virtual bool equals( const Target& ) = 0;
    
		virtual Target detach( int = -1 );
    //virtual void   setParent( const Target &t );
    virtual void   setParent( TargetValue *t );
    //virtual void   setlocation( const Point3d& );
    virtual Target getTarget( targetType type );
    //virtual Target getowner( void );
    //virtual Target getdeliverer( void );

		virtual String name( NameOption=n_default );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		char letter;
		bool still;
    //char quality;
    Target controller; // Who last had control of this item?
    Target owner;      // Who legally owns this item?

		// Constructor is protected so you can't create an Item on the stack.
		Item( const Option* );
		Item( std::iostream&, TargetValue *TVP );
		Item( Item& );

//    virtual void allocInventory( void );

	};


#endif // ITEM_HPP
