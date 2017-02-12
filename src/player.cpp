// [player.cpp]
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
#if !defined ( PLAYER_CPP )
#define PLAYER_CPP

#include "player.hpp"
#include "global.hpp"
#include "message.hpp"
#include "enum.hpp"
#include "level.hpp"
#include "menu.hpp"
#include "dialog.hpp"
#include "file.hpp"
#include "grammar.hpp"
#include "event.hpp"
#include "timeline.hpp"
#include "playerui.hpp"
#include "temp.hpp"
#include "menu.hpp"
#include "random.hpp"

#include "weapon.hpp"
#include "food.hpp"
#include "clothing.hpp"
#include "fluid.hpp"
#include "bagbox.hpp"
#include "quantity.hpp"
#include "money.hpp"
#include "ammo.hpp"
#include "attack.hpp"

#include "scope.hpp"
#include "widget.hpp"

#include "book.hpp"
#include "visuals.hpp"
#include "input.hpp"

#include "compiler.hpp"
#ifdef OBSOLETE_INCLUDE
#  include <stdio.h>
#  include <ctype.h>
#else
#  include <cstdio>
#  include <cctype>
#endif


#define HEALTHCOLOR	(current.hp >= maximum.hp)?cGreen:((current.hp <= maximum.hp / 2)?cRed:cGrey)


Point3d Player::levLoc(0,0,0);
Target Player::thePlayer;


Target Player::create( Option &o ) {

  Player *player = new Player( &o );

  TargetValue *THIS = new TargetValue( player );

	player->setTHIS(THIS);

  assert (THIS != NULL);
  // Error::fatal("Player::create THIS is NULL.");

  PlayerUI::Option ui_opt;
  ui_opt.object = Target(THIS);
  
  // If the user doesn't 'q'uit out,
  if (player->init()) {
    // static link to unique player:
    thePlayer = Target(THIS);
    // Create a player user interface:
  	player->brain = PlayerUI::create( ui_opt );

    // Start the metabolism:
    ActionEvent::create( Target(THIS), aMetabol, 5000 );
    }

	return thePlayer;
	}

Player::~Player( void ) {
  thePlayer = Target(NULL);
	}

Player *Player::load( std::iostream &ios, TargetValue *TVP ) {

	Player *ret = new Player(ios, TVP);

  assert (TVP != NULL);
  // Error::fatal("Player::load TVP is NULL.");

//  ret->setTHIS( TVP );
  thePlayer = Target(TVP);

  Map::open_z = levLoc.getz();
      
  return ret;

	}

Player::Player( const Option *o )
: Creature( o ) {

  // all Players are created Human:
  polymorph(crHuman);
	image.val.appearance = '@';
  image.val.color = cLGrey;

  int i;
  for (i = 0; i < sk_num; ++i) {
    skill[i] = 0;
    skill[i].set( (Skill) i );
    }

  feat_normal = f_none;
  feat_weapon = p_none;

  current_xp = 0;
  current_lev = 1;

	moved = false;
  gender = sex_none;
  burden = bur_none;
  hunger = hs_normal;
  }


const char* stat_names [stat_total] = {
  "Coordination",
  "Perception",
  "Strength",
  "Toughness",
  "Willpower",
  "Knowledge",
  "Concentration",
  "Speed",
  "Health"
  };

bool Player::init( void ) {

  int key;

  Point center(23, 7), // Center point for Name, Gender & Class Menus
        stringPoint = Point(0,-3) + center,
        mfPoint = Point(-9,1) + center,
        class_point = Point(6,3) + center,
        pane_point = Point(60,13);


	// Get character name:
	StringDialog stringdialog( "New Character", "Name? ", 20, alphanumeric, global::skin.main );
  stringdialog.setOrigin(Point(40,10));
	stringdialog.setInput( "RANDOM" );
  stringdialog.setOrigin(stringPoint);

	c_name = stringdialog.readString(false);
	if (c_name == "") c_name = "RANDOM";

  // Get gender:
  // Male/Female
  Temp::Option male_opt, female_opt, quit_opt;
  male_opt.image = Image(cWhite,'m');
  male_opt.str1 = "Male";
  female_opt.image = Image(cWhite,'f');
  female_opt.str1 = "Female";

  quit_opt.image = Image(cWhite, 'q');
  quit_opt.str1 = "Quit";
  
  Target male = Temp::create( male_opt ),
         female = Temp::create( female_opt ),
         quit = Temp::create( quit_opt ) ;
  List<Target> mfList;
  
  mfList.add(male);
  mfList.add(female);
  mfList.add(quit);

  Menu<Target> mfMenu(mfList, global::skin.main);
  mfMenu.setTitle("Gender");
  mfMenu.setOrigin(mfPoint);
  mfMenu.display();

	key = kInvalid;
	while (key!=kEnter) {
		key = mfMenu.getCommand();

		switch (key) {
			case kEnter:

        if (mfMenu.get() == quit)
          return false;
          
				if (mfMenu.get() == male)
          gender = sex_male;
				else
          gender = sex_female;

				break;

			default:
				break;
			}
		if (mfMenu.numitems()) mfMenu.display();

		}

//	mfMenu.hide();

  //--[NEW CHARACTER CLASS CODE:]--//

  enum PC_Class {

                  // Menu Letter  Fantasy Allegory  Weapons
    pc_aviator,       //   a       Swashbuckler      Pistol, knife
    pc_deserter,      //   d       Rogue             Sword, pistol
    pc_engineer,      //   e       Weaponsmith       Wrench, pistol
    pc_gunslinger,    //   g       Knight            Revolvers, big knife
    pc_priest,        //   p       Priest            Staff, faith
    pc_sealancer,     //   s       Pirate            Needle rifle, harpoon
    pc_witchhunter,   //   w       Paladin           Pistol, Wooden stake

//  Optional, probably unbalanced or out of theme character classes:
#   ifdef PCCLASS_LOTUSSCOUT
    pc_lotusscout,    //   l       Ranger            Bow, sword
#   endif // PCCLASS_LOTUSSCOUT

#   ifdef PCCLASS_BOUNTY
    pc_bountykiller,  //   b       Mercenary         Rifle, pistol
#   endif // PCCLASS_BOUNTY

#   ifdef PCCLASS_MISSIONARY
    pc_missionary,    //   m       Priest            Staff
#   endif // PCCLASS_MISSIONARY

#   ifdef PCCLASS_OCCULTIST
    pc_occultist,     // Magician      Ancient sword, pistol
#   endif // PCCLASS_OCCULTIST

#   ifdef PCCLASS_CHEMIST
    pc_chemist,       // Alchemist     Pistol
#   endif // PCCLASS_CHEMIST

#   ifdef PCCLASS_HUCKSTER
 // Huckster magic seems too 'Deadlands', not gritty enough.
    pc_huckster,      // Shaman        Pistol, stiletto
#   endif // PCCLASS_HUCKSTER

#   ifdef PCCLASS_GRENADIER
 // PC starting with grenades seems unfair ATM.
    pc_grenadier,
#   endif // PCCLASS_GRENADIER

#   ifdef PCCLASS_HUNTER
 // Sort of out of theme safari hunter
    pc_hunter,
#   endif // PCCLASS_HUNTER

#   ifdef PCCLASS_INVESTOR
 // Joke class - Investor starts with money and golfing gear.
 // I like the investor as a class, but I don't want it in yet
 // and won't add it until it clearly fits without killing theme
 // too much.  I can always add another joke class later.
    pc_investor,
#   endif // PCCLASS_INVESTOR

    pc_num,
    pc_invalid = pc_num
  
    } pc_class = pc_invalid;

# define START_PISTOLS 2
# define START_CLOTHES 4
  struct PC_Data {
    PC_Class pc_class;
    const char *name;
    char letter;
    StatType st1, st2;
    Skill sk1, sk2, sk3;

    Feat        feat_normal;
    Proficiency feat_weapon;

    int fame, demeanor; // Initial notoriety
    
    const char *desc,
    
         *pistol[START_PISTOLS],
         *ammo[START_PISTOLS],
         *melee,
         *clothes[START_CLOTHES];

    int ammo_multiplier;
    enum {
      q_none,
      q_band,
      q_belt,
      q_quiv
      } bandolier;

    Color color;
         
    } pc_data [pc_num] = {

    { pc_aviator,     "Aviator",'a', stat_crd, stat_spd, sk_acro, sk_mark, sk_pilo, f_none, p_none,   notorious / 10, lawful,
                      "Although the Aviator helped spread the blight during the Last Dust War, their heroism and panache is unparalleled.",
                      {"an imperial luger", NULL}, {NULL, NULL}, "a heavy knife", {"a pair of goggles", "an aviator jacket", "a pair of service breeches", "a pair of kneeboots"}, 1, PC_Data::q_band, cLBlue },

#   ifdef PCCLASS_BOUNTY
    { pc_bountykiller,"Bounty Killer",'b', stat_per, stat_wil, sk_dipl, sk_figh, sk_mark, f_none, p_none,   notorious / 20, lawless,
                      "Where life has no value, death, sometimes, has its price.  The Bounty Killer is no more than a mercenary who dares not yet cross the Gunslingers.",
                      {"an imperial rifle", "a light revolver"}, {NULL, NULL}, "a tomahawk", {"an overcoat", "a shabby suit", "a pair of spurred boots", NULL}, 1, PC_Data::q_band, cLBlue },
#   endif // PCCLASS_BOUNTY

#   ifdef PCCLASS_CHEMIST
    { pc_chemist,     "Chemist",      'c', stat_kno, stat_wil, sk_appr, sk_awar, sk_chem, f_none, p_none,   notorious / 20, lawful,
                      "In the Age of Gunpowder, the skilled Chemist plays the enviable, lucrative roles of both pharmacist and ammunition supplier.",
                      {"a light revolver", NULL}, {NULL, NULL}, "a ceramic club", {"a pair of eyeglasses", "a lab coat", "a shabby suit", "a pair of rubber gloves"}, 1, PC_Data::q_none, cLGreen },
      //"an empty vial"
      //"a microscope"
#   endif // PCCLASS_CHEMIST

    { pc_deserter,    "Deserter",     'd', stat_crd, stat_tuf, sk_athl, sk_figh, sk_stea, f_none, p_none,   notorious / 10, lawless,
                      "In the wake of the Last Dust War, the villainous Deserter abandoned his regiment and betrayed the people of Texarkana.",
                      {"an imperial mauser", NULL}, {NULL, NULL}, "a cavalry sabre", {"a bearskin cap", "an imperial uniform", "a pair of boots", NULL}, 1, PC_Data::q_band, cYellow },

    { pc_engineer,    "Engineer",     'e', stat_str, stat_tuf, sk_figh, sk_mech, sk_pilo, f_none, p_none,   notorious / 20, neutral,
                      "Although no road-trains have run since the Last Dust War, the Engineer bears valuable insight into the few machines that still operate.",
                      {"a sawn-off shotgun", NULL}, {NULL, NULL}, "a heavy wrench", {"a heavy reefer jacket", "a pair of jeans", "a pair of leather gloves", "a pair of understudded boots"}, 1, PC_Data::q_none, cBrown },

#   ifdef PCCLASS_GRENADIER
    { pc_grenadier,   "Grenadier",    'G', stat_crd, stat_str, sk_awar, sk_figh, sk_mark, f_none, p_none,   notorious / 20, neutral,
                      "GRENADIER DESCRIPTION",
                      {"a spencer carbine", NULL}, {NULL, NULL}, NULL, {"a bearskin cap", "a cuirass", "a pair of striped trousers", "a pair of boots"}, 1, PC_Data::q_band, cLGreen },

#   endif // PCCLASS_GRENADIER

    { pc_gunslinger,  "Gunslinger",   'g', stat_crd, stat_per, sk_acro, sk_figh, sk_mark,  f_none, p_none,   notorious / 10, lawful,
                      "The last of an elite class of gun-knights, the Gunslinger is a master of pistols and the final bastion of order.",
                      {"a gilean sixgun", NULL}, {NULL, NULL}, "a machete", {"a royal pendant", "a drawstring shirt", "a pair of faded jeans", "a pair of spurred boots"}, 2, PC_Data::q_band, cWhite },

#   ifdef PCCLASS_HUCKSTER
    { pc_huckster,    "Huckster",     'h', stat_crd, stat_wil, sk_dipl, sk_mark, sk_poke, f_none, p_none,   notorious / 10, neutral,
                      "Strange powers choose unlikely wielders; the Huckster lounges in gambling dens and seems to have the Devil's luck, but his unique talent keeps malign forces at bay.",
                      {"a deringer", NULL}, {NULL, NULL}, "a stiletto", {"a cowboy hat", "an overcoat", "a shabby suit", "a pair of boots" }, 1, PC_Data::q_band, cLRed },

#   endif // PCCLASS_HUCKSTER

#   ifdef PCCLASS_HUNTER
    { pc_hunter,      "Hunter",       'H', stat_per, stat_wil, sk_heal, sk_rese, sk_surv, f_none, p_none,   notorious / 20, lawful,
                      "HUNTER DESCRIPTION",
                      {"a repeating rifle", NULL}, {NULL, NULL}, "a machete", {"a pith helmet", "a safari suit", "a pair of boots", NULL}, 1, PC_Data::q_band, cLBlue },

#   endif // PCCLASS_HUNTER

#   ifdef PCCLASS_INVESTOR
    { pc_investor,    "Investor",     'i', stat_kno, stat_wil, sk_awar, sk_dipl, sk_mark, f_none, p_none,   notorious / 10, lawless,
                      "INVESTOR DESCRIPTION",
                      {NULL, NULL}, {NULL, NULL}, "a golf club", {"a polo shirt", "a wristwatch", "a pair of golf pants", "a pair of oxford shoes"}, 1, PC_Data::q_none, cLBlack },
     //"a dollar"

#   endif // PCCLASS_INVESTOR

#   ifdef PCCLASS_LOTUSSCOUT
    { pc_lotusscout,  "Lotus Scout",  'l', stat_per, stat_tuf, sk_anim, sk_figh, sk_mark, f_none, p_none,   notorious / 20, neutral,
                      "There are no warriors so brave or bold as the Lotus Scout.  Their adherence to the old ways and understanding of animals made them well revered after the fall.",
                      {"a pepperlock pistol"}, {NULL, NULL}, "a katana", {"a leather jacket", "a pair of buckskin pants", "a pair of moccasins", NULL}, 1, PC_Data::q_band, cIndigo },
#   endif // PCCLASS_LOTUSSCOUT

#   ifdef PCCLASS_MISSIONARY
    { pc_missionary,  "Missionary",   'm', stat_wil, stat_kno, sk_awar, sk_dipl, sk_rese, f_none, p_none,   notorious / 10, lawful,
                      "THE MISSIONARY",
                      {"a cult pistol", NULL}, {NULL, NULL}, "an ironwood staff", {"a boneweave breastplate", "a pair of buckskin pants", "a pair of moccasins", NULL}, 1, PC_Data::q_quiv, cIndigo },
#   endif // PCCLASS_MISSIONARY
     
#   ifdef PCCLASS_OCCULTIST
    { pc_occultist,   "Occultist",  'o', stat_per, stat_kno, sk_appr, sk_rese, sk_occu, f_none, p_none,   notorious / 20, lawless,
                      "The world has moved on, but the Occultist knows that salvation may lie in artefacts of ancient times and rituals from forgotten lands.",
                      {"a light revolver", NULL}, {NULL, NULL}, "an antique sword", {"a cravat", "an opera cloak", "a tailored suit", "a pair of oxford shoes"}, 1, PC_Data::q_none, cViolet },
#   endif // PCCLASS_OCCULTIST
     //"a bag"

    { pc_priest,      "Priest", 'p', stat_wil, stat_kno, sk_dipl, sk_fait, sk_rese, f_none, p_none,   notorious / 10, lawful,
                      "The Priest still holds onto faith when all others have moved on.  Confessional has been unattended for years but the Priest may have enough faith to save all others. ",
                      {NULL, NULL}, {NULL, NULL}, "an ironwood staff", {"a string of rosary beads", "a heavy robe", "a pair of black trousers", "a pair of leather sandals"}, 1, PC_Data::q_none, cIndigo },

    { pc_sealancer,   "Sea Lancer", 's', stat_per, stat_wil, sk_dipl, sk_figh, sk_mark, f_none, p_none,   notorious / 20, lawful,
                      "Familiarity with the horrific Fishmen and other deep ones readied the brave Sea Lancer for the trip to ancient Abura Tan.",
                      {"a harpoon gun", NULL}, {NULL, NULL}, "a cutlass", {"a headscarf", "a waistcoat", "a pair of black trousers", "a pair of kneeboots"}, 1, PC_Data::q_band, cLBlue },
                      
                      
    { pc_witchhunter, "Witch Hunter", 'w', stat_str, stat_wil, sk_awar, sk_fait, sk_figh, f_none, p_none,   notorious / 10, neutral,
                      "Of all the deviants that shamble from the blackened wastes, the witch is the most insidious.  The Witch Hunter fights to keep The West pure of these miscreants.",
                      {"a long pistol", NULL}, {"a silver bullet", NULL}, "a longsword", {"a stovepipe hat", "a frock coat", "a pair of striped trousers", "a pair of boots"}, 1, PC_Data::q_band, cDGrey },
     // Witch hunters should have silver bullets?
     // Witch hunters definitely should have wooden stakes x3
    };

  Target menu_opt;
  List<Target> class_list;
  int i;

  for (i = 0; i < pc_num; ++i) {
    Temp::Option temp_opt;
    temp_opt.image = Image(cWhite,pc_data[i].letter);
    temp_opt.temp_val = pc_data[i].pc_class;

    if ( Skills[ pc_data[i].sk1 ].implemented
      && Skills[ pc_data[i].sk2 ].implemented
      && Skills[ pc_data[i].sk3 ].implemented )
      temp_opt.str1 = pc_data[i].name;
    else
      temp_opt.str1 = "<_"+String(pc_data[i].name)+">";

    menu_opt = Temp::create( temp_opt );
    class_list.add(menu_opt);
    }

  class_list.add(quit);
  
  Menu<Target> class_menu(class_list, global::skin.main);
  class_menu.setTitle("Class");
  class_menu.setOrigin(class_point);

  // Set up classinfo pane:
  Pane class_pane( global::skin.main );
  class_pane.setTitle("Class Information");
  class_pane.setOrigin(pane_point);

  TextArea desc_title, equip_title, stat_title, skill_title,
           desc_body, equip_body,
           stat_1, stat_2,
           skill_1, skill_2, skill_3;

  desc_title.setText("<_Description:>");
  desc_title.setBorder( Rectangle(0,0,36,0) );
  class_pane.add( desc_title );

  desc_body.setBorder( Rectangle(1,1,36,5) );
  class_pane.add( desc_body );

  equip_title.setText("<_Equipment:>");
  equip_title.setBorder( Rectangle(0,6,36,6) );
  class_pane.add( equip_title );

  equip_body.setBorder( Rectangle(1,7,36,10) );
  class_pane.add( equip_body );

  stat_title.setText("<_Statistics:>");
  stat_title.setBorder( Rectangle(0,11,36,11) );
  class_pane.add( stat_title );

  stat_1.setBorder( Rectangle(1,12,36,12) );
  class_pane.add( stat_1 );
  stat_2.setBorder( Rectangle(1,13,36,13) );
  class_pane.add( stat_2 );

  skill_title.setText("<_Skills:>");
  skill_title.setBorder( Rectangle(0,14,36,14) );
  class_pane.add( skill_title );

  skill_1.setBorder( Rectangle(1,15,36,15) );
  class_pane.add( skill_1 );
  skill_2.setBorder( Rectangle(1,16,36,16) );
  class_pane.add( skill_2 );
  skill_3.setBorder( Rectangle(1,17,36,17) );
  class_pane.add( skill_3 );

  Point cursor;

	key = kInvalid;
  bool done = false;
	while (!done) {

    cursor = Screen::cursorat();
    // Set Pane variables:
    int index = class_list.lock();
    if (index < pc_num ) {
      desc_body.setText( pc_data[ index ].desc );

      String body_text;

      List <String> body_list;
      for (i = 0; i < START_PISTOLS; ++i)
        if ( pc_data[ index ].pistol[i] )
          body_list.add( pc_data[ index ].pistol[i] );

      if ( pc_data[ index ].melee )
        body_list.add( pc_data[ index ].melee );
          
      for (i = 0; i < START_CLOTHES; ++i)
        if ( pc_data[ index ].clothes[i] )
          body_list.add( pc_data[ index ].clothes[i] );

      if ( body_list.numData() > 0 ) {

        body_text = body_list.get();
        
        bool end_list = !body_list.next();

        while (!end_list) {

          String addition = body_list.get();
          end_list = !body_list.next();

          if (end_list)
            body_text += " and ";
          else
            body_text += ", ";

          body_text += addition;
          }
        }
         
      equip_body.setText( body_text.sentence() );

      if ( Skills[ pc_data[index].sk1 ].implemented )
        skill_1.setText( String( Skills[ pc_data[ index ].sk1 ].name ).titlecase() );
      else
        skill_1.setText( "<!"+String( Skills[ pc_data[ index ].sk1 ].name ).titlecase() + " - Skill Incomplete>" );
        
      if ( Skills[ pc_data[index].sk2 ].implemented )
        skill_2.setText( String( Skills[ pc_data[ index ].sk2 ].name ).titlecase() );
      else
        skill_2.setText( "<!"+String( Skills[ pc_data[ index ].sk2 ].name ).titlecase() + " - Skill Incomplete>" );
        
      if ( Skills[ pc_data[index].sk3 ].implemented )
        skill_3.setText( String( Skills[ pc_data[ index ].sk3 ].name ).titlecase() );
      else
        skill_3.setText( "<!"+String( Skills[ pc_data[ index ].sk3 ].name ).titlecase() + " - Skill Incomplete>" );
        
      stat_1.setText( String( stat_names[ pc_data[ index ].st1 ] ).titlecase() );
      stat_2.setText( String( stat_names[ pc_data[ index ].st2 ] ).titlecase() );
      class_pane.display();
      }
    else
      class_pane.hide();
    Screen::locate(cursor);

    // Display Pane and Menu:
    class_menu.display();
    
		key = class_menu.getCommand();

		switch (key) {
			case kEnter: {

        cursor = Screen::cursorat();

        if (class_menu.get() == quit)
          return false;

        index = class_list.lock();
        bool confirm = ( Skills[ pc_data[index].sk1 ].implemented
                      && Skills[ pc_data[index].sk2 ].implemented
                      && Skills[ pc_data[index].sk3 ].implemented );

        if (!confirm) {
  				Dialog dialog("Class Relies on Incomplete Skills", String("This class may be difficult to play.  Continue? [y/N]"), global::skin.error );
  				dialog.display();

  				int key_2;
  				do {

  					key_2 = dialog.getKey();

  					} while (  Keyboard::toCommand(key_2) != kEscape
  									&& Keyboard::toCommand(key_2) != kEnter
  									&& Keyboard::toCommand(key_2) != kSpace
  									&& key_2 != 'y' && key_2 != 'Y' && key_2 != 'n' && key_2 != 'N' );

  				if ( key_2 == 'y' || key_2 == 'Y')
  					confirm = true;
          }

        if ( confirm ) {
          pc_class = (PC_Class)class_menu.get()->getVal(attrib_temp);
          done = true;
          }
          
        Screen::locate(cursor);
          
				break;
        }

			default:
				break;
			}

		}

  // Give the character their default colour:
  image.val.color = pc_data[ pc_class ].color;
  // Todo: Implement clothing-based colour.
  
  // Give the character starting equipment, stats and skills:
  Target item, container;
  Quantity::Option q_opt;

  // Give a bandolier if appropriate:
  // Must be first to make sure we load ammo into it.
  switch ( pc_data[ pc_class ].bandolier ) {

     case PC_Data::q_band: perform( aWear, Quiver::create()->Force("a bandolier") );
      break;
     case PC_Data::q_quiv: perform( aWear, Quiver::create()->Force("a quiver") );
      break;

     default: break;
     }
  
  // Give the ranged weapons and ammo:
  for (i = 0; i < START_PISTOLS; ++i)
    if ( pc_data[ pc_class ].pistol[i] ) {
      Target pistol;
      pistol = Ranged::create()->Force(pc_data[ pc_class ].pistol[i]);
      pistol->doPerform( aGainLev );
      perform( aWield, pistol );

      Ammo::Option ammo_opt;
      ammo_opt.type = (AmmoType) pistol->getVal(attrib_ammotype);
      item = Ammo::create(ammo_opt);
      if (pc_data[ pc_class ].ammo[i])
        item->Force(pc_data[ pc_class ].ammo[i]);
        
      q_opt.target = item;
      q_opt.quantity = DiceRoll(1,6,18).roll();
      container = Quantity::create(q_opt);

      // Make sure we own the ammo before we load it:
      container->perform(aSetOwner, THIS);
      
      pistol->perform(aLoad, container);
      if (capable(aWear, container) )
        perform( aWear, container );
      else
        perform( aContain, container );
      }

  // Give the melee weapons:
  if ( pc_data[ pc_class ].melee ) {
    Target melee = Melee::create()->Force(pc_data[ pc_class ].melee);
    melee->doPerform( aGainLev );
    doPerform( aSwitch );
    perform( aWield, melee );
    }
  // If they have two weapons wielded, switch them
  // so that the melee is the secondary weapon:
  if (!!secondary)
    doPerform( aSwitch );

  // Give the clothing:
  for (i = 0; i < START_CLOTHES; ++i)
    if ( pc_data[ pc_class ].clothes[i] )
      perform( aWear, Clothing::create()->Force(pc_data[ pc_class ].clothes[i]) );

  //
  // Reject from the days of lighting:
  //  // Everone gets a lightsource:
  //  //perform( aContain, Light::create()->Force("a torch") );
  //
  
  // Increment Class Stats:
  ++maximum[ pc_data[ pc_class ].st1 ];
  ++maximum[ pc_data[ pc_class ].st2 ];
  // Increment Class Skills:
  ++skill[ pc_data[ pc_class ].sk1 ];
  ++skill[ pc_data[ pc_class ].sk2 ];
  ++skill[ pc_data[ pc_class ].sk3 ];

  // Set Class Reputation
  notoriety = Notoriety( pc_data[ pc_class ].fame,
                         pc_data[ pc_class ].demeanor );

  // Set feats and proficiency:
  feat_normal = pc_data[ pc_class ].feat_normal;
  feat_weapon = pc_data[ pc_class ].feat_weapon;
  
  // Everybody gets:
  // 2 Food Rations:
  q_opt.quantity = 2;
  q_opt.target = Food::create()->Force("a food ration");
  perform( aContain, Quantity::create(q_opt) );
  // 5 to 6 dollars:
  q_opt.quantity = 5;
  q_opt.target = Money::create()->Force("a dollar");
  perform( aContain, Quantity::create( q_opt ) );
  
  q_opt.quantity = DiceRoll(1,99).roll();
  q_opt.target = Money::create()->Force("a cent");
  perform( aContain, Quantity::create( q_opt ) );


# ifdef TEST_BUILD
    //perform( aContain, Scope::create()->Force("a kaleidoscope") );

    /*
    q_opt.quantity = 3;
    Widget::Option w_opt;
    for ((int)w_opt.form = 0; w_opt.form < w_num; ((int)w_opt.form)++) {
      q_opt.target = Widget::create(w_opt);
      perform( aContain, Quantity::create(q_opt) );
      }
    */

    /*// Fuel and motors have been removed to streamline the build system:
    FuelType fuel_type;
    for ( ((int)fuel_type) = 0; fuel_type < fuel_num; ++((int)fuel_type)) {
      Fuel::Option fuel_opt;
      fuel_opt.fuel = fuel_type;
      perform( aContain, Fuel::create(fuel_opt) );

      Motor::Option motor_opt;
      motor_opt.fuel = fuel_type;
      perform( aContain, Motor::create(motor_opt) );
      }
    */

# endif // TEST_BUILD

  current = maximum;

  // If the player wants a random name, assign it now:
  if ( c_name == "RANDOM" ) {
    c_name = Random::name( gender==sex_male, /*bgTheme*/
    
#   ifdef PCCLASS_LOTUSSCOUT
    (pc_class == pc_lotusscout)? th_native :
#   endif // PCCLASS_LOTUSSCOUT
      th_none );
  	stringdialog.setInput( c_name.toString() );
    stringdialog.display();
    }
    
  updateStatus();

  return true;
	}

Player::Player( Player &copy )
: Creature( copy ) {
	moved = copy.moved;
  burden = copy.burden;
  hunger = copy.hunger;
	}

Target Player::clone( void ) {

	Player *newcopy = new Player( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

Player::Player( std::iostream &inFile, TargetValue *TVP )
: Creature(inFile, TVP) {

	// Load bool moved:
	inFile.read( (char*) &moved, sizeof(moved) );

  // Load Burden burden
  inFile.read( (char*) &burden, sizeof(burden) );
  
  // Load HungerState hunger
  inFile.read( (char*) &hunger, sizeof(hunger) );

	// Open "save/player.dat"
	File myFile;
	String player_dat = SAVEDIR+"player.dat";

	// Load struct skill:
	myFile[player_dat].read( (char*) &skill, sizeof(skill) );

	myFile[player_dat].read( (char*) &feat_normal, sizeof(feat_normal) );
	myFile[player_dat].read( (char*) &feat_weapon, sizeof(feat_weapon) );
  

	myFile[player_dat].read( (char*) &notoriety, sizeof(notoriety) );

  // Load the shk accounts:
  int numaccounts;
  myFile[player_dat].read( (char*) &numaccounts, sizeof(numaccounts) );
  Target read;
  account.clr();
  for (int i = 0; i < numaccounts; ++i) {
    myFile[player_dat] >> read;
    account.add(read);
    }

  // Don't update status here - because we print the weapons
  // info now at the bottom of the screen, it will cause
  // an infinite loop, constantly trying to load the player
  // again and again.  I don't know why - let's just ignore it :)
	//updateStatus();
	}

void Player::toFile( std::iostream &outFile ) {

	Creature::toFile( outFile );

	// Save bool moved:
	outFile.write( (char*) &moved, sizeof(moved) );

  // save Burden burden
  outFile.write( (char*) &burden, sizeof(burden) );
  
  // save HungerState hunger
  outFile.write( (char*) &hunger, sizeof(hunger) );

	// Open "save/player.dat"
	File myFile;
	String player_dat = SAVEDIR+"player.dat";

	// Save struct skill:
	myFile[player_dat].write( (char*) &skill, sizeof(skill) );

	myFile[player_dat].write( (char*) &feat_normal, sizeof(feat_normal) );
	myFile[player_dat].write( (char*) &feat_weapon, sizeof(feat_weapon) );

	myFile[player_dat].write( (char*) &notoriety, sizeof(notoriety) );

  // Save the shk accounts:
  int numaccounts = account.numData();
  myFile[player_dat].write( (char*) &numaccounts, sizeof(numaccounts) );
  if (account.reset())
  do {
    myFile[player_dat] << account.get();
    } while ( account.next() );

	// Open "save/player.lev"
	// Write level location:
	String levelInf = SAVEDIR+"player.lev";
	myFile[levelInf].write( (char*) &levLoc, sizeof(levLoc) );

  // Save thePlayer reference
  myFile[levelInf] << thePlayer;
  thePlayer = Target(NULL);

	}

void Player::loadInfo( void ) {

	File myFile;
	String levelInf = SAVEDIR+"player.lev";
	// Load level location:
	myFile[levelInf].read( (char*) &levLoc, sizeof(levLoc) );

  // Load thePlayer reference
  myFile[levelInf] >> thePlayer;
	}


String Player::name( NameOption n_opt ) {
  switch (n_opt) {
    case n_noun: return c_name.toString();
    case n_owner: return String("your");
    default: return String("you");
    }
  }


targetType Player::gettype( void ) const {
	return tg_player;
	}
bool Player::istype( targetType t ) const {
	if (t == tg_player)
		return true;
	else
		return Creature::istype(t);
	}

bool Player::alive( void ) const {
	return true;
	}

void Player::draw( void ) {
	Creature::draw();
	}

bool Player::capable( Action a, const Target &t ) {

	switch( a ) {

    case aView:
      // Player is *always* visible:
      return true;

    case aAssault:
      // Player can choose to attack anything unprovoked:
      
      // (exceptions: nothing OR itself)
      if (!t || t == Target(THIS))
        return false;
      
      return true;

    case aDeductBill:
      return account.find(t);
      
		case aSpMessage:
			return false;

		default:
			return Creature::capable(a,t);

		}

	}


struct {
  long exp;
  int skill;
  bool talent;
  bool stat;
  
  } forLev [20] = {


    {        0,  3, true,  false}, //clev1
    {     1000,  2, false, false},
    {     3000,  2, true,  false},
    {     6000,  2, false, true },
    {    10000,  2, false, false},
    {    15000,  1, true,  false},
    {    21000,  1, false, false},
    {    28000,  1, false, true },
    {    36000,  1, true,  false},
    {    45000,  1, false, false},
    {    55000,  1, false, false},
    {    66000,  1, true,  true },
    {    78000,  1, false, false},
    {    91000,  1, false, false},
    {   105000,  1, true,  false},
    {   120000,  1, false, true },
    {   136000,  1, false, false},
    {   153000,  1, true,  false},
    {   171000,  1, false, false},
    {   190000,  1, false, true }


  };

bool Player::perform( Action a, const Target &t ) {

	switch (a)  {

    case aPray:
      Message::add("You pray to the Man Jesus.");

      // Make a list of the prayers that are known:

      // Make sure that only prayers of a level less
      // than or equal to the player's faith skill
      // are available for activation:

      // Display the list:

        // If the player chooses an activated prayer,
          // deactivate it:

        // If the player chooses a non-active prayer,
        // and it's the right level,
        // and the player has enough concentration,
          // Check to see if it's an instant (timed) prayer:
          // If it's an instant prayer,
            // perform the action:
            // create an action event for the player:

          // Otherwise, it's a toggle prayer,
            // activate it:

      // The player's done.  Return.
      return true;

    case aQuickReload: {
      bool success = Creature::perform(a,t);
      if (success)
        Message::add("You reload.");
      return success;
      }
    case aFire:
    case aAttack: {
      // --- Check to see if the player really wants to attack
      //     a peaceful or not-yet-hostile creature:
      
  		// Prompt the player:
      if (t->getVal(attrib_vis_mon) != t->getVal(attrib_vis_enemy)) {

				Dialog dialog("Peaceful Target!", String("Are you sure? [y/N]"), global::skin.error );
				dialog.display();

				int key;
				do {

					key = dialog.getKey();

					} while (  Keyboard::toCommand(key) != kEscape
									&& Keyboard::toCommand(key) != kEnter
									&& Keyboard::toCommand(key) != kSpace
									&& key != 'y' && key != 'Y' && key != 'n' && key != 'N' );

				if ( key != 'y' && key != 'Y')
          return false;

        }

      // Maybe consider checking all possible targets in
      // line when firing?  Do we want to coddle the player?
      return Creature::perform(a,t);
    
      }

    case aAddBill: {
      if (!t) return false;

      Target t_owner = t->getTarget(tg_owner);
      
      if (!!t_owner && t_owner != THIS) {
        //Grammar::Subject(THIS);
        //Message::add("You owe "+Grammar::Object(t_owner)+" money for "+Grammar::plural(t)+".");
        if (!account.find(t))
          account.add(t);
          
      	if ( global::isNew.shop ) {
      		Message::add("<?Press '\\" + Keyboard::keytostr(keyMap[kChat].dkey) + "' to settle your account.>");
      		global::isNew.shop = false;
      		}
        }
      return true;
      }

    case aDeductBill: {
      bool deducted = false;

      while ( account.find(t) ) {
        deducted = true;
        account.remove();
        }
      
      return deducted;
      }
      
    case aSettle: {

      if (!t) return false;
      
      long total = 0, debt = 0;

      List<Target> settle_list;
      
      if (account.reset())
      do {

        if (account.get()->getTarget(tg_owner) == t) {
        
          if ( !settle_list.find(account.get()) ) {
            settle_list.add(account.get());
            total += account.get()->getVal( attrib_price );
            debt = total;
            }
          }

        } while (account.next());

      if (settle_list.numData()) {
        Menu<Target> settle_menu( settle_list, global::skin.main );
        settle_menu.setTitle("Settle");
        Command key = kInvalid;
        bool done = false;
        while (!done) {
          settle_menu.display();
          key = settle_menu.getCommand();

          switch (key) {
            case kEnter:

              if ( perform(aPayFor,settle_menu.get()) ) {
                perform(aDeductBill,settle_menu.get());
                debt -= settle_menu.get()->getVal(attrib_price);
                settle_menu.get()->perform(aSetOwner, THIS );
                settle_menu.remove();
                if (! settle_menu.numitems() )
                  done = true;
                doPerform(aUpdateStatus);
                }
              else {
                if (getVal(attrib_cash) >= settle_menu.get()->getVal(attrib_price) )
                  Message::add("You may only pay with money kept in your general inventory.");
                else
                  Message::add("You have insufficient money.");
                }
                
              break;

            case kEscape:
              done = true;
              break;
              
            default:
              break;
            }

          Point cursor = Screen::cursorat();
          Message::print();
          Screen::locate(cursor);

          }
        }

      Grammar::Subject(THIS);
      String still_str;
      if (total)
        if (debt == total)
          still_str = "still ";
        else
          still_str = "now ";

      if (debt||!total)
        Message::add("You "+still_str+"owe "+Grammar::Object(t,Grammar::det_definite)+" "+(debt?PriceStr(debt):String("nothing"))+".");
      else
        Message::add(Grammar::Object(t,Grammar::det_definite) + " says, \"A pleasure doing business with you, "+name(n_noun)+"\"");
        
      return true;
      }


    case aDetails: {
      // Todo:
      //  Display attack speed
      //  Display combat values better
      //  give a character-relevant messages
    
     /* +-[Character Information]----------------------------+
      * |                                                    |
      * | Name: Fred               Defence:                  |
      * | Nororiety: Unknown        Base AC:  12             |
      * |                           Pierce: none             |
      * | Primary: +8,range 5       Bash:   Poor             |
      * | Secondary: +8,+6 to chop  Cut:    Good             |
      * | Melee Speed: 5            Chop:   none             |
      * | Ranged Speed: 6           Crush:  none             |
      * |                           Splash: Excellent        |
      * | Description:              Vapor:  Poor             |
      * |  He is a blah blah                                 |
      * |                                                    |
      * |  And he looks nice.                                |
      * |                                                    |
      * +----------------------------------------------------+
      */
      
      Pane char_pane( global::skin.main );
      char_pane.setTitle("Character Information");

      TextArea name_area, note_area, desc_title, desc_body,
               primary_area, secondary_area,
               m_spd_area, r_spd_area,
               defence_title, combat_title, *defence_text[pen_num];

      name_area.setText("<_Name:> " + name(n_noun));
      name_area.setBorder( Rectangle(2,0,28,0) );
      char_pane.add( name_area );

      note_area.setText("<_Notoriety:> " + notoriety.toString());
      note_area.setBorder( Rectangle(2,1,28,1) );
      char_pane.add( note_area );

      desc_title.setText("<_->[<^Description>]<_--->");
      desc_title.setBorder( Rectangle(0,8,30,8) );
      char_pane.add( desc_title );

      desc_body.setText(name(n_noun)+" is an average (5'8\"), tanned "+((gender == sex_male)?"man":"woman")+" with black hair.");
      desc_body.setBorder( Rectangle(2,9,60,11) );
      char_pane.add( desc_body );

      combat_title.setText("<_->[<^Combat>]<_--->");
      combat_title.setBorder( Rectangle(0,2,28,2) );
      char_pane.add( combat_title );

      defence_title.setText("<_->[<^Defence>]<_--->");
      defence_title.setBorder( Rectangle(31,0,60,0) );
      char_pane.add( defence_title );

      // Allocate weapon textareas:
      CombatValues primary_cv, secondary_cv;
      String primary_str, secondary_str;
      // Primary:
      int primary_bonus
        = getSkill(sk_figh)
          - ( !!primary && !!secondary &&
              ( primary->getVal(attrib_combat_style)
              & secondary->getVal(attrib_combat_style) )
               ? TWO_WEAPON
               : 0  );
        
      if (primary_bonus >= 0)
        primary_str << (char) '+';
      else
        primary_str << (char) '-';
        
      primary_str << (int) primary_bonus << ", ";
        
      if (!!primary) {
        // Add the penetration level/type:
        primary->GET_STRUCT( s_offence_cv, primary_cv );
        primary_str += primary_cv.toString();
        // Add the firearm range:
        int range_unit = primary->getVal(attrib_range_unit);
        if (range_unit > 0)
          primary_str << "range " << (long) range_unit;
        }
      else {
        primary_str = "<_(innate)>";
        }

      primary_area.setText("<_Primary:> " + primary_str);
      primary_area.setBorder( Rectangle(2,3,28,3) );
      char_pane.add( primary_area );

      // Secondary:
      int secondary_bonus
        = getSkill(sk_figh)
          - ( !!primary && !!secondary &&
              ( primary->getVal(attrib_combat_style)
              & secondary->getVal(attrib_combat_style) )
               ? TWO_WEAPON
               : 0  );
      
      if (!!secondary && !secondary->getVal(attrib_balanced))
        secondary_bonus -= UNBALANCED_2ND;
      if (secondary_bonus >= 0)
        secondary_str << (char) '+';
      else
        secondary_str << (char) '-';
        
      secondary_str << (int) secondary_bonus << ", ";
      
      if (!!secondary) {
        // Add the penetration level/type:
        secondary->GET_STRUCT( s_offence_cv, secondary_cv );
        secondary_str += secondary_cv.toString();
        // Add the firearm range:
        int range_unit = primary->getVal(attrib_range_unit);
        if (range_unit > 0)
          primary_str << "range " << (long) range_unit;
        }
      else {
        secondary_str = "<_(innate)>";
        }

      secondary_area.setText("<_Secondary:> " + secondary_str);
      secondary_area.setBorder( Rectangle(2,4,28,4) );
      char_pane.add( secondary_area );

      // Weapon Speeds:
      const char *combat_spd[5] = {
        "<GLightning>", //  1 .. 2 (0 == "none")
        "<BFast>",      //  3 .. 5
        "<iAverage>",   //  6 .. 8
        "<vSlow>",      //  9 ..11
        "<RSluggish>",  // 12+
        };
        
      String melee_spd, ranged_spd;

      melee_spd << combat_spd[ min( getVal( attrib_melee_spd ) / 3, static_cast<long>(4)) ];
      if (getVal(attrib_melee_spd) == 0)
        melee_spd = "<_none>";
      m_spd_area.setText("<_Melee Speed:>  " + melee_spd );
      m_spd_area.setBorder( Rectangle(2,5,28,5) );
      char_pane.add( m_spd_area );

      ranged_spd << combat_spd[ min( getVal( attrib_ranged_spd ) / 3, static_cast<long>(4)) ];
      if (getVal(attrib_ranged_spd) == 0)
        ranged_spd = "<_none>";
      r_spd_area.setText("<_Ranged Speed:> " + ranged_spd );
      r_spd_area.setBorder( Rectangle(2,6,28,6) );
      char_pane.add( r_spd_area );


      // Allocate defence textareas:
      CombatValues defence;
      GET_STRUCT( s_defence_cv, defence );
      
      const char *defence_name[pen_num] = {
        "Pierce:", "Bash:  ", "Cut:   ",  "Chop:  ",
        "Crush: ", "Splash:", "Vapor: "
        };
      const char *def_adj[8] = {
      // Adjectives remind me a little of those used in
      // Angband, but here they represent defence values
      // rather than skill values.  Not sure if the terms
      // chosen are a little too weighted toward positive
      // (really, 'Great' is a low value and 'Excellent'
      // is just above the halfway point) but I feel that
      // the alternative would mean that *most* characters
      // would have ratings like, 'Poor', 'Dismal', 'Okay'
      // etc., which would be just depressing.
        "<_none>",       //  0
        "<rPoor>",       //  1 .. 3
        "<oModerate>",   //  4 .. 6
        "<YGreat>",      //  7 .. 9
        "<GExcellent>",  // 10 ..12
        "<BSuperb>",     // 13 ..15
        "<iLegendary>",  // 16 ..18
        "<vUltimate>"    // 19+
        };

      for (int index = 0; index < pen_num; ++index) {
        defence_text[index] = new TextArea;
        
        defence_text[index]->setBorder( Rectangle(33,1+index,60,1+index) );

        // Calculate which adjective we will use:
        int adj_index = (defence.defence[index] + 2) / 3;
        if (adj_index > 7) adj_index = 7;
        
        defence_text[index]->setText(
            String() << "<_"
                     << defence_name[index]
                     << ">  "
                     << def_adj[adj_index] );
                     
        char_pane.add( *defence_text[index] );
        }
      
      // Display and get the cursor away:
      char_pane.display();
      Screen::locate(char_pane.getBorder().geta());
      
  
      Command key = kInvalid;
      while (key != kSpace && key != kEnter &&key != kEscape) {
  
        key = Keyboard::getCommand();
        }

      // Cleanup defence textareas:
      for (int index = 0; index < pen_num; ++index) {
        delptr( defence_text[index] );
        }
        
      return true;
      }

    case aLookAround:
    case aSniffAround:
      //assert (location != OFF_MAP);
      assert( !!parent );
      // Error::fatal("Player cannot look/sniff around when not in any location!");

      //return Map::getTile(location)->perform(a);
      return parent->doPerform(a);


    case aPolymorph:
      Creature::perform(a,t);
      updateStatus();

      // This update will not be visible until the player moves again,
      // so they get a `feel' for being human again.
      if ( form == crHuman )
        image.val.appearance = '@';
      return true;

    case aChat:
      if (!!t) return t->perform( aRespond, THIS );
      
      return false;

    case aRespond:

      if (current.wil < 5)
        Message::add("You can't think of anything to say.");
      else if (current.wil > 5)
        Message::add("You respond wittily.");
      else
        Message::add("You discuss the weather.");
      return true;

    case aGainExp:
      Creature::perform(a,t);
      updateStatus();
      return true;

    case aWear:
    case aWield:
    case aSwitch:
    case aTakeOff: {

      // Special case for wearing/removing items
      // that affect awareness or perception:
      int awareness = getSkill( sk_awar );

      bool ret = Creature::perform(a,t);
      updateStatus();

      // IF the awareness/perception has changed,
      if ( awareness != getSkill(sk_awar) )
        // redraw the line of sight:
        doPerform( aLOS );
      
      // If you are a man wearing ladies clothes,
      // Todo: Now that clothing can be specifically male as well,
      // could give messages to women wearing strictly for-men clothing.
      if (a == aWear && gender == sex_male && !!t && t->getVal(attrib_gender) == sex_female )
      switch( Random::randint(3) ) {
        case 0: Message::add("You feel like a new woman!"); break;
        case 2: Message::add("You wonder what your parents would think."); break;
        default:Message::add(Grammar::Subject(t,Grammar::det_definite)+" does not fit you very well.");
        }
      return ret;
      }

    case aEnterLev:
    case aLeaveLev:

      assert ( getVal(attrib_on_level) );
      // Error::fatal("Error in Player::perform(a???Lev)");

      // If the player is entering a location, tell the map to
      // focus on player position:
      if ( a == aEnterLev )
        Map::focus(getlocation());

      return Creature::perform(a,t);

		case aLOS:
      //if ( ! getVal(attrib_on_level) )
      //  Error::fatal("Error in Player::perform(aLOS)");

      //--- Set up the global view distance:
      global::view_distance = getVal(stat_per)
                            + getSkill(sk_awar);
                            
      // If nighttime or we're underground, restrict view:
      if ( getVal(attrib_on_level) && levLoc.getz() != 0 )
        global::view_distance = getSkill(sk_awar);
                            
      //--- Perform LOS functions:

      Screen::hidecursor();
      //getlevel().LOS( Target(THIS) );
      //assert(location!=OFF_MAP);
      //assert(getVal(attrib_on_level));
      // GULP!
      if (getVal(attrib_on_level))
        Map::LOS( Target(THIS) );
      //getlevel().redraw();
      Screen::showcursor();
			return true;
		case aHeal:
			Creature::perform(aHeal, t);
			//image.val.color = HEALTHCOLOR;
			draw();
			updateStatus();
			return true;
		case aUpdateStatus:
			updateStatus();
			return true;

		case aStartTurn:
			moved = false;
      updateLocation();

      // Gain level:
      while ( current_xp >= forLev[current_lev].exp ) {

        Message::add(String("Welcome to level ") << (current_lev+1) << '.');
        Message::print();
        Message::more();
        
        // Increase hit points
        int hp_inc = (maximum.wil+maximum.tuf) / 2
                   + getSkill(sk_athl, false);
                   
        maximum.hp += hp_inc;
        current.hp += hp_inc;

        // Stat increase?
        if ( forLev[current_lev].stat ) {

          List<Target> stList;

          Temp::Option temp;

          temp.image = Image(cWhite,'c');  temp.str1 = stat_names[stat_crd];
          Target crd = Temp::create( temp );
          temp.image = Image(cWhite,'p');  temp.str1 = stat_names[stat_per];
          Target per = Temp::create( temp );
          temp.image = Image(cWhite,'s');  temp.str1 = stat_names[stat_str];
          Target str = Temp::create( temp );
          temp.image = Image(cWhite,'t');  temp.str1 = stat_names[stat_tuf];
          Target tuf = Temp::create( temp );
          temp.image = Image(cWhite,'w');  temp.str1 = stat_names[stat_wil];
          Target wil = Temp::create( temp );
          temp.image = Image(cWhite,'k');  temp.str1 = stat_names[stat_kno];
          Target kno = Temp::create( temp );
                 
          stList.add( crd );
          stList.add( per );
          stList.add( str );
          stList.add( tuf );
          stList.add( wil );
          stList.add( kno );

          Menu<Target> stMenu(stList, global::skin.inventory);
          String title = "Increase Stat";
          stMenu.setTitle(title);

          Command key = kInvalid;
          bool done = false;

          while ( !done ) {
            stMenu.display();
            key = stMenu.getCommand();

            switch (key) {
              case kEnter:

                if ( stMenu.get() == crd ) {
                  maximum.crd++;
                  current.crd++;
                  }
                else if ( stMenu.get() == per ) {
                  maximum.per++;
                  current.per++;
                  }
                else if ( stMenu.get() == str ) {
                  maximum.str++;
                  current.str++;
                  }
                else if ( stMenu.get() == tuf ) {
                  maximum.tuf++;
                  current.tuf++;
                  }
                else if ( stMenu.get() == wil ) {
                  maximum.wil++;
                  current.wil++;
                  }
                else if ( stMenu.get() == kno ) {
                  maximum.kno++;
                  current.kno++;
                  }
                
                done = true;
                  
                break;

              default:
                break;
             }

            }

          }
        
        // Get a new skill?
        //
        // Can only raise skills if current rank is less than
        //      minimum ( character level, relevant stat  )
        // exception: can always raise any skill
        //            by one rank per level increase
        if ( forLev[current_lev].skill ) {

          List<SkillVal> skList;

          int i;
          int skillsChosen = 0, skillsTotal = max(current.per/3,current.kno/2)+forLev[current_lev].skill;
          for (i = 0; i < sk_num; ++i) {
            skill[i].train = true;
            skList.add( skill[i] );
            }

          Menu<SkillVal> skMenu(skList, global::skin.inventory);
          String title = "Train Skills";
          skMenu.setTitle(title +" "+(long)(skillsChosen+1)+" of "+(long)skillsTotal );

          Command key = kInvalid;
          bool done = false;

          while ( !done ) {
            skMenu.display();

            Point cursor = Screen::cursorat();

            Message::print();

            Screen::locate( cursor );
            
            key = skMenu.getCommand();

            switch (key) {
              case kEnter:

                if ( skill[skMenu.get().skill].train
                  || skill[skMenu.get().skill] < min( (long)current_lev+1, getVal(Skills[skMenu.get().skill].stat) ) ) {
                  skill[skMenu.get().skill] ++;
                  skMenu.get() ++;
                  skill[skMenu.get().skill].train = false;

                  if (++skillsChosen >= skillsTotal )
                    done = true;
                  else
                    skMenu.setTitle(title +" "+(long)(skillsChosen+1)+" of "+(long)skillsTotal );
                  }
                else {
                  String reason;
                  if ( skill[skMenu.get().skill].value >= current_lev+1 )
                    reason = "too inexperienced";
                  else
                    reason = String("not ") + StatStr[Skills[skMenu.get().skill].stat].adjective + " enough";

                  Message::add("You are "+reason+" to train in "+Skills[skMenu.get().skill].name+".");
                    
                  }
                  
                break;

              default:
                break;
             }

            }

          }
          
        // New talent?
        if ( forLev[current_lev].talent ) {
          // Add feat code here!
        
          }
        
        
        current_lev++;
        
        calcBurden();
        updateStatus();
        }
      
			return true;

    case aMetabol:
      hunger = (HungerState)(hunger + 1);
      
      switch (hunger) {

        case hs_normal:
          Message::add("You are no longer full.");
        	ActionEvent::create( Target(THIS), aMetabol, TIME_HNORMAL );
          break;

        case hs_hungry:
          Message::add("You are now hungry.");
        	ActionEvent::create( Target(THIS), aMetabol, TIME_HHUNGRY );
          break;

        case hs_starving:
          Message::add("<!You are now starving!>");
        	ActionEvent::create( Target(THIS), aMetabol, TIME_HSTARVING );
          break;

        case hs_dead:
          Message::add("<!You die of hunger.>");
       		global::quit = global::quit_dead;
      		Message::print();
      		Message::more();
          break;

        default:
          Message::add("Error: Your hunger level seems abnormal.");
          hunger = hs_full;
         	ActionEvent::create( Target(THIS), aMetabol, TIME_HFULL );
      
        }
        
      updateStatus();
      return true;

    case aEat:
    case aQuaff: {

      assert (!! t);
      // Error::fatal("Player attempt to eat/drink NULL item!");

      if (!Creature::perform(a,t)) return false;
      
      long nextMetabol = ActionEvent::remove( Target(THIS), aMetabol ) - Timeline::now();

      assert (nextMetabol);
      // Error::fatal("Player has no metabolism!");

      if (a == aEat)
        nextMetabol += t->getVal(attrib_nutrition);
      else
        nextMetabol += t->getVal(attrib_refreshment);

      bool message = false;

      if ( hunger == hs_starving && nextMetabol > TIME_HSTARVING ) {
        nextMetabol -= TIME_HSTARVING;
        hunger = hs_hungry;
        message = true;
        }
      if ( hunger == hs_hungry && nextMetabol > TIME_HHUNGRY ) {
        nextMetabol -= TIME_HHUNGRY;
        hunger = hs_normal;
        message = true;
        }
      if ( hunger == hs_normal && nextMetabol > TIME_HNORMAL ) {
        nextMetabol -= TIME_HNORMAL;
        hunger = hs_full;
        message = true;
        }
      if ( hunger == hs_full && nextMetabol > TIME_HFULL ) {
        nextMetabol = TIME_HFULL;
        message = true;
        }

      if (message) switch (hunger) {
      
        case hs_hungry:
          Message::add("You are still hungry.");
          break;
          
        case hs_normal:
          Message::add("You are no longer hungry.");
          break;
          
        case hs_full:
          if ( nextMetabol == TIME_HFULL ) {
            String eatStr = "eat";
            if (a == aQuaff) eatStr = "drink";
            Message::add("<-You cannot "+eatStr+" another mouthful!>");
            }
          else
            Message::add("<-You are full.>");
          break;
          
        default: break;
        }
      
     	ActionEvent::create( Target(THIS), aMetabol, nextMetabol );

      if (message) updateStatus();
      return true;

      }

		case aAscend:
		case aDescend: {
			//Level *lev = &getlevel();

      // If we're *scending stairs:
      if (t->istype( tg_stairs )) {
        // Ascend or descend the stairs...
        // Find out where we are going:
        Point3d cur_loc = getlocation();
        cur_loc.setz( t->getVal(attrib_connecting) );
        levLoc.setz( t->getVal(attrib_connecting) );

        // make sure detach doesn't clean us up:
        Target tTHIS = THIS;
  			detach();

        // Focus on the new location:
        Map::focus(cur_loc);
        
        // Enter new tile:
  			Target newPos = Map::getTile( cur_loc );
  			newPos->perform(aContain, Target(THIS) );
        }

      return Creature::perform(a,t);
      }

		case aSpMessage:
			return true;

    //case aContain:
    //  doPerform(aUpdateStatus);
    //  return Creature::perform(a,t);

		default:
			return Creature::perform(a,t);
		}

	}

Target Player::detach( int i ) {

	moved = true;
	return Creature::detach(i);

	}


//void Player::setlocation( const Point3d &p3d ) {
  //if (location == p3d)
  //  return;

void Player::setParent( TargetValue *p ) {

  Screen::hidecursor();
	//Creature::setlocation(p3d);
  
	moved = true;
  //if (location != OFF_MAP)
  
	Creature::setParent(p);
  
  doPerform(aLOS);
     
  updateLocation();
  
	levLoc = getlevel().loc();
  
	if ( global::isNew.map && levLoc.getz() == 0 ) {
		Message::add( String("<?Press '\\") + Keyboard::keytostr(keyMap[kMap].dkey) + "' to view the territory map.>");
		global::isNew.map = false;
		}
    
  Screen::showcursor();
	}

void Player::updateLocation( void ) {
// Write the name of the level at the bottom of the screen:
# ifdef ALWAYS_PRINT_LOCATION
 	Screen::locate(1,map_bottom+3);
  String name = Grammar::plural(getlevel().name(),1,false,false).abbrev(40).titlecase(),
         blank = String().fillnchar(' ',40-name.length());
	Screen::write(name+blank);
# endif // ALWAYS_PRINT_LOCATION
// Set the cursor over the player:
  Point3d location = getlocation();
	LEV_LOCATE( location.getx(),location.gety());
  Screen::focusCursor();
  }
  
#define NAME_PRINT_LEN 24
void Player::updateStatus( void ) {

  Screen::hidecursor();


  // How to calculate Job String:
  // For "John the Mechanic" display.
  String job;

  if ( form == crHuman ) {
    Skill best_skill = (Skill)0;
    int best_val = 0;
    for ( int i = 0; i < sk_num; ++i )
      if ( skill[i] > best_val ||
         ( skill[i] == best_val
        && getVal(Skills[i].stat) >= getVal(Skills[best_skill].stat) ) ) {

        best_skill = (Skill)i;
        best_val = skill[i];
        }

    job = Skills[best_skill].profession;
    }
  else
    job = creature::List[form].name;

  
	// Write name:
  Screen::locate( 1, map_bottom+1 );
	Screen::writef( String(c_name.toString()+" the "+notoriety.toString().titlecase()/*+" "+job.titlecase()*/).trunc(NAME_PRINT_LEN) );
  Screen::clearline();

  // Print weapons as follows:
  //
  // -> 1 a sword  2 a pistol [3/6]
  // or
  // -> 2 a pistol [3/6]
  // or
  // -> 1 Empty handed
  
  String out_string;

  if (!!primary ) {
    out_string = "<_1> " + primary->menustring();
    }

  if (!!secondary ) {

    // Abbreviate the name if there isn't enough room for both
    // item descriptions:
    if (!!primary ) {
      if (  ( primary->menustring().length()
            + secondary->menustring().length() + 3 )
          > ( Screen::width - NAME_PRINT_LEN )  )
        out_string.abbrev( (Screen::width - NAME_PRINT_LEN) / 2 );
    
      out_string += " ";
      }
      
    out_string += "<_2> " + secondary->menustring();
    }
  else if (!primary )
    out_string += "Empty handed";

	Screen::locate( NAME_PRINT_LEN + 2, map_bottom+1 );
  Screen::write( String().fillnchar( ' ', Screen::width - NAME_PRINT_LEN ) );
	Screen::locate( NAME_PRINT_LEN + 2, map_bottom+1 );
  Screen::writef( out_string.abbrev( Screen::width - NAME_PRINT_LEN ) );


  // Stats:
  Screen::locate( 1, map_bottom+2 );
	char buffer[160];
	sprintf(  buffer, "Crd:<%02i%-2i> Per:<%02i%-2i> Str:<%02i%-2i> Tuf:<%02i%-2i> Wil:<%02i%-2i> Kno:<%02i%-2i> Con:<%02i%-2i> Spd:<%02i%-2i>",
					 (current.crd == maximum.crd)?cGrey:cRed, current.crd,
					 (current.per == maximum.per)?cGrey:cRed, current.per,
					 (current.str == maximum.str)?cGrey:cRed, current.str,
					 (current.tuf == maximum.tuf)?cGrey:cRed, current.tuf,
					 (current.wil == maximum.wil)?cGrey:cRed, current.wil,
					 (current.kno == maximum.kno)?cGrey:cRed, current.kno,
					 (current.con == maximum.con)?cGrey:cRed, current.con,
					 (current.spd == maximum.spd)?cGrey:cRed, current.spd);
	Screen::writef(buffer);

	Screen::locate( 1, map_bottom+3 );
	sprintf( buffer, "HP:<%02i%3i>/%-3i  ",
					 HEALTHCOLOR,
					 current.hp, maximum.hp);
	Screen::writef(buffer);
/*
	sprintf( buffer, "Def:%li[%li]  ",
           getVal(DC_hitmelee),
           getVal(attrib_ac) );
	Screen::writef(buffer);
*/
  sprintf( buffer, "Exp:%2i/%-5li  ",
           current_lev, current_xp);
	Screen::writef(buffer);

  long cash = getVal(attrib_cash);
  Screen::writef( "$:" + PriceStr(cash) + "  " );


  // Write burden level:
  String burdenStr;
  switch (burden) {
    case bur_stressed: burdenStr = "Stressed  "; break;
    case bur_overload: burdenStr = "Overloaded!  "; break;
    default:
      break;
    }
	Screen::write(burdenStr);

  // Write hunger status:
  int hungerColor = cGrey;
  if (hunger == hs_full) hungerColor = cGreen;
  if (hunger == hs_starving) hungerColor = cLRed;
  String hungerStr;

  switch (hunger) {
    case hs_full: hungerStr = "Full"; break;
    case hs_hungry: hungerStr = "Hungry"; break;
    case hs_starving: hungerStr = "Starving"; break;
    default:
      break;
    }
	Screen::write(hungerStr,hungerColor);

  Screen::clearline();

  Screen::showcursor();

	}

void Player::takeDamage( const Attack &a ) {

	Creature::takeDamage(a);

	if ( global::isNew.rest && current.hp < maximum.hp && current.hp > 0 ) {
		Message::add( "<?Press '\\" + Keyboard::keytostr(keyMap[kRest].dkey) + "' to rest and recuperate.>");
		global::isNew.rest = false;
		}

//	image.val.color = HEALTHCOLOR;
	draw();

	updateStatus();

	if (current.hp <= 0 && global::quit == global::quit_false) {

		// Things to do on player death
		// Mark the game as over:
		global::quit = global::quit_dead;
		Grammar::clr();
		// Give the death notice:
		Message::add("<!You die.>");

    // Inheritance:
    // (feat here means *feature*, not Feat as in 'talent')
    Target feat_owner = getTarget(tg_feature)->getTarget(tg_owner);
    if (!!feat_owner && feat_owner->getVal(attrib_alive)) {
      Message::add(Grammar::Subject(feat_owner,Grammar::det_definite)+" inherits all of your possessions.");
      }
    
		Message::add("So long " + c_name.toString() + ".");
    
		String subject = Grammar::Subject(THIS),
           object = Grammar::Object(a.deliverer);

    if (a.deliverer != Target(THIS))
      object = a.deliverer->menustring();
    
    bool suicide = (Target(THIS)==a.deliverer);
    String deathStr;
    if (a.deathStr)
      deathStr = a.deathStr;
    else
      deathStr = KillString( a.c_values );

    Message::add( subject +(suicide?" ":" were ")+deathStr+(suicide?" ":" by ") + object + "." );
    
		Message::print();
		Message::more();

    // Get rid of further metabolism updates, etc.
    ActionEvent::remove( Target(THIS), aAll );

    // One last chance to kill the brain:
		if (!!brain)
			brain->doPerform(aSleep);
		brain = Target(NULL);

		// Todo:
		// Give the player the DYWYPI messages
		// Destroy the savegame
		}

	}

void Player::calcBurden( void ) {

  Burden initial = burden;
  // Work out our weight limits:
  long stressed = current.str * 2
                * Material::data[ m_flesh ].weight
                * max(creature::List[form].volume-creature::List[crHuman].volume,1);
  long overload = current.str * 3
                * Material::data[ m_flesh ].weight
                * max(creature::List[form].volume-creature::List[crHuman].volume,1);

  long weight = getVal(attrib_weight);

  if ( weight > overload ) {
    current.spd = 1;
    burden = bur_overload;
    }
  else if ( weight > stressed ) {
    current.spd = max(maximum.spd - ( maximum.spd * (weight-stressed) / (overload-stressed) ), (long)1);
    burden = bur_stressed;
    }
  else {
    current.spd = maximum.spd;
    burden = bur_none;
    }

  if ( burden != initial )
    updateStatus();
    
  };

long Player::getVal( StatType s ) {

	switch(s) {

    case attrib_feat:
      return feat_weapon;
      
    case attrib_proficiency:
      return feat_normal;
  
    case attrib_visible:
    // Player is *always* visible:
      return true;
      
    case attrib_demeanor:
      return notoriety.demeanor;

    case attrib_fame:
      return notoriety.fame;
    
		case attrib_moved:
			return moved;

		case attrib_plural:
			return true;

		case attrib_person :
			return Grammar::p_second;

		case attrib_vis_mon :
			return false;

    case attrib_search_radius:
      return skill[sk_awar].investment+1;
      
		case stat_spd : {

      // Effective Speed is reduced for all actions
      // when stressed.
      calcBurden();
      return current.spd;

      }

    default:
			return Creature::getVal(s);
		}

	}

long Player::getSkill( Skill sk, bool get_total ) {

  return skill[sk]+Creature::getSkill(sk,get_total);
  }

void Player::modSkill( Skill sk, int increment ) {

  skill[sk].value += increment;
  if (skill[sk].value < skill[sk].investment)
    skill[sk].investment = skill[sk].value;
  updateStatus();
  }


Result Player::test( Skill test_skill, int DC, const Target & ) {

  // Todo:  check talents list for special modifiers.

//  StatType stat = Skills[s].stat;
  int roll = DiceRoll(1,20).roll();

  if ( roll + getSkill(test_skill) < DC ) {

    if (roll == 1) return result_fumble;
    return result_fail;
  
    }
  else {

    if (roll == 20) return result_perfect;
    return result_pass;

    }

  }

Player &Player::get( void ) {

  assert ( thePlayer.raw() );
  return (Player&)*thePlayer;
  }

Target Player::handle( void ) {

  assert ( thePlayer.raw() );
  
  return thePlayer;
  }

void Player::save( void ) {
  // Save the player if allocated and in memory:
  if ( !!thePlayer )
    thePlayer.save();
  // Clear this link to player:
  thePlayer = Target();
  
  }
  

void Player::add( const Target &t ) {

  Creature::add(t);

  calcBurden();

  if (!!t && t->getVal(attrib_cash))
    doPerform(aUpdateStatus);

  }
  
bool Player::remove( const Target &t ) {

  if ( Creature::remove(t) ) {
    calcBurden();

    if (!!t && t->getVal(attrib_cash))
      doPerform(aUpdateStatus);
      
    return true;
    }

  return false;
  }


void Player::setDayTime( TimeOfDay tod ) {


    //Level &l = getlevel();

    if ( tod == tod_dawn ) {
      //l.sunrise();
      //Map::sunrise();
      // Todo: set LOS range distance based on TOD
      
      if ( getVal(attrib_sunlit) )
        // Todo: sun messages altered for players w/out sight
        // Message could be "You feel the first rays of dawn." ?
        Message::add("You see the first rays of dawn.");
      }
    else if ( tod == tod_day ) {
      if ( getVal(attrib_sunlit) )
        Message::add("The sun has risen.");
        // Todo: LOS distance
      }
    else if ( tod == tod_dusk ) {
      if ( getVal(attrib_sunlit) )
        Message::add("The daylight grows faint.");
        // Todo: LOS distance
      }
    else if ( tod == tod_night ) {
      //if ( getVal(attrib_sunlit) )
      if (levLoc.getz() >= 0)
        Message::add("The sun sets.");
        // Todo: LOS distance
      }


  }

#endif // PLAYER_CPP
