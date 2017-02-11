// [monster.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( MONSTER_HPP )
#define MONSTER_HPP

#include "creature.hpp"
#include "actor.hpp"
#include "map.hpp"
#include "quest.hpp"

class Monster : public Creature {
	public:
	 // Construction / Destruction:

    struct Option : public Creature::Option {
      CreatureType form;
      Theme theme;
		  Odor odor;
      Map::levType habitat;
      long max_xp;
      Option( void ) : form(crInvalid), theme(th_all),
                       odor(od_invalid), habitat(Map::any), max_xp(LONG_MAX) {}
      };
      
    METHOD_create_br_opt(Monster);
    METHOD_create_br_def(Monster);
    
		static Monster *load( std::iostream &ios, TargetValue *TVP ) {

			return new Monster(ios, TVP);

			}

		virtual Target clone( void );
		virtual void toFile( std::iostream & );
		virtual ~Monster( void );

		virtual targetType gettype( void ) const;
		virtual bool istype( targetType ) const;

		virtual bool alive( void ) const;

		virtual String name( NameOption=n_default );

		virtual void draw( void );

		virtual bool perform( Action, const Target &t );
		virtual void takeDamage( const Attack & );
		virtual long getVal( StatType );
    virtual long getSkill( Skill, bool = true );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		Monster( const Option* );
		Monster( Monster & );
		Monster( std::iostream&, TargetValue *TVP );

	};

enum Job {

  /*
  //--[ Jobs 0.94 ]--//
  job_golfer,
  job_magnate,
  job_journalist,
  job_artist,
  job_clergy,
  job_tourist,
  job_soldier,
  job_eccentric,
  job_servant,
  //-----------------//
  */

  //--[ Jobs 0.95 ]--//
  job_bandit,
  job_freebooter,
  job_desperado,
  job_outlaw,
  job_harrier,

  job_cultist,
  job_prophet,
  
  job_gunslinger,
  job_sheriff,
  job_marshal,

  job_undertaker,

  job_cardshark,
  job_maverick,
  
  job_hillbilly,
  job_redneck,
  job_yokel,
  job_rustic,

  //--[ Jobs 0.95 ]--//
  job_scout,
  job_brave,
  job_warrior,
  job_shaman,
  //-----------------//


  job_librarian,
  job_bartender,
  job_sales,
  job_pharmacist,
  job_stablehand,
  job_blackmarket,

  job_num,
  job_first_shk = job_librarian,
  job_invalid = job_num

  };

class Human : public Creature {
	public:
	 // Construction / Destruction:

    struct Option : public Creature::Option {
      Job job;
      Option( void ) : job(job_invalid) {}
      };

    METHOD_create_br_opt(Human);
    METHOD_create_br_def(Human);
    
    static Human *load( std::iostream &ios, TargetValue *TVP ) {

			return new Human(ios, TVP);

			}

		virtual Target clone( void );
		virtual void toFile( std::iostream & );
		virtual ~Human( void );

		virtual targetType gettype( void ) const;
		virtual bool istype( targetType ) const;

		virtual bool alive( void ) const;

		virtual String name( NameOption=n_default );

		virtual void draw( void );

    virtual bool capable( Action, const Target &t );
    virtual bool perform( Action, const Target &t );
    
		virtual void takeDamage( const Attack & );
		virtual long getVal( StatType );
    virtual long getSkill( Skill, bool = true );

    virtual void setTHIS( TargetValue * );
    //virtual void   setlocation( const Point3d& );
    //virtual Target findTarget( targetType, const Target& );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		Human( const Option* );
		Human( Human & );
		Human( std::iostream&, TargetValue *TVP );

    Job job;
    Target quest;

	};


class Mount : public Creature {
	public:
	 // Construction / Destruction:

    struct Option : public Creature::Option {
      CreatureType form;
      Theme theme;
		  Odor odor;
      Map::levType habitat;
      long max_xp;
      Option( void ) : form(crInvalid), theme(th_all),
                       odor(od_invalid), habitat(Map::any), max_xp(LONG_MAX) {}
      };
      
    METHOD_create_option(Mount);
    METHOD_create_default(Mount);
    
		static Mount *load( std::iostream &ios, TargetValue *TVP ) {

			return new Mount(ios, TVP);

			}

		virtual Target clone( void );
		virtual void toFile( std::iostream & );
		virtual ~Mount( void );

		virtual targetType gettype( void ) const;
		virtual bool istype( targetType ) const;

		virtual bool alive( void ) const;

		virtual String name( NameOption=n_default );

		virtual void draw( void );

    virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );
		virtual void takeDamage( const Attack & );
		virtual long getVal( StatType );
    virtual long getSkill( Skill, bool = true );

    virtual Target getTarget( targetType );
    
	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		Mount( const Option* );
		Mount( Mount & );
		Mount( std::iostream&, TargetValue *TVP );

	};


#endif // MONSTER_HPP
