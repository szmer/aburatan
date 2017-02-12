// [weapon.cpp]
// (C) Copyright 2000-2002 Michael Blackney
/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 **/
#if !defined ( WEAPON_CPP )
#define WEAPON_CPP

#include <cstring> // Sz. Rutkowski 11.02.2017

#include "weapon.hpp"
#include "random.hpp"
#include "grammar.hpp"
#include "message.hpp"
#include "level.hpp"
#include "path.hpp"
#include "global.hpp"
#include "menu.hpp"
#include "widget.hpp"

//-[ class Weapon ]-----------------

targetType Weapon::gettype( void ) const {
	return tg_weapon;
	}

bool Weapon::istype( targetType t ) const {
	if ( t == tg_weapon )
		return true;
	else return Buildable::istype(t);
	}

bool Weapon::capable( Action a, const Target &t ) {

  switch ( a ) {

    default:
      return Buildable::capable(a,t);
    }
	}

bool Weapon::perform( Action a, const Target &t ) {

	if ( Buildable::perform(a, t) ) return true;

	return false;
	}

bool Weapon::equals( const Target &rhs ) {
	return Buildable::equals(rhs);
	}

Weapon::Weapon( const Option *o )
: Buildable(o) {
	}

Weapon::Weapon( Weapon &rhs )
: Buildable(rhs) {
	}


void Weapon::toFile( std::iostream &outFile ) {

	Buildable::toFile( outFile );

	}

Weapon::Weapon( std::iostream &inFile, TargetValue *TVP )
: Buildable(inFile, TVP) {
	}


String Weapon::name( NameOption n_opt ) {
  switch (n_opt) {
    default:
      return Buildable::name(n_opt);
    }
  }

//-[ class Ranged ]-----------------

// Later to be loaded and parsed from a file.
// But, what the hey... it's not finished.
struct rangedStats {

	const char *desc;
	const char *name;

  int weight;
  long price;
  
	bool twohanded;

	AmmoType ammo;

	char ammocapacity;

	char speed;

	char range_unit;

	char to_hit;

	char color;

  bool known;

  Theme theme;

	static rangedStats *RangedStats;
	static int numRanged;
	};


rangedStats RangedStorage [] = {
/* Name                        weight |   price   |    two |      ammo      | spd | rng | to_hit | color | known |theme    /
/                                     |           | handed |  type     clip |     |     |        |                       */

// Handguns:
 { NULL,              "deringer",   15, Dollars( 1), false, bul_lpistol,   1,    3,    2,   +2,  cBlue,  false, th_none },
 { "gilean ",         "pistol",     20, Dollars( 4), false, bul_lpistol,   2,    5,    2,   +1,  cBlue,  false, th_rare },
 { "cult ",           "pistol",     18, Dollars( 2), false, bul_lpistol,   6,    5,    2,   +0,  cBlue,  false, th_none },
 { "light ",          "revolver",   20, Dollars( 3), false, bul_lpistol,   6,    5,    2,   +0,  cBlue,  false, th_none },
 { "long ",           "pistol",     22, Dollars( 4), false, bul_lpistol,   6,    7,    3,   +0,  cBlue,  false, th_none },
 { "pepperlock ",     "pistol",     28, Dollars( 6), false, bul_hpistol,   4,    4,    2,   +0,  cLBlue, false, th_none },
 { "service ",        "revolver",   20, Dollars( 7), false, bul_hpistol,   6,    6,    2,   +1,  cLBlue, false, th_none },
 { "gilean ",         "sixgun",     25, Dollars( 5), false, bul_hpistol,   6,    6,    3,   +1,  cLBlue, false, th_rare },
 { "imperial ",       "luger",      25, Dollars( 8), false, bul_hpistol,   8,    5,    2,   -1,  cLBlue, false, th_none },

// Long arms:
 { "smoothbore ",     "musket",     30, Dollars( 2), true,  bul_rifle,     1,   10,    3,   -2,  cGreen,  false, th_none },
 { "rifled ",         "musket",     35, Dollars( 5), true,  bul_rifle,     1,   10,    3,   -2,  cGreen,  false, th_none },
 { "repeating ",      "rifle",      30, Dollars( 9), true,  bul_rifle,    15,    9,    4,   -1,  cLGreen, false, th_none },
 { "cavalry ",        "carbine",    25, Dollars( 9), true,  bul_rifle,     7,   10,    5,   -1,  cGreen,  false, th_none },
 { "imperial ",       "rifle",      30, Dollars(11), true,  bul_rifle,    12,    8,    4,   -1,  cLGreen, false, th_none },
 { "imperial ",       "carbine",    25, Dollars(12), true,  bul_rifle,     8,    7,    5,   -1,  cGreen,  false, th_none },
 { "imperial ",       "mauser",     30, Dollars(15), true,  bul_rifle,     8,    4,    4,   -1,  cLGreen, false, th_rare },
   
// Shotguns:
 { "sawn-off ",       "shotgun",    35, Dollars( 6), false, bul_shotgun,   2,    9,    1,   +1,  cBrown,  false, th_none },
 { "double-barrel ",  "shotgun",    35, Dollars(10), true,  bul_shotgun,   2,   12,    2,   -1,  cYellow, false, th_none },

// Harpoon weapons:
 { "harpoon ",        "pistol",     15, Dollars( 4), true,  bul_harpoon,   1,    8,    2,   +1,  cGrey,   false, th_none },
 { "harpoon ",        "gun",        20, Dollars( 7), true,  bul_harpoon,   2,   12,    3,   +0,  cLGrey,  false, th_none },

// Needle weapons:
 { "needle ",         "rifle",      25, Dollars(13), true,  bul_needle,    8,    7,    3,   +1,  cLRed,   false, th_rare },
 { "needle ",         "carbine",    20, Dollars(14), true,  bul_needle,    6,    6,    4,   +1,  cRed,    false, th_rare },

// Bows:
 { "short ",          "bow",        10,  Cents(120), true,  bul_arrow,     1,    4,    4,   -2,  cViolet, false, th_none },
 { "long ",           "bow",        15,  Cents(180), true,  bul_arrow,     1,    5,    6,   -2,  cIndigo, false, th_none }

 };


rangedStats *rangedStats::RangedStats = RangedStorage;
int rangedStats::numRanged = 24;

// OLD :
// COMBO RANGED WEAPONS:
// LongArm(x) + telescope    = sniper x (sniper rifle, sniper carbine)
// LongArm(x) + sling        = hip-x (hip-shotgun, hip-carbine)
// Pistol(x)  + telescope    = deadshot x (deadshot revolver, deadshot Deringer)
// Firearm(x) + motor        = autox (autoshotgun, autocarbine, autopistol)
// Firearm(x) + tesla coil   = tesla-x (tesla-Dragoon, tesla-rifle)
// LongArm(x) + energy proj. = ?? blaster-x (blaster-rifle, blaster-carbine)
// Pistol(x)  + energy proj. = ?? blaster

//
// Categories:
//  AIM     - telescope, sling
//  BARREL  - tesla coil, energy proj.
//  AMMO    - motor
//

// NEW :
// COMBO RANGED WEAPONS:
// Categories:
//  CHAMBER   - revolving ch., magfeed ch., breech-ch., gatling ch.,
//              magneto, tesla-coil
//  CASING    - pistol casing, rifle casing, carbine casing
//  SIGHT     - telescopic sight,
//  ACCESSORY - sling
//
// EXAMPLES:
//
//  CASING:         REVOLVING         MAGFEED            BREECH
//  light pistol    service revolver  automatic          deringer
//  heavy pistol    revolver          luger              one-shot
//  carbine         repeating carbine auto-carbine       breech-loading carbine
//  rifle           repeating rifle   auto-rifle         breech-loading rifle
//  shotgun         tommy-gun         pump-action        shotgun
//
//                  GATLING           MAGNETO            TESLA_COIL
//  light pistol    pepperbox         gauss-pistol       shock-pistol
//  heavy pistol    gatling pistol    heavy gauss-pistol tesla-pistol
//  carbine         gatling carbine   sniper-carbine     tesla-carbine
//  rifle           gatling rifle     sniper-rifle       tesla-rifle
//  shotgun         peppercannon      railgun            shockgun
//


Target Ranged::getTarget( targetType type ) {

  switch( type ) {
    case tg_ammo:
      // detach and return ammo:
      if (inventory && inventory->reset())
        return inventory->get()->detach(1);
        
      // else fall through to:
    default:
      return Weapon::getTarget(type);
    }
    
  }


targetType Ranged::gettype( void ) const {
	return tg_ranged;
	}

bool Ranged::istype( targetType t ) const {

	if ( t == tg_ranged )
		return true;
	else return Weapon::istype(t);

	}

long Ranged::getSkill( Skill skill, bool ) {
// H2H Combat with ranged weapons isn't advised, but
// this value is also used to determine whether a firearm will
// be blocked when fired:
  if (skill == sk_figh) {
    if (getVal(attrib_two_handed))
      return -2;
    }
  
  return 0;

  }


bool Ranged::getStruct( structType type, void *buffer, size_t size ) {

  switch (type) {
    case s_shoot_cv: {
      // Get the weapon's shooting offensive values:
    
      // Make sure that there's the exact amount of space:
      assert( size == sizeof(CombatValues) );
      CombatValues cv;

      // Fill cv with combat values:

      // ATM ranged weapons just add their to-hit score:
      cv.to_hit += getVal(attrib_to_hit_bonus);

      // Copy our CombatValues to the buffer:
      memcpy( buffer, (const void*) &cv, size );
      
      return true;
      }

    default:
      return Weapon::getStruct(type,buffer,size);
    }

  }



bool Ranged::capable( Action a, const Target &t ) {

	switch (a) {

		case aLoad:

			// If target is given:
			if (!!t) {
  			// See if target is ammo:
  			if (! t->istype(tg_ammo) )
  				return false;

  			// Is target of correct ammo type?
  			if ( getVal(attrib_ammotype) != t->getVal(attrib_ammotype) )
  				return false;
        }

			// If empty, I can be loaded:
			if (!inventory || !inventory->reset())
				return true;
			// Not empty... is there any room left for ammo?
			// Assumes that ranged weapons only contain ammo.
			if ( inventory->get()->quantity() < rangedStats::RangedStats[form].ammocapacity )
				return true;
			// No room for ammo:
			return false;

		case aUnload:
			// Can unload if the weapon has an inventory:
			if (inventory && inventory->reset())
				return true;
			return false;

		case aFire:
			// can only fire if loaded:
			return isCapable(aUnload);

		default:
			return Weapon::capable(a,t);
		}

	}

bool Ranged::perform( Action a, const Target &t ) {

	switch (a) {

    case aPolymorph:

      if ( inventory && inventory->reset() ) {
        // Polymorph ammo:
        inventory->get()->doPerform(aPolymorph);
        // Then polyself into something that can accomodate the ammo:
        while ( getVal(attrib_ammotype) != inventory->get()->getVal(attrib_ammotype) ) {
    	  	form = Random::randint( rangedStats::numRanged  );
          }
        // If we now contain too much ammo, kick some of it out:
  			if ( inventory->get()->quantity() > rangedStats::RangedStats[form].ammocapacity ) {
          Target detach = inventory->get()->detach( inventory->get()->quantity()-rangedStats::RangedStats[form].ammocapacity );
          // Give it to the parent if possible:
          //Target parent = getTarget(tg_parent);
          if (!!parent && parent->capable(aContain,detach) ) parent->perform(aContain,detach);
          //Otherwise, extra ammo is abandoned.
          }
        }
      else
  	  	form = Random::randint( rangedStats::numRanged  );
        
    	image.val.color = rangedStats::RangedStats[form].color;
      return Weapon::perform(aPolymorph,t);
     
		case aLoad: {
			int roomLeft = rangedStats::RangedStats[form].ammocapacity;
			if (inventory && inventory->reset())
				// Assumes that ranged weapons only contain ammo.
				roomLeft -= inventory->get()->quantity();

			if (roomLeft > 0)
				perform(aContain,t->detach( roomLeft ));

			return true;
      }

		case aUnload:
			if (inventory && inventory->reset()) {
				if (inventory->find(t)) {
          Target p = getTarget(tg_parent);
          assert(!!parent);
					parent->perform(aContain,t);
          }
				return true;
				}
			return false;


		case aFire: {

      // Fire is now taken care of without the ranged weapon.
      // Creatures do all the work themselves.
      assert(0);
      return false;
    
      }


		default:
			return Weapon::perform(a, t);
		}

	}

long Ranged::getVal( StatType s ) {

	switch( s ) {
    case stat_spd:
      return rangedStats::RangedStats[form].speed;
      
    case attrib_combat_style:
      // Todo: return 2 & (1 if true(also melee))
      return 2;
    
    case attrib_balanced:
      return (getVal(attrib_ammotype) == bul_lpistol);
    
    case attrib_to_hit_bonus:
      return rangedStats::RangedStats[form].to_hit;

    case attrib_range_unit:
      return rangedStats::RangedStats[form].range_unit;

    case attrib_boom:
      // 0 == silent
      // 1 == "Blam!"
      // 2 == "BOOM!"
      if (rangedStats::RangedStats[form].ammo != bul_arrow)
        return 1;

    case attrib_draw:
      return rangedStats::RangedStats[form].ammo == bul_hpistol
          || rangedStats::RangedStats[form].ammo == bul_lpistol;
      
    case attrib_price: {
      long price = rangedStats::RangedStats[form].price;
      if (quality > 0)
        price *= quality+1;
      if (quality < 0)
        price /= abs(quality)+1;
        
      return price+Weapon::getVal(s);
      }
  
    case attrib_sort_category:
      return Weapon::getVal(s) + form;
      
    case attrib_weight:
    case attrib_sink_weight:
			return rangedStats::RangedStats[form].weight+Weapon::getVal(attrib_weight);

		case attrib_ammotype:
			return rangedStats::RangedStats[form].ammo;

		case attrib_two_handed:
			return rangedStats::RangedStats[form].twohanded;

		case attrib_form:
			return form;

    case attrib_capacity:
      return rangedStats::RangedStats[form].ammocapacity;

		default:
			return Weapon::getVal(s);
		}

	}



Ranged::Ranged( const Option *o )
: Weapon( o ) {


	if ( o->ammo == bul_none )
		form = Random::randint( rangedStats::numRanged );
	else for ( int i = 0; i < NUM_ABORT; ++i ) {
		form = Random::randint( rangedStats::numRanged  );
		if ( rangedStats::RangedStats[form].ammo == o->ammo
         && (rangedStats::RangedStats[form].theme & o->theme) ) break;
		}

	image.val.color = rangedStats::RangedStats[form].color;

	}

Ranged::Ranged( Ranged &rhs )
: Weapon(rhs) {

	form = rhs.form;
	}

Ranged::Ranged( std::iostream &inFile, TargetValue *TVP )
// Read Weapon base information:
: Weapon (inFile, TVP) {

	// Read char form
	inFile.read( (char*) &form, sizeof(form) );


	}

void Ranged::toFile( std::iostream &outFile ) {

	// Write Weapon base information:
	Weapon::toFile( outFile );

	// Write char form
	outFile.write( (char*) &form, sizeof(form) );


	}


bool Ranged::equals( const Target &rhs ) {

	bool retVal = Weapon::equals( rhs );

	if (retVal)
		// Good so far - same inventories and both of same type.
		if ( form != ( (Ranged*) rhs.raw() )->form ) retVal = false;

	return retVal;

	}

Target Ranged::clone( void ) {

	Ranged *newcopy = new Ranged( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

String Ranged::name( NameOption n_opt ) {

  String desc = rangedStats::RangedStats[form].desc,
         name = rangedStats::RangedStats[form].name,
         quality_str;

/*  if (combo != -1) {
    if (!!motor)
      desc = motor->name(n_prefix);
    else
      desc = "";
    name = String(rangedComboStats[combo].prefix)
           +( rangedComboStats[combo].use_verb
            ? rangedStats::RangedStats[form].name
            : "" );
    }*/
    
  if (quality != 0)
    quality_str += String((quality>0?"+":"-"))
                 + (long) quality + " ";
                 
  String ammo_count;

  switch (n_opt) {
    case n_prefix:
      return desc + Weapon::name(n_opt);
    case n_noun:
      return name + Weapon::name(n_opt);
  
    case n_short:
    	return String("#")
             +name+"~" + Weapon::name(n_opt);

    case n_menu_noprice:
    case n_menu: {

    	long numshells = 0, Capacity = rangedStats::RangedStats[form].ammocapacity;

    	if (inventory && inventory->reset()) {
    		numshells = inventory->get()->quantity();
    		}
        
      if (Capacity == 1) {
        if (rangedStats::RangedStats[form].ammo == bul_arrow) {
      	  if (numshells)
            ammo_count = " (nocked)";
          else
            ammo_count = "";
          }
        else {
      	  if (numshells)
            ammo_count = " (loaded)";
          else
            ammo_count = " (empty)";
          }

        }
      else
      	ammo_count = String(" [")+numshells+"/"+Capacity+"]";

      }
      // Fall through to:
    default: {
    	return "#"
             +quality_str
             +desc
             +name+"~" + ammo_count + Weapon::name(n_opt);
      }
  	}
  }

void Ranged::identify( IDmask id_mask ) {

  if (id_mask & id_form)
    rangedStats::RangedStats[form].known = true;

  Weapon::identify(id_mask);
  }
  
bool Ranged::identified( IDmask id_mask ) {
  IDmask override = id_none;

  if (rangedStats::RangedStats[form].known)
    (int&) override |= (int) id_form;

  return Weapon::identified( (IDmask) (id_mask & ~override) );
  }



//-[ class Melee ]-----------------

struct meleeStats {

	const char *desc;
	const char *name;

  long price;

  int weight;
	bool twohanded;

	MeleeType type;

	char speed;

	char accuracy;

  // Penetration values:
  char pierce, bash, cut, chop;

  // Parry defence bonus:
  char parry;

  // Balanced weapons are penalised less when used as a secondary weapon
  bool balanced;

  bool known;

  materialClass material;

  Theme theme;

	static meleeStats *MeleeStats;
	static int numMelee;

	};


meleeStats MeleeStorage [] = {

// Later to be loaded and parsed from a file. Probably.

// Desc.      Name         price     weight | 2-h | mel_type |spd|acc| pin,bsh,cut,chp |parry|balance|known|material| theme
	{"meat ",   "hook",     Cents( 30),    4,  false, mel_knife, 5, -1,  +4, +0, +0, +0,   0,   true, false, m_steel,  th_kitchen } ,
	{"gaff ",   "hook",     Cents( 20),    4,  false, mel_knife, 6, -2,  +5, +0, +0, +0,   0,   true, false, m_steel,  th_none } ,

	{NULL,      "scalpel",  Cents(120),    4,  false, mel_knife, 4, +1,  +1, +0, +2, +0,   0,   true, false, m_steel,  th_lab } ,
	{"pocket",  "knife",    Cents( 50),    5,  false, mel_knife, 4,  0,  +3, +0, +2, +0,   0,   true, false, m_steel,  th_none } ,
	{"kitchen ","knife",    Cents( 50),    8,  false, mel_knife, 5,  0,  +3, +1, +3, +0,   1,   true, false, m_steel,  th_kitchen },
	{"heavy ",  "knife",   Dollars( 1),   10,  false, mel_knife, 4,  0,  +3, +1, +4, +0,   1,   true, false, m_steel,  th_none } ,
  {"meat ",   "cleaver", Dollars( 2),   10,  false, mel_knife, 4, +1,  +1, +1, +0, +3,   0,   true, false, m_steel,  th_kitchen } ,
	{NULL,      "stiletto", Cents(150),    5,  false, mel_knife, 5, -1,  +4, +0, +0, +0,   0,   true, false, m_steel,  th_none } ,
	{"wooden ", "stake",    Cents( 50),   12,  false, mel_knife, 6, -2,  +3, +0, +0, +0,   0,   true, false, m_wood,   th_garden } ,

	{NULL,      "machete", Dollars( 4),   18,  false, mel_sword, 4, +2,  +0, +3, +1, +4,   2,  false, false, m_steel,  th_garden } ,
	{"antique ","sword",   Dollars(25),   25,  false, mel_sword, 6, +2,  +4, +2, +5, +1,   3,  false, false, m_steel,  th_rare } ,
	{NULL,      "cutlass", Dollars( 8),   20,  false, mel_sword, 5, +2,  +3, +2, +7, +2,   4,  false, false, m_steel,  th_rare } ,
	{NULL,      "sabre",   Dollars(15),   20,  false, mel_sword, 5, +2,  +5, +1, +5, +1,   4,  false, false, m_steel,  th_none } ,
	{"cavalry ","sabre",   Dollars(18),   20,  false, mel_sword, 5, +2,  +5, +1, +6, +1,   5,  false, false, m_steel,  th_none } ,
	{"long",    "sword",   Dollars(14),   15,  false, mel_sword, 5, +3,  +4, +0, +5, +1,   4,  false, false, m_steel,  th_none } ,
	{"silver ", "sabre",   Dollars(35),   20,  false, mel_sword, 5, +2,  +4, +1, +6, +1,   4,  false, false, m_silver, th_rare } ,

	{NULL,      "hatchet", Dollars( 6),   25,  false, mel_haft,  5,  0,  +0, +1, +1, +3,   1,   true, false, m_steel,  th_workshop } ,
	{NULL,      "tomahawk",Dollars( 8),   25,  false, mel_haft,  4,  0,  +0, +2, +1, +4,   2,  false, false, m_stone,  th_none } ,
	{NULL,      "axe",     Dollars(10),   35,  true,  mel_haft,  7, +1,  +0, +3, +2, +5,   2,  false, false, m_steel,  th_workshop } ,

	{"ceramic ","club",     Cents(210),   15,  false, mel_club,  5,  0,  +0, +4, +0, +0,   0,   true, false, m_stone,  th_lab } ,
	{"golf ",   "club",     Cents( 10),   20,  false, mel_club,  6, -1,  +0, +4, +0, +0,   0,  false, false, m_steel,  th_garden } ,
	{NULL,      "wrench",   Cents(120),   25,  false, mel_club,  5, +1,  +0, +5, +0, +0,   2,   true, false, m_steel,  th_workshop } ,
	{"heavy ",  "wrench",   Cents(280),   35,  false, mel_club,  6,  0,  +0, +7, +0, +0,   3,  false, false, m_steel,  th_workshop } ,
	{"claw ",   "hammer",   Cents(140),   30,  false, mel_club,  6,  0,  +2, +5, +0, +0,   0,  false, false, m_steel,  th_workshop } ,
	{NULL,      "shovel",   Cents( 80),   45,  false, mel_club,  7, -1,  +0, +4, +3, +2,   2,  false, false, m_steel,  th_garden } ,
	{"carpentry ","hammer",Dollars( 3),   35,  false, mel_club,  6, +1,  +2, +6, +0, +0,   0,  false, false, m_steel,  th_workshop } ,
	{"sledge ", "hammer",  Dollars( 2),   50,  true,  mel_club,  8, -1,  +2, +8, +0, +0,   0,  false, false, m_steel,  th_workshop } ,

	{"ironwood ","staff",  Dollars( 1),   35,  true,  mel_pole,  6, +2,  +0, +4, +0, +0,   3,  false, false, m_wood,   th_garden } ,
	{"ghostwood ","staff", Dollars( 8),   15,  true,  mel_pole,  5, +2,  +0, +3, +0, +0,   4,  false, false, m_wood,   th_rare } ,
	{"antique ","halberd", Dollars(10),   75,  true,  mel_pole,  7,  0,  +4, +3, +6, +6,   2,  false, false, m_steel,  th_rare } ,
	{"antique ","spear",   Dollars( 8),   35,  true,  mel_pole,  6, +1,  +8, +0, +2, +0,   1,  false, false, m_steel,  th_rare } ,
	{"pitch",   "fork",      Cents(80),   45,  true,  mel_pole,  6, +1,  +8, +0, +0, +0,   0,  false, false, m_steel,  th_workshop } ,
	{"heavy ", "harpoon",   Cents(180),   25, false,  mel_pole,  5, +1,  +6, +0, +1, +0,   1,  false, false, m_steel,  th_sewers },

};

meleeStats *meleeStats::MeleeStats = MeleeStorage;
int meleeStats::numMelee = 33;


targetType Melee::gettype( void ) const {
	return tg_melee;
	}

bool Melee::istype( targetType t ) const {

	if ( t == tg_melee )
		return true;
	else return Weapon::istype(t);

	}

bool Melee::capable( Action a, const Target &t ) {

  switch (a) {

  	default:
      return Weapon::capable(a, t);
    }

	}


bool Melee::perform( Action a, const Target &t ) {

  switch (a) {

    /*- Motors removed:
    case aLoad:
    case aUnload:
      if (!!motor)
        return motor->perform(a,t);
      return false;
    */

    case aPolymorph:
  		form = Random::randint( meleeStats::numMelee  );
    	if ( (meleeStats::MeleeStats[form].type == mel_haft)
    		 || (meleeStats::MeleeStats[form].type == mel_club) ) {
    		image.val.appearance = '|';
    		image.val.color = cBrown;
    		}
    	else if (meleeStats::MeleeStats[form].type == mel_pole) {
    		image.val.appearance = '\\';
    		image.val.color = cBrown;
    		}
      else {
    		image.val.appearance = ')';
    		image.val.color = cGrey;
        }
      return Weapon::perform(aPolymorph,t);

  	default:
      return Weapon::perform(a, t);

    }

	}

long Melee::getVal( StatType s ) {

	switch( s ) {

    case stat_spd:
      return meleeStats::MeleeStats[form].speed;

    case attrib_combat_style:
      // Todo: return 1 & (1 if true(also ranged))
      return 1;
      
		case attrib_balanced:
			return meleeStats::MeleeStats[form].balanced;
      
		case attrib_draw:
			return meleeStats::MeleeStats[form].type == mel_sword;
      
    case attrib_sort_category:
      return Weapon::getVal(s) + form;

    case attrib_price: {
      long price = meleeStats::MeleeStats[form].price;;
      if (quality > 0)
        price *= quality+1;
      if (quality < 0)
        price /= abs(quality)+1;
        
      return price+Weapon::getVal(s);
      }
    
    case attrib_weight:
			return meleeStats::MeleeStats[form].weight+Weapon::getVal(attrib_weight);
    
		case attrib_meleetype:
			return meleeStats::MeleeStats[form].type;

		case attrib_two_handed:
			return meleeStats::MeleeStats[form].twohanded;

		case attrib_form:
			return form;

		default:
			return Weapon::getVal(s);
		}

	}

long Melee::getSkill( Skill skill, bool get_total ) {
// Melee currently can only add to fighting skill:
  if (skill == sk_figh) {
    if (!!blade)
      return blade->getSkill(skill,get_total)+quality;
      
    return quality;
    }
  
  return 0;

  }

Attack Melee::getAttack( void ) {

  Target deliverer = controller;
  if (!deliverer)
    deliverer = getTarget(tg_controller);

  CombatValues c_val;
  c_val.penetration[ pen_pierce ] = meleeStats::MeleeStats[form].pierce;
  c_val.penetration[ pen_bash ]   = meleeStats::MeleeStats[form].bash;
  c_val.penetration[ pen_cut ]    = meleeStats::MeleeStats[form].cut;
  c_val.penetration[ pen_chop ]   = meleeStats::MeleeStats[form].chop;
  
  Attack ret_at( c_val,
                 deliverer,
								 THIS
							 );

  if (!!blade) ret_at |= blade->getAttack();
  
	return ret_at;
	}

Melee::Melee( const Option *o )
: Weapon( o ), form(0) {

	if ( o->type == mel_none )
    doPerform(aPolymorph);

	else for ( int i = 0; i < NUM_ABORT; ++i ) {

    doPerform(aPolymorph);
		if ( meleeStats::MeleeStats[form].type == o->type
        && (meleeStats::MeleeStats[form].theme & o->theme ) ) break;
		}

	}

Melee::Melee( Melee &rhs )
: Weapon(rhs) {

	form = rhs.form;
	}

Melee::Melee( std::iostream &inFile, TargetValue *TVP )
// Read Weapon base information:
: Weapon (inFile, TVP) {

	inFile.read( (char*) &form, sizeof(form) );

	}

void Melee::toFile( std::iostream &outFile ) {

	// Write Weapon base information:
	Weapon::toFile( outFile );

	outFile.write( (char*) &form, sizeof(form) );


	}


bool Melee::equals( const Target &rhs ) {

	bool retVal = Weapon::equals( rhs );

	if (retVal) {
		// Good so far - same inventories and both of same type.
		if ( form != ( (Melee*) rhs.raw() )->form ) retVal = false;
    }

	return retVal;

	}

Target Melee::clone( void ) {

	Melee *newcopy = new Melee( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}


/*
void Melee::setlocation( const Point3d &p3d ) {

  if (!! blade) blade->setlocation(p3d);

  Weapon::setlocation(p3d);

  }
*/

/*
Target Melee::findTarget( targetType type, const Target& t ) {

  switch(type) {
    case tg_possessor:
    case tg_parent: {
      assert( location == t->getlocation() );

      if (blade == t) return THIS;

      Target found;

      if (!!blade) {
        found = blade->findTarget(type,t);
        if (!!found) return found;
        }

      return Weapon::findTarget(type,t);
      }
      
    default:
      return Weapon::findTarget(type,t);
    }
  }
*/
  
String Melee::name( NameOption n_opt ) {

  String desc = meleeStats::MeleeStats[form].desc,
         name = meleeStats::MeleeStats[form].name,
         quality_str;

  /*
  if (combo != -1) {
    //if (!!motor)
    //  desc = motor->name(n_prefix);
    //else
    desc = "";
    name = String(meleeComboStats[combo].prefix)
           +( meleeComboStats[combo].use_verb
            ? meleeStats::MeleeStats[form].name
            : "" );
    }
  */
  if (quality != 0)
    quality_str += String((quality>0?"+":"-"))
                 + (long) quality + " ";

  switch (n_opt) {
    case n_prefix:
      return desc + Weapon::name(n_opt);
    case n_noun:
      return name + Weapon::name(n_opt);
  
    case n_short:
    	return String("#")
             +name+"~" + Weapon::name(n_opt);
    default: {
    	return "#"
             +quality_str
             +desc
             +name+"~" + Weapon::name(n_opt);
      }
  	}
    
  }

bool Melee::getStruct( structType type, void *buffer, size_t size ) {

  switch (type) {
    case s_offence_cv: {
      // Get the weapon's offensive values:
    
      // Make sure that there's the exact amount of space:
      assert( size == sizeof(CombatValues) );
      CombatValues cv;

      // Fill cv with combat values

      // Penetration:
      cv.defence[pen_pierce] = meleeStats::MeleeStats[form].pierce;
      cv.defence[pen_bash]   = meleeStats::MeleeStats[form].bash;
      cv.defence[pen_cut]    = meleeStats::MeleeStats[form].cut;
      cv.defence[pen_chop]   = meleeStats::MeleeStats[form].chop;

      // Add in cv of blade attachments:
      if (!!blade) {
        CombatValues blade_v;
        blade->GET_STRUCT( s_offence_cv, blade_v );
        cv += blade_v;
        }

      // Damage types:
      cv.damage[dam_physical] = 100; // 100% physical damage as standard
      
      // Todo: Apply purity damage for silver/wooden weapons:
      if(  (meleeStats::MeleeStats[form].material == m_wood
          ||meleeStats::MeleeStats[form].material == m_silver)

         // but only if they break the skin (since the reason
         // these weapons do purity damage is that they are capable
         // of breaking the skin of unnatural creatures)
         && (cv.defence[pen_pierce]
          || cv.defence[pen_cut]
          || cv.defence[pen_chop])
          )
        cv.damage[dam_purity] = 100;

      // Copy our CombatValues to the buffer:
      memcpy( buffer, (const void*) &cv, size );
      
      return true;
      }

    case s_defence_cv: {
      // Weapons currently only have defence values for parry:

      // Make sure that there's the exact amount of space:
      assert( size == sizeof(CombatValues) );
      CombatValues cv;
      cv += meleeStats::MeleeStats[form].parry;

      // Could possibly have weapons that give immunity to
      // certain damage types, or that provide armour protection
      // though I'm not leaning that way at the moment.  I can't
      // think of any thematically consistent defensive melee weapons
      // (main gauche?  no!) so at best this may come down to weapon
      // attachments.

      // Add in cv of blade attachments:
      if (!!blade) {
        CombatValues blade_v;
        blade->GET_STRUCT( s_defence_cv, blade_v );
        cv += blade_v;
        }

      // Copy our CombatValues to the buffer:
      memcpy( buffer, (const void*) &cv, size );
      
      }

    default:
      return Weapon::getStruct(type,buffer,size);
    }

  }

void Melee::identify( IDmask id_mask ) {

  if (id_mask & id_form)
    meleeStats::MeleeStats[form].known = true;

  Weapon::identify(id_mask);
  }
  
bool Melee::identified( IDmask id_mask ) {
  IDmask override = id_none;

  if (meleeStats::MeleeStats[form].known)
    (int&) override |= (int)id_form;

  return Weapon::identified( (IDmask) (id_mask & ~override) );
  }


#endif // WEAPON_CPP
