// [monster.cpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( MONSTER_CPP )
#define MONSTER_CPP

#include "monster.hpp"
#include "message.hpp"
#include "level.hpp"
#include "grammar.hpp"
#include "random.hpp"
#include "crlist.hpp"
#include "event.hpp"
#include "weapon.hpp"
#include "clothing.hpp"
#include "player.hpp"
#include "global.hpp"
#include "input.hpp"
#include "dialog.hpp"
#include "quantity.hpp"
#include "money.hpp"
#include "quest.hpp"
#include "book.hpp"
#include "widget.hpp"

//--[ class Monster ]--//

Monster::Monster( const Option *o )
: Creature( o ) {

  // Polymorph into an appropriate species
  CreatureType toForm = o->form;
  
  // -- New, guaranteed creation:
  int rarity = 0, mon_rarity = 1;
  // If we're creating a random creature:
	if ( toForm == crInvalid )
	for ( int i = FIRST_MONSTER; i <= LAST_MONSTER; ++i ) {

    // ATM all monsters are equal rarity:
    //mon_rarity =  creature::List[i].rarity;

    // if the creature matches our give options:
    if (
         ( // Correct odor:
           creature::List[i].odor <= o->odor
         )

         &&
         ( // Right theme:
           (creature::List[i].theme & o->theme)
           || o->theme == th_all
         )
         &&
         ( // Required habitat:
           creature::List[i].habitat == o->habitat
           || o->habitat == Map::any
         )
         //&&
         //( // Desired difficulty:
         //  getVal(attrib_xp) <= o->max_xp
         //)
       )
      // and its random number comes up:
      if (Random::randint(rarity += mon_rarity) < mon_rarity)
        // It's the current creature:
        toForm = (CreatureType)i;
    }

  if (toForm == crHorse) {
    Message::add((long)(o->odor == od_stale));
    Message::add((long)(o->theme == th_sewers));
    Message::add((long)(o->habitat == Map::any));
    }

    
  assert(toForm != crInvalid);

  polymorph(toForm);
  // Set monster level based on initial form:
  current_lev = 1;//(int)creature::List[toForm].odor;
  //if ( current_lev < o->odor ) current_lev += DiceRoll(2,(int)creature::List[toForm].odor).roll();

  // Level is always 1 or higher:
  //current_lev++;
  assert(current_lev > 0);

  maximum.hp += maximum.tuf * (current_lev-1);
    
  current = maximum;
  
  // Maintain gender preference
  gender = o->gender;
  if (gender == sex_any)
    gender = (Random::randbool())?sex_male:sex_female;
	}

Monster::Monster( Monster &copy )
: Creature( copy ) {

	}

Target Monster::clone( void ) {

	Monster *newcopy = new Monster( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

Monster::~Monster( void ) {
	}

Monster::Monster( std::iostream &inFile, TargetValue *TVP )
: Creature(inFile, TVP) {

	}

void Monster::toFile( std::iostream &outFile ) {

	Creature::toFile( outFile );

	}

String Monster::name( NameOption n_opt ) {

  String name = creature::List[form].name;
  if (gender != sex_female) {
    if (creature::List[form].male)
      name = creature::List[form].male;
    }
  else {
    if (creature::List[form].female)
      name = creature::List[form].female;
    }
    
  switch (n_opt) {
    case n_menu_noprice:
    case n_menu: {
      String ret_str = "#";
      if (current.hp <= 0)
        // Todo: monsters that can be rebuilt/regenerate will
        // be repectively called "broken" and "comatose".
        ret_str += "dead ";
      else
        ret_str += String::digitorder(current_lev) + " level ";
        
      ret_str += name + "~";

#     ifdef TEST_BUILD
      long real_xp = getVal( attrib_xp ),
           lev_diff = getVal(attrib_level)-Player::get().getVal(attrib_level)+1,
           xp = real_xp;

      while (lev_diff-- > 0)
        xp += real_xp / (lev_diff+1);
      
      ret_str += String(" (")+(long)xp+"xp)";
#     endif // TEST_BUILD
      
    	return ret_str + Item::name(n_opt);
      }
      
    case n_noun:
      if (c_name.generated()&&(id_mask&id_name))
        return c_name.toString();
      return name;
      
    default:
    	return String( String("#") + ((current.hp <= 0)?"dead ":"") + name + "~" )
           + Item::name(n_opt);
    }
  }

targetType Monster::gettype( void ) const {
	return tg_monster;
	}
bool Monster::istype( targetType t ) const {
	if (t == tg_monster)
		return true;
	else
		return Creature::istype(t);
	}

bool Monster::alive( void ) const {
	return true;
	}

void Monster::draw( void ) {
	Creature::draw();
	}


const char *VoiceStr [vo_num] = {

  NULL,
  NULL,
  "#bark~",
  "#meow~",
  "#howl~",
  "#moo~",
  "#neigh~",
  "#oink~",
  "#baa~",
  "#bleet~",
  "#cluck~",
  "#squeek~",
  "#quack~",
  "#coo~",
  "#squawk~",
  "#hiss~",
  "#roar~",
  "#growl~",
  "#buzz~",
  "#pipe~",
  "#click~"

  };

bool Monster::perform( Action a, const Target &t ) {

	switch (a)  {

    case aAssault:
      // We used to give a message when a monster decides that you're
      // on the menu, but now we just let the player guess.  Hell,
      // it happens often enough that it's best left as a surprise!

      //if (RawTarget::getVal(attrib_visible)) {
      //  String subject = Grammar::Subject(THIS,Grammar::det_definite);
      //  Message::add( subject + " hungrily look"+( getVal(attrib_plural)?"":"s" ) +" at "+ Grammar::Object(t,Grammar::det_definite) + "." );
      //  }
      return true;
      
    case aChat:
      return true;

    case aRespond: {
      String response = " nothing.";

      Skill sk_chat = creature::List[form].sk_chat;
      int dc_chat = creature::List[form].dc_chat;

      // Brain must have time to react first:
      if (!!brain) brain->perform(aRespond, t);

      if ( t->getSkill(sk_chat,false) >= dc_chat ) {
        // Special case for talking creatures

        enum Response {
        
          re_happy,
          re_angry,
          
          re_num,
          re_invalid = re_num
          
          };

        Response type = (Response) Random::randint( re_happy );
        if ( !!brain && !brain->capable(aChat,t) ) type = re_angry;

        switch (type) {
          case re_happy:
            if (VoiceStr[creature::List[form].voice])
              response = ".";
            else
              response = " nothing.";
            if (creature::List[form].quote)
              response = String(", \"")+creature::List[form].quote+"\"";
            break;

          default:
            if (capable(aEat,t))
              response = ", \"Mmmm, dinner!\"";
            else
              response = ", \"Shut up and fight me!\"";
            if (creature::List[form].quote)
              response = String(", \"")+creature::List[form].quote+"\"";
            break;
          }
        }
      else {
        // Otherwise give a message based on voice:
        if (VoiceStr[ creature::List[form].voice ]) {
          response =  ".  ";

          String subject = Grammar::Subject(t, Grammar::det_definite);
          response += subject.titlecase()
                   + " can not understand "
                   + Grammar::Object(THIS, Grammar::det_definite) + ".";

          }
        }

      String says = "says";
      if (VoiceStr[creature::List[form].voice])
        says = Grammar::plural(VoiceStr[ creature::List[form].voice ],2,true,false);

      Message::add( Grammar::Subject(THIS, Grammar::det_definite) +" "+says+ response);
          
      return true;
      }
     

		default:
			return Creature::perform(a,t);
		}

	}


void Monster::takeDamage( const Attack &a ) {


  Target retain(THIS);
  
	if (current.hp <= 0) {
		Creature::takeDamage(a);
		return;
		}

	Creature::takeDamage(a);

  // If we are domestic, tell the owner we're being picked on:
  if (!!getTarget(tg_owner))
    getTarget(tg_owner)->perform( aAlertPetDam, THIS );

	if (current.hp <= 0) {

		// If dead, change image:
		image.val.appearance = '%';
		// And no longer heal:
		ActionEvent::remove( Target(THIS), aHeal );
		// or think:
		if (!!brain)
			brain->doPerform(aSleep);
		brain = Target(NULL);

    // Special monster corpses:
    // These are handled here because the items dropped are *part of* the corpse,
    // i.e. the shabby suit is part of the scarecrow's body, not just something worn.
    //Target parent = getTarget(tg_parent);
    if (!!parent) {
      Target trophy;
      switch (form) {
      // Scarecrows always drop shabby suits:
        case crScarecrow:
             trophy = Clothing::create()->Force("a shabby suit");
             break;
             
      // Bears drop bearskin caps:
        case crGrizzlyBear:
        case crBlackBear:
             trophy = Clothing::create()->Force("a bearskin cap");
             break;
       
     
      // The following animals *will* drop trophies,
      // but only when I make that part of gameplay
      // viable.  For now, commented out.
      // The following animals drop trophies:
      /*
        case crArmadillo:
        case crBuffalo:       case crCoyote:
        case crEagle:         case crGila:
        case crRabbit:        case crPorcupine:
        case crCougar:        case crRattle:
        case crScorpion:      case crSkunk:
        case crVulture:       case crWolverine:
        case crWolf:
             trophy = Remains::create(form);
             break;
      */
      
        default: break;
        }

      // If we created a trophy:
      if (!!trophy) {
        // If the creature is visible:
        if (getVal(attrib_visible))
          // Know the form of trophy (e.g. *eagle* feather)
          trophy->identify(id_form);
        // Put the trophy down:
        parent->perform( aContain, trophy );
        }
      
      }

		// Maybe get rid of corpse:
		if ( !!parent
       && Random::randint(100) >= creature::List[form].corpse_pc ) {
			detach();
      ActionEvent::remove( Target(THIS), aAll );
      }
    // Or make sure the corpse will rot away:
    else {
      ActionEvent::create( Target(THIS), aRot, TIME_ROT );
      draw();
      }

    // Make sure that the killer gets the experience:
    if (!!a.deliverer)
      a.deliverer->perform(aGainExp,THIS);
		}

	}


long Monster::getVal( StatType s ) {

	switch(s) {
		case attrib_person :
			if (current.hp<=0 || form != crHuman || gender==sex_none)
				return Grammar::p_third_n;
			return (gender==sex_male)
               ? Grammar::p_third_m
               : Grammar::p_third_f;

		default:
			return Creature::getVal(s);
		}

	}

long Monster::getSkill( Skill skill, bool get_total ) {
// Monsters get +1 to fight skills per experience level:
  if (skill == sk_figh || skill == sk_mark)
    return current_lev + Creature::getSkill(skill,get_total);

  // ATM, otherwise just return skill as per stats:
  return Creature::getSkill(skill,get_total);
  }
  
//--[ class Human ]--//


struct JobStats {

  const char *n_default,
             *n_female;

  int demeanor;
  int level;
  int color;
  const char *quote,
             *quote_angry;

  };

const JobStats jobStats [ job_num ] = {
  /*
  //--[ Jobs 0.94 ]--//
  {"golfer",     NULL,         3, cLGreen, "Quiet on the green!", "Fore!" },
  {"magnate",    NULL,         2, cViolet, NULL, "I have friends in high places!" },
  {"journalist", NULL,         2, cOrange, NULL, "What a scoop!" },
  {"artist",     NULL,         1, cRed,    "You don't understand the complexities of my work.", NULL },
  {"vicar",      "nun",        1, cDGrey,  "The meek shall inherit the earth!", "Sinner!" },
  {"tourist",    NULL,         1, cYellow, "I think I just missed my train.", "How exciting!" },
  {"soldier",    NULL,         5, cLBlue,  "Move along.", "Take no prisoners!" },
  {"eccentric",  NULL,         1, cLGrey,  "Count the spoons!", "Please, end it all!" },
  {"butler",     "maid",       2, cIndigo, NULL, NULL },
  //-----------------//
  */
  
  //--[ Jobs 0.95 ]--//
  {"bandit",     NULL,       lawless/3, 1, cGreen,  NULL, NULL },
  {"desperado",  NULL,       lawless/3, 2, cGreen,  NULL, NULL },
  {"freebooter", NULL,       lawless/2, 3, cGreen,  NULL, NULL },
  {"outlaw",     NULL,       lawless/2, 4, cGreen,  NULL, NULL },
  {"harrier",    NULL,         lawless, 5, cGreen,  NULL, NULL },

  {"cultist",    NULL,       lawless/2, 3, cGrey,   NULL, NULL },
  {"prophet",    NULL,       lawless/2, 5, cGrey,   NULL, NULL },

  {"gunslinger", NULL,          lawful, 8, cLGrey,  NULL, NULL },
  {"sheriff",    NULL,          lawful, 5, cYellow, NULL, NULL },
  {"marshal",    NULL,          lawful, 5, cYellow, NULL, NULL },

  {"undertaker", NULL,         neutral, 2, cDGrey,  NULL, NULL },

  {"cardshark",  NULL,       lawless/4, 5, cLGrey,  NULL, NULL },
  {"maverick",   NULL,       lawless/4, 5, cLGrey,  NULL, NULL },

  {"yokel",      NULL,         neutral, 1, cRed,    NULL, NULL },
  {"rustic",     NULL,         neutral, 2, cRed,    NULL, NULL },
  {"hillbilly",  NULL,         neutral, 3, cRed,    NULL, NULL },
  {"redneck",    NULL,         neutral, 4, cRed,    NULL, NULL },

  //--[ Jobs 0.96 ]--//
  {"scout",      NULL,        lawful/3, 1, cIndigo, NULL, NULL },
  {"brave",      NULL,        lawful/3, 1, cIndigo, NULL, NULL },
  {"warrior",    "hunter",    lawful/3, 3, cIndigo, NULL, NULL },
  {"shaman",     NULL,        lawful/2, 4, cIndigo, NULL, NULL },

  //-----------------//

  //--[ Occupations ]--//
  {"librarian",  NULL,        lawful/2, 4, cWhite,  "Shh!", "I told you: Shh!" },
  {"bartender",  "barmaid",   lawful/3, 5, cWhite,  "What'll it be?", "I think you've had enough to drink!" },
  {"salesman",   "saleswoman",lawful/2, 4, cWhite,  NULL, NULL },
  {"pharmacist", NULL,        lawful/2, 4, cWhite,  NULL, NULL },
  {"stablehand", NULL,        lawful/2, 4, cWhite,  "There's no better transport than a sturdy horse.", NULL },
  {"black-market dealer",
                 NULL,       lawless/2, 6, cWhite,  NULL, NULL },
  //-------------------//

  };


Human::Human( const Option *o )
: Creature( o ), job(o->job) {

  polymorph(crHuman);

  gender = o->gender;
  if (gender == sex_none || gender == sex_any)
    gender = (Random::randbool())?sex_male:sex_female;
    
  image.val.appearance = 't';

  if (o->job == job_invalid)
    job = (Job)Random::randint( job_first_shk );
  else
    job = o->job;

  current_lev = jobStats[job].level;
  image.val.color = jobStats[job].color;
  
  // Give inventory:

  switch (job) {

    case job_bandit:    case job_freebooter:
    case job_desperado: case job_outlaw:
    case job_harrier:
      // Banditos get a bandolier, pistols and a sombrero
      break;
  
    case job_cultist:   case job_prophet: {
      // Cult members get books on the occult:
      //Book::Option book_opt;
      //book_opt.skill = sk_occu;
      //perform( aContain, Book::create(book_opt) );
      break;
      }
  
    case job_gunslinger:
      // Gunslingers get pistols:
      break;

    case job_sheriff:   case job_marshal:
      // Lawmen get a rifle:
      break;

    case job_undertaker:
      // Undertakers get a hammer and a stovepipe hat
      perform( aContain, Melee::create()->Force("a carpentry hammer") );
      perform( aWear, Clothing::create()->Force("a stovepipe hat") );
      break;

    case job_cardshark: case job_maverick:
      // Cardplayers get a stiletto and a shabby suit:
      perform( aContain, Melee::create()->Force("a stiletto") );
      perform( aWear, Clothing::create()->Force("a shabby suit") );
      break;
  
    case job_hillbilly: case job_redneck:
    case job_yokel:     case job_rustic:
      // Farmers get overalls and a pitchfork:
      perform( aContain, Melee::create()->Force("a pitchfork") );
      perform( aWear, Clothing::create()->Force("a pair of overalls") );
      break;

    case job_scout:   case job_brave:
    case job_warrior: case job_shaman: {
      // Natives get tomahawks and bows & arrows:
      perform( aContain, Ranged::create()->Force("a long bow") );
      perform( aContain, Melee::create()->Force("a tomahawk") );
      // Arrows:
      Ammo::Option ammo_opt;
      ammo_opt.type = bul_arrow;
      Quantity::Option q_opt;
      // get 2d6 of them:
      q_opt.target = Ammo::create(ammo_opt);
      q_opt.quantity = DiceRoll(3,6).roll();
      
      perform( aContain, Quantity::create(q_opt) );
      
      break;
      }
  
    default:
      // Shopkeepers all get axes:
      perform( aContain, Melee::create()->Force("an axe") );
      break;
    }

  
  // Give a name if a shopkeeper:
  if (job >= job_first_shk)
    c_name = Random::name( gender==sex_male, /*bgTheme*/th_none );

  // Give a quest to any person clev 5 or more:
//  if (current_lev >= 5) {
  // Except at the moment, give one to nobody:
  //  quest = Quest::create();
  //  quest->setParent(THIS);
//    }
    
	}


/*
void Human::setlocation( const Point3d &p3d ) {

  if (!! quest) quest->setlocation(p3d);
  Creature::setlocation(p3d);
  } */

void Human::setTHIS( TargetValue *TVP ) {

	THIS = TVP;

  if (!! quest) quest->setParent(THIS);

  Creature::setTHIS(TVP);
	}

/*
Target Human::findTarget( targetType type, const Target& t ) {

  switch (type) {
    case tg_parent:
    case tg_possessor: {
    
      //assert( location == t->getlocation() );
      if ( quest == t ) return THIS;

      Target found;
      if (!!quest) {
        found = quest->findTarget(type,t);
        if (!!found) return found;
        }
      return Creature::findTarget(type,t);
      }
    default:
      return Creature::findTarget(type,t);
    }
  }
*/

Human::Human( Human &copy )
: Creature( copy ) {

  job = copy.job;

  if (!!copy.quest)
    quest = copy.quest->clone();
	}

Target Human::clone( void ) {

	Human *newcopy = new Human( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

Human::~Human( void ) {

	if (!!quest) quest.save();

	}

Human::Human( std::iostream &inFile, TargetValue *TVP )
: Creature(inFile, TVP) {

	inFile.read( (char*) &job, sizeof(job) );
  
	inFile >> quest;
	long saveLoc = inFile.tellg();
  if (!!quest) quest->parent = THIS;
	inFile.seekg(saveLoc);

	}

void Human::toFile( std::iostream &outFile ) {

	Creature::toFile( outFile );

	outFile.write( (char*) &job, sizeof(job) );

  outFile << quest;

	}

String Human::name( NameOption n_opt ) {

//  return String("#") + (long)(THIS) + "~";

  if ( current.hp <= 0 && n_opt != n_owner ) return String("#cadaver~");

  String name = jobStats[job].n_default;
  if (gender == sex_female && jobStats[job].n_female)
    name = jobStats[job].n_female;

  switch (n_opt) {
    case n_owner: {
      String o_name;
      if (c_name.generated()&&(id_mask&id_name))
        o_name = c_name.toString()+"~";
      else
        o_name = ( "#" + name + "~" );

      if (o_name[o_name.length()-1] == 's')
        o_name += "'";
      else
        o_name += "'s";

      if ( current.hp <= 0 )
        o_name = "#abandoned~";
        
      o_name = Grammar::plural( o_name, 1, false, false );
        
      return o_name;
      }
      
    case n_menu_noprice:
    case n_menu: {
      String ret_str;
      
      if (c_name.generated() && (id_mask&id_name))
        ret_str = c_name.toString() + ", ";
        
      ret_str += "#";
      ret_str += String::digitorder(current_lev) + " level ";
      ret_str += name + "~";
      
#     ifdef TEST_BUILD
      long xp = getVal(attrib_xp),
           mult = current_lev-Player::get().getVal(attrib_level)+1;
      if (mult > 0)
        xp *= mult;
      ret_str += String(" (")+(long)(xp)+"xp)";
#     endif // TEST_BUILD

    	return ret_str;
      }

    case n_noun:
      if (c_name.generated()&&(id_mask&id_name))
        return c_name.toString();
      return name;
 
    default:
      if (c_name.generated()&&(id_mask&id_name))
        return String( c_name.toString() + "~" );
    	return String( "#" + name + "~" );
    }

  }

targetType Human::gettype( void ) const {
	return tg_human;
	}
bool Human::istype( targetType t ) const {
	if (t == tg_human)
		return true;
	else
		return Creature::istype(t);
	}

bool Human::alive( void ) const {
	return true;
	}

void Human::draw( void ) {
	Creature::draw();
	}

bool Human::capable( Action a, const Target &t ) {

  switch (a) {
    case aAssault: {
      // Is the human capable of an unprovoked assault on Target t?
      // By default the only reason to attack is alignment:
      // This should be the town reputation rather than actual
      // alignment.

      // If no target is given, assume they mean the player:
      Target target = t;
      
      if (!target)
        target = Player::handle();
      
			// If the creature is not ourself:
			if ( Target(THIS) != target
      // and is different enough of alignment :
      &&   Notoriety( 0, getVal(attrib_demeanor) )
           .impressionOf( Notoriety( 0, target->getVal(attrib_demeanor) ) )
            == imp_assault )
        return true;

      return Creature::capable(aAssault,target);
      }
      
    default:
      return Creature::capable(a,t);
    }
  }
  
bool Human::perform( Action a, const Target &t ) {

	switch (a)  {

    case aAssault:
      if (RawTarget::getVal(attrib_visible)) {
        String subject = Grammar::Subject(THIS,Grammar::det_definite);
        // Todo: Response should indicate a reason for attack
        //       as in, "Stop, lawbreaker!" or "Stand and deliver!"
        // Message::add( subject + " prepares for action!" );

        // Prepare for combat:
        // - No, this should take a turn, so is handled in the AI
        // doPerform(aWield);
        }
      return true;

    case aWield: {
      // If we know what to wield, do it:
      if (!!t) return Creature::perform(a,t);

      // Otherwise, find the best item in inventory to wield:
      // Currently only looks for melee weapons:
      Target best = primary;
      CombatValues cv_best, cv_current;

      // If we're already wielding a weapon, assume it's the best:
      if (!!best)
        return false;
      
      if ( inventory && inventory->reset() )
      do {

        if ( inventory->get()->istype(tg_melee) ) {

          if (!best) {
            best = inventory->get();
            best->GET_STRUCT(s_offence_cv,cv_best);
            }
          else {
            inventory->get()->GET_STRUCT(s_offence_cv,cv_current);

            if (cv_current.penetrated() > cv_best.penetrated()
            &&  cv_current.damaged()    > cv_best.damaged() ) {
              best = inventory->get();
              cv_best = cv_current;
              }

            }
            
          }
          
        } while ( inventory->next() );

      if (!!best)
        return Creature::perform(aWield,best);

      return false;

      }
      
    case aAlertPetDam: {
      // Alert of a pet's being damaged:
      if (!t)
        return false;

      // We can only care if we're alive:
      if (!getVal(attrib_alive) )
        return false;
        
      // find out who is doing the damaging:
      Target attacker = t->getTarget(tg_controller);
      
      // make sure the attacker is a living thing:
      if (!attacker || attacker == t || !attacker->getVal(attrib_alive) )
        return false;

      // Become aggravated towards the attacker:
      if (!!brain)
        brain->perform(aWarn,attacker);
      
      // and give a message:
      if (RawTarget::getVal(attrib_visible))
        Message::add(Grammar::Subject(THIS,Grammar::det_definite)
                   + " says, \"Keep away from " + Grammar::plural(t,Grammar::det_demonst_wout) + "!\"");

      return true;
        
      }

    case aAlertTrespass: {
    
      if (!t || t == THIS)
        return false;
      // Ignore unless the item is in our house
      //if ( t->getTarget(tg_feature)->getTarget(tg_owner) != THIS )
      //  return false;

      // If the item is a creature we don't own:
      if ( t->istype(tg_creature) && t->getTarget(tg_owner)!=THIS ) {
        // Say hello to creatures:
        if ( RawTarget::getVal(attrib_visible)&&t->getVal(attrib_visible) ) {

          String hello = Grammar::Subject(THIS,Grammar::det_definite)+" says, \"";

          // Todo: add a check for reputation:
          // Creatures are welcome in proportion
          // to how much they are idolised.
          bool is_welcome = getVal( attrib_shk );

    			// If the creature is not ourself:
    			if ( Target(THIS) != t
          // and is close enough of alignment :
          &&   Notoriety( 0, getVal(attrib_demeanor) )
               .impressionOf( Notoriety( 0, t->getVal(attrib_demeanor) ) )
                == imp_assault )
            return true;


          // If we're dead, we can't talk!
          if (!getVal(attrib_alive)) {

            // But we can tell the player the place is abandoned:
            if (!t->getTarget(tg_feature)->identified( id_abandoned ))
              if (Random::randbool())
                Message::add("This place seems to be deserted.");
              else
                Message::add("A layer of dust coats the floor.");

            t->getTarget(tg_feature)->identify( (IDmask)(id_abandoned | id_owner) );

            return true;
           }
          
          // We now know who owns the place:
          t->getTarget(tg_feature)->identify(id_owner);

          // If the creature is welcome, it's not trespassing:
          if ( is_welcome ) {
          
            hello += "Hello, "+t->name(n_noun)+", ";
            if (!(id_mask&id_name)&&c_name.generated()) {
              id_mask = (IDmask)(id_mask | id_name);
              hello += "I am "+Grammar::plural(THIS) +"; ";
              }
            
            hello += "welcome to ";
            }
          else {
            // Todo: chance to decrease reputation for trespassing:
            hello += "Hey!  Get out of ";
            }

          hello += Grammar::plural(t->getTarget(tg_feature),Grammar::det_demonst_with)+"!\"";
          Message::add(hello);

        	if ( global::isNew.chat ) {
        		Message::add("<?Press '\\" + Keyboard::keytostr(keyMap[kChat].dkey) + "' to chat to people and animals.>");
        		global::isNew.chat = false;
        		}
          
          }

        return true;
        }
      else if (t->getVal(attrib_still) && getVal(attrib_alive)) {
        // Buy items:
        if (!!t->getTarget(tg_owner) && t->getTarget(tg_owner)->getVal(attrib_alive)) {

          String says = Grammar::Subject(THIS,Grammar::det_definite)+" says, ";

          if (t->getTarget(tg_owner) != THIS) {
            // todo: check to see if the human would want the item:
            bool wants = true;
            if (wants) {
              // Todo: a check to see if the shopkeeper is 'legit'
             if ( t->getTarget(tg_controller)->getVal(attrib_alive) )
              if ( t->getTarget(tg_controller) != t->getTarget(tg_owner) && jobStats[job].demeanor > -DEMEANOR_MID_BAND )
                Message::add(says+"\"I don't deal in stolen merchandise!\"");
              else {
                // Price depends on ratio of shopkeeper and buyer diplomacy.
                Target buyer = t->getTarget(tg_controller);
                if (!!buyer && buyer->istype(tg_player) ) {
                
                  long price = t->getVal(attrib_price)
                             * buyer->getSkill(sk_dipl)
                             / getSkill(sk_dipl);

                  Message::add(says+"\"I'll pay "+PriceStr(price,false)+" for "+Grammar::Object(t,Grammar::det_demonst_with)+".\"");
                  Message::print();

              		Dialog dialog("Selling", String("Accept ")+PriceStr(price,false)+"? [Y/n]", global::skin.quantity );
              		dialog.display();

              		int key;
              		do {

              			key = dialog.getKey();

              			} while (  Keyboard::toCommand(key) != kEscape
              							&& Keyboard::toCommand(key) != kEnter
              							&& Keyboard::toCommand(key) != kSpace
              							&& key != 'y' && key != 'Y' && key != 'n' && key != 'N' );

              		if ( key == 'y' || key == 'Y'||
                       Keyboard::toCommand(key) == kEnter || Keyboard::toCommand(key) == kSpace ) {


                    Quantity::Option q_opt;

                    int dollars = WholeDollars(price),
                          cents = WholeCents(price);
                    if (dollars) {
                      q_opt.quantity = dollars;
                      q_opt.target = Money::create()->Force("a dollar");
                      buyer->perform( aContain, Quantity::create( q_opt ) );
                      }

                    if (cents) {
                      q_opt.quantity = cents;
                      q_opt.target = Money::create()->Force("a cent");
                      buyer->perform( aContain, Quantity::create( q_opt ) );
                      }

                    t->perform(aSetOwner, THIS);
                    t->doPerform(aRestack);

                    buyer->doPerform(aUpdateStatus);
                    }
                    
                  }
                }
             }
            else
              Message::add(says+"\"Take your junk with you!\"");
            }
          else // owner == THIS
            if (getVal(attrib_trader)) {
              if (t->getTarget(tg_possessor) != THIS) {
                // Someone has moved one of our items:
                Target possessor = t->getTarget(tg_possessor);
                if (!possessor) {
                  if (RawTarget::getVal(attrib_visible))
                    Message::add(says+"\"You'll pay for that!\"");
                  }
                else {
                  // Someone picked up one of our items:
                  if (RawTarget::getVal(attrib_visible)) {
                    long price = t->getVal(attrib_price);
                    Message::add(says+"\"Only "+PriceStr(price)+" for "+Grammar::Object(t,Grammar::det_demonst_wout)+".\"");
                    t->identify(id_owner);
                    }
                  Target deliverer = t->getTarget(tg_controller);
                  
                  assert ( !!deliverer && deliverer != t);
                  
                  deliverer->perform(aAddBill,t);

                  //  Error::fatal("Item delivered nohow!");
                  }
                }
              else { // possessor == THIS && owner == THIS
                Target deliverer = t->getTarget(tg_controller);
                if (!!quest && quest->capable(aDeductBill,t) ) {
                  if (RawTarget::getVal(attrib_visible) )
                    quest->doPerform(aDeductBill);
                  }
                if ( !!deliverer && deliverer != t
                    && deliverer->perform(aDeductBill,t)
                    && RawTarget::getVal(attrib_visible) )

                    Message::add(says+"\"Deducted.\"");


                }
              }
            else {
              if (t->getTarget(tg_possessor) != THIS) {
                if (RawTarget::getVal(attrib_visible) )
                  Message::add("\"Please be careful with that!\"");
                }
              else { // possessor == THIS
//                if (RawTarget::getVal(attrib_visible) )
                  if (!!quest && quest->capable(aDeductBill,t) )
                    quest->perform(aDeductBill,t);
                }
              
              }
            
          }
        else t->perform(aSetOwner,THIS);
        }
      return true;
      }

    case aPolymorph:
      // Todo: Human polymorphing is disabled for the moment.
      if (RawTarget::getVal(attrib_visible))
        Message::add("A strange aura surrounds "+Grammar::plural(THIS)+" for a moment.");
      return true;
      
    case aChat:
      return true;

    case aRespond: {
      String response = " nothing.";

      Skill sk_chat = creature::List[form].sk_chat;
      int dc_chat = creature::List[form].dc_chat;

      // Brain must have time to react first:
      if (!!brain)
        brain->perform(aRespond, t);
      else {
        // and if we don't have a brain, this creature is dead!
        Message::add(Grammar::Subject(THIS) + " is dead!" );
        current.hp = 0;
        }
      

      if ( t->getSkill(sk_chat,false) >= dc_chat ) {
        // Special case for talking creatures

        enum Response {
        
          re_fluff,
/*
          re_fortune,
          re_req,
          re_analysis,
          re_salute,
          re_welcome,
          re_tourism,
*/
          re_happy,
          

          re_angry,
          
          
          re_num,
          re_invalid = re_num
          
          };

        Response type = (Response) Random::randint( re_happy );
        if ( !!brain && !brain->capable(aChat,t) ) type = re_angry;

        // Now if we have a quest and we're not angry,
        // let the quest do the talking:
        if (type != re_angry && !!quest)
          return quest->perform(aRespond, t);

        switch (type) {
          case re_fluff:

            if (jobStats[job].quote)
              response = ", \"" + String(jobStats[job].quote) + "\"";
            else
              response = " nothing.";
            
            break;
/*
          case re_fortune:
            response = ", \""+Random::fortune().sentence()+"\"";
            break;
          case re_req:
            response = ", \"I know of an abandoned mine to the north.\"";
            break;
          case re_analysis:
            response = ", \"You look like a hero.\"";
            break;
          case re_salute:
            // Todo: identify name
            // identify( id_name );
            response = ", \"Hi, I am "+Grammar::plural(THIS)+".\"";
            break;
          case re_welcome:
            // identify town name
            getlevel().identify( id_name );
            response = ", \"Welcome to "+Grammar::plural(getlevel().name(), 1, false, false, Grammar::det_definite)+".\"";
            break;
          case re_tourism:
            response = ", \"Take a look around, we have many nice houses.\"";
            break;
          case re_quest:
            response = ", \"Come back later and I'll offer you a quest.\"";
            break;
*/
          default:
            if (jobStats[job].quote_angry)
              response = ", \"" + String(jobStats[job].quote_angry) + "\"";
            else
              response = ", \"You have made a grave error.\"";
            break;
          }
        }
      else {
        // Otherwise give a message based on voice:
        if (VoiceStr[ creature::List[form].voice ]) {
          response =  ".  ";

          String subject = Grammar::Subject(t, Grammar::det_definite);
          response += subject.titlecase()
                   + " can not understand "
                   + Grammar::Object(THIS, Grammar::det_definite) + ".";

          }
        }

      String says = "says";
      if (VoiceStr[creature::List[form].voice])
        says = Grammar::plural(VoiceStr[ creature::List[form].voice ],2,true,false);

      Message::add( Grammar::Subject(THIS, Grammar::det_definite) +" "+says+ response);
          
      return true;
      }
     
    case aEnterLev:
    case aLeaveLev:
      if (!!quest)
        quest->perform(a,t);
      return Creature::perform(a,t);

  
		default:
			return Creature::perform(a,t);
		}

	}


void Human::takeDamage( const Attack &a ) {

	if (current.hp <= 0) {
		Creature::takeDamage(a);
		return;
		}

	Creature::takeDamage(a);

	if (current.hp <= 0) {
		//current.hp = 1;
		//if ( RawTarget::capable( aView ) )
		//	Message::add( Grammar::Subject(THIS,Grammar::det_definite) +" dies.");
		//current.hp = 0;

		// If dead, change image:
		image.val.appearance = '%';
		// And no longer heal:
		ActionEvent::remove( Target(THIS), aHeal );
		// or think:
		if (!!brain)
			brain->doPerform(aSleep);
		brain = Target(NULL);
    // or give out quests:
    if (!!quest)
      quest->doPerform(aSleep);

		// Maybe get rid of corpse:
		if (parent != NULL && Random::randint(100) >= creature::List[form].corpse_pc ) {
			detach();
      ActionEvent::remove( Target(THIS), aAll );
      }
    // Or make sure the corpse will rot away:
    else {
      ActionEvent::create( Target(THIS), aRot, TIME_ROT );
      draw();
      }

    // Make sure that the killer gets the experience:
    if (!!a.deliverer)
      a.deliverer->perform(aGainExp,THIS);
		}

	}


long Human::getVal( StatType s ) {

	switch(s) {

    // For humans, shk is the same as trader:
    case attrib_trader:
    case attrib_shk:
      return (job >= job_first_shk);

    case attrib_demeanor:
      return jobStats[job].demeanor;

    case attrib_person :
			if ( current.hp<=0 )
				return Grammar::p_third_n;
			return (gender==sex_male)
               ? Grammar::p_third_m
               : Grammar::p_third_f;

		default:
			return Creature::getVal(s);
		}

	}

long Human::getSkill( Skill skill, bool get_total ) {
// ATM Humans are considered to have one point in every skill per level:
  return current_lev + Creature::getSkill(skill,get_total);
  }

//--[ class Mount ]--//

Mount::Mount( const Option *o )
: Creature( o ) {

  // Polymorph into an appropriate species
  CreatureType toForm = o->form;
  
  // -- New, guaranteed creation:
  int rarity = 0, mon_rarity = 1;
  // If we're creating a random creature:
	if ( toForm == crInvalid )
	for ( int i = FIRST_MOUNT; i <= LAST_MOUNT; ++i ) {

    // ATM all mounts are equal rarity:
    //mon_rarity =  creature::List[i].rarity;

    // if the creature matches our give options:
    if (
         ( // Correct odor:
           creature::List[i].odor <= o->odor
         )

         &&
         ( // Right theme:
           (creature::List[i].theme & o->theme)
           || o->theme == th_all
         )
         &&
         ( // Required habitat:
           creature::List[i].habitat == o->habitat
           || o->habitat == Map::any
         )
         //&&
         //( // Desired difficulty:
         //  getVal(attrib_xp) <= o->max_xp
         //)
       )
      // and its random number comes up:
      if (Random::randint(rarity += mon_rarity) < mon_rarity)
        // It's the current creature:
        toForm = (CreatureType)i;
    }

  if (toForm == crHorse) {
    Message::add((long)(o->odor == od_stale));
    Message::add((long)(o->theme == th_sewers));
    Message::add((long)(o->habitat == Map::any));
    }

    
  assert(toForm != crInvalid);

  polymorph(toForm);
  // Set mount level based on initial form:
  current_lev = 1;//(int)creature::List[toForm].odor;
  //if ( current_lev < o->odor ) current_lev += DiceRoll(2,(int)creature::List[toForm].odor).roll();

  // Level is always 1 or higher:
  //current_lev++;
  assert(current_lev > 0);

  maximum.hp += maximum.tuf * (current_lev-1);
    
  current = maximum;
  
  // Maintain gender preference
  gender = o->gender;
  if (gender == sex_any)
    gender = (Random::randbool())?sex_male:sex_female;
	}

Mount::Mount( Mount &copy )
: Creature( copy ) {

	}

Target Mount::clone( void ) {

	Mount *newcopy = new Mount( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

Mount::~Mount( void ) {
	}

Mount::Mount( std::iostream &inFile, TargetValue *TVP )
: Creature(inFile, TVP) {

	}

void Mount::toFile( std::iostream &outFile ) {

	Creature::toFile( outFile );

	}

String Mount::name( NameOption n_opt ) {

  String name = creature::List[form].name;
  if (gender != sex_female) {
    if (creature::List[form].male)
      name = creature::List[form].male;
    }
  else {
    if (creature::List[form].female)
      name = creature::List[form].female;
    }
    
  switch (n_opt) {
    case n_menu_noprice:
    case n_menu: {
      String ret_str = "#";
      if (current.hp <= 0)
        ret_str += "broken ";
      // Don't have mount level displayed:
      //else
      //  ret_str += String::digitorder(current_lev) + " level ";
        
      ret_str += name + "~";

#     if (FALSE) // Because Mounts aren't worth XP
      long real_xp = getVal( attrib_xp ),
           lev_diff = getVal(attrib_level)-Player::get().getVal(attrib_level)+1,
           xp = real_xp;

      while (lev_diff-- > 0)
        xp += real_xp / (lev_diff+1);
      
      ret_str += String(" (")+(long)xp+"xp)";
#     endif //
      
    	return ret_str + Item::name(n_opt);
      }
      
    case n_noun:
      if (c_name.generated()&&(id_mask&id_name))
        return c_name.toString();
      return name;
      
    default:
    	return String( String("#") + ((current.hp <= 0)?"dead ":"") + name + "~" )
           + Item::name(n_opt);
    }
  }

targetType Mount::gettype( void ) const {
	return tg_mount;
	}
bool Mount::istype( targetType t ) const {
	if (t == tg_mount)
		return true;
	else
		return Creature::istype(t);
	}

bool Mount::alive( void ) const {
	return true;
	}

void Mount::draw( void ) {
	Creature::draw();
	}


bool Mount::capable( Action a, const Target &t ) {

  switch (a) {

		case aSpMessage:
      // Mounts give no special message:
			return false;
      
    case aBeAscended:
    case aBeMounted:
      return true;

		case aBlock:
			// Mounts don't block:
			return false;

    default:
      return Creature::capable(a,t);
    
    }
  }

bool Mount::perform( Action a, const Target &t ) {

	switch (a)  {

    case aAssault:
      // We used to give a message when a mount decides that you're
      // on the menu, but now we just let the player guess.  Hell,
      // it happens often enough that it's best left as a surprise!

      //if (RawTarget::getVal(attrib_visible)) {
      //  String subject = Grammar::Subject(THIS,Grammar::det_definite);
      //  Message::add( subject + " hungrily look"+( getVal(attrib_plural)?"":"s" ) +" at "+ Grammar::Object(t,Grammar::det_definite) + "." );
      //  }
      return true;
      
    case aChat:
      return true;

    case aBlock:
      // No autoaction:
      return false;

    case aRespond: {
      String response = " nothing.";

      Skill sk_chat = creature::List[form].sk_chat;
      int dc_chat = creature::List[form].dc_chat;

      // Brain must have time to react first:
      if (!!brain) brain->perform(aRespond, t);

      if ( t->getSkill(sk_chat,false) >= dc_chat ) {
        // Special case for talking creatures

        enum Response {
        
          re_happy,
          re_angry,
          
          re_num,
          re_invalid = re_num
          
          };

        Response type = (Response) Random::randint( re_happy );
        if ( !!brain && !brain->capable(aChat,t) ) type = re_angry;

        switch (type) {
          case re_happy:
            if (VoiceStr[creature::List[form].voice])
              response = ".";
            else
              response = " nothing.";
            if (creature::List[form].quote)
              response = String(", \"")+creature::List[form].quote+"\"";
            break;

          default:
            if (capable(aEat,t))
              response = ", \"Mmmm, dinner!\"";
            else
              response = ", \"Shut up and fight me!\"";
            if (creature::List[form].quote)
              response = String(", \"")+creature::List[form].quote+"\"";
            break;
          }
        }
      else {
        // Otherwise give a message based on voice:
        if (VoiceStr[ creature::List[form].voice ]) {
          response =  ".  ";

          String subject = Grammar::Subject(t, Grammar::det_definite);
          response += subject.titlecase()
                   + " can not understand "
                   + Grammar::Object(THIS, Grammar::det_definite) + ".";

          }
        }

      String says = "says";
      if (VoiceStr[creature::List[form].voice])
        says = Grammar::plural(VoiceStr[ creature::List[form].voice ],2,true,false);

      Message::add( Grammar::Subject(THIS, Grammar::det_definite) +" "+says+ response);
          
      return true;
      }
     

		default:
			return Creature::perform(a,t);
		}

	}


void Mount::takeDamage( const Attack &a ) {


  Target retain(THIS);
  
	if (current.hp <= 0) {
		Creature::takeDamage(a);
		return;
		}

	Creature::takeDamage(a);

  // If we are domestic, tell the owner we're being picked on:
  if (!!getTarget(tg_owner))
    getTarget(tg_owner)->perform( aAlertPetDam, THIS );

	if (current.hp <= 0) {

		// If dead, change image:
		image.val.appearance = '%';
		// And no longer heal:
		ActionEvent::remove( Target(THIS), aHeal );
		// or think:
		if (!!brain)
			brain->doPerform(aSleep);
		brain = Target(NULL);

    // Special mount corpses:
    // These are handled here because the items dropped are *part of* the corpse,
    // i.e. the shabby suit is part of the scarecrow's body, not just something worn.
    //Target parent = getTarget(tg_parent);
    if (!!parent) {
      Target trophy;
      switch (form) {
      // Scarecrows always drop shabby suits:
        case crScarecrow:
             trophy = Clothing::create()->Force("a shabby suit");
             break;
             
      // Bears drop bearskin caps:
        case crGrizzlyBear:
        case crBlackBear:
             trophy = Clothing::create()->Force("a bearskin cap");
             break;
       
     
      // The following animals drop trophies:
        case crArmadillo:
        case crBuffalo:       case crCoyote:
        case crEagle:         case crGila:
        case crRabbit:        case crPorcupine:
        case crCougar:        case crRattle:
        case crScorpion:      case crSkunk:
        case crVulture:       case crWolverine:
        case crWolf:
             trophy = Remains::create(form);
             break;

        default: break;
        }

      // If we created a trophy:
      if (!!trophy) {
        // If the creature is visible:
        if (getVal(attrib_visible))
          // Know the form of trophy (e.g. *eagle* feather)
          trophy->identify(id_form);
        // Put the trophy down:
        parent->perform( aContain, trophy );
        }
      
      }

		// Maybe get rid of corpse:
		if ( !!parent
       && Random::randint(100) >= creature::List[form].corpse_pc ) {
			detach();
      ActionEvent::remove( Target(THIS), aAll );
      }
    // Or make sure the corpse will rot away:
    else {
      ActionEvent::create( Target(THIS), aRot, TIME_ROT );
      draw();
      }

    // Make sure that the killer gets the experience:
    if (!!a.deliverer)
      a.deliverer->perform(aGainExp,THIS);
		}

	}


long Mount::getVal( StatType s ) {

	switch(s) {

    case attrib_sort_category:
      return (tg_machine_part  << TYPE_SORT_SHIFT )
             + form;

		case attrib_person :
			if (current.hp<=0 || form != crHuman || gender==sex_none)
				return Grammar::p_third_n;
			return (gender==sex_male)
               ? Grammar::p_third_m
               : Grammar::p_third_f;

		default:
			return Creature::getVal(s);
		}

	}

long Mount::getSkill( Skill skill, bool get_total ) {
// Mounts get +1 to fight skills per experience level:
  if (skill == sk_figh || skill == sk_mark)
    return current_lev + Creature::getSkill(skill,get_total);

  // Mounts have piloting difficulty ratings:
  if (skill == sk_pilo) return 10;

  // ATM, otherwise just return skill as per stats:
  return Creature::getSkill(skill,get_total);
  }
  

Target Mount::getTarget( targetType type ) {
  switch( type ) {
    case tg_possessor:
      // Mounts can have possessors like normal items:
      return Item::getTarget(type);
    default:
      return Creature::getTarget(type);
    }
  }

#endif // MONSTER_CPP
