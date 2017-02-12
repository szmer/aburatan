// [ammo.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( AMMO_HPP )
#define AMMO_HPP

#include "enum.hpp"
#include "item.hpp"


enum AmmoType {
// Types of ammunition used in ranged weapons.

	bul_none,

	bul_lpistol,
	bul_hpistol,
	bul_rifle,
	bul_shotgun,
  bul_harpoon,
  bul_needle,
  bul_grenade,
  bul_arrow,

	bul_total
	};

enum AmmoEffect {
// Effects that ammo has when it hits the target:

  ae_none,
  
  ae_explode,
  ae_smoke,
  ae_gas,

  ae_invalid
  };

struct ammoStats {

	const char *name;
	AmmoType type;

  int color;

  long price;
  
  int weight;

	char numslugs;
		char accuracy;

  AmmoEffect effect;

	ammoStats( const char*, AmmoType, int, long, int, char, char, AmmoEffect );

	static ammoStats *AmmoStats;
	static int numAmmo;

	};




class Ammo : public Item {

	public:

    struct Option : public Item::Option {
      char form;
      AmmoType type;
      Option( void ) : form(0), type(bul_none) {
        image=Image(cGrey,'{');
        }
      };
      
    METHOD_create_option(Ammo);
    METHOD_create_default(Ammo);

		static Ammo *load( std::iostream &io, TargetValue *TVP ) {
			return new Ammo(io, TVP);
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
    virtual bool getStruct( structType, void*, size_t );

		virtual Attack getAttack(void);

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		char form;
		bool fired;

		Ammo( const Option *o );
		Ammo( Ammo & );
		Ammo( std::iostream&, TargetValue *TVP );



	};

#endif // AMMO_HPP

