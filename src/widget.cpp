// [widget.cpp]
// (C) Copyright 2003 Michael Blackney
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
#if !defined ( WIDGET_CPP )
#define WIDGET_CPP

#include "widget.hpp"
#include "message.hpp"
#include "random.hpp"
#include "grammar.hpp"
#include "player.hpp"
#include "level.hpp"
#include "skill.hpp"
#include "weapon.hpp"
#include "menu.hpp"

//--[ class MachinePart ]--//

struct MachinePartStats {

  const char *desc;
  const char *name;
  
  char color, appearance;
  
  int quality;     // How much it improves installed weapon

  // Additional properties conveyed
  AttackPenetration pen_type;
  int pen_bonus;
	AttackDamage dam_type;
  int dam_bonus;

  bool known;
  };

MachinePartStats machinePartStats [ w_num ] = {

  { "firearm",     "crest",     cLGreen,'~',  +1, pen_invalid,0, dam_invalid,0, true  },
  { "metal",       "spike",     cViolet,'~',  +1, pen_invalid,0, dam_invalid,0, true  },
  { "metal",       "stud",      cViolet,'*',  +1, pen_invalid,0, dam_invalid,0, true  },

  { NULL,          "gear",      cGrey,  '*',  +0, pen_invalid,0, dam_invalid,5, true  },
  { NULL,          "chainwheel",cWhite, '*',  +0, pen_invalid,0, dam_invalid,5, true  },
  { NULL,          "piston",    cDGrey, '~',  +0, pen_invalid,0, dam_invalid,5, true  },
  { "scrap",       "metal",     cDGrey, '*',  +0, pen_invalid,0, dam_invalid,5, true  },

  { NULL,          "chain",     cDGrey, ')',  +0, pen_chop,  +4, dam_invalid,0, true  },
  { "buzzsaw",     "blade",     cGrey,  ')',  +0, pen_cut,   +5, dam_invalid,0, true  },
  { "spark",       "plug",      cViolet,'*',  +0, pen_cut,   +2, dam_purity,+4, true  },
  { "blasting",    "cap",       cViolet,'*',  +0, pen_cut,   +2, dam_purity,+4, true  },
  { "antnomic",    "cylinder",  cLBlue, '*',  +0, pen_invalid,0, dam_invalid,0, false },
  { "drill",       "bit",       cLRed,  '~',  +0, pen_invalid,0, dam_invalid,0, true  },
  { "tesla",       "coil",      cYellow,'~',  +0, pen_invalid,0, dam_elec,  +5, false },

  { "revolver",    "chamber",   cBlue,  '}',  +0, pen_invalid,0, dam_invalid,0, true  },
  { "magfeed",     "chamber",   cIndigo,'}',  +0, pen_invalid,0, dam_invalid,0, false },
  { "breech-load", "chamber",   cGreen, '}',  +0, pen_invalid,0, dam_invalid,0, true  },
  { "gatling",     "chamber",   cOrange,'}',  +0, pen_invalid,0, dam_invalid,0, false },

  };

bool MachinePart::getStruct(  structType type, void *buffer, size_t size ) {

  switch (type) {
    case s_offence_cv: {
      // Get the machine_part's offensive values:
    
      // Make sure that there's the exact amount of space:
      assert( size == sizeof(CombatValues) );
      CombatValues cv;

      // Fill cv with combat values:

      // ATM machine_parts only can give a bonus to one penetration type
      // and one damage type each.  This may change.
      
      // Penetration:
      if (machinePartStats[form].pen_type != pen_invalid )
        cv.defence[machinePartStats[form].pen_type] = machinePartStats[form].pen_bonus;

      // Damage:
      if (machinePartStats[form].dam_type != dam_invalid )
        cv.defence[machinePartStats[form].dam_type] = machinePartStats[form].dam_bonus;

      // Copy our CombatValues to the buffer:
      memcpy( buffer, (const void*) &cv, size );
      
      return true;
      }

    default:
      return false;
    }

  }


targetType MachinePart::gettype( void ) const {
	return tg_machine_part;
	}

bool MachinePart::istype( targetType t ) const {

	if ( t == tg_machine_part )
		return true;
	else return Item::istype(t);

	}

bool MachinePart::capable( Action a, const Target &t ) {

	switch( a ) {

		default:
			return Item::capable(a, t);
		}
	}

bool MachinePart::perform( Action a, const Target &t ) {

	switch( a ) {
    case aPolymorph:
      form = (PartType) Random::randint(w_num);

      image = Image( machinePartStats[form].color,
                     machinePartStats[form].appearance );

      return Item::perform(a,t);

		default:
			return Item::perform(a, t);
		}

	}

long MachinePart::getVal( StatType s ) {
	switch(s) {
    case attrib_price:
      return Dollars(1);
    case attrib_sort_category:
      return Item::getVal(s) + form;
    case attrib_disposable:
      return (form == w_ranged);
    case attrib_form:
      return form;
		default:
			return Item::getVal(s);
		}
	}

long MachinePart::getSkill( Skill skill, bool ) {
// MachinePart currently can only add to fighting skill:
  if (skill == sk_figh)
    return machinePartStats[form].quality;
  
  return 0;

  }

MachinePart::MachinePart( const Option *o )
: Item(o) {
  form = o->form;

  if (form >= w_num)
    doPerform(aPolymorph);

  image = Image( machinePartStats[form].color,
                 machinePartStats[form].appearance );
  
	}

MachinePart::MachinePart( MachinePart &rhs )
: Item(rhs) {

	form = rhs.form;
	}

MachinePart::MachinePart( std::iostream &inFile, TargetValue *TVP )
// Read Item base information:
: Item (inFile, TVP) {

	inFile.read( (char*)&form, sizeof(form) );

	}

void MachinePart::toFile( std::iostream &outFile ) {

	// Write Item base information:
	Item::toFile( outFile );

	outFile.write( (char*)&form, sizeof(form) );

	}

Target MachinePart::clone( void ) {

	MachinePart *newcopy = new MachinePart( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

bool MachinePart::equals( const Target &rhs ) {

	bool retVal = Item::equals( rhs );

	if (retVal) {
		// Good so far - same inventories and both of same type.
		if ( form != ( (MachinePart*) rhs.raw() )->form ) retVal = false;
		}

	return retVal;

	}

Attack MachinePart::getAttack( void ) {

  CombatValues c_values;

  c_values.penetration[ machinePartStats[form].pen_type ]
    = machinePartStats[form].pen_bonus;

  c_values.damage[ machinePartStats[form].dam_type ]
    = machinePartStats[form].dam_bonus;
                 
  return Attack( c_values,
                 Target(),
								 Target(),
                 NULL
							 );
  }

String MachinePart::name( NameOption n_opt ) {

  String adjective, noun;

  bool no_space = false;
  
  noun = machinePartStats[form].name;

  if (  machinePartStats[form].known
     && machinePartStats[form].desc  )
    adjective = String(machinePartStats[form].desc) + (no_space?"":" ");

  switch (n_opt) {
    default:
      return "#" + adjective + noun + "~" + Item::name(n_opt);

    }
    
	}

void MachinePart::identify( IDmask id_mask ) {

  if (id_mask & id_form)
    machinePartStats[form].known = true;

  Item::identify(id_mask);
  }
  
bool MachinePart::identified( IDmask id_mask ) {
  IDmask override = id_none;

  if (machinePartStats[form].known)
    (int&) override |= (int)id_form;

  return Item::identified( (IDmask) (id_mask & ~override) );
  }
  
//--[ class Remains ]--//
bool Remains::getStruct(  structType type, void *buffer, size_t size ) {

  switch (type) {
    case s_offence_cv: {
      // Get the remains's offensive values:
    
      // Make sure that there's the exact amount of space:
      assert( size == sizeof(CombatValues) );
      CombatValues cv;

      // Fill cv with combat values:

      // ATM remainss only can give a bonus to one penetration type
      // and one damage type each.  This may change.

      /*
      // Penetration:
      if (remainsStats[form].pen_type != pen_invalid )
        cv.defence[remainsStats[form].pen_type] = remainsStats[form].pen_bonus;

      // Damage:
      if (remainsStats[form].dam_type != dam_invalid )
        cv.defence[remainsStats[form].dam_type] = remainsStats[form].dam_bonus;
      */

      // Copy our CombatValues to the buffer:
      memcpy( buffer, (const void*) &cv, size );
      
      return true;
      }

    default:
      return false;
    }

  }


targetType Remains::gettype( void ) const {
	return tg_remains;
	}

bool Remains::istype( targetType t ) const {

	if ( t == tg_remains )
		return true;
	else return Item::istype(t);

	}

bool Remains::capable( Action a, const Target &t ) {

	switch( a ) {

		default:
			return Item::capable(a, t);
		}
	}

bool Remains::perform( Action a, const Target &t ) {

	switch( a ) {
    case aPolymorph:
      form = (CreatureType) Random::randint(crNum);

      image = Image( Image(creature::List[form].image).val.color, '~' );

      return Item::perform(a,t);

		default:
			return Item::perform(a, t);
		}

	}

long Remains::getVal( StatType s ) {
	switch(s) {
    case attrib_price:
      return Dollars(1);
    case attrib_sort_category:
      return Item::getVal(s) + form;
    case attrib_form:
      return form;
		default:
			return Item::getVal(s);
		}
	}

long Remains::getSkill( Skill skill, bool ) {
// Remains currently can only add to fighting skill:
  if (skill == sk_figh)
    return 1; //return remainsStats[form].quality;
  
  return 0;

  }

Remains::Remains( const Option *o )
: Item(o) {
  form = o->form;

  if (form >= crNum)
    doPerform(aPolymorph);

  image = Image( Image(creature::List[form].image).val.color, ';' );
 
	}

Remains::Remains( Remains &rhs )
: Item(rhs) {

	form = rhs.form;
	}

Remains::Remains( std::iostream &inFile, TargetValue *TVP )
// Read Item base information:
: Item (inFile, TVP) {

	inFile.read( (char*)&form, sizeof(form) );

	}

void Remains::toFile( std::iostream &outFile ) {

	// Write Item base information:
	Item::toFile( outFile );

	outFile.write( (char*)&form, sizeof(form) );

	}

Target Remains::clone( void ) {

	Remains *newcopy = new Remains( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

bool Remains::equals( const Target &rhs ) {

	bool retVal = Item::equals( rhs );

	if (retVal) {
		// Good so far - same inventories and both of same type.
		if ( form != ( (Remains*) rhs.raw() )->form ) retVal = false;
		}

	return retVal;

	}

Attack Remains::getAttack( void ) {

  CombatValues c_values;

  /*
  c_values.penetration[ remainsStats[form].pen_type ]
    = remainsStats[form].pen_bonus;

  c_values.damage[ remainsStats[form].dam_type ]
    = remainsStats[form].dam_bonus;
  */
                 
  return Attack( c_values,
                 Target(),
								 Target(),
                 NULL
							 );
  }

String Remains::name( NameOption n_opt ) {

  String adjective, noun;

  noun = "bone";

  adjective = creature::List[form].name;

  switch (n_opt) {
    default:
      return "#" + adjective + " " + noun + "~" + Item::name(n_opt);

    }
  
	}

void Remains::identify( IDmask ) {
/*
  if (id_mask & id_form)
    remainsStats[form].known = true;

  Item::identify(id_mask);
*/
  }
  
bool Remains::identified( IDmask ) {
/*
  IDmask override = id_none;

  if (remainsStats[form].known)
    (int&) override |= (int)id_form;

  return Item::identified( (IDmask) (id_mask & ~override) );
*/ return true;
  }
  

//-[ class Buildable ]-----------------

targetType Buildable::gettype( void ) const {
	return tg_buildable;
	}

bool Buildable::istype( targetType t ) const {
	if ( t == tg_buildable )
		return true;
	else return Item::istype(t);
	}

bool Buildable::capable( Action a, const Target &t ) {

  switch ( a ) {
    case aBuild:
      // Yes, you can build from a buildables:
      if (!t)
        return true;

      return false;

    default:
      return Item::capable(a,t);
    }
	}

struct ComboListItem {
  int combo_num;

  String menu;
  Target blade;
  char letter;
  
  String menustring( int len = 80 ) {
    return menu.abbrev(len);
    }
  Image menuletter() {
    return Image(cGrey,letter);
    }
  };



// COMBO MELEE WEAPONS:

// - BASE - + - BLADE -     + - MOTOR -
// Sword(x) + bicycle chain + PowerSupply(y) = y chainsword (steam chainsword)
// Axe(x)   + bicycle chain + PowerSupply(y) = y chainaxe (clockwork chainaxe)
// Melee(x) + tesla coil    + PowerSupply(y) = thunderx (thundersword, thunderspear)
// Melee(x) + venom sac                      = venom x (venom axe, venom knife)
// Melee(x) + archaean crystal               = powerx (powersabre, powershovel)
// Melee(x) + ether null kit                 = impulse x (impulse wrench)
// Onehnd(x)+ magneto                        = balanced x (balanced cleaver)
// Twohnd(x)+ magneto                        = impulse x (impulse axe)


struct MeleeComboStats {
  const char *prefix;
  bool use_verb;

  int difficulty;

  MeleeType mel_type;

  Skill skill;
  PartType widget;
  };

#define NUM_M_COMBO 9
const MeleeComboStats meleeComboStats [ NUM_M_COMBO ] = {

  // (BUZZSAW BLADE)
  // Sword -> Buzzsword -> Heavy Buzzsword
  { "heavy buzzsword",  false, 8, mel_sword, sk_mech, w_buzz },

  // (CHAIN)
  // Sword -> Chainsword -> Heavy Chainsword
  { "heavy chainsword", false, 7, mel_sword, sk_mech, w_chain },
  // Axe -> Chainaxe -> Heavy Chainaxe
  { "heavy chainaxe",   false, 7, mel_haft,  sk_mech, w_chain },

  // (SPARK PLUG)
  // Hammer -> Shock Hammer -> Thunderhammer
  { "thunder",     true,  9, mel_any,   sk_mech, w_spark },
  // (TESLA COIL)
  // Sword -> Energy Sword -> Power Sword
  { "power",       true,  8, mel_sword,   sk_mech, w_tesla },
  // Spear -> Energy Lance -> Arc Lance
  { "arc lance",   false,  8, mel_pole,   sk_mech, w_tesla },

  // (DRILL BIT)
  // Spear -> Drillspear -> Heavy Drillspear
  { "heavy drill", true,  8, mel_pole,   sk_mech, w_drill },
  
  // (BLASTING CAP)
  // Hammer -> Tremorhammer -> Quakehammer
  { "quake",       true,  8, mel_club,   sk_mech, w_blast },
  
  // (ANTNOMIC CYLINDER)
  // Sword -> Gammasword -> Blightsword
  { "blight",      true,  8, mel_any,   sk_mech, w_antnomic },

  };

/*

Damage types that weapons can take advantage of:
 pierce, crush, bash, cut,
 chop, purity, electricity,
 fire, poison, heal

------------
Technologies that weapons can take advantantage of:
 magnetism
 the ether
 clockwork
 steam
 electrics
 chemical
 atomic (or radiation)
 
*/

// Buildable experience gain:

long xpForLev [20] = {

      100,
      300,
      600,
     1000,
     1500,
     2100,
     2800,
     3600,
     4500,
     5500,
     6600,
     7800,
     9100,
    10500,
    12000,
    13600,
    15300,
    17100,
    19000,
    23000, };


void Buildable::gainExp( long xp ) {
// Gain xp experience points
// -> to be called when weapon hits opponent or
//    when armour blocks a blow.
  exp += xp;

  // See if we go to the next quality level:
  while ( exp >= xpForLev[quality] )
   ++quality;
  
  }

bool Buildable::perform( Action a, const Target &t ) {

  switch (a) {

    case aGainLev: {

      // Go to next level:
      exp = xpForLev[quality];
      ++quality;
      
      return true;
      }

    case aBuild: {
      // We need to have a creature from which to sample our parts:
      if (!t || !t->itemList())
        return false;

      // Build a list of possible inventions:
      // The list should look something like:
      //
      // --[ Build: a +1 sabre ]----------
      //   a a +2 sabre
      //   b a +1 clockwork chainsword
      //   d a +1 clockwork thundersabre
      //   f a +1 venom sabre
      //   g a +1 powersabre
      // ---------------------------------
      //
      // or
      //
      // --[ Build: a +1 steam chainsword ]--
      //   a a +2 steam chainsword
      //   b Dismantle
      // ------------------------------------

      // Our combo constants for types not in the combo list:
#     define COMBO_IMPROVE -1
#     define COMBO_DISMANTLE -2

      List<ComboListItem> combo_list;
      ComboListItem temp;
      temp.letter = 'a';
      temp.combo_num = COMBO_IMPROVE;
      bool found = false;
      int i;
      
      List<Target> &item_pool = *t->itemList();
      
      // Try to find a simple, one-use improver

      /*
      found = false;
      item_pool.reset();
      while (!found) {

        if ( item_pool.get()->istype(tg_machine_part)
          && item_pool.get()->getVal(attrib_form) == w_melee ) {
            found = true;
            temp.blade = item_pool.get();
            }
            
        if (!item_pool.next()) break;
        }

      if (found) {
        temp.menu = String("#+") + (long) (quality+1) + " "
                    +name( n_prefix )
                    +name( n_noun );
        temp.menu = Grammar::plural(temp.menu,1,false,false,Grammar::det_indefinite);
        combo_list.add(temp);
        temp.letter++;
        }
      */
      
      // If we are already built up, we can break down:
      if (!!blade ) {
        temp.menu = "Dismantle";
        temp.combo_num = COMBO_DISMANTLE;
        temp.blade = Target(NULL);
        //temp.motor = Target(NULL);
        combo_list.add(temp);
        temp.letter++;
        }
      // If we aren't built up, we can build:
      // For each item in the MeleeCombo list,
      else for (i = 0, temp.combo_num = 0;
                i < NUM_M_COMBO;
                ++i, ++temp.combo_num ) {
        // See if we are of a relevant weapon type:
        if ( getVal(attrib_meleetype) == meleeComboStats[i].mel_type
          || meleeComboStats[i].mel_type == mel_any ) {
          // Check that we have an appropriate widget:
          found = false;
          item_pool.reset();
          while (!found) {

            if ( item_pool.get()->istype(tg_machine_part)
              && item_pool.get()->getVal(attrib_form) == meleeComboStats[i].widget ) {
                found = true;
                temp.blade = item_pool.get();
                }
            
            if (!item_pool.next()) break;
            }
          
          if (found) {
            /* Motors removed:
            // If an engine is required,
            if (meleeComboStats[i].need_motor) {
              // For each type of engine,
              for (int fuel = fuel_none; fuel < fuel_num; ++fuel) {
                // Check that we have one of the engines:
                found = false;
                item_pool.reset();
                while (!found) {

                  if ( item_pool.get()->istype(tg_motor)
                    && item_pool.get()->getVal(attrib_fuel) == fuel ) {
                      found = true;
                      temp.motor = item_pool.get();
                      }
            
                  if (!item_pool.next()) break;
                  }

                if (found) {
                  // If so, add this combo:widget/engine to the list
                  temp.menu = String("#+") + (long) quality + " "
                              +temp.motor->name(n_prefix)
                              +meleeComboStats[i].prefix
                              +(meleeComboStats[i].use_verb?meleeStats::MeleeStats[form].name:"~");
                  temp.menu = Grammar::plural(temp.menu,1,false,false,Grammar::det_indefinite);
                  combo_list.add(temp);
                  temp.letter++;
                  }

                }

              }
            */
            // Else if no engine is required,
            //else {
              // Add this combo:widget to the list
              temp.menu = String("#+") + (long) quality + " "
                          +meleeComboStats[i].prefix
                          +(meleeComboStats[i].use_verb?"NAME_HERE":"~");
              temp.menu = Grammar::plural(temp.menu,1,false,false,Grammar::det_indefinite);
              //temp.motor = Target(NULL);
              combo_list.add(temp);
              temp.letter++;
            //  }
            }
          }
        }

      if (!combo_list.reset()) {
        return false;
        }
      
      Menu<ComboListItem> combo_menu(combo_list, global::skin.inventory);
      combo_menu.setTitle("Build: "+menustring());
      
      Command key = kInvalid;
      bool done = false;
      while (!done) {
        combo_menu.display();
        key = combo_menu.getCommand();
        switch (key) {
          case kEnter: {
            // Check to see if we are a skilled enough mechanic to
            // build this combination:
            temp = combo_menu.get();

            // Find which skill to use:
            // Assume mechanics:
            Skill sk_build = sk_mech;
            // If it's already build, skill depends on current combo:
            if (temp.combo_num == COMBO_DISMANTLE)
              sk_build = meleeComboStats[combo].skill;
            // else if we're building a combo, skill depends on new combo:
            if (temp.combo_num >= 0 && temp.combo_num < NUM_M_COMBO)
              sk_build = meleeComboStats[temp.combo_num].skill;
            // otherwise, go with the first assumption.

            // If we're using a power-up, can only improve to the same level
            // as our mechanics skill:
            if ( (temp.combo_num == COMBO_IMPROVE && quality < t->getSkill(sk_build,false))
            // If we're dismantling, can only dismantle if we are good
            // enough to build it in the first place:
              || (temp.combo_num == COMBO_DISMANTLE
               && t->getSkill(sk_build)
                 >= meleeComboStats[combo].difficulty )
            // Otherwise, we can build if our mech+kno is at least as high
            // as the combo difficulty:
              || t->getSkill(sk_build)
                 >= meleeComboStats[temp.combo_num].difficulty ) {

              if (temp.combo_num >= 0)
                combo = temp.combo_num;
              else if (temp.combo_num == COMBO_DISMANTLE) {
                combo = COMBO_IMPROVE;
                if (!!blade) {
                  t->perform(aContain,blade);
                  blade = Target(NULL);
                  }
                /*
                if (!!motor) {
                  t->perform(aContain,motor);
                  motor = Target(NULL);
                  }
                */
                }
              else if (temp.combo_num == COMBO_IMPROVE) {
                quality++;
                }
              if (!!temp.blade)
                if (temp.blade->getVal(attrib_disposable))
                  getTarget(tg_controller)->perform(aAddBill,temp.blade->detach(1));
                else
                  blade = temp.blade->detach(1);
              /*
              if (!!temp.motor)
                motor = temp.motor->detach(1);
              */
              done = true;
              }
            else { // insufficiently skilled:
              Point cursor = Screen::cursorat();
              String design;
              switch (sk_build) {
                case sk_mech: design = "design"; break;
                //case sk_occu: design = "charm"; break;
                default: design = "<!ERROR>"; break;
                }
              Message::add("This "+design+" is currently too complex for you.",true);
              Message::print();
              Screen::locate(cursor);
              done = false;
              }


            break;
            }
            
          case kEscape: done = true; break;
          default: break;
          }
        }

      if (key == kEnter) {
        t->doPerform(aUpdateStatus);
        }
        
      return key == kEnter;
      }


    default:
      return Item::perform(a,t);
    }

	return false;
	}

bool Buildable::equals( const Target &rhs ) {

  bool ret_val = Item::equals(rhs);

  if (ret_val) {
		if ( exp     != ( (Buildable*) rhs.raw() )->exp ) ret_val = false;
		if ( combo   != ( (Buildable*) rhs.raw() )->combo ) ret_val = false;
		if ( quality != ( (Buildable*) rhs.raw() )->quality ) ret_val = false;
    if ( !!blade || !!((Buildable*)rhs.raw())->blade ) ret_val = false;
    }

  return ret_val;
	}

Buildable::Buildable( const Option *o )
: Item(o), exp(0), quality(0), combo(-1), blade(NULL) {
	}

Buildable::Buildable( Buildable &rhs )
: Item(rhs),
  exp(rhs.exp),
  quality(rhs.quality),
  combo(rhs.combo) {
  if (!!rhs.blade)
    blade = rhs.blade->clone();
	}


void Buildable::toFile( std::iostream &outFile ) {

	Item::toFile( outFile );

	outFile.write( (char*) &exp, sizeof(exp) );
	outFile.write( (char*) &quality, sizeof(quality) );
	outFile.write( (char*) &combo, sizeof(combo) );
  outFile << blade;
  
	}

Buildable::Buildable( std::iostream &inFile, TargetValue *TVP )
: Item(inFile, TVP) {

	inFile.read( (char*) &exp, sizeof(exp) );
	inFile.read( (char*) &quality, sizeof(quality) );
	inFile.read( (char*) &combo, sizeof(combo) );

  long saveLoc;
	inFile >> blade;
	saveLoc = inFile.tellg();
  if (!!blade) blade->parent = THIS;
	inFile.seekg(saveLoc);

  }


String Buildable::name( NameOption n_opt ) {
  switch (n_opt) {
    default:
      return Item::name(n_opt);
    }
  }


#endif // WIDGET_CPP
