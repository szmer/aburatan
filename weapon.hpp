// [weapon.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( WEAPON_HPP )
#define WEAPON_HPP

#include "item.hpp"
#include "attack.hpp"
#include "ammo.hpp"
#include "widget.hpp"

class Weapon : public Buildable {

	public:

		virtual void toFile( std::iostream & );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

		virtual bool equals( const Target& );

		virtual String name( NameOption=n_default );
    
	protected:

		Weapon( const Option *o );
		Weapon( Weapon & );
		Weapon( std::iostream&, TargetValue *TVP );

	};


class Ranged : public Weapon {

	public:

    struct Option : public Item::Option {
      char form;
      AmmoType ammo;
      Theme theme;
      Option( void ) : form(0), ammo(bul_none), theme(th_all) {
        image=Image(cGrey,'}');
        }
      };

    METHOD_create_option(Ranged);
    METHOD_create_default(Ranged);

		static Ranged *load( std::iostream &ios, TargetValue *TVP ) {
			return new Ranged(ios, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual bool equals( const Target& );
		virtual Target clone( void );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

		virtual String name( NameOption=n_default );

		virtual long getVal( StatType );
    virtual long getSkill( Skill, bool = true );

    virtual void identify( IDmask );
    virtual bool identified( IDmask );

    virtual Target getTarget( targetType );

    virtual bool getStruct( structType, void*, size_t );
    
	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		char form;

		Ranged( const Option* );
		Ranged( Ranged& );
		Ranged( std::iostream&, TargetValue *TVP );

	};


// enum MeleeType
// Categories of melee weapons.
enum MeleeType {

	mel_none,

  mel_brawl,  // Makeshift weapons each with their own style
	mel_knife,
	mel_sword,
	mel_haft,   // Hafted - axes and hammers etc.
	mel_club,
	mel_artic,  // Articulated weapons - flails etc.
	mel_pole,

	mel_total,
  mel_any = mel_total
  
	};

class Melee : public Weapon {

	public:

    struct Option : public Item::Option {
      char form;
      MeleeType type;
      Theme theme;
      Option( void ) : form(0), type(mel_none), theme(th_all) {
        image=Image(cGrey,')');
        }
      };


    METHOD_create_option(Melee);
    METHOD_create_default(Melee);

		static Melee *load( std::iostream &ios, TargetValue *TVP ) {
			return new Melee(ios, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual bool equals( const Target& );
		virtual Target clone( void );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

		virtual String name( NameOption=n_default );

		virtual long getVal( StatType );
    virtual long getSkill( Skill, bool = true );
    virtual bool getStruct( structType, void*, size_t );

		virtual Attack getAttack( void );
    //virtual void   setlocation( const Point3d& );
    //virtual Target findTarget( targetType, const Target& );

    virtual void identify( IDmask );
    virtual bool identified( IDmask );
    
	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		char form;

		Melee( const Option *o );
		Melee( Melee & );
		Melee( std::iostream&, TargetValue *TVP );

	};


#endif // WEAPON_HPP
