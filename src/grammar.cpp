// [grammar.cpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( GRAMMAR_CPP )
#define GRAMMAR_CPP

#include "grammar.hpp"
#include "file.hpp"
#include "rawtargt.hpp"


#include "compiler.hpp"
#ifdef OBSOLETE_INCLUDE
#  include <stdio.h>
#  include <string.h>
#else
#  include <cstdio>
#  include <cstring>
#endif

/* PRONOUNS:
 *
 *    TPN      TPM       TPF       SP         TPP
 *  S~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~S
 *  S it       he        she       you        they       S subjective
 *  S it       him       her       you        them       S objective
 *  S itself   himself   herself   yourself   themselves S reflexive
 *  S                                                    S
 *  S its      his       her       your       their      S possessive
 *  S                                                    S
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  The Reflexive pronoun is used when the subject and object are the same.
 *   -> "You kick yourself."
 *   -> "The orcs kill themselves."
 *
 *  SP = Second Person
 *  TP = Third Person
 *  M = Masculine
 *  F = Feminine
 *  N = Neuter
 *  P = Plural
 *
 */

Target *Grammar::s_current = NULL;
Target *Grammar::s_last = NULL;
Target *Grammar::o_current = NULL;
Target *Grammar::o_last = NULL;
bool Grammar::pronoun = false;

String Grammar::Subject( const Target &t, determiner d ) {

  assert( !!t ); // Error::fatal("NULL Target passed to Grammar::subject!");
        
//	if ( t->istype(tg_temp) ) return "nothing";
	if ( t->getVal(attrib_on_level) && !t->getVal(attrib_visible) )
    switch( t->getVal(attrib_person) ) {
      case p_third_m:
      case p_third_f:
        return "someone";
        
      case p_third_n:
        return "something";
        
      default:
        break;
      }

	pronoun = false;

	if ( s_last && *s_last == t ) pronoun = true;
	if ( o_last && *o_last == t ) pronoun = true;

	if ( s_last && o_last )
		if ( (*s_last)->getVal( attrib_person ) == (*o_last)->getVal( attrib_person ) )
			pronoun = false;

	if (!s_current) s_current = new Target(t);
	else       		(*s_current) = t;

	if (pronoun) {

		switch ( t->getVal( attrib_person ) ) {
			case p_second : return "you";
			case p_third_m : return "he";
			case p_third_f : return "she";
			case p_third_n : return "it";
			case p_third_pl : return "they";
			}

		}

//	return t->menustring();
	return plural(t,d);
	}

String Grammar::Object( const Target &t, determiner d ) {

  assert( !!t ); // Error::fatal("NULL Target passed to Grammar::object!");

//	if ( t->istype(tg_temp) ) return "nothing";
	if ( t->getVal(attrib_on_level) && !t->getVal(attrib_visible) )
    switch( t->getVal(attrib_person) ) {
      case p_third_m:
      case p_third_f:
        return "someone";
        
      case p_third_n:
        return "something";
        
      default:
        break;
      }

	pronoun = false;

//	if ( s_last && *s_last == t ) pronoun = true;
	if ( o_last && *o_last == t &&
       (!s_current || (*s_current)->getVal(attrib_person) != t->getVal(attrib_person) ) ) pronoun = true;

	if ( s_last && o_last )
		if ( (*s_last)->getVal( attrib_person ) == (*o_last)->getVal( attrib_person ) )
			pronoun = false;

	if (!o_current) o_current = new Target(t);
	else       		(*o_current) = t;

	bool reflexive = false;
	if ( s_current ) {
		reflexive = ( *s_current == t );
		}

	if (pronoun || reflexive) {

		switch ( t->getVal( attrib_person ) ) {
			case p_second : if (reflexive) return "yourself"; else return "you";
			case p_third_m : if (reflexive) return "himself"; else return "him";
			case p_third_f : if (reflexive) return "herself"; else return "her";
			case p_third_n : if (reflexive) return "itself"; else return "it";
			case p_third_pl : if (reflexive) return "themselves"; else return "them";
			}

		}

  if ( s_current && s_current->raw()->name() == t->name())
  	return plural(t,det_another);

	return plural(t,d);
	}

const bool Grammar::Pronoun( void ) {
	return pronoun;
	}

void Grammar::newsentence( void ) {

	if (!s_current&&o_current)
		delptr( s_current );
	if (s_current&&!o_current)
		delptr( o_current );

	if ( s_current ) {

		if ( !s_last ) s_last = new Target( *s_current );
		else *s_last = *s_current;

		delptr( s_current );

		}
//	else delptr( s_last );

	if ( o_current ) {

		if ( !o_last ) o_last = new Target( *o_current );
		else *o_last = *o_current;

		delptr( o_current );

		}
//	else delptr( o_last );

	}


void Grammar::clr( void ) {
	delptr( s_current );
	delptr( s_last );
	delptr( o_current );
	delptr( o_last );
	}


/* DETERMINERS, ARTICLES, PLURALS:
 *
 * The  : definite article
 * a[n] : indefinite article
 * This, these, that, those : demonstrative determiners
 * My, your, his/her/its : possessive determiners (also pronouns)
 * one, two, ... : numbers
 *
 * When pluralising a regular name (of the form "%scard%s")
 * you may specify the determiner you wish to use.
 *
 *   determiner          Examples
 * S~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~S
 * S det_definite        "the gun"       S
 * S                     "the bullets"   S
 * S                                     S
 * S det_indefinite      "a gun"         S
 * S  (default)          "4 bullets"     S
 * S                                     S
 * S det_demonst_with    "this gun"      S
 * S                     "these bullets" S
 * S                     "my hat"        S
 * S                                     S
 * S det_demonst_wout    "that gun"      S
 * S                     "those bullets" S
 * S                     "my hat"        S
 * S                                     S
 * S det_possessive      "your gun"      S
 * S                     "her bullets"   S
 * S                                     S
 * S det_abstract        "gun"           S
 * S                     "bullets"       S
 * S                                     S
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 */


String Grammar::plural( const Target &t, determiner d ) {

  Target owner;

  if ( t->identified(id_owner) )
    owner = t->getTarget(tg_owner);

  NameOption n_opt = n_default;

  if (d == det_definite)
    n_opt = n_short;

  if ( t->getVal(attrib_unique) )
    d = det_definite;
    
  return plural( t->name(n_opt), t->quantity(),
                 t->getVal(attrib_abstract), t->getVal(attrib_pair),
                 d, owner );

  }

String Grammar::plural( String s, int num, bool abstract, bool pair, determiner d, Target owner ) {

/* REGULAR & IRREGULAR PLURALS:
 *
 *  Regular plurals are often of the form [word]s
 *
 *  Some plural forms add -es to the end,
 *   -> Churches, dishes, christmases, Kisses
 *       (operative - ch, sh, ss, [vowel]s, x )
 *   (tentatively complete)
 *
 *  Nouns ending in a consonant then -y have -ies as their
 *  regular plural ending.
 *   -> Berries, daisies.
 *   (tentatively complete)
 *
 *  Irregular plurals are those that do not follow general
 *  rules.
 *   -> Man, men; tooth, teeth; ox, oxen;
 *      loaf, loaves (but roof, roofs!)
 *   (incomplete)
 *
 *  There also exist forms gained from other languages.
 *   -> Stimulus, stimuli; larva, larvae; criterion, criteria.
 *   (incomplete)
 *
 */

	String strNum, strS;
	if (num > 1) {
		switch (d) {
			case det_definite : strNum += "the "; break;
			case det_demonst_with : if (!owner) { strNum += "these "; break; }
			case det_demonst_wout : if (!owner) { strNum += "those "; break; }
        // fall through to:
			case det_possessive :
        /* Todo: his,her,its,their */
        // fall through to:
			case det_indefinite :
				if (abstract) break;
				strNum += (long) num;
				strNum += " ";
				if (d != det_possessive) break;
        if (!!owner) {
          String owner_name = owner->name(n_owner);
          if (d == det_demonst_with || d == det_demonst_wout)
            if (s_current && *s_current == owner)
              owner_name = "my";
          strNum += "of "+owner_name+" ";
          }
        break;
        
			case det_abstract :	break;
      case det_another : strNum = "some other "; break;
			}
		if (pair) strNum += "pairs of ";

    long tilde = (long) (strrchr(s.toChar(), '~')-s.toChar());
    if (tilde > 1) {
      String last2;
      last2 += s[tilde-2];
      last2 += s[tilde-1];

      if (last2 == "ch" || last2 == "sh" || last2 == "ss" || last2[1] == 'x'
          || (last2[1] == 's' && std::strchr("aeiouAEIOU", last2[0]) ) )
        strS = last2+"es";
      else if ( last2 == "ff" || last2 == "fe" )
        strS = "ves";
      else if (last2[1] == 'y' && !std::strchr("aeiouAEIOU", last2[0]) ) {
        strS += last2[0];
        strS += "ies";
        }
      else
    		strS = last2+"s";

      int i;
      String exchange;
      for (i = 0; i < tilde-2; ++i)
        exchange += s[i];

      exchange += "%s";
      exchange += s.toChar() + tilde+1;
      s = exchange;
      }
    // Tilde will not always be found:
    else {
      assert (!!pair);
      } // Error::fatal("No tilde ('~') found in: \""+s+"\"");
		}
	else {

		// Determine if the word should be preceeded by 'a' or 'an':
		const char *cstr = s.toChar();
		const char *Itemnum = "#";
		const char *alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-";
		int locItemnum, distFirstWord;
    char c = 'b';

		locItemnum = std::strcspn(cstr, Itemnum);

    if (locItemnum < s.length()) {
      distFirstWord = std::strcspn(cstr+locItemnum+1, alphabet);
   		c = *(cstr+locItemnum+1+distFirstWord);
      }

		switch (d) {
			case det_definite : strNum += "the "; break;
			case det_demonst_with : if (!owner) {
                                if (pair)
                                  strNum += "these ";
                                else
                                  strNum += "this ";
                                break;
                                }
			case det_demonst_wout : if (!owner) {
                                if (pair)
                                  strNum += "those ";
                                else
                                  strNum += "that ";
                                break;
                                }
        // fall through to:
			case det_possessive :
      /* Todo: his,her,its,their */
        if (!!owner) {
          String owner_name = owner->name(n_owner);
          if (d == det_demonst_with || d == det_demonst_wout)
            if (s_current && *s_current == owner)
              owner_name = "my";
          strNum = owner_name+" ";
          break;
          }
        // fall through to:
			case det_indefinite :
      
				if ( abstract ) break; // Abstract - "water" not "a water"

				if ( std::strchr("aeiouAEIOU", c) && ! pair )
          strNum = "an ";
        else
          strNum = "a ";

				if ( pair ) {
          strNum += "pair of "; break;
          }
          
        break;
			case det_abstract :	break;

      case det_another :

        strNum = "another ";

				if ( pair ) {
          strNum += "pair of "; break;
          }
        break;
			}

		strS = "";

    long find = (long) strrchr(s.toChar(), '~'), tilde=0;
    if (find != 0) tilde = find-(long)s.toChar();
    
    assert(tilde < 1000); // Error::fatal(String("Silly string of length ")+tilde+" found in Grammar::plural()!");
    
    if (tilde > 1) {
      int i;
      String exchange;
      for (i = 0; i < tilde; ++i)
        exchange += s[i];

      exchange += "%s";
      exchange += s.toChar() + tilde+1;
      s = exchange;
      }

		}

  // Swap Tilde '~' with "s" or "":
  char buffer[80];
	std::sprintf(buffer, s.toChar(), strS.toChar());
  s = buffer;


  // Swap Pound '#' with quantity string:
  int i;
  long pound = (long) (strrchr(s.toChar(), '#')-s.toChar());

  if (pound >= 0) {
    String for_insert;
    for (i = 0; i < pound; ++i)
      for_insert += s[i];

    for_insert += "%s";
    for_insert += s.toChar() + pound+1;
    s = for_insert;

  	std::sprintf(buffer, s.toChar(), strNum.toChar() );
    }


	return String(buffer);
	}


#endif // GRAMMAR_CPP
