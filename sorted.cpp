// [sorted.cpp]
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
#if !defined ( SORTED_CPP )
#define SORTED_CPP

#include "sorted.hpp"

#ifdef SORTED__TEST

#include "visuals.hpp"
#include "point.hpp"
#include "rectngle.hpp"
#include "enum.hpp"
#include "string.hpp"
#include "counted.hpp"
#include "error.hpp"
#include "random.hpp"
#include "alloc.h"

void writelist( Sorted<int> &s ) {

	int n = 0;

	Screen::write("-------\n\r");

	if ( s.reset() )

	do
		Screen::write( String("Member ") + n++ + " = " + s.get() + "\n\r");
		while ( s.next() );

	}

void mainroutine () {

	Screen::clr();
	Screen::locate(1,1);
	Random::randomize();

	Sorted<int> int_list;

	for (int i = 0; i < 8; ++i) {
		int_list.add( Random::randint(10) );
		}


	writelist( int_list );

	}


int main() {
	long initialMem = coreleft();

	Random::randomize();

	Screen::clr();

	mainroutine();

	Screen::locate(50, 11);
	Screen::write( "Initial Mem: " );
	Screen::write( (long) initialMem );
	Screen::locate(50, 12);
	Screen::write( "Current Mem: " );
	Screen::write( (long) coreleft() );
	Screen::write("\r\n");

	return 0;
	}

#endif // SORTED__TEST

#endif // SORTED_CPP
