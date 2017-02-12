// [widget.hpp]
// (C) Copyright 2003 Michael Blackney
#if !defined ( WIDGET_HPP )
#define WIDGET_HPP

#include "item.hpp"
#include "name.hpp"
#include "bagbox.hpp"
#include "crlist.hpp"

// Widgets encompasses both machine parts and
// monster remains that can be used to improve
// weapons.

enum PartType {

  // Power-ups:
  w_ranged, // an improved firing pin?
  w_spike,  // a spike attachment: + to pierce
  w_stud,   // a set of metal studs: + to bash
            // no simple bonus to cut
            // no simple bonus to chop

  // Currency widgets:
  w_gear,
  w_chainwheel,
  w_piston,
  w_scrap,
  
  // Generic attachments:
  w_chain,    // for chainswords, chainaxes etc. ++chop
  w_buzz,     // for buzzswords, buzzgloves etc. +cut, +chop
  w_spark,    // spark plug, for thunderhammers, shockgloves, etc. +bash, elec
  w_blast,    // blasting cap, for quakehammers, quakeboots, etc.  ++bash
  w_antnomic, // antnomic cylinder, for gammaswords, blightaxes, etc. poison
  w_drill,    // drill bit, for drillspears, heavy drillforks, etc. ++pierce
  w_tesla,    // tesla coil for power swords, arc lances, etc. +cut, elec
  
  // Firearm chambers:
  w_revolve,
  w_magfeed,
  w_breech,
  w_gatling,

  w_num,
  w_invalid
  };

class MachinePart : public Item {

	public:

    struct Option : public Item::Option {
      PartType form;
      Option( void ) : form(w_invalid) {}
      Option( PartType f ) : form(f) {}
      };

    METHOD_create_option(MachinePart);
    METHOD_create_default(MachinePart);
    
		static MachinePart *load( std::iostream &ios, TargetValue *TVP ) {
			return new MachinePart(ios, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual Target clone( void );
		virtual bool equals( const Target& );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

		virtual String name( NameOption = n_default );

		virtual long getVal( StatType );
    virtual long getSkill( Skill, bool = true );
    virtual bool getStruct( structType, void*, size_t );

		virtual Attack getAttack( void );

    virtual void identify( IDmask );
    virtual bool identified( IDmask );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
    PartType form;

		MachinePart( const Option *o );
		MachinePart( MachinePart & );
		MachinePart( std::iostream&, TargetValue *TVP );

	};


class Remains : public Item {

	public:

    struct Option : public Item::Option {
      CreatureType form;
      Option( void ) : form(crInvalid) {}
      Option( CreatureType f ) : form(f) {}
      };

    METHOD_create_option(Remains);
    METHOD_create_default(Remains);
    
		static Remains *load( std::iostream &ios, TargetValue *TVP ) {
			return new Remains(ios, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual Target clone( void );
		virtual bool equals( const Target& );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

		virtual String name( NameOption = n_default );

		virtual long getVal( StatType );
    virtual long getSkill( Skill, bool = true );
    virtual bool getStruct( structType, void*, size_t );

		virtual Attack getAttack( void );

    virtual void identify( IDmask );
    virtual bool identified( IDmask );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
    CreatureType form;

		Remains( const Option *o );
		Remains( Remains & );
		Remains( std::iostream&, TargetValue *TVP );

	};


class Buildable : public Item {

	public:

		virtual void toFile( std::iostream & );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

		virtual bool equals( const Target& );

		virtual String name( NameOption=n_default );

    virtual void gainExp( long );
    
	protected:
    long exp;
    char quality;
    char combo;
    Target blade;

		Buildable( const Option *o );
		Buildable( Buildable & );
		Buildable( std::iostream&, TargetValue *TVP );

	};



#endif // WIDGET_HPP
