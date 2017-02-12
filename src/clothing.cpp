// [clothing.cpp]
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
#if !defined ( CLOTHING_CPP )
#define CLOTHING_CPP

#include <cstring> // Sz. Rutkowski 11.02.2017

#include "clothing.hpp"
#include "random.hpp"
#include "grammar.hpp"

struct ClothingStats {

	const char *desc;
	const char *name;
  long price;
	Slot slot;

  int weight;

  // Defensive armor values:
  int def_pierce,  // Stiletto, bullet, stabbing weapons
      def_bash,    // Club, staff
      def_cut,     // Short Sword, slicing weapons
      def_chop,    // Axe, machete, severing weapons
      def_crush,   // Snake Constriction, enveloping attack
      def_splash,  // Pouring a bottle of poison, spit attacks
      def_vapor;   // Gas attacks, meant to be inhaled or affect eyes

	materialClass material;
	bool pair;

  Theme theme;

  int special;

  int appearance;
  
  Skill skill;
  int sk_bonus;

  Gender gender;

	static int numClothing;

	} clothing_stats [] = {

/* Defensive values for clothing:
 *
 *   pierce    rare       armour of proof/plate
 *   bash      uncommon   padded clothing is best
 *   cut       common     the thicker/stronger material the better
 *   chop      rare       plate, chain mail
 *   crush     rare       shell armour, carapace, helms
 *   splash    common     thick weave or solid plastic/metal is best
 *   vapor     rare       facewear, mouth or eye coverings
 *
 */


// s_head                                      p b c c c s v
 	{"bearskin ", "cap~", Cents(50), s_head,  6, 0,0,0,0,0,1,0,  m_fur,     false, th_none, cl_warm, +0, sk_invalid, 0, sex_any },
	{NULL, "bowler~",     Cents(75), s_head,  6, 0,0,0,0,0,1,0,  m_wool,    false, th_none, cl_none, +0, sk_invalid, 0, sex_any },
	{"cowboy ", "hat~",   Cents(60), s_head,  5, 0,0,0,0,0,1,0,  m_wool,    false, th_none, cl_none, +0, sk_invalid, 0, sex_any },
	{"forage ", "cap~",   Cents(40), s_head,  6, 0,0,0,0,0,1,0,  m_cotton,  false, th_none, cl_none, +0, sk_invalid, 0, sex_any },
  {NULL, "headdress~",  Cents(10), s_head,  5, 0,0,0,0,0,1,0,  m_leather, false, th_none, cl_none, +2, sk_invalid, 0, sex_any },
	{"pith ", "helmet~", Cents(120), s_head,  6, 1,1,2,1,1,1,0,  m_cotton,  false, th_none, cl_none, +0, sk_invalid, 0, sex_any },
	{NULL, "sombrero~",   Cents(60), s_head,  8, 0,0,0,0,0,2,0,  m_wool,    false, th_none, cl_none, +0, sk_invalid, 0, sex_any },
	{"stovepipe ","hat~",Cents(105), s_head,  8, 0,0,0,0,0,1,0,  m_wool,    false, th_none, cl_none, +1, sk_invalid, 0, sex_any },
	{"top ", "hat~",      Cents(95), s_head,  6, 0,0,0,0,0,1,0,  m_wool,    false, th_none, cl_none, +1, sk_invalid, 0, sex_any },
  {"steel ", "helm~",  Dollars(3), s_head, 25, 2,2,2,2,2,1,0,  m_steel,   false, th_none, cl_none, +0, sk_invalid, 0, sex_any },
	{"wool ", "cap~",     Cents(40), s_head,  6, 0,0,0,0,0,1,0,  m_wool,    false, th_none, cl_warm, +0, sk_invalid, 0, sex_any },

  // Todo: headscarf and bandanna should be merged into the same item, to be worn in either slot!
  {NULL, "headscarf~",  Cents(20), s_head,  1, 0,0,0,0,0,1,0,  m_cotton,  false, th_none, cl_none, -1, sk_invalid, 0, sex_any },
  
  //ten-gallon hat
  //cottage bonnet
  //straw hat
  //sunbonnet

//	s_face                                      p b c c c s v
	{NULL, "bandana~",     Cents(20), s_nose,  1, 0,0,0,0,0,1,3,   m_cotton,  false, th_none, cl_nose, -1, sk_invalid, 0, sex_any },
	{NULL, "eyeglasses",   Cents(80), s_eyes,  1, 0,0,0,0,0,2,1,   m_glass,   true,  th_none, cl_eyes, +0, sk_awar, 2, sex_any },
	{"gas ", "mask~",     Dollars(2), s_face, 15, 0,0,0,0,0,3,8,   m_steel,   false, th_none, cl_nose|cl_eyes, +0, sk_invalid, 0, sex_any },
	{NULL, "goggles",     Cents(120), s_eyes,  2, 0,0,0,0,0,3,4,   m_glass,   true,  th_none, cl_eyes, +0, sk_awar, 1, sex_any },
	{NULL, "monacle~",     Cents(50), s_eyes,  1, 0,0,0,0,0,1,2,   m_glass,   false, th_none, cl_none, +1, sk_awar, 1, sex_any },

//	s_neck                                               p b c c c s v
	{"neck ", "scarf~",              Cents(40), s_neck, 1, 0,0,0,0,0,1,0,   m_wool,   false, th_none, cl_nose|cl_warm, +0, sk_invalid, 0, sex_any },
	{"white aviator ", "scarf~",     Cents(40), s_neck, 1, 0,0,0,0,0,2,0,   m_wool,   false, th_none, cl_nose|cl_warm, +0, sk_invalid, 0, sex_any },
	{NULL, "cravat~",                Cents(55), s_neck, 1, 0,0,0,0,0,0,0,   m_cotton, false, th_none, cl_none, +2, sk_invalid, 0, sex_any },
	{"neck", "tie~",                 Cents(40), s_neck, 1, 0,0,0,0,0,0,0,   m_cotton, false, th_none, cl_none, +1, sk_invalid, 0, sex_any },
	{NULL, "gorget~",               Cents(150), s_neck, 1, 2,2,2,2,4,1,0,   m_steel,  false, th_none, cl_none, +0, sk_invalid, 0, sex_any },
	{"royal ","pendant~",            Cents(40), s_neck, 1, 0,0,0,0,0,0,0,   m_silver, false, th_none, cl_none, +2, sk_dipl, 2, sex_any },
	{"string~ of ", "rosary beads", Cents(240), s_neck, 1, 0,0,0,0,0,0,0,   m_stone,  false, th_none, cl_none, +2, sk_fait, 2, sex_any },
// , +2 faith
  
//	s_coat
// Coats:                                       p b c c c s v
	{"frock ", "coat~",   Cents(595), s_coat, 25, 0,0,0,0,0,3,0,   m_wool,   false, th_none, cl_none, +1, sk_invalid, 0, sex_any },
	{"lab ", "coat~",     Cents(250), s_coat, 10, 0,0,0,0,0,5,0,   m_cotton, false, th_none, cl_none, +0, sk_invalid, 0, sex_any },
	{NULL, "poncho~",     Cents(150), s_coat, 15, 0,0,0,0,0,4,0,   m_wool,   false, th_none, cl_warm, +0, sk_invalid, 0, sex_any },
	{"over", "coat~",     Cents(945), s_coat, 25, 0,1,0,0,0,4,0,   m_wool,   false, th_none, cl_warm, +0, sk_invalid, 0, sex_any },
	{"trench", "coat~",   Cents(450), s_coat, 25, 0,2,0,0,0,4,0,   m_wool,   false, th_none, cl_warm, +0, sk_invalid, 0, sex_any },
  //serape
  //coonskin coat
  //shooting coat

// Wraps:                                       p b c c c s v
	{"opera ", "cloak~",  Cents(690), s_coat, 15, 1,0,0,0,0,3,0,   m_wool, false, th_none, cl_warm, +0, sk_invalid, 0, sex_any },
	{"ugly ", "cloak~",   Cents(120), s_coat, 25, 1,1,1,0,0,2,0,   m_wool, false, th_none, cl_none, -2, sk_invalid, 0, sex_any },
	{"heavy ","robe~",    Cents( 80), s_coat, 30, 1,2,1,0,0,2,0,   m_wool, false, th_none, cl_none, +0, sk_invalid, 0, sex_any },
  //cottage cloak
  //gypsy cloak
  //matchcoat
  //spanish cloak
  
//	s_body                                                 p b c c c s v
	{NULL, "cuirass~",             Dollars(12), s_jack, 150, 5,3,5,4,4,2,0,   m_steel,   false, th_none, cl_none, +0, sk_invalid, 0, sex_any },
	{NULL, "waistcoat~",            Cents(350), s_jack,  10, 0,1,0,0,0,1,0,   m_cotton,  false, th_none, cl_none, +1, sk_invalid, 0, sex_any },
	{"leather ", "jacket~",         Cents(350), s_jack,  15, 0,0,2,0,0,2,0,   m_leather, false, th_none, cl_warm, -1, sk_invalid, 0, sex_any },
  {"heavy reefer ", "jacket~",    Cents(150), s_jack,  30, 1,2,2,0,2,2,0,   m_wool,    false, th_none, cl_warm, +0, sk_invalid, 0, sex_any },
  {"aviator ", "jacket~",         Cents(250), s_jack,  25, 0,2,1,0,0,3,0,   m_leather, false, th_none, cl_warm, +0, sk_invalid, 0, sex_any },
  //camisole
  //dinner jacket
  //smoking jacket
  //judo coat

//	s_shirt                                       p b c c c s v
	{"dress ", "shirt~",     Cents(95), s_shirt, 5, 0,1,0,0,0,1,0,   m_cotton, false, th_all, cl_none, +1, sk_invalid, 0, sex_any },
	{"drawstring ","shirt~", Cents(95), s_shirt, 5, 0,1,1,0,0,1,0,   m_cotton, false, th_all, cl_none, +0, sk_invalid, 0, sex_any },
	{"sport ", "shirt~",     Cents(45), s_shirt, 5, 0,1,0,0,0,1,0,   m_cotton, false, th_none, cl_none, +0, sk_invalid, 0, sex_any },
	{"work ", "shirt~",      Cents(99), s_shirt, 6, 0,1,1,0,0,1,0,   m_cotton, false, th_workshop, cl_none, +0, sk_invalid, 0, sex_any },
	{"halter ", "blouse~",  Cents(150), s_shirt, 5, 0,1,0,0,0,1,0,   m_cotton, false, th_none, cl_none, +1, sk_invalid, 0, sex_female },
  //sash blouse - f
  //hunting shirt
  //drawstring shirt
  
//	s_arms                                     p b c c c s v
	{NULL, "wristwatch~", Cents(250), s_arms, 1, 0,0,0,0,0,0,0,   m_steel, false, th_none, cl_time, +0, sk_invalid, 0, sex_any },
	{NULL, "bracers",     Dollars(8), s_arms, 5, 2,2,4,2,1,1,0,   m_steel, true,  th_none, cl_none, +0, sk_invalid, 0, sex_any },
	{"bazu ", "bands",    Dollars(4), s_arms, 3, 1,1,3,1,1,1,0,   m_steel, true,  th_none, cl_none, +0, sk_invalid, 0, sex_any },

//	s_hands                                       p b c c c s v
	{"steel ", "gauntlets",Dollars(6), s_gaunt, 25, 2,0,2,1,1,1,0,   m_steel,   true, th_none, cl_none, +0, sk_invalid, 0, sex_any },
	{"leather ", "gloves",  Cents(40), s_hands,  4, 0,1,1,0,0,1,0,   m_leather, true, th_workshop, cl_none, +0, sk_invalid, 0, sex_any },
	{"white ", "gloves",    Cents(80), s_hands,  2, 0,0,0,0,0,1,0,   m_cotton,  true, th_none, cl_none, +1, sk_invalid, 0, sex_any },
	{"rubber ", "gloves",   Cents(60), s_hands,  2, 0,0,0,0,0,3,0,   m_rubber,  true, th_none, cl_none, +0, sk_invalid, 0, sex_any },
  //riding gloves
  //wool gloves
  //wool mittens

// s_suit
// Dresses:                                       p b c c c s v
	{"apron ", "dress~",    Dollars(4), s_suit, 10, 0,0,0,0,0,5,0,   m_cotton, false, th_none, cl_none, +0, sk_invalid, 0, sex_female },
	{"mourning ", "dress~", Dollars(9), s_suit, 10, 0,0,0,0,0,3,0,   m_cotton, false, th_none, cl_none, +2, sk_invalid, 0, sex_female },
  //sleeveless dress - f
  //cocktail dress - f
  //evening gown - f
  //kimono dress - f
  //knitted dress - f
  
// Suits:                                         p b c c c s v
	{NULL, "overalls",      Cents(345), s_suit, 14, 0,0,1,0,0,5,0,   m_wool,   true,  th_workshop, cl_none, +0, sk_invalid, 0, sex_any },
	{"safari ", "suit~",    Cents(625), s_suit, 16, 0,0,1,0,0,2,0,   m_cotton, false, th_none, cl_none, +0, sk_stea,    2, sex_any },
	{"shabby ", "suit~",    Cents(150), s_suit, 18, 0,1,0,0,0,3,0,   m_cotton, false, th_none, cl_none, -1, sk_invalid, 0, sex_any },
	{"imperial ","uniform~",Cents(150), s_suit, 20, 0,0,2,0,0,3,0,   m_cotton, false, th_none, cl_none, +1, sk_invalid, 0, sex_any },
	{"tailored ", "suit~", Cents(1250), s_suit, 18, 0,0,0,0,0,3,0,   m_cotton, false, th_none, cl_none, +2, sk_invalid, 0, sex_any },
  //bolero suit
  
//	s_legs
// Skirts:                                          p b c c c s v
	{"pleated ", "skirt~",    Cents(450), s_legs,  6, 0,0,0,0,0,1,0,   m_cotton, false, th_none, cl_none, +0, sk_invalid, 0, sex_female },
  //bell skirt - f
  //broom skirt - f
  //divided skirt - f
  //parasol skirt - f
  //riding skirt - f
  
// Trousers:                                       p b c c c s v
	{"corduroy ", "pants",    Cents(250), s_legs,10, 0,1,0,0,0,2,0,   m_cotton, true, th_none, cl_none, +0, sk_invalid, 0, sex_any },
	{"leather ", "pants",     Cents(150), s_legs,10, 0,1,1,0,0,3,0,   m_leather,true, th_none, cl_none, +0, sk_invalid, 0, sex_any },
	{"buckskin ", "pants",    Cents( 75), s_legs,10, 0,1,1,0,0,2,0,   m_leather,true, th_none, cl_none, +0, sk_invalid, 0, sex_any },
	{"black ", "trousers",    Cents(450), s_legs, 8, 0,0,0,0,0,2,0,   m_cotton, true, th_none, cl_none, +0, sk_invalid, 0, sex_any },
 	{"khaki ", "trousers",    Cents(650), s_legs, 8, 0,0,0,0,0,2,0,   m_cotton, true, th_none, cl_none, +0, sk_stea,    1, sex_any },
	{"knee", "pants",         Cents(350), s_legs, 6, 0,0,0,0,0,1,0,   m_cotton, true, th_none, cl_none, +0, sk_invalid, 0, sex_any },
	{NULL, "jeans",           Cents(250), s_legs,10, 0,0,1,0,0,2,0,   m_cotton, true, th_none, cl_none, +0, sk_invalid, 0, sex_any },
	{"faded ", "jeans",       Cents(150), s_legs, 9, 0,0,1,0,0,2,0,   m_cotton, true, th_none, cl_none, +0, sk_invalid, 0, sex_any },
 	{"striped ", "trousers",  Cents(350), s_legs, 8, 0,0,0,0,0,2,0,   m_cotton, true, th_none, cl_none, +0, sk_invalid, 0, sex_any },
	{"service ", "breeches",  Cents(400), s_legs, 8, 0,0,1,0,0,1,0,   m_cotton, true, th_none, cl_none, +1, sk_invalid, 0, sex_any },

//	s_feet                                          p b c c c s v
	{"spurred ", "boots",      Cents(50), s_feet, 18, 0,0,1,0,0,1,0,   m_leather, true, th_none, cl_feet, +0, sk_invalid, 0, sex_any },
	{"work ", "boots",         Cents(95), s_feet, 20, 0,1,1,0,0,2,0,   m_leather, true, th_workshop, cl_feet, +0, sk_invalid, 0, sex_any },
	{"understudded ", "boots",Cents(105), s_feet, 25, 0,1,2,0,0,2,0,   m_leather, true, th_workshop, cl_feet, +0, sk_invalid, 0, sex_any },
	{"knee", "boots",         Cents(435), s_feet, 25, 0,2,1,0,0,3,0,   m_leather, true, th_none, cl_feet, +0, sk_invalid, 0, sex_any },
	{NULL, "moccasins",        Cents(60), s_feet, 16, 0,1,0,0,0,1,0,   m_leather, true, th_none, cl_warm|cl_feet, +0, sk_invalid, 0, sex_any },
	{NULL, "boots",           Cents(150), s_feet, 20, 0,1,1,0,0,1,0,   m_leather, true, th_none, cl_feet, +0, sk_invalid, 0, sex_any },
	{"leather ", "sandals",    Cents(40), s_feet, 10, 0,0,0,0,0,0,0,   m_leather, true, th_none, cl_feet, +0, sk_invalid, 0, sex_any },
  //sandals
  //slippers
  //snow boots
  //heeled boots - f

  };

int ClothingStats::numClothing = 74;

// pair of gauntlets + pistol x2 = pair of gunfists
// pair of bazu bands + Bow(x)   = pair of bazu bows
// pair of bracers + Bow(x)      = pair of wristbows

targetType Clothing::gettype( void ) const {
	return tg_clothing;
	}

bool Clothing::istype( targetType t ) const {

	if ( t == tg_clothing )
		return true;
	else return Buildable::istype(t);

	}

compval Clothing::compare( RawTarget &rt ) {

	if ( rt.istype( gettype() ) ) {

		if ( clothing_stats[form].slot > clothing_stats[rt.getVal( attrib_form )].slot ) return k_larger;
		if ( clothing_stats[form].slot < clothing_stats[rt.getVal( attrib_form )].slot ) return k_smaller;
		return k_same;

		}

	return Buildable::compare(rt);

	}


bool Clothing::capable( Action a, const Target &t ) {

	switch( a ) {
		case aBeWorn:
			return true;
    case aTellTime:
      return (clothing_stats[form].special&cl_time)?true:false;
		default:
			return Buildable::capable(a, t);
		}
	}

bool Clothing::perform( Action a, const Target &t ) {

	switch( a ) {
    case aPolymorph:
      form = Random::randint( ClothingStats::numClothing );
    	image.val.color = Material::data[ clothing_stats[form].material ].color;
      return Buildable::perform(a, t);
		default:
			return Buildable::perform(a, t);
		}

	}

long Clothing::getSkill( Skill skill, bool /*get_total*/ ) {


  if ( clothing_stats[form].skill == skill )
    return clothing_stats[form].sk_bonus;

  return 0;

  }


long Clothing::getVal( StatType s ) {

	switch( s ) {

    case attrib_price:
      return clothing_stats[form].price;
    case attrib_gender:
      return clothing_stats[form].gender;
    case attrib_weight:
    case attrib_sink_weight:
      // Maybe some clothes should float?  Life jacket?
      return clothing_stats[form].weight;
      
		case attrib_slot:
			return clothing_stats[form].slot;

		case attrib_plural:
		case attrib_pair:
			return clothing_stats[form].pair;

		case attrib_form:
			return form;

		default:
			return Buildable::getVal(s);
		}

	}

Clothing::Clothing( const Option *o )
: Buildable(o) {

	if ( o->slot == s_none )
    doPerform(aPolymorph);
	else for ( int i = 0; i < NUM_ABORT; ++i ) {
    doPerform(aPolymorph);
		if (clothing_stats[form].slot == o->slot
       && (clothing_stats[form].theme & o->theme)
       && (o->gender & clothing_stats[form].gender) ) break;
		}

	}

Clothing::Clothing( Clothing &rhs )
: Buildable(rhs) {

	form = rhs.form;
	}

Clothing::Clothing( std::iostream &inFile, TargetValue *TVP )
// Read Buildable base information:
: Buildable (inFile, TVP) {

	// Read char form
	inFile.read( (char*) &form, sizeof(form) );


	}

void Clothing::toFile( std::iostream &outFile ) {

	// Write Buildable base information:
	Buildable::toFile( outFile );

	// Write char form
	outFile.write( (char*) &form, sizeof(form) );


	}

Target Clothing::clone( void ) {

	Clothing *newcopy = new Clothing( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

bool Clothing::equals( const Target &rhs ) {

	bool retVal = Buildable::equals( rhs );

	if (retVal)
		// Good so far - same inventories and both of same type.
		if ( form != ( (Clothing*) rhs.raw() )->form ) retVal = false;

	return retVal;

	}


String Clothing::name( NameOption n_opt ) {
  switch (n_opt) {
    case n_short:
    	return String("#")
             +clothing_stats[form].name
             + Buildable::name(n_opt);
             
    default: {
      String desc = clothing_stats[form].desc;
    	return "#"
             +desc
             +clothing_stats[form].name
             + Buildable::name(n_opt);
      }
    }
	}


bool Clothing::getStruct( structType type, void *buffer, size_t size ) {

  switch (type) {
    // Clothing gives the same bonuses in melee and dodging ranged attacks:
    case s_dodge_cv:
    case s_defence_cv: {
      // Get the clothing's defensive values:
    
      // Make sure that there's the exact amount of space:
      assert( size == sizeof(CombatValues) );
      CombatValues cv;

      // Fill cv with combat values

      // Defense:
      cv.defence[pen_pierce] = clothing_stats[form].def_pierce;
      cv.defence[pen_bash]   = clothing_stats[form].def_bash;
      cv.defence[pen_cut]    = clothing_stats[form].def_cut;
      cv.defence[pen_chop]   = clothing_stats[form].def_chop;
      cv.defence[pen_crush]  = clothing_stats[form].def_crush;
      cv.defence[pen_splash] = clothing_stats[form].def_splash;
      cv.defence[pen_vapor]  = clothing_stats[form].def_vapor;

      // Todo: Resists:

      // Copy our CombatValues to the buffer:
      memcpy( buffer, (const void*) &cv, size );
      
      return true;
      }

    default:
      return false;
    }

  }


#endif // CLOTHING_CPP
