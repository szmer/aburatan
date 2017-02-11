// [stats.cpp]
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
#if !defined ( STATS_CPP )
#define STATS_CPP

#include "stats.hpp"
#include "enum.hpp"

void Stats::reset(void) {

  crd = 0;/* Coordination  */
  per = 0;/* Perception    */
  str = 0;/* Strength      */
  tuf = 0;/* Toughness     */
  wil = 0;/* Willpower     */
  kno = 0;/* Knowledge     */
  con = 0;/* Concentration */
  spd = 0;/* Speed         */
  hp  = 0;/* Hit points    */

  }

Stats &Stats::operator +=( const Stats &rhs ) {
  crd += rhs.crd;/* Coordination  */
  per += rhs.per;/* Perception    */
  str += rhs.str;/* Strength      */
  tuf += rhs.tuf;/* Toughness     */
  wil += rhs.wil;/* Willpower     */
  kno += rhs.kno;/* Knowledge     */
  con += rhs.con;/* Concentration */
  spd += rhs.spd;/* Speed         */
  hp  += rhs.hp;/* Hit points    */

  return *this;
  }
  
Stats &Stats::operator -=( const Stats &rhs ) {
  crd -= rhs.crd;/* Coordination  */
  per -= rhs.per;/* Perception    */
  str -= rhs.str;/* Strength      */
  tuf -= rhs.tuf;/* Toughness     */
  wil -= rhs.wil;/* Willpower     */
  kno -= rhs.kno;/* Knowledge     */
  con -= rhs.con;/* Concentration */
  spd -= rhs.spd;/* Speed         */
  hp  -= rhs.hp;/* Hit points    */

  return *this;
  }

char &Stats::operator []( int index ) {

  switch (index) {
    case stat_crd: return crd;
    case stat_per: return per;
    case stat_str: return str;
    case stat_tuf: return tuf;
    case stat_wil: return wil;
    case stat_kno: return kno;
    case stat_con: return con;
    case stat_spd: return spd;
    
    default: return crd;
    }

  }

#endif // STATS_CPP
