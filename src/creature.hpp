// [creature.hpp]
// (C) Copyright 2000 Michael Blackney
#if !defined ( CREATURE_HPP )
#define CREATURE_HPP
#include "crlist.hpp"
#include "enum.hpp"
#include "viewable.hpp"
#include "target.hpp"
#include "stats.hpp"
#include "string.hpp"
#include "list.hpp"
#include "item.hpp"
#include "map.hpp"
#include "name.hpp"

/* Reputation is local to each town
 * and is influenced by:
 *
 *  - initial impression, i.e. notoriety (mildly) ;
 *  - how you are seen to behave there (strongly) ;
 *  - ongoing notoriety.
 *
 **/
 
#define DEMEANOR_MID_BAND 33

enum Reputation {
  rep_despised = -100,
  rep_neutral  =    0,
  rep_idolised =  100,

  //rep_assault  =  -DEMEANOR_MID_BAND
  rep_invalid
  };

enum Impression {

  imp_none, // Impression not relevant

  imp_assault,  // So hated that seer will attack
  imp_disgust,  // Seer is disgusted by target - no attack but no trade

  imp_tolerate, // Seer will accept the presence of target - neutral reaction

  imp_revere,   // Seer likes/fears target enough to offer light assistance
  imp_venerate, // Seer loves/is terrified of target and will offer greatly

  imp_invalid
  };

/* Notoriety is global and represents your
 * worldly reputation.
 *
 * Notoriety is 2d: axes are Fame and Demeanor.
 *
 * Fame begins at 0 and increases (up to a max
 * of 100) when the player acts heroically or
 * villainously.
 * Fame slowly decreases to 0 over time.
 *
 * Demeanor begins at 0 and ranges between
 * -100 (lawless) and +100 (lawful).  Behaving
 * lawfully increases the value and behaving
 * lawlessly decreases it.
 * The adjustment value is calculated by
 * dividing the fame of the act by the current
 * fame magnitude.  Hence, the more famous a
 * character is, the harder to change public
 * opinion.
 *
 *                            Demeanor: -100..-34    -33..33    34..100
 *             100
 *              -                - 85+: Infamous;   Notorious;  Famous
 *              |
 *  \\\\\\\\\\  |F  ////////     - 60+: Dishonored; Popular;    Honored
 *  \Infamous\  |a  /Famous/
 *  \\\\\\\\\\  |m  ////////     - 35+: Scandalous; Noted;      Eminent
 *              |e
 *              |                - 10+: Shady;      Obscure;    Humble
 *              |
 *  |-----------+-----------|    - 0-10:            Unknown
 *           Demeanor
 * 100          0          100
 *  :           :           :
 *  Lawless  Neutral   Lawful
 *
 **/


struct Notoriety {

  Notoriety( void ) : fame(0), demeanor(0) {}
  Notoriety( int f, int d) : fame(f), demeanor(d) {}
  
  Impression impressionOf( const Notoriety& ) const;
  String toString( void ) const;

  int fame;
  int demeanor;
  };

enum {
  lawless = -100,
  neutral =    0,
  lawful  =  100,

  unknown =     0,
  notorious = 100
  };
  

// Creature combat bonuses/penalties:
#define RANGE_MOD      2  // Minus for every range unit from target
#define RANGE_BONUS   3/2 // Range bonus for the far shot feat
#define TWO_WEAPON     4  // Minus for each hand when twoweaponing
#define UNBALANCED_2ND 2  // Minus for off-hand if twoweaponing with
                          // unbalanced 2nd weapon
#define BACKSTAB       4  // Bonus to backstab hit a peaceful enemy

//////////////////////////
//--[ class Creature ]--//
//////////////////////////

class Creature : public Item {
	public:

    struct Option : public Item::Option {
      Gender gender;
      Option( void ) : gender(sex_any) {}
      };

    // Construction / Destruction:
		virtual ~Creature( void );

		virtual void toFile( std::iostream & );

		// Interface:
		char polymorph( CreatureType );  // Change form.

		const String getname( void ) const;
//		CreatureType getform( void ) const;

		virtual targetType gettype( void ) const;
		virtual bool istype( targetType ) const;

		virtual bool equals( const Target& );
		virtual compval compare( RawTarget& );

		virtual String name( NameOption=n_default );

		virtual Image menuletter();
		virtual void setletter( char ) { }

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );
		virtual Action autoAction( void );

		virtual long getVal( StatType );
    virtual long getSkill( Skill, bool = true );
    virtual Result test( Skill, int, const Target& );
    virtual bool getStruct( structType, void*, size_t );

		virtual void takeDamage( const Attack& );
    virtual void gainExp( long );

    //virtual void   setlocation( const Point3d& );
    virtual void setTHIS( TargetValue * );

    //virtual Target findTarget( targetType, const Target& );
    virtual Target getTarget( targetType );

	protected:
		// Protected data members:
		// Any additions to class members must be reflected
		// in changes to copy constuctor, file stream constuctor and toFile.
		CreatureType form;
    Gender gender;

    Stats maximum;
		Stats current;
    long current_xp;
    int  current_lev;
    
    Name c_name;
		Target brain;

		// equipped list
		Sorted<Target> equipped;
		// Weapons; primary and secondary:
		Target primary;
		Target secondary;
    // Item currently mounted:
    Target mount;
    // Pointer to most recently thrown:
    Target thrown;

		Creature( const Option* );
		Creature( std::iostream&, TargetValue *TVP );
		Creature( Creature & );

		virtual bool remove( const Target & );

		bool slotFilled( Slot );


    enum HungerState {

      hs_full,
      hs_normal,
      hs_hungry,
      hs_starving,
      hs_dead,

      hs_invalid
    
      };
	};


#define TIME_HFULL       6000
#define TIME_HNORMAL    40000
#define TIME_HHUNGRY    20000
#define TIME_HSTARVING  10000
#define TIME_ROT         4000

#endif // CREATURE_HPP
