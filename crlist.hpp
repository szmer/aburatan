// [crlist.hpp]
// (C) Copyright 2000 Michael Blackney
// CreatureInfo - contains information about all the different types
// of creatures.
#if !defined ( CRLIST_HPP )
#define CRLIST_HPP
#include "stats.hpp"
#include "types.hpp"
#include "enum.hpp"
#include "map.hpp"
#include "skill.hpp"
#include "food.hpp"
#include "fluid.hpp"

#define SAMPLECREATURES


enum Voice {

  vo_none,
  vo_omni,
  
  vo_bark,    // dog
  vo_mew,     // cat

  vo_howl,    // wolf
  
  vo_moo,     // cow
  vo_neigh,   // horse
  vo_oink,    // pig
  vo_baa,     // sheep
  vo_bleet,   // goat
  vo_cluck,   // chicken

  vo_squeek,  // mouse
  
  vo_quack,   // duck
  vo_coo,     // pigeon
  vo_squawk,  // seagull
  
  vo_hiss,    // snake
  
  vo_roar,    // lion
  vo_growl,   // bear

  vo_buzz,    // bee

  vo_pipe,    // archaean

  vo_click,   // lobstrosity

  vo_num,
  vo_invalid
  
  };

enum InnateAttack {

  ia_hit   = 0x0001,
  ia_kick  = 0x0002,
  ia_claw  = 0x0004,
  ia_bite  = 0x0008,
  ia_peck  = 0x0010,
  ia_butt  = 0x0020,
  ia_gore  = 0x0040,
  ia_spit  = 0x0080,
  ia_sting = 0x0100,
  ia_choke = 0x0200,
  ia_crush = 0x0400,
  ia_touch = 0x0800,
  ia_spray = 0x1000,

  ia_none  = 0x0000,
  ia_all   = 0x7FFF,
  ia_invalid
  };

struct creature {

 // Creature listing data members:
  const char *name;
  const char *male;
  const char *female;
  const char *child;
  const char *quote;

  Odor odor;

  Voice voice;
  Skill sk_chat;
  int dc_chat;

	Stats stats;

  const CombatValues *cv_pointer;
  
  InnateAttack innate_attack;      // Type(s) of attack penetration
  int          innate_penetration; // Penetration level for attacks
  AttackDamage innate_damage;      // Type of damage sustained
  int          innate_lethality;   // Damage level

  Volume volume;

	int image;

	char corpse_pc, blood_pc; // _pc == percentage to drop on death
  materialClass element;
	materialClass blood;

  Map::levType habitat;
  Theme theme;

  unsigned char num_generated;
  unsigned char num_killed;

  Appetite appetite;
  Thirst thirst;

 // Interface:
	static creature *List;

	creature( const char *n, const char *m, const char *f, const char *c,
              Odor od, Voice v, Skill sk, int chat,
              Appetite ap, Thirst dr,

            char crd, char per, char str, char tuf,
						char wil, char kno, char con, char spd,
						char hp,

            const CombatValues *cv,

            long i_a, int i_p, AttackDamage i_d, int i_l,
            Volume vol,
						int img,
						char c_pc, char b_pc,
						materialClass el,
						materialClass bl,
            Map::levType h,
            Theme th,
            const char *q ) {

		name = n;
    male = m;
    female = f;
    child = c;
    quote = q;
    
    odor = od;
    voice = v;
    dc_chat = chat;
    sk_chat = sk;

    appetite = ap;
    thirst = dr;
    
		stats.crd = crd;
		stats.per = per;
		stats.str = str;
		stats.tuf = tuf;
		stats.wil = wil;
		stats.kno = kno;
		stats.con = con;
		stats.spd = spd;
		stats.hp  = hp;

    cv_pointer = cv;
    
    innate_attack = (InnateAttack)i_a;
    innate_penetration = i_p;
    innate_damage = i_d;
    innate_lethality = i_l;

    volume = vol;

		image = img;

		corpse_pc = c_pc;
		blood_pc = b_pc;
    element = el;
		blood = bl;
    
    habitat = h;
    theme = th;

    num_generated = 0;
    num_killed = 0;
		}


	};


#define FIRST_HUMAN crHuman
#define  LAST_HUMAN crHuman

#define FIRST_MOUNT crBicycle
#define  LAST_MOUNT crMechBull

#define FIRST_MONSTER crCat
#define  LAST_MONSTER crWraith

enum CreatureType {

	//--- Humans
  
	crHuman,

  //--- Mounts
  crBicycle,
  crMechBull,

  //--- Monsters
	// Animals

		// Domestic
		crCat,
		crDog,

		// Farm
		crCow,
		crHorse,
		crPig,
		crChicken,
		crSheep,

		// Game
		crDeer,
		crDuck,
		crGoat,
    crBuffalo,

		// Predators
		crGrizzlyBear,
		crBlackBear,
    crMeggieBear,
    crCougar,
		crSnake,
    crRattle,
    crWolf,
    crCoyote,
    crEagle,
    crVulture,
    crCrow,
    crWolverine,

		// Vermin
		crRat,
		crPigeon,
		crSeagull,

		// Lizards
		crIguana,
		crCrocodile,
    crGila,

		// Arachnids
		crTarantula,
		crScorpion,

    // Misc.
    crPorcupine,
    crArmadillo,
    crRabbit,
    crSkunk,


	// Dangerous Plants
	crTriffid,
	crVenusaur,
  crCactusman,
  crThistleman,

  // Somewhat Generic Undead/Demons
    crKasha,
    crKuangShi,
    crZombi,
    crWyrdalak,
    crRevenant,
    crDemon,
    crDustDevil,
    crShadow,

  // Somewhat Generic Mutants
    crDeviant,
    crMiscreant,
    crFreak,
    crAbortion,
    crBlackFetus,
    crWhiteHorror, // u. P.D. from 'Petey'

  // Random creatures
    crDustSoldier,

    crBoilerman,

  // Mimic:
    crMimic,

  // Morlocks:
    crMorlock,
    crMorlockFighter,
    crMorlockCook,
    crMorlockThrummer,
    crMorlockChief,

  // From Oz
  	crBronzeMan,
  	crTinMan,  // u. The Tin Woodsman
  	crClockworkMan,
  	crSteelMan,
  	crIronGiant,

    crAcidWorm,
    crAcidSlug,
    crAcidLeech,

    crScarecrow, // u. Scarecrow

  // Gorey
    // Demons
    crBlackDoll, // u. Figbash and The Black Doll
    crRagDoll,   // u. Skrump
    crLaceDoll,  // u. Naeelah
    crDeadToy,   // u. Hooglyboo

  // The Dark Tower
    crTaheen,
    // crNotMan,
    crSlowMutant,
    crLobstrosity,
    // crServomechanism,

  // H. P. Lovecraft, Mythos
    crDeepOne,   // u. Dagon  -> The Shadow over Innsmouth

    crWhiteApe,  // -> Facts Concerning the Late Arthur Jermyn and His Family

    crArchaean,  // aka ELDER THINGS  -> At the Mountains of Madness
    crShoggoth,  // -> At the Mountains of Madness

    crMiGo,      // -> The Whisperer in Darkness

  // Robert W. Chambers
    crGhoul,   // u. Graveworm
    crWraith,  // u. The King in Yellow

	crNum,
	crInvalid = crNum
	};

#endif // CRLIST_HPP
