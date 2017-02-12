// [attack.cpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( ATTACK_CPP )
#define ATTACK_CPP

#include "attack.hpp"
#include "assert.h"

/*
Attack &Attack::operator +=( const Attack &rhs ) {

	strength += rhs.strength;
	damage = rhs.damage;
	penetration += rhs.penetration;
	c_values = rhs.c_values;

  if (!deathStr) deathStr = rhs.deathStr;


	return *this;

	}
*/
Attack &Attack::operator |=( const Attack &rhs ) {
	c_values += rhs.c_values;
  if (!deathStr) deathStr = rhs.deathStr;
	return *this;
	}


const char *HitString( const CombatValues &c_values ) {
// Returns the "stab" part of "You stab the orc!"

  const char *ret_str = "hit~";

  int penetrated = c_values.penetrated();
  if (penetrated && DamStringArr[ penetrated ].hit)
    ret_str = DamStringArr[ penetrated ].hit;
  
  return ret_str;
  }

const char *KillString( const CombatValues &c_values ) {
// Returns the "slain" part of "You were slain by an elf."

  const char *ret_str = "killed";

  int penetrated = c_values.penetrated();
  if (penetrated && DamStringArr[ penetrated ].kill)
    ret_str = DamStringArr[ penetrated ].kill;
  
  return ret_str;
  }


//--[ struct CombatValues ]--

CombatValues::CombatValues( void ) {
// All empty:
  int i;
  hit_str = NULL;
  to_hit = 0;
  for ( i = 0; i < pen_num; ++i )  penetration[i] = 0;
  for ( i = 0; i < dam_num; ++i )  damage[i] = 0;
  
  }
  
/*
CombatValues::CombatValues( AttackDamage dam_type, int dam_val ) {
// 1 dam type, splash penetration
  int i;
  for ( i = 0; i < pen_num; ++i )  penetration[i] = 0;
  for ( i = 0; i < dam_num; ++i )  damage[i] = 0;

  hit_str = NULL;
  penetration[ pen_splash ] = dam_val;
  penetration[ dam_type ] = dam_val;
  }
*/

CombatValues::CombatValues( AttackPenetration pen_type, int pen_val,
                            AttackDamage dam_type, int dam_val ) {
// 1 pen type, defaults to 100% physical damage
  to_hit = 0;
  
  int i;
  for ( i = 0; i < pen_num; ++i )  penetration[i] = 0;
  for ( i = 0; i < dam_num; ++i )  damage[i] = 0;

  hit_str = NULL;
  penetration[ pen_type ] = pen_val;
       damage[ dam_type ] = dam_val;
  }

CombatValues::CombatValues( int hit,
              int p0, int p1,int p2,int p3,int p4,int p5,int p6,
              int d0, int d1,int d2,int d3,int d4,int d5,int d6,int d7
              ) : to_hit(hit) {

  penetration[0] = p0;
  penetration[1] = p1;
  penetration[2] = p2;
  penetration[3] = p3;
  penetration[4] = p4;
  penetration[5] = p5;
  penetration[6] = p6;

  damage[0] = d0;
  damage[1] = d1;
  damage[2] = d2;
  damage[3] = d3;
  damage[4] = d4;
  damage[5] = d5;
  damage[6] = d6;
  damage[7] = d7;
  }

  
CombatValues::CombatValues( const CombatValues &copy ) {
// Copy constructor
  to_hit = copy.to_hit;
  
  int i;
  hit_str = copy.hit_str;
  for ( i = 0; i < pen_num; ++i )  penetration[i] = copy.penetration[i];
  for ( i = 0; i < dam_num; ++i )  damage[i] = copy.damage[i];
  }

AttackPenetration CombatValues::penetrated( void ) const {
// Returns index of highest penetration value or pen_invalid
  AttackPenetration pen_best = pen_invalid;
  
  for ( int i = 0; i < pen_num; ++i )
    if ( (penetration[i] && pen_best == pen_invalid)
    || (pen_best != pen_invalid && penetration[pen_best] < penetration[i]))
      pen_best = (AttackPenetration) i;

  return pen_best;
  }
  
AttackDamage CombatValues::damaged( void ) const {
// Returns index of highest damage value or dam_invalid
  AttackDamage dam_best = dam_invalid;
  
  for ( int i = 0; i < dam_num; ++i )
    if ((damage[i] && dam_best == dam_invalid)
    || (dam_best != dam_invalid && damage[dam_best] < damage[i]))
      dam_best = (AttackDamage) i;

  return dam_best;
  }


CombatValues &CombatValues::operator +=( int delta ) {
// Adds to to_hit
  to_hit += delta;
  return *this;
  }

CombatValues &CombatValues::operator +=( const CombatValues &rhs ) {
// Adds values, no cap:
  int i;
  for ( i = 0; i < pen_num; ++i )  penetration[i] += rhs.penetration[i];
  for ( i = 0; i < dam_num; ++i )  damage[i] += rhs.damage[i];

  return *this;
  }

CombatValues  CombatValues::operator + ( const CombatValues &rhs ) {
// Adds values, no cap:
  CombatValues ret_val(*this);
  ret_val += rhs;
  return ret_val;
  }

/*
CombatValues &CombatValues::operator -=( const CombatValues &rhs ) {
// Subtracts values, capped:
  int i;
  for ( i = 0; i < pen_num; ++i ) {
    penetration[i] -= rhs.penetration[i];
    if (penetration[i] < 0) penetration[i] = 0;
    }
  for ( i = 0; i < dam_num; ++i ) {
    damage[i] -= rhs.damage[i];
    if (damage[i] < 0) damage[i] = 0;
    }

  return *this;
  }

CombatValues  CombatValues::operator - ( const CombatValues &rhs ) {
// Subtracts values, capped:
  CombatValues ret_val(*this);
  ret_val -= rhs;
  return ret_val;
  }
*/

CombatValues CombatValues::combatResult( const CombatValues &rhs ) const {
  // This is the meat function, such that calling:
  //
  // // attack_cv.combatResult( defence_cv )
  //
  // will return a CV struct outlining the strength of the hit
  // and the damage done.

  CombatValues ret_cv;

  ret_cv.to_hit = to_hit - rhs.to_hit;
  
  int i;
  for ( i = 0; i < pen_num; ++i ) {
    // Make sure that we only reduce penetration for near-hits:
    if (ret_cv.to_hit > 0) {
      if (ret_cv.to_hit < rhs.penetration[i])
        ret_cv.penetration[i] = penetration[i]
                              - (rhs.penetration[i]-ret_cv.to_hit)
                              - max( 0, rhs.penetration[i] - penetration[i]);
      else
        ret_cv.penetration[i] = penetration[i]
                              - max( 0, rhs.penetration[i] - penetration[i]);
      }
    else
      if (ret_cv.to_hit + penetration[i] - rhs.penetration[i] > 0)
        ret_cv.penetration[i] = ret_cv.to_hit + penetration[i] - rhs.penetration[i]
                              - max( 0, rhs.penetration[i] - penetration[i]);
      else
        ret_cv.penetration[i] = 0;
      
    // penetration >= 0
    if (ret_cv.penetration[i] < 0) ret_cv.penetration[i] = 0;
    }
    
  for ( i = 0; i < dam_num; ++i ) {
  // damage_multiplier = attack% - defence%;
    ret_cv.damage[i] = damage[i] - rhs.damage[i];
    // damage is between 0 and 100, inclusive
    if (ret_cv.damage[i] < 0) ret_cv.damage[i] = 0;
    if (ret_cv.damage[i] > 100) ret_cv.damage[i] = 100;
    }

  return CombatValues(ret_cv);
  }


DiceRoll damageDice( AttackPenetration pen_type, int pen_level ) {
// Contains the dice roll ranges per penetration type, pen level
  DiceRoll dam_dice(0,0);
  if (pen_level <= 0)
    return dam_dice;

  // LEVEL     1      2      3      4    ...   n               10      Range
  // pierce  1d4-2  2d4-3  3d4-4  4d4-5  ...  Ld4-(L+1)    10d4-11  0..29
  // bash    1d1+2  1d2+3  1d3+4  1d4+5  ...  1dL+(L+1)    1d10+11  12..21
  // cut      2d2    2d3    2d4    2d5   ...  2d(L+1)      2d11     2..22
  // chop     1d3    2d3    3d3    4d3   ...  Ld3          10d3     10..30
  // crush   1d2+2  1d4+2  1d6+2  1d8+2  ...  1d(L*2)+2    1d20+2   3..22
  // splash  1d6-2  1d8-2  1d10-2 1d12-2 ...  1d(L*2+4)-2  1d24-2   0..22
  // vapor    1d4    1d6    1d8   1d10   ...  1d(L*2+2)    1d22     1..22

  switch (pen_type) {
    case pen_pierce: dam_dice = DiceRoll(pen_level,4,-(pen_level+1)); break;
    case pen_bash:   dam_dice = DiceRoll(1,pen_level,pen_level+1); break;
    case pen_cut:    dam_dice = DiceRoll(2,pen_level+1); break;
    case pen_chop:   dam_dice = DiceRoll(pen_level,3); break;
    case pen_crush:  dam_dice = DiceRoll(1,pen_level*2,2); break;
    case pen_splash: dam_dice = DiceRoll(1,pen_level*2+4,-2); break;
    case pen_vapor:  dam_dice = DiceRoll(1,pen_level*2+2); break;
    default: assert(0); // Should never get called with no penetration!
    }

  return dam_dice;
  }

DiceRoll CombatValues::damageRoll( void ) const {
// returns the dice we will roll

  AttackPenetration pen_type = penetrated();
  AttackDamage dam_type = damaged();

  if (pen_type == pen_invalid || dam_type == dam_invalid)
    return DiceRoll(0,0,0);

  int pen_level = penetration[pen_type];

  DiceRoll dam_roll = damageDice( pen_type, pen_level );
  dam_roll += DiceRoll(0,0,to_hit/4); // Give a slight bonus to good hits.

  return dam_roll;
  }

int CombatValues::rollDamage( void ) const {
// Rolls damage for a combat result
  AttackDamage dam_type = damaged();
  int dam_percent = damage[dam_type];

  return damageRoll().roll() * dam_percent / 100;
  }



String CombatValues::toString( void ) const {
// return a string representation
// of the form:
// +2, +5 (pierce) // <50% purity>

  String ret_str;

  if (to_hit) {
    // +2 to hit
    if (to_hit > 0)
      ret_str << (char) '+';
      
    ret_str << (int) to_hit << " to hit";
    }

  // Penetration
  AttackPenetration pen = penetrated();
  if ( pen != pen_invalid ) {

    if (to_hit)
      ret_str += ", ";
  
    // +5 (pierce)
    ret_str << (char) '+' << (int) penetration[pen] << " to " << (char*)DamStringArr[pen].formal;
    }

  // Damage
  /*
  AttackDamage dam = damaged();
  if ( dam != dam_invalid ) {
    // <5 purity> // only when not natural (physical) dam
    if (pen != pen_invalid)
      ret_str += " ";
    else if (to_hit)
      ret_str += ", ";


    ret_str << "\\<" << (int) damage[dam] << "% " << (char*)DamStringArr[dam+pen_num].formal << "\\>";

    }
  */
  
  return ret_str;
    

  }

#endif // ATTACK_CPP

