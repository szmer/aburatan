// [skill.hpp]
// (C) Copyright 2003 Michael Blackney
#if !defined ( SKILL_HPP )
#define SKILL_HPP

#include "enum.hpp"
#include "types.hpp"
#include "string.hpp"

//--[ Skills ]--//

enum Skill {

  // NOTE: New skills must have their own
  //       SkillRec and Skill sk_enum
  
  sk_acro,
  sk_anim,
//  sk_appr,
  sk_athl,
  sk_awar,
//  sk_chem,
  sk_dipl,
  sk_figh,
  sk_fait,
  sk_heal,
  sk_mark,
  sk_mech,
//  sk_occu,
  sk_pilo,
  sk_rese,
//  sk_sham,
  sk_stea,

  sk_num,
  sk_invalid = sk_num,
  sk_none = sk_invalid

  };


enum SkillType {

    tNorm,   // Normal skill
    tInvest, // In skill menu can choose to invest con points in this skill
    tApply   // Selecting in skill menu will act in some way
    
    };



struct SkillRec {

  char letter;
  const char *name;
  const char *profession;
  StatType stat;

  SkillType type;

  bool implemented; // True when enough of the skill is implemented
                    // that characters who rely on the skill will not
                    // be seriously disadvantaged.
  };

const SkillRec Skills[sk_num] = {

  // NOTE: New skills must have their own
  //       SkillRec and Skill sk_enum

  { 'a', "acrobatics",      "acrobat",      stat_crd, tNorm,   true },
  { 'e', "animal empathy",  "farmer",       stat_wil, tApply,  true },
//  { 'p', "appraisal",       "antiquarian",  stat_kno, tApply,  false },
  { 'A', "athletics",       "athlete",      stat_str, tNorm,   true },
  { 'w', "awareness",       "investigator", stat_per, tInvest, true },
//  { 'c', "chemistry",       "chemist",      stat_kno, tApply,  false },
  { 'd', "diplomacy",       "diplomat",     stat_wil, tApply,  true },
  { 'f', "fighting",        "soldier",      stat_crd, tNorm,   true },
  { 'F', "faith",           "priest",       stat_wil, tNorm,   false },
  { 'h', "healing",         "surgeon",      stat_kno, tApply,  true },
  { 'm', "marksmanship",    "rifleman",     stat_per, tNorm,   true },
  { 'M', "mechanics",       "mechanic",     stat_kno, tApply,  true },
//  { 'o', "occult",          "occultist",    stat_kno, tNorm,   false },
  { 'p', "piloting",        "pilot",        stat_crd, tNorm,   false },
  { 'r', "research",        "scholar",      stat_kno, tNorm,   true },
  { 's', "stealth",         "drifter",      stat_crd, tInvest, true },

  };


struct SkillVal {

  int value, investment;
  Skill skill;
  bool train;

  SkillVal();
  SkillVal( int, Skill );
  
  operator int( void );
  int &operator =( int );
  SkillVal &operator =( const SkillVal& );

  int &operator ++();
  const int operator ++( int );

  void set( Skill );

  void invest( int );
  int  getInvestment( void );

  String menustring(char length=80);
	Image menuletter(void);

  };

typedef SkillVal SkillSet[sk_num] ;


//--[ Feats ]--//

// Nonweapon Feat bitfield:
enum Feat {

  f_none   = 0x0000,

  f_ambi   = 0x0001, // Ambidexterous (prim and secondary have same bonuses)
  f_shot   = 0x0002, // Far Shot (bonus 50% firearm range)
  f_crit   = 0x0004, // Critical hit (bonus chance to score critical hits)
  f_pbsh   = 0x0008, // Point Blank Shot (bonus for adjacent ranged attacks)
  f_draw   = 0x0010, // Quickdraw (no time taken to switch weapons)
  f_stab   = 0x0020, // Backstab (bonus when attacking peaceful monsters)
  f_vete   = 0x0040, // Veteran (bonus to serious injury rolls)
  f_2hcm   = 0x0080, // 2 handed combat (no penalty for 2h combat)
  f_heav   = 0x0100, // Heavy weapon specialist (no penalty for heavy weapons)
  f_fast   = 0x0200, // Fast Shot (33% speed bonus for 2h use of 1h firearm)
  f_pilo   = 0x0400, // Master pilot (no penalty for wielding while riding)

  f_all    = 0xFFFF

  };

// Weapon Feat bitfield:

enum Proficiency {

  p_none   = 0x0000,

  p_pierce = 0x0001, // Proficiency with piercing weapons
  p_bash   = 0x0002, //                  concussion weapons
  p_cut    = 0x0004, //                  slashing weapons
  p_chop   = 0x0008, //                  chopping weapons

  p_weapon = 0x000F, // Only weapon proficiencies

  p_l_arm  = 0x0010, // with light armour
  p_m_arm  = 0x0020, //      medium armour
  p_h_arm  = 0x0040, //      heavy armour
  p_shield = 0x0080, //      shields

  p_armour = 0x00F0, // Only armour proficiencies
  
  p_all    = 0xFFFF

  };

//--[ Stats ]--//

// Stats names

struct StatRec {

  const char *name;
  const char *adjective;

  };

const StatRec StatStr[stat_total] = {

	{ "coordination", "coordinated" },
	{ "perception", "perceptive" },
	{ "strength", "strong" },
	{ "toughness", "tough" },
	{ "willpower", "willful" },
	{ "knowledge", "knowledgable" },
	{ "concentration", "focussed" },
	{ "speed", "fast" },
	{ "health", "healthy" }

  };

#endif // SKILL_HPP

