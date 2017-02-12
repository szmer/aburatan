// [crlist.cpp]
// (C) Copyright 2000 Michael Blackney
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
#if !defined ( CRLIST_CPP )
#define CRLIST_CPP
#include "crlist.hpp"
#include "enum.hpp"

#include "visuals.hpp"
#include <values.h>

//SAMPLE CREATURE LIST.  For debugging & development.
#if defined ( SAMPLECREATURES )
/*
	v_tiny,       //     x1 a coin

	v_short,      //    x25 a mouse, a short sword
  v_long,       //    x50 a rat, a long sword

	v_medium,     //   x250 a child, a dog

	v_tall,       //   x500 an adult

	v_large,      //  x2500 a cow, a bear
	v_huge,       //  x5000 an elephant

	v_gigantic,   // x25000 a dinosaur
	v_cyclopean   // x50000 cthulhu

  od_stale
  od_rotten
  od_foul
  od_fetid
  od_noisome
  od_mephitic

*/

// WORKING:
// abcdefghijklmnopqrstuvwxyz
// uiro erunurii  euopon o^ o
// trem limslizm  radiwf rm m
// odae izaelsai  sdedno mo b
// m ts nzncyarc  ornesr  u i
// a ut elot ld   nutrmm  n e
// t ri  yio i     p  ae  t
// o ec   di d     e  nd
// n       d       d

// THIS NEEDS REVISION
// scarecrows, effigies, stuffy-guys, and all dead toys should be one char
// 'R' Robot - mechanical men should be another char
// near-humans (true humanoids - fishmen and taheens) should be 'h'

// ABCDEFGHIJKLMNOPQRSTUVWXYZ
// n a  ehun r o  euonrnai ^
// g n  loms i r  rybaefmn c
// e i  isae s l  slokeopg a
// l n  ntnc a o  ottesmie c
// i e  e ot l c  nh   erd t
// c      is i k   u   de  u
//        d  d     l       s
//                 g

// --- Stat cluster defines:
// Races:                   c p s t w k c s hp
#define STATS_HUMAN         5,5,5,5,5,5,5,5,18 // Humans are our baseline (as standard)
#define STATS_MORLOCK       7,4,6,6,5,3,5,5,30 // Morlocks of all types
#define STATS_LIVESTOCK     4,5,6,6,2,2,2,5,30 // Horses, cows, buffalo
#define STATS_PET           5,5,4,4,4,5,5,6,12 // Dogs and cats
#define STATS_BIRD          4,3,3,3,3,2,2,6, 8
#define STATS_AUTOMATON     4,4,7,5,2,5,5,4,34

// Attributes:              c     p     s     t     w     k     c     s    hp
#define STATS_WEAK         +0,   +0,*8/10,*8/10,   +0,   +0,   +0,   +0,*8/10
#define STATS_STRONG       +0,   +0,*10/8,*10/8,   +0,   +0,   +0,   +0,*10/8
#define STATS_PASSIONATE   +0,   +0,*12/8,*10/8,*10/8,   +0,   +0,*10/8,   +0
#define STATS_SMALL        +0,   +0,*8/10,*8/10,   +0,   +0,   +0,   +0,   +0
#define STATS_TASTY        +0,   +0,   +0,*12/8,*8/10,   +0,   +0,*8/10,*12/8
#define STATS_FAST      *10/8,   +0,   +0,   +0,   +0,   +0,   +0,*10/8,   +0
#define STATS_LIGHT     *10/8,   +0,*8/10,*8/10,   +0,   +0,   +0,*10/8,*8/10
#define STATS_HUGE         +0,   +0,   *2,   *2,   +0,   +0,   +0,   +0,   *2

// --- For fine-tuning:
// Single stats:             c  p  s  t  w  k  c  s  hp
#define STAT_COORD(x)        x,+0,+0,+0,+0,+0,+0,+0,+0
#define STAT_PERCEPTION(x)  +0, x,+0,+0,+0,+0,+0,+0,+0
#define STAT_STRENGTH(x)    +0,+0, x,+0,+0,+0,+0,+0,+0
#define STAT_TOUGHNESS(x)   +0,+0,+0, x,+0,+0,+0,+0,+0
#define STAT_WILLPOWER(x)   +0,+0,+0,+0, x,+0,+0,+0,+0
#define STAT_KNOWLEDGE(x)   +0,+0,+0,+0,+0, x,+0,+0,+0
#define STAT_CONCENTRATE(x) +0,+0,+0,+0,+0,+0, x,+0,+0
#define STAT_SPEED(x)       +0,+0,+0,+0,+0,+0,+0, x,+0
#define STAT_HP(x)          +0,+0,+0,+0,+0,+0,+0,+0, x


// --- The mechanism for joining stat clusters together:
// First pass:
#define JOIN_STATS(a,b) JOIN_STATS18(a,b)
// Second pass:
#define JOIN_STATS18(a,b,c,d,e,f,g,h,i,a2,b2,c2,d2,e2,f2,g2,h2,i2)\
                   (a a2),(b b2),(c c2),(d d2),(e e2),(f f2),(g g2),(h h2),(i i2)

const CombatValues

 // Natural creatures are immune to rust and purity damage:
 cv_natural(
  // Base AC:
  0,
  // pi ba cu ch cr sp va
      0, 0, 0, 0, 0, 0, 0,
  // phy pur ele bur poi rus dro cho
       0,100,  0,  0,  0,100,  0,  0
  ),

 // Ghosts are easy to hit, but only take damage from purity:
 cv_ghost(
  // Base AC:
  0,
  // pi ba cu ch cr sp va
      0, 0, 0, 0, 0, 0, 0,
  // phy pur ele bur poi rus dro cho
     100,  0,100,100,100,100,100,100
  ),

 // Mechanical men are quite resistant to sharp attacks, and
 // are only really damaged by bash attacks:
 cv_mech_man(
  // Base AC:
  0,
  // pi ba cu ch cr sp va
      8, 0, 6, 3, 8, 0, 0,
  // phy pur ele bur poi rus dro cho
       0,100,100, 60,100,  0,100,100
  ),

 // Light Mounts are lightly resistant to all
 // penetration types:
 cv_light_mount(
  // Base AC:
  0,
  // pi ba cu ch cr sp va
      8, 5, 6, 4, 8, 0, 0,
  // phy pur ele bur poi rus dro cho
       0,100, 90, 80,100,  0,100,100
  ),

 // Aquatic creatures can't be drowned:
 cv_aquatic(
  // Base AC:
  0,
  // pi ba cu ch cr sp va
      0, 0, 0, 0, 0, 0, 0,
  // phy pur ele bur poi rus dro cho
       0,100,  0,  0,  0,100,100,  0
  ),

 // Slimy creatures (slugs and leeches) must be chopped or burned:
 cv_slimy(
  // Base AC:
  0,
  // pi ba cu ch cr sp va
      7, 9, 2, 0, 4, 0, 0,
  // phy pur ele bur poi rus dro cho
       0,100,100, 40,100,100,100,100
  ),

 // Demons take low damage from everything but purity:
 cv_demon(
  // Base AC:
  0,
  // pi ba cu ch cr sp va
      0, 0, 0, 0, 0, 0, 0,
  // phy pur ele bur poi rus dro cho
      60,  0, 50, 50, 70,100,100,100
  ),

 // Hides protect creatures from cutting, chopping and bashing
 cv_hide(
  // Base AC:
  0,
  // pi ba cu ch cr sp va
      0, 3, 4, 3, 0, 0, 0,
  // phy pur ele bur poi rus dro cho
       0,100,  0,  0,  0,100,  0,  0
  ),

 // Bristles protect creatures from bashing
 cv_bristles(
  // Base AC:
  0,
  // pi ba cu ch cr sp va
      0, 5, 0, 0, 0, 0, 0,
  // phy pur ele bur poi rus dro cho
       0,100,  0,  0,  0,100,  0,  0
  ),

 // Snakes must be cut or chopped (or whacked if necessary):
 cv_snake(
  // Base AC:
  0,
  // pi ba cu ch cr sp va
      6, 3, 0, 0, 8, 0, 0,
  // phy pur ele bur poi rus dro cho
       0,100,  0,  0,  0,100,  0,  0
  ),

 // Scales protect from cutting and blunt damage:
 cv_scales(
  // Base AC:
  0,
  // pi ba cu ch cr sp va
      0, 4, 4, 0, 0, 0, 0,
  // phy pur ele bur poi rus dro cho
       0,100,  0,  0,  0,100,  0,  0
  ),

 // Exo armour is like tank armour, but more brittle:
 cv_exo(
  // Base AC:
  0,
  // pi ba cu ch cr sp va
      4, 2, 4, 4, 2, 0, 0,
  // phy pur ele bur poi rus dro cho
       0,100,  0,  0,  0,100,  0,  0
  ),

 // Tank-type armour is great on all levels
 cv_tank(
  // Base AC:
  0,
  // pi ba cu ch cr sp va
      4, 6, 6, 4, 4, 0, 0,
  // phy pur ele bur poi rus dro cho
       0,100,  0,  0,  0,100,  0,  0
  ),

 // Ragdolls take little damage from pierce and bash attacks
 // (Being demons, they take purity damage)
 cv_ragdoll(
  // Base AC:
  0,
  // pi ba cu ch cr sp va
      9, 9, 0, 0, 8, 0, 0,
  // phy pur ele bur poi rus dro cho
       0,  0,  0,  0,100,100,100,100
  );
 


creature creaturelist [] ={
//--[ for class Human ]--//
// Human
	creature( "human",   "man", "woman", "child",    od_none, vo_omni, sk_none, 0, ap_omni, dr_water,  STATS_HUMAN, &cv_natural, ia_hit|ia_kick|ia_choke,2,dam_physical,50, v_tall,   ('h' | (cBrown  << BITSPERBYTE) ), 30, 30, m_flesh, m_blood, Map::any, th_none, NULL ),

//--[ for class Mount ]--//
	creature( "bicycle", NULL, NULL, NULL,           od_none, vo_omni, sk_none, 0, ap_omni, dr_water,  5,5,7,5,5,2,5,6,18, &cv_light_mount, ia_hit,2,dam_physical,50, v_medium,  ('x' | (cGrey  << BITSPERBYTE) ), 30, 30, m_steel, m_oil, Map::any, th_none, NULL ),
	creature( "mechanical bull", NULL, NULL, NULL,   od_none, vo_omni, sk_none, 0, ap_omni, dr_water,  5,5,7,5,5,2,5,6,18, &cv_light_mount, ia_hit|ia_gore|ia_kick,8,dam_physical,100, v_large,  ('x' | (cLGrey  << BITSPERBYTE) ), 30, 30, m_steel, m_oil, Map::any, th_none, NULL ),


//--[ for class Monster ]--//
	// Animals
	// Domestic
	creature( "cat",     "tomcat", NULL, "kitten",   od_none, vo_mew, sk_anim, 4,  ap_carn, dr_water,  JOIN_STATS(STATS_PET,STATS_WEAK), &cv_natural, ia_bite|ia_claw,2,dam_physical,50, v_long,   ('f' | (cYellow << BITSPERBYTE) ),100, 30, m_flesh, m_blood, Map::any, th_none, NULL ),
	creature( "dog",     NULL, NULL, "puppy",        od_none, vo_bark, sk_anim, 4, ap_carn, dr_water,  STATS_PET,                        &cv_natural, ia_bite,3,dam_physical,50, v_medium, ('d' | (cLGreen << BITSPERBYTE) ),100, 30, m_flesh, m_blood, Map::any, th_none, NULL ),
                           // ^- NOT 'bitch' for obvious reasons...
	// Farm
	creature( "cow",     "bull", NULL, "calf",       od_none, vo_moo,   sk_anim, 2, ap_herb, dr_water,  JOIN_STATS(STATS_LIVESTOCK,STATS_TASTY),      &cv_natural, ia_bite,2,dam_physical,50, v_large,  ('q' | (cWhite  << BITSPERBYTE) ),100, 30, m_flesh, m_blood, Map::plains, th_none, NULL ),
	creature( "horse",   "stallion", "mare", "foal", od_none, vo_neigh, sk_anim, 3, ap_herb, dr_water,  JOIN_STATS(STATS_LIVESTOCK,STATS_PASSIONATE), &cv_natural, ia_kick,5,dam_physical,50, v_large,  ('q' | (cBrown  << BITSPERBYTE) ), 40, 30, m_flesh, m_blood, Map::plains, th_none, NULL ),
	creature( "pig",     "boar", "sow", "piglet",    od_none, vo_oink,  sk_anim, 2, ap_herb, dr_water,  JOIN_STATS(STATS_LIVESTOCK,STATS_SMALL),      &cv_natural, ia_bite,2,dam_physical,50, v_medium, ('q' | (cLRed   << BITSPERBYTE) ),100, 30, m_flesh, m_blood, Map::plains, th_none, NULL ),
	creature( "chicken", "rooster", "hen", "chick",  od_none, vo_cluck, sk_anim, 1, ap_bird, dr_water,  JOIN_STATS(STATS_BIRD,STATS_TASTY),           &cv_natural, ia_peck,1,dam_physical,50, v_long,   ('b' | (cYellow << BITSPERBYTE) ),100, 30, m_flesh, m_blood, Map::plains, th_none, NULL ),
	creature( "sheep",   "ram", "ewe", "lamb",       od_none, vo_baa,   sk_anim, 2, ap_herb, dr_water,  JOIN_STATS(STATS_LIVESTOCK,STATS_WEAK),       &cv_natural, ia_butt,2,dam_physical,50, v_medium, ('q' | (cBlue   << BITSPERBYTE) ),100, 30, m_flesh, m_blood, Map::plains, th_none, NULL ),

	// Game
	creature( "deer",    "stag", "doe", "fawn",      od_none, vo_none, sk_anim, 99,ap_herb, dr_water,  JOIN_STATS(STATS_LIVESTOCK,STATS_LIGHT), &cv_natural, ia_butt|ia_kick,3,dam_physical,50, v_tall,   ('q' | (cGreen  << BITSPERBYTE) ), 60, 30, m_flesh, m_blood, Map::forest, th_wild, NULL ),
	creature( "duck",    NULL, NULL, "duckling",     od_none, vo_quack, sk_anim, 1,ap_bird, dr_water,  STATS_BIRD, &cv_natural, ia_peck,2,dam_physical,50, v_long,   ('b' | (cGrey  << BITSPERBYTE) ), 60, 30, m_flesh, m_blood, Map::river, th_wild, NULL ),
	creature( "goat",    "billy goat", NULL, "kid",  od_none, vo_bleet, sk_anim, 2,ap_herb, dr_water,  JOIN_STATS(STATS_LIVESTOCK,STATS_SMALL), &cv_natural, ia_butt,2,dam_physical,50, v_tall,   ('q' | (cGrey  << BITSPERBYTE) ), 60, 30, m_flesh, m_blood, Map::mountains, th_wild, NULL ),
	creature( "buffalo", NULL, NULL, NULL,           od_none, vo_moo, sk_anim, 2,  ap_herb, dr_water,  JOIN_STATS(STATS_LIVESTOCK,STATS_STRONG), &cv_hide, ia_gore,4,dam_physical,60, v_large,  ('q' | (cYellow << BITSPERBYTE) ),100, 30, m_flesh, m_blood, Map::plains, th_wild, NULL ),

	// Predators
//                                                                                                        c p s t w k c s hp
	creature( "grizzly bear",NULL,NULL, "grizzly cub",    od_none, vo_growl, sk_anim, 5, ap_carn, dr_water, 5,5,7,5,5,2,5,6,18, &cv_hide, ia_claw|ia_hit,7,dam_physical,50, v_large,  ('g' | (cBrown  << BITSPERBYTE) ), 40, 30, m_flesh, m_blood,Map::forest, th_wild, NULL ),
	creature( "black bear", NULL, NULL, "black bear cub", od_none, vo_growl, sk_anim, 5, ap_carn, dr_water, 5,5,7,5,5,2,5,6,22, &cv_hide, ia_claw|ia_hit,8,dam_physical,50, v_large,  ('g' | (cDGrey << BITSPERBYTE) ), 40, 30, m_flesh, m_blood,Map::hills, th_wild, NULL ),
	creature( "meggie bear", "gruffie", "bear queen", "meggie cub",
                                                        od_none, vo_growl, sk_anim, 1, ap_herb, dr_water, 6,8,7,5,5,5,6,5,30, &cv_hide, ia_claw|ia_bite|ia_kick,8,dam_physical,50, v_medium,  ('g' | (cIndigo << BITSPERBYTE) ), 0, 30, m_flesh, m_blood,Map::forest, th_wild, "Pleased to meet you!" ),
	creature( "cougar",     NULL, NULL, "cougar cub",     od_none, vo_roar, sk_anim, 4,  ap_carn, dr_water, 5,5,6,5,5,2,5,8,12, &cv_natural, ia_bite|ia_claw,3,dam_physical,50, v_tall,   ('F' | (cYellow << BITSPERBYTE) ), 30, 40, m_flesh, m_blood,Map::hills, th_wild, NULL ),
	creature( "python",     NULL, NULL, NULL,             od_none, vo_hiss, sk_anim, 3,  ap_carn, dr_water, 5,5,5,2,5,2,5,4, 5, &cv_snake, ia_bite|ia_crush,3,dam_physical,50, v_medium, ('S' | (cGreen  << BITSPERBYTE) ), 25, 50, m_flesh, m_blood,Map::plains, th_wild, NULL ),
	creature( "rattlesnake",NULL, NULL, NULL,             od_none, vo_hiss, sk_anim, 3,  ap_carn, dr_water, 5,5,5,2,5,2,5,4, 5, &cv_snake, ia_bite,3,dam_poison,90, v_medium, ('S' | (cRed    << BITSPERBYTE) ), 25, 50, m_flesh, m_blood,Map::desert, th_wild, NULL ),
	creature( "wolf",       NULL, NULL, "wolf pup",       od_none, vo_howl, sk_anim, 4,  ap_carn, dr_water, 5,5,5,4,4,2,5,6,10, &cv_natural, ia_bite,3,dam_physical,50, v_medium, ('C' | (cGrey  << BITSPERBYTE) ),100, 30, m_flesh, m_blood, Map::plains, th_wild, NULL ),
	creature( "coyote",     NULL, NULL, "coyote pup",     od_none, vo_howl, sk_anim, 4,  ap_carn, dr_water, 5,5,5,4,4,2,5,6, 8, &cv_natural, ia_bite,3,dam_physical,50, v_medium, ('C' | (cYellow << BITSPERBYTE) ),100, 30, m_flesh, m_blood, Map::desert, th_wild, NULL ),

	creature( "eagle",   NULL, NULL, NULL,                od_none, vo_squawk,sk_anim, 1, ap_carn, dr_water, 6,8,3,3,4,2,2,5, 6, &cv_natural, ia_peck,1,dam_physical,50, v_long,   ('b' | (cLGrey  << BITSPERBYTE) ),80, 30, m_flesh, m_blood, Map::forest, th_wild, NULL ),
	creature( "vulture", NULL, NULL, NULL,                od_none, vo_squawk,sk_anim, 1, ap_allmeat, dr_water, 3,8,2,4,2,2,2,5, 8, &cv_natural, ia_peck,2,dam_physical,100, v_long,   ('b' | (cIndigo << BITSPERBYTE) ),80, 30, m_flesh, m_blood, Map::desert, th_wild, NULL ),
	creature( "crow",    NULL, NULL, NULL,                od_none, vo_squawk,sk_anim, 1, ap_allmeat, dr_water, 4,8,2,4,2,2,2,5, 8, &cv_natural, ia_peck,3,dam_physical,100, v_long,   ('b' | (cDGrey  << BITSPERBYTE) ),80, 30, m_flesh, m_blood, Map::any, th_wild, NULL ),
	creature( "wolverine",  NULL, NULL, NULL,             od_none, vo_howl, sk_anim, 4,  ap_carn, dr_water, 5,5,5,6,4,2,5,6,14, &cv_natural, ia_bite|ia_claw,3,dam_physical,50, v_medium, ('C' | (cBrown << BITSPERBYTE) ),100, 30, m_flesh, m_blood, Map::forest, th_wild, NULL ),

	// Vermin
	creature( "rat",     NULL, NULL, NULL,   od_stale, vo_squeek, sk_anim, 1,ap_herb, dr_water, 5,5,2,2,5,5,5,5, 4, &cv_natural, ia_bite,2,dam_poison,30, v_long,   ('r' | (cBrown  << BITSPERBYTE) ), 20, 30, m_flesh, m_blood,Map::any, th_wild | th_sewers, NULL ),
	creature( "pigeon",  NULL, NULL, NULL,    od_none, vo_coo, sk_anim, 1,   ap_bird, dr_water, 5,5,2,2,5,5,5,5, 5, &cv_natural, ia_peck,2,dam_poison,30, v_long,   ('b' | (cGrey  << BITSPERBYTE) ), 10, 30, m_flesh, m_blood,Map::any, th_wild, NULL ),
	creature( "seagull", NULL, NULL, NULL,    od_none, vo_squawk, sk_anim, 1,ap_bird, dr_water, 5,5,2,2,5,5,5,5, 5, &cv_natural, ia_peck,2,dam_poison,30, v_long,   ('b' | (cWhite  << BITSPERBYTE) ), 10, 30, m_flesh, m_blood,Map::river, th_wild, NULL ),

	// Lizards
	creature( "iguana",   NULL, NULL, NULL,   od_none, vo_none, sk_anim, 99, ap_carn, dr_water, 5,5,5,5,5,5,5,5, 7, &cv_hide, ia_bite,2,dam_physical,50, v_long,   ('l' | (cGreen  << BITSPERBYTE) ), 30, 30, m_flesh, m_blood,Map::desert, th_wild, NULL ),
  creature( "crocodile",NULL, NULL, NULL,   od_none, vo_none, sk_anim, 99, ap_carn, dr_water, 5,5,6,5,5,5,5,5,14, &cv_scales, ia_bite,6,dam_physical,50, v_tall,   ('l' | (cLGreen << BITSPERBYTE) ), 50, 30, m_flesh, m_blood,Map::swamp, th_wild | th_sewers, NULL ),
  creature( "gila monster", NULL,NULL,NULL, od_none, vo_none, sk_anim, 99, ap_carn, dr_water, 5,5,6,5,5,5,5,5,14, &cv_hide, ia_bite,4,dam_poison,  50, v_tall,   ('l' | (cYellow << BITSPERBYTE) ), 50, 30, m_flesh, m_blood,Map::desert,th_wild, NULL ),

	// Arachnids
	creature( "tarantula",NULL, NULL, NULL,   od_none, vo_none, sk_anim, 99, ap_carn, dr_water, 5,5,4,2,5,5,5,5, 2, &cv_exo, ia_bite,2,dam_poison,50, v_short,  ('s' | (cGrey  << BITSPERBYTE) ), 10,  0, m_flesh, m_blood,Map::any, th_wild|th_sewers, NULL ),
	creature( "scorpion", NULL, NULL, NULL,   od_none, vo_none, sk_anim, 99, ap_carn, dr_water, 5,5,4,2,5,5,5,5, 3, &cv_exo, ia_sting,6,dam_poison,50, v_short,  ('s' | (cYellow << BITSPERBYTE) ), 10,  0, m_flesh, m_blood,Map::desert, th_wild, NULL ),

  // Misc.
	creature( "porcupine",  NULL, NULL, NULL, od_none, vo_none, sk_anim, 4,  ap_carn, dr_water, 5,5,5,4,4,2,5,6, 8, &cv_bristles, ia_sting,5,dam_physical,50, v_medium, ('c' | (cYellow << BITSPERBYTE) ), 40, 30, m_flesh, m_blood, Map::desert, th_wild, NULL ),
	creature( "armadillo",  NULL, NULL, NULL, od_none, vo_none, sk_anim, 4,  ap_carn, dr_water, 5,5,5,4,4,2,5,6, 8, &cv_tank, ia_hit, 3,dam_physical,40, v_medium, ('c' | (cYellow << BITSPERBYTE) ), 40, 30, m_flesh, m_blood, Map::desert, th_wild, NULL ),
	creature( "rabbit",     NULL, NULL, NULL, od_none, vo_none, sk_anim, 4,  ap_carn, dr_water, 5,5,5,4,4,2,5,6, 8, &cv_natural, ia_bite,3,dam_physical,20, v_medium, ('r' | (cGrey   << BITSPERBYTE) ), 40, 30, m_flesh, m_blood, Map::desert, th_wild, NULL ),
	creature( "skunk",      NULL, NULL, NULL, od_none, vo_none, sk_anim, 4,  ap_carn, dr_water, 5,5,5,4,4,2,5,6, 8, &cv_natural, ia_spray,3,dam_poison,  50, v_medium, ('f' | (cDGrey  << BITSPERBYTE) ), 40, 30, m_flesh, m_blood, Map::desert, th_wild, NULL ),

	// Dangerous Plants
  //                                                                                          c p s t w k c s hp
	creature( "triffid",  NULL, NULL, NULL,   od_none, vo_none, sk_none, 99, ap_carn, dr_water, 2,4,5,5,2,1,1,2,10, &cv_natural, ia_sting,8,dam_poison,100, v_large,  ('T' | (cLGreen << BITSPERBYTE) ), 20, 40, m_mulch, m_t_oil, Map::forest, th_wild, NULL ),
	creature( "venusaur", NULL, NULL, NULL,   od_none, vo_none, sk_none, 99, ap_none, dr_water, 2,2,5,5,2,1,1,1,10, &cv_natural, ia_sting,8,dam_poison,100, v_large,  ('T' | (cLBlue  << BITSPERBYTE) ), 20, 30, m_mulch, m_sap,   Map::forest, th_wild, NULL ),
  creature( "cactusman",  NULL, NULL, NULL, od_none, vo_omni, sk_none, 99, ap_carn, dr_water, 6,5,6,4,3,4,5,4,14, &cv_bristles, ia_sting,6,dam_poison,100, v_tall,   ('Y' | (cYellow << BITSPERBYTE) ), 20, 40, m_mulch, m_water, Map::desert, th_wild, NULL ),
  creature( "thistleman", NULL, NULL, NULL, od_none, vo_omni, sk_none, 99, ap_carn, dr_water, 4,5,5,5,3,4,5,5, 8, &cv_bristles, ia_sting,8,dam_poison,100, v_tall,   ('Y' | (cGreen  << BITSPERBYTE) ), 20,  0, m_mulch, m_sap,   Map::desert, th_wild, NULL ),

  // Somewhat Generic Undead/Demons
	creature( "kasha",           NULL, NULL, NULL,   od_none, vo_none, sk_anim, 99, ap_carn, dr_water, JOIN_STATS(STATS_HUMAN,STATS_SMALL),     &cv_demon, ia_hit|ia_claw|ia_bite,6,dam_physical,100, v_large,  ('z' | (cRed    << BITSPERBYTE) ), 10, 40, m_steel,m_poison,Map::forest, th_demon, NULL ),
	creature( "kuang shi",       NULL, NULL, NULL,   od_none, vo_none, sk_anim, 99, ap_carn, dr_water, JOIN_STATS(STATS_HUMAN,STATS_LIGHT),     &cv_demon, ia_hit|ia_claw|ia_bite,7,dam_physical,100, v_large,  ('z' | (cLRed   << BITSPERBYTE) ), 10, 40, m_steel,m_poison,Map::forest, th_demon, NULL ),
	creature( "zombi",           NULL, NULL, NULL, od_rotten, vo_none, sk_anim, 99, ap_carn, dr_water, JOIN_STATS(STATS_HUMAN,STATS_WEAK),      &cv_demon, ia_hit|ia_claw|ia_bite,4,dam_poison,  100, v_large,  ('z' | (cBrown  << BITSPERBYTE) ), 10, 40, m_steel,m_poison,Map::forest, th_demon|th_sewers, NULL ),
	creature( "wyrdalak",        NULL, NULL, NULL,   od_foul, vo_none, sk_anim, 99, ap_carn, dr_water, JOIN_STATS(STATS_HUMAN,STATS_STRONG),    &cv_demon, ia_hit|ia_claw|ia_bite,8,dam_physical,100, v_large,  ('z' | (cDGrey  << BITSPERBYTE) ), 10, 40, m_steel,m_poison,Map::forest, th_demon, NULL ),
	creature( "unholy revenant", NULL, NULL, NULL,  od_fetid, vo_none, sk_anim, 99, ap_carn, dr_water, JOIN_STATS(STATS_HUMAN,STATS_HUGE),      &cv_demon, ia_hit|ia_claw|ia_bite,4,dam_physical,100, v_large,  ('&' | (cLGreen << BITSPERBYTE) ), 10, 40, m_steel,m_poison,Map::forest, th_demon|th_sewers, NULL ),
	creature( "bone demon",      NULL, NULL, NULL,   od_none, vo_none, sk_anim, 99, ap_carn, dr_water, JOIN_STATS(STATS_HUMAN,STATS_PASSIONATE),&cv_demon, ia_hit|ia_claw|ia_bite,5,dam_physical,100, v_large,  ('&' | (cLGrey  << BITSPERBYTE) ), 10, 40, m_bone, m_ichor, Map::forest, th_demon|th_mines, NULL ),
	creature( "dust devil",      NULL, NULL, NULL,   od_none, vo_none, sk_anim, 99, ap_carn, dr_water, JOIN_STATS(STATS_HUMAN,STATS_TASTY),     &cv_demon, ia_hit|ia_claw|ia_bite,6,dam_physical,100, v_large,  ('&' | (cBrown  << BITSPERBYTE) ),  0,100, m_sand, m_sand,  Map::forest, th_demon, NULL ),
	creature( "sickly shadow",   NULL, NULL, NULL, od_rotten, vo_none, sk_anim, 99, ap_carn, dr_water, JOIN_STATS(STATS_HUMAN,STATS_FAST),      &cv_demon, ia_hit|ia_claw|ia_bite|ia_choke,8,dam_physical,100, v_large,  ('&' | (cDGrey  << BITSPERBYTE) ),  0, 40, m_ash,  m_ichor, Map::forest, th_mines|th_demon|th_sewers, NULL ),

  // Somewhat Generic Mutants
	creature( "deviant",       NULL,NULL,NULL, od_none, vo_none, sk_anim, 99, ap_carn, dr_water, JOIN_STATS(STATS_HUMAN,STAT_PERCEPTION(-1)), &cv_natural, ia_hit|ia_bite,1,dam_physical,100, v_large,  ('k' | (cBlue   << BITSPERBYTE) ), 20, 40, m_flesh, m_bile, Map::forest, th_mutant, NULL ),
	creature( "freak",         NULL,NULL,NULL, od_none, vo_none, sk_anim, 99, ap_carn, dr_water, 5,4,5,5,5,5,5,5,10, &cv_natural, ia_hit|ia_bite,4,dam_physical,100, v_large,  ('k' | (cIndigo << BITSPERBYTE) ), 20, 40, m_flesh, m_bile, Map::forest, th_mutant, NULL ),
	creature( "abomination",   NULL,NULL,NULL, od_none, vo_none, sk_anim, 99, ap_carn, dr_water, 5,4,5,5,5,5,5,5,10, &cv_natural, ia_hit|ia_bite,5,dam_physical,100, v_large,  ('k' | (cViolet << BITSPERBYTE) ), 20, 40, m_flesh, m_bile, Map::forest, th_mutant, NULL ),
	creature( "decayed beast", NULL,NULL,NULL, od_none, vo_none, sk_anim, 99, ap_carn, dr_water, 5,4,5,5,5,5,5,5,10, &cv_natural, ia_hit|ia_bite,6,dam_physical,100, v_large,  ('k' | (cLRed   << BITSPERBYTE) ), 20, 40, m_flesh, m_bile, Map::forest, th_mutant, NULL ),
	creature( "black horror",  NULL,NULL,NULL, od_none, vo_none, sk_anim, 99, ap_carn, dr_water, 5,4,5,5,5,5,5,5,10, &cv_natural, ia_hit|ia_bite,7,dam_physical,100, v_large,  ('k' | (cDGrey  << BITSPERBYTE) ), 20, 40, m_flesh, m_bile, Map::forest, th_mutant, NULL ),
	creature( "white horror",  NULL,NULL,NULL, od_none, vo_none, sk_anim, 99, ap_carn, dr_water, 5,4,5,5,5,5,5,5,10, &cv_natural, ia_hit|ia_bite,8,dam_physical,100, v_large,  ('k' | (cWhite  << BITSPERBYTE) ), 20, 40, m_flesh, m_bile, Map::forest, th_mutant, NULL ),

  // Random creatures
	creature( "dust soldier",     NULL, NULL, NULL,  od_none,  vo_omni, sk_none, 0, ap_omni, dr_water, 5, 5, 5, 5, 5, 5, 5, 5,12, &cv_natural, ia_hit|ia_kick|ia_choke,4,dam_physical,100, v_tall,   ('h' | (cDGrey  << BITSPERBYTE) ), 30, 30, m_flesh, m_blood, Map::any, th_none, NULL ),

	creature( "boilerman",        NULL, NULL, NULL,  od_none,  vo_omni, sk_none, 0, ap_none, dr_oil,   4,4, 7,6,2,5,5,5,30, &cv_mech_man, ia_hit,2,dam_physical,100, v_tall,   ('R' | (cLGrey  << BITSPERBYTE) ), 30, 30, m_iron,  m_oil, Map::any, th_steam|th_mines, NULL ),

  // Mimic:
	creature( "mimic",            NULL, NULL, NULL,  od_none,  vo_omni, sk_none, 0, ap_carn, dr_water,                                 STATS_HUMAN, &cv_natural, ia_hit,8,dam_physical,100, v_tall, ('m' | (cWhite  << BITSPERBYTE) ), 0, 0, m_steel, m_blood, Map::any, th_all & ~th_wild, NULL ),
  
  // Morlocks:
	creature( "morlock",          NULL, NULL, NULL,  od_rotten,  vo_omni, sk_none, 0, (Appetite)(ap_omni|ap_allmeat), dr_water,                               STATS_MORLOCK, &cv_natural, ia_hit|ia_kick|ia_choke,4,dam_physical,80, v_tall, ('M' | (cBrown  << BITSPERBYTE) ), 0, 0, m_flesh, m_blood, Map::any, th_sewers, NULL ),
	creature( "morlock fighter",  NULL, NULL, NULL,  od_fetid,   vo_omni, sk_none, 0, (Appetite)(ap_omni|ap_allmeat), dr_water,      JOIN_STATS(STATS_MORLOCK,STATS_STRONG), &cv_natural, ia_hit|ia_kick|ia_choke,4,dam_physical,80, v_tall, ('M' | (cRed    << BITSPERBYTE) ), 0, 0, m_flesh, m_blood, Map::any, th_sewers, NULL ),
	creature( "morlock cook",     NULL, NULL, NULL,  od_rotten,  vo_omni, sk_none, 0, (Appetite)(ap_omni|ap_allmeat), dr_water,       JOIN_STATS(STATS_MORLOCK,STATS_LIGHT), &cv_natural, ia_hit|ia_kick|ia_choke,6,dam_physical,80, v_tall, ('M' | (cYellow << BITSPERBYTE) ), 0, 0, m_flesh, m_blood, Map::any, th_sewers, NULL ),
	creature( "morlock thrummer", NULL, NULL, NULL,  od_foul,    vo_omni, sk_none, 0, (Appetite)(ap_omni|ap_allmeat), dr_water,        JOIN_STATS(STATS_MORLOCK,STATS_FAST), &cv_natural, ia_hit|ia_kick|ia_choke,5,dam_physical,80, v_tall, ('M' | (cLGreen << BITSPERBYTE) ), 0, 0, m_flesh, m_blood, Map::any, th_sewers, NULL ),
	creature( "morlock chieftan", NULL, NULL, NULL,  od_fetid,   vo_omni, sk_none, 0, (Appetite)(ap_omni|ap_allmeat), dr_water,  JOIN_STATS(STATS_MORLOCK,STATS_PASSIONATE), &cv_natural, ia_hit|ia_kick|ia_choke,7,dam_physical,80, v_tall, ('M' | (cIndigo << BITSPERBYTE) ), 0, 0, m_flesh, m_blood, Map::any, th_sewers, NULL ),
  

  // From Oz
	// Mechanical Men, automatons

//
	creature( "bronze soldier",   NULL, NULL, NULL,  od_none, vo_omni, sk_none, 0, ap_none, dr_oil,      JOIN_STATS(STATS_AUTOMATON,STATS_WEAK), &cv_mech_man, ia_hit,7,dam_physical,100, v_tall,   ('R' | (cOrange << BITSPERBYTE) ), 40, 40, m_bronze, m_oil, Map::any, th_steam|th_mines, NULL ),
	creature( "tin soldier",      NULL, NULL, NULL,  od_none, vo_omni, sk_none, 0, ap_none, dr_oil,                             STATS_AUTOMATON, &cv_mech_man, ia_hit,8,dam_physical,100, v_tall,   ('R' | (cGrey  << BITSPERBYTE) ), 40, 40, m_tin,    m_oil, Map::any, th_steam|th_mines, NULL ),
	creature( "clockwork soldier",NULL, NULL, NULL,  od_none, vo_omni, sk_none, 0, ap_none, dr_oil,JOIN_STATS(STATS_AUTOMATON,STATS_PASSIONATE), &cv_mech_man, ia_hit,8,dam_physical,100, v_tall,   ('R' | (cDGrey << BITSPERBYTE) ), 30, 30, m_steel,  m_oil, Map::any, th_steam|th_mines, NULL ),
	creature( "steel soldier",    NULL, NULL, NULL,  od_none, vo_omni, sk_none, 0, ap_none, dr_oil,    JOIN_STATS(STATS_AUTOMATON,STATS_STRONG), &cv_mech_man, ia_hit,9,dam_physical,100, v_tall,   ('R' | (cWhite  << BITSPERBYTE) ), 30, 30, m_steel,  m_oil, Map::any, th_steam|th_mines, NULL ),
	creature( "iron giant",       NULL, NULL, NULL,  od_none, vo_omni, sk_none, 0, ap_none, dr_oil,      JOIN_STATS(STATS_AUTOMATON,STATS_HUGE), &cv_mech_man, ia_hit,14,dam_physical,100, v_huge,   ('R' | (cGrey  << BITSPERBYTE) ), 20, 30, m_iron,   m_oil, Map::any, th_steam, NULL ),

  // Leeches for the Mechanical Mines
  creature( "acid worm",        NULL, NULL, NULL, od_stale, vo_none, sk_anim, 4, ap_carn, dr_none,  3,5, 4,4,4,2,5,4, 8, &cv_slimy, ia_spit,3,dam_burn,100, v_medium, ('w' | (cGreen << BITSPERBYTE) ), 40, 30, m_flesh, m_blood, Map::any, th_mines, NULL ),
  creature( "acid slug",        NULL, NULL, NULL,od_rotten, vo_none, sk_anim, 4, ap_carn, dr_none,  4,5, 4,4,4,2,5,4, 8, &cv_slimy, ia_spit|ia_touch,4,dam_burn,100, v_medium, ('j' | (cGreen << BITSPERBYTE) ), 40, 30, m_flesh, m_blood, Map::any, th_mines, NULL ),
  creature( "acid leech",       NULL, NULL, NULL,  od_foul, vo_none, sk_anim, 4, ap_carn, dr_none,  6,5, 6,4,4,2,5,5, 8, &cv_slimy, ia_touch,5,dam_burn,100, v_medium, ('j' | (cLGreen << BITSPERBYTE) ), 40, 30, m_flesh, m_blood, Map::any, th_mines, NULL ),

  // Other OZ
	creature( "scarecrow",        NULL, NULL, NULL,  od_none, vo_omni, sk_none, 0, ap_none, dr_none,  3,4, 4,8,2,5,5,5,10, &cv_ragdoll, ia_hit,1,dam_physical,100, v_tall,   ('h' | (cYellow << BITSPERBYTE) ), 20,  0, m_straw,  m_straw, Map::plains, th_none, NULL ),

  // Gorey
  // Demons
	creature( "black doll",  NULL, NULL, NULL,      od_fetid, vo_omni, sk_rese, 0, ap_none, dr_water, 8,4, 4,6,6,5,5,6,10, &cv_ragdoll, ia_hit,5,dam_physical,100, v_medium,   ('&' | (cDGrey << BITSPERBYTE) ), 20, 16, m_cotton, m_ichor, Map::any, th_demon,
            "En effet, absolument la fin."  ),
	creature( "rag doll",    NULL, NULL, NULL,       od_foul, vo_omni, sk_rese, 0, ap_none, dr_water, 6,4, 7,7,6,5,5,4,12, &cv_ragdoll, ia_hit,5,dam_physical,100, v_medium,   ('&' | (cBrown  << BITSPERBYTE) ), 20, 16, m_cotton, m_ichor, Map::any, th_demon,
            "There's no going to town in a bathtub." ),
	creature( "lace doll",   NULL, NULL, NULL,       od_foul, vo_omni, sk_rese, 0, ap_none, dr_water, 6,8, 5,6,6,5,5,8, 8, &cv_ragdoll, ia_hit,5,dam_physical,100, v_medium,   ('&' | (cWhite  << BITSPERBYTE) ), 20, 16, m_cotton, m_ichor, Map::any, th_demon,
            "Short sheets make the bed look longer." ),
	creature( "dead toy",    NULL, NULL, NULL,       od_foul, vo_omni, sk_rese, 0, ap_none, dr_water, 6,4, 5,8,6,5,5,5,14, &cv_ragdoll, ia_hit,5,dam_physical,100, v_medium,   ('&' | (cIndigo << BITSPERBYTE) ), 20, 16, m_wool,  m_ichor, Map::any, th_demon|th_sewers,
            "Turnips are turnips, and prunes are prunes." ),

  // The Dark Tower
	creature( "taheen",         NULL, NULL, NULL,     od_rotten,vo_squawk, sk_none, 0, ap_carn, dr_water, 6, 5, 6, 6, 5, 6, 5, 6, 8, &cv_mech_man, ia_hit|ia_peck,3,dam_physical,100, v_tall,   ('h' | (cIndigo << BITSPERBYTE) ), 20, 40, m_flesh,  m_blood, Map::any, th_none,
            "Have you found Algul Siento?" ),
	creature( "slow mutant",    NULL, NULL, NULL,      od_stale,  vo_omni, sk_none, 0, ap_allmeat, dr_water, 5, 5, 6, 6, 4, 5, 5, 5,12, &cv_mech_man, ia_hit|ia_claw,5,dam_physical,100, v_tall,   ('k' | (cLGreen << BITSPERBYTE) ), 20, 40, m_flesh,  m_blood, Map::desert, th_mutant|th_sewers, NULL ),
	creature( "lobstrosity",    NULL, NULL, NULL,       od_none, vo_click, sk_anim, 0, ap_carn, dr_water, 5, 5, 7, 6, 5, 6, 5, 5,14, &cv_mech_man, ia_bite|ia_claw,4,dam_physical,100, v_long,   ('s' | (cLRed   << BITSPERBYTE) ), 20, 40, m_flesh,  m_blood, Map::desert, th_wild,
            "Did-a-chick!" ),

  // H. P. Lovecraft
  //                                                                                                c  p  s  t  w  k  c  s hp
	creature( "deep one", "frogman", "frogwoman", "slimy child",
                                                 od_rotten, vo_omni, sk_none, 0, ap_carn, dr_water, 5, 5, 7, 6, 5, 6, 5, 5,14, &cv_aquatic, ia_hit|ia_claw,4,dam_physical,100, v_tall,   ('h' | (cGreen  << BITSPERBYTE) ), 20, 40, m_flesh,  m_blood, Map::any, th_none|th_sewers, NULL ),
  creature( "white ape",  NULL, NULL, NULL,      od_stale,  vo_omni, sk_none, 0, ap_carn, dr_water, 5, 5, 6, 6, 4, 5, 5, 5,12, NULL, ia_hit|ia_bite,4,dam_physical,100, v_tall,   ('h' | (cWhite  << BITSPERBYTE) ), 20, 40, m_flesh,  m_blood, Map::desert, th_none, NULL ),
	creature( "archaean",    NULL, NULL, NULL,     od_rotten, vo_pipe, sk_rese, 5, ap_omni, dr_water, 8, 8,18,10, 7, 8, 5, 6,26, NULL, ia_hit|ia_claw|ia_sting,3,dam_physical,100, v_large,  ('T' | (cViolet << BITSPERBYTE) ), 30, 30, m_wood,   m_sap,   Map::any, th_none,
            "Beware the Shoggoth!" ),
	creature( "shoggoth",    NULL, NULL, NULL,    od_noisome, vo_howl, sk_rese, 0, ap_all,  dr_water, 3, 6,30,22, 5, 4, 5, 6,60, &cv_slimy, ia_hit|ia_crush|ia_spit,5,dam_physical,100, v_huge,  ('U' | (cDGrey  << BITSPERBYTE) ),  0, 30, m_oil,    m_oil,   Map::any, th_none|th_sewers,
            "Tekeli-li! Tekeli-li!"),
  creature( "mi-go",       NULL, NULL, NULL,     od_rotten, vo_buzz, sk_rese, 0, ap_alien,dr_alien, 7, 6, 5, 5, 6, 6, 6, 8,12, NULL, ia_hit|ia_claw,5,dam_physical,100, v_large,  ('i' | (cLRed   << BITSPERBYTE) ),  0, 30, m_flesh,  m_ichor, Map::any, th_none, NULL ),

  // Robert W. Chambers
	creature( "ghoul",       NULL, NULL, NULL,     od_rotten, vo_omni, sk_anim, 0, ap_carn, dr_water, 4,4, 6,4,1,5,5,5,18, &cv_demon, ia_touch,9,dam_physical,100, v_tall,   ('z' | (cGrey   << BITSPERBYTE) ), 80, 40, m_putrid,  m_bile,  Map::any, th_catacomb|th_undead, NULL ),
	creature( "wraith",      NULL, NULL, NULL,       od_foul, vo_howl, sk_anim, 0, ap_none, dr_water, 5,7, 7,5,7,7,5,5,30, &cv_ghost, ia_claw,5,dam_physical,100, v_tall,   ('G' | (cHaze   << BITSPERBYTE) ), 30, 30, m_ash,  m_ichor, Map::any, th_catacomb|th_undead, NULL ),

	};

creature *creature::List = creaturelist;

#endif


#endif // CRLIST_CPP
