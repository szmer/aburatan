// [skill.cpp]
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
#if !defined ( SKILL_CPP )
#define SKILL_CPP

#include "skill.hpp"
#include "visuals.hpp"

SkillVal::SkillVal()
: value(0), investment(0), skill(sk_invalid), train(false) {
  }
  
SkillVal::SkillVal( int i, Skill sk )
: value(i), investment(0), skill(sk), train(false) {
  }
  
SkillVal::operator int( void ) {

  return value;
  }

int &SkillVal::operator =( int i ) {
  return value = i;
  }

SkillVal &SkillVal::operator =( const SkillVal &rhs ) {
  value = rhs.value;
  return *this;
  }
  

String SkillVal::menustring(char length) {

  String retStr;
  switch ( Skills[skill].type ) {
    case tNorm:
      retStr = "<07";
      break;

    default:
      if ( (Skills[skill].type != tInvest) || value )
        retStr = "<11";
      else
        retStr = "<01";
      break;
    }

  retStr += String(Skills[skill].name).titlecase();
  retStr += ">";

  String space;
  space.fillnchar(' ', 19-retStr.length()+(value>9?0:1) );
  retStr += space;
  
  if (value) {

    if (value > 0)
      retStr += "+";
    retStr += (long) value;

    }
  else {
    retStr += "  ";
    }
  
  if (Skills[skill].type == tInvest) {
    retStr += " <++";
    retStr += (long) investment;
    retStr += ">";
    }
  else {
    retStr += "   ";
    }

  return retStr.abbrev(length);
  }
  
Image SkillVal::menuletter(void) {
  return Image(cGrey, Skills[skill].letter );
  }

void SkillVal::set( Skill sk ) {
  skill = sk;
  }

void SkillVal::invest( int i ) {
  investment = i;
  }

int SkillVal::getInvestment( void ) {
  return investment;
  }

int &SkillVal::operator ++() {
  return ++value;
  }
  
const int SkillVal::operator ++( int ) {
  return value++;
  }


#endif // SKILL_CPP
