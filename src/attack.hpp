// [attack.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( ATTACK_HPP )
#define ATTACK_HPP

#include "diceroll.hpp"
//#include "enum.hpp"
#include "target.hpp"
#include "string.hpp"

/*
 * Attacks can be avoided/resisted/reduced in the following stages:
 *
 *  1. Delivery - avoided
 *     + Chance based on combat/feint/dodge skills
 *       of combatants.
 *
 *  2. Penetration - resisted
 *     + Based on natural and worn armour
 *
 *  3. Damage - reduced (reduced to nothing == ignored)
 *     + Based on immunities and resistances
 *
 *
 * example:
 *
 * CombatValues attack;
 * CombatValues armour;
 *
 * // Venom Sword
 * attack.penetration[pen_pierce] = +3;
 * attack.penetration[pen_cut]    = +5;
 * attack.penetration[pen_chop]   = +1;
 * attack.damage[dam_physical] = 100%;
 * attack.damage[dam_venom]    =  80%;
 *
 * // Chain mail:
 * armour.defence[pen_pierce] = +2;
 * armour.defence[pen_bash] = +2;
 * armour.defence[pen_cut] = +8;
 * armour.defence[pen_chop] = +5;
 * armour.defence[pen_splash] = +1;
 *
 */

// AttackPenetration: How the attack plans to get past your armour.
enum AttackPenetration {

  // Four standard weapon penetration types:
  
  pen_pierce,  // Stiletto, bullet, stabbing weapons
  pen_bash,    // Club, staff
  pen_cut,     // Short Sword, slicing weapons
  pen_chop,    // Axe, machete, severing weapons

  // Other types for varied attacks/armour

  pen_crush,   // Snake Constriction, enveloping attack
  pen_splash,  // Pouring a bottle of poison, spit attacks
  pen_vapor,   // Gas attacks, meant to be inhaled or affect eyes

  pen_num,
  pen_invalid
  };

// AttackDamage: What happens when the attack gets past armour.
enum AttackDamage {

  dam_physical,// Solid implement          Immune: ghosts
  dam_purity,  // Silver, wood, holy water Immune: natural creatures
  dam_elec,    // Thundergun, lightning    Immune: automatons
  dam_burn,    // Fire, chemicals          Immune: devils
  dam_poison,  // Venom                 Immune: unnatural creatures
  dam_rust,    // Wetting attacks       Immune: non-ferrous material
  dam_drown,   // Suffocation by water  Immune: fishmen
  dam_choke,   // Other suffocation     Immune: non air-breathers

  dam_num,
  dam_invalid
  };
  

struct CombatValues {

  const char *hit_str;

  int to_hit;

  union {
    int penetration[ pen_num ];
    int defence[ pen_num ];
    };
    
  union {
    int damage[ dam_num ];
    int resist[ dam_num ];
    };

  // Construction:
  CombatValues( void );                  // Empty
  
  CombatValues( AttackPenetration pen_type, int pen_val,
                AttackDamage dam_type=dam_physical, int dam_val=100 );

  CombatValues( int hit,
                int p0, int p1,int p2,int p3,int p4,int p5,int p6,
                int d0, int d1,int d2,int d3,int d4,int d5,int d6,int d7
                );
  
  
  CombatValues( const CombatValues & ); // Copy

  // Member functions:
  AttackPenetration penetrated( void ) const;// Returns index of highest penetration value
  AttackDamage      damaged   ( void ) const;// Returns index of highest damage value

  String            toString( void ) const; // Returns a string rep. of best pen.

  // This is the meat function:
  CombatValues combatResult( const CombatValues & ) const;
  // Such that calling:
  //
  // // attack_cv.combatResult( defence_cv )
  //
  // will return a CV struct outlining the strength of the hit
  // and the damage done.

  int      rollDamage( void ) const; // Rolls damage for a combat result
  DiceRoll damageRoll( void ) const; // returns the dice we will roll
  
  // Overloaded operators:
  CombatValues &operator +=( const CombatValues & );
  CombatValues  operator + ( const CombatValues & );
  CombatValues &operator +=( int ); // Adds to to_hit

  //CombatValues &operator -=( const CombatValues & );
  //CombatValues  operator - ( const CombatValues & );
  };


struct Attack {

	Attack( const CombatValues &cv, const Target &del, const Target &ins, const char *const dstr=NULL )
	: c_values(cv), deliverer(del), instrument(ins), deathStr(dstr) {}

	CombatValues c_values;

  // The intelligence behind the attack...
 	Target deliverer;
  // and the weapon used to achieve it (often one & the same)
	Target instrument;

  const char *deathStr;

  /*
	Attack &operator +=( const Attack& );
  */
	Attack &operator |=( const Attack& );


	};


const char *HitString( const CombatValues& );
const char *KillString( const CombatValues& );


struct DamStrType {
  const char *formal; // Technical name
  const char *hit;    // What we say when a hit is scored
  const char *kill;   // How were you killed?
  };

const DamStrType DamStringArr[pen_num+dam_num] = {
  { "pierce",   "stab~",    NULL },
  { "bash",     "bash~",   "pulverised" },
  { "cut",      "slash~",  "slaughtered" },
  { "chop",     "hack~",   "butchered" },
  { "crush",    "crush~",  "crushed" },
  { "splash",   "splash~",  NULL },
  { "vapor",    "spray~",   NULL },
  { "physical", "hit~",     NULL },
  { "purity",    NULL,     "cleansed" },
  { "electric", "zap~",    "electrocuted" },
  { "burn",     "burn~",   "incinerated" },
  { "poison",   "poison~",  NULL },
  { "rust",     "rust~",    NULL },
  { "drown",    "drown~",  "drowned" },
  { "choke",    "choke~",  "suffocated" }
  };

  

#endif // ATTACK_HPP
