
// [book.cpp]
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
#if !defined ( BOOK_CPP )
#define BOOK_CPP

#include "book.hpp"
#include "grammar.hpp"
#include "random.hpp"
#include "message.hpp"
#include "timeline.hpp"

bookStats BookStorage [] = {

// A note on book entries:
// Books temporarily increase skills by a value determined by their
// level, for a short, variable length of time.
// Books may increase any skill *except* research, the only benefit
// of which is to add to the time and bonus given by reading books.

  { NULL, "tumbling", NULL, sk_acro, 0, 1, +2, TURN_PER_MINUTE*20 },
  
  { NULL, "animal language",  NULL, sk_anim, 0, 1, +2, TURN_PER_MINUTE*20 },
  
//  { NULL, "16th century figurines", NULL, sk_appr, 0, 1, +2, TURN_PER_MINUTE*20 },

  { NULL, "breathing techniques", NULL, sk_athl, 0, 1, +2, TURN_PER_MINUTE*20 },
  
  { NULL, "crime investigation", NULL, sk_awar, 0, 1, +2, TURN_PER_MINUTE*20 },
  
//  { NULL, "chemical solutions", NULL, sk_chem, 0, 1, +2, TURN_PER_MINUTE*20 },
  
  { NULL, "making friends",     NULL, sk_dipl, 0, 1, +2, TURN_PER_MINUTE*20 },
  { NULL, "influencing people", NULL, sk_dipl, 2, 3, +2, TURN_PER_MINUTE*20 },
  
  { NULL, "feinting",  NULL, sk_figh, 0, 1, +2, TURN_PER_MINUTE*20 },
  
  { NULL, "first aid training", NULL, sk_heal, 0, 1, +2, TURN_PER_MINUTE*20 },
  
  { NULL, "archery", NULL, sk_mark, 0, 1, +2, TURN_PER_MINUTE*20 },
  
  { "The Clockwork Compendium", NULL, NULL, sk_mech, 0, 1, +2, TURN_PER_MINUTE*20 },
  { "The Steamtech Compendium", NULL, NULL, sk_mech, 2, 3, +2, TURN_PER_MINUTE*20 },
  
//  { "The Raging Tide",  NULL,   "Edward Gorey", sk_occu, 0, 1, +2, TURN_PER_MINUTE*20 },
//  { "The King in Yellow",  NULL, NULL,           sk_occu, 2, 3, +2, TURN_PER_MINUTE*20 },
  
//  { "Straights and Flushes", NULL, NULL, sk_poke, 0, 1, +2, TURN_PER_MINUTE*20 },
//  { NULL, "seven card stud",   NULL, sk_poke, 2, 3, +2, TURN_PER_MINUTE*20 },
  
  { NULL, "prison escapes", NULL, sk_stea, 0, 1, +2, TURN_PER_MINUTE*20 },
  
//  { NULL, "poisonous plants", NULL, sk_surv, 0, 1, +2, TURN_PER_MINUTE*20 },


  };

bookStats *bookStats::BookStats = BookStorage;
int bookStats::numBook = 12;


targetType Book::gettype( void ) const {
	return tg_book;
	}

bool Book::istype( targetType t ) const {

	if ( t == tg_book )
		return true;
	else return Item::istype(t);

	}

bool Book::capable( Action a, const Target &t ) {

	switch( a ) {

    case aBeRead:
      return true;
      
		default:
			return Item::capable(a, t);
		}
	}


bool Book::perform( Action a, const Target &t ) {

	switch( a ) {

    case aPolymorph:
     	form = Random::randint( bookStats::numBook );
      return Item::perform(a,t);

    case aBeRead:
      // Temporarily Increase the player's skill level:
      if (!!t && t->istype(tg_player) ) {
        Skill skill = bookStats::BookStats[form].skill;
        String study;
        int curr_lev = t->getSkill(skill,false), // Player level in this skill
            book_lev_min = bookStats::BookStats[form].min_level,
            book_lev_max = bookStats::BookStats[form].max_level,
            bonus = bookStats::BookStats[form].bonus,
            time = bookStats::BookStats[form].time;
        // If the player is of too low a level:
        if ( curr_lev < book_lev_min )
          study = "look at the pictures";
        // If the player is of too high a level:
        // Currently disabled - may not be wanted.
        /*
        else if ( curr_lev == book_lev_max * 3 ) {
          study = "discover an error";
          // Give the player a reward for fooling around:
          t->incSkill(skill);
          // This is a *permanent* skill bonus, but can only
          // be acquired once per book.
          }
        */
        else if ( curr_lev > book_lev_max * 2 )
          study = "reminisce";
        else if ( curr_lev > book_lev_max )
          study = "know the text well enough";
        // If the player level is just right:
        else {

          // Todo: test player's luck for extended bonus time
          // Test the player's skill for extended bonus time & extra bonus:
          Result research = t->test(sk_rese,book_lev_max,THIS);
          switch (research) {
            case result_pass:
              // Pass means +(skill/4)*time, +1 bonus
              study = "research";
              time += time * t->getSkill(sk_rese,false) / 4;
              bonus+= 1;
              break;
            
            case result_perfect:
              // Perfect means +(skill/2)*time, +2 bonus
              study = "cram";
              time += time * t->getSkill(sk_rese,false) / 2;
              bonus+= 2;
              break;
            
            case result_fumble:
              // Fumble means -25% time, -1 bonus
              study = "delve into";
              time -= time / 4;
              if (bonus > 1)
                bonus -= 1;
              break;
            
            default:
              // Fail - no additional bonuses
              study = "study";
              break;
            }
            
          study += String(" ") + Skills[skill].name;
          
          // Give the player a temporary skill bonus:
          t->modSkill(skill,bonus);
          // Set an event to remove the bonus in (time) turns:
          SkillLevelEvent::create(skill,-bonus,time);
          }
   			Message::add(Grammar::Subject(t,Grammar::det_definite)+" "+study+".");
        }
      return true;
      
		default:
			return Item::perform(a, t);
		}

	}

long Book::getVal( StatType s ) {

	switch( s ) {

    case attrib_price:
      // Price = (range+bonus) * max_level^2 * time
      return Cents(
              (bookStats::BookStats[form].max_level //   ( +max )
              -bookStats::BookStats[form].min_level //   ( -min )
              +bookStats::BookStats[form].bonus )   //   ( +bon )
             * bookStats::BookStats[form].max_level // *  max
             * bookStats::BookStats[form].max_level // *  max
             * bookStats::BookStats[form].time
             / TURN_PER_MINUTE                      // * time
                  );


    case attrib_sort_category:
      return Item::getVal(s) + form;
    case attrib_weight:
    case attrib_sink_weight:
      return 5;
		case attrib_form:
			return form;

		default:
			return Item::getVal(s);
		}

	}

Book::Book( const Option *o )
: Item( o ) {


  // No skill choice made:
	if ( o->skill == sk_none )
    doPerform(aPolymorph);
    
  // Create based on skill:
	else for ( int i = 0; i < NUM_ABORT; ++i ) {
    doPerform(aPolymorph);
		if ( bookStats::BookStats[form].skill == o->skill) break;
		}

	}

Book::Book( Book &rhs )
: Item(rhs) {

	form = rhs.form;
	}

Book::Book( std::iostream &inFile, TargetValue *TVP )
// Read Item base information:
: Item (inFile, TVP) {

	// Read char form
	inFile.read( (char*) &form, sizeof(form) );

	}

void Book::toFile( std::iostream &outFile ) {

	// Write Item base information:
	Item::toFile( outFile );

	// Write char form
	outFile.write( (char*) &form, sizeof(form) );

	}

Target Book::clone( void ) {

	Book *newcopy = new Book( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

bool Book::equals( const Target &rhs ) {

  // Abook booksources must not stack!
//  return false;


  bool retVal = Item::equals( rhs );

	if (retVal) {
		// Good so far - same inventories and both of same type.
		if ( form != ( (Book*) rhs.raw() )->form ) retVal = false;
		}

	return retVal;

	}


String Book::name( NameOption n_opt ) {
  String retstr = "#book~";
  
  switch (n_opt) {
    case n_menu_noprice:
    case n_menu: {
      String desc;
      if (bookStats::BookStats[form].title)
        desc = String()+ " titled \"" + bookStats::BookStats[form].title + "\"";
      else if (bookStats::BookStats[form].subject)
        desc = String()+" on " + bookStats::BookStats[form].subject;
      else if (bookStats::BookStats[form].author)
        desc = String()+" by " + bookStats::BookStats[form].author;
     	return String( retstr + desc ) + Item::name(n_opt);
    	}
      
    default:
      return retstr + Item::name(n_opt);
    }
    
	}




#endif // BOOK_CPP
