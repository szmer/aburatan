// [diceroll.cpp]
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
#if !defined ( DICEROLL_CPP )
#define DICEROLL_CPP

#include "diceroll.hpp"
#include "random.hpp"

String DiceRoll::toString() {

  if (num == 0 || sides == 0)
    return "<_none>";

	String retStr;

	retStr += (long) num;
	retStr += 'd';
	retStr += (long) sides;

	if (bonus) {
		if (bonus > 0)
			retStr += '+';
		retStr += (long) bonus;
		}

	return retStr;

	}

int DiceRoll::roll() const {

	int ret = 0;

	for (int count = 0; count < num; count++)
		ret += Random::randint( sides ) + 1;

	ret += bonus;

	return ret;

	}

DiceRoll &DiceRoll::operator +=( const DiceRoll &rhs ) {

	num += rhs.num;
	sides += rhs.sides;
	bonus += rhs.bonus;

	return *this;

	}

  

//#define __DICEHARN
#ifdef __DICEHARN

#include "string.hpp"
#include "random.hpp"
#include "counted.hpp"

#include "compiler.hpp"
#ifdef OBSOLETE_INCLUDE
#  include <stdio.h>
#else
#  include <stdio>
#endif

int main() {

	Random::randomize();

	DiceRoll dice (3, 6);

	printf( "Rolling %s\r\n", dice.toString().toChar() );

	for (int i = 0; i < 5; i++ )
		printf( "  roll #%i: %i\r\n", i+1, dice.roll() );

	return 0;
	}

#endif // __DICEHARN

#endif // DICEROLL_CPP
