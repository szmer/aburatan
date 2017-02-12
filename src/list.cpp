// [list.cpp]
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
#if !defined ( LIST_CPP )
#define LIST_CPP

#include "list.hpp"

//#define __LIST_HARNESS

#ifdef __LIST_HARNESS


#include "compiler.hpp"

#ifdef OBSOLETE_INCLUDE
#  include <stdio.h>
#  include <alloc.h>
#else
#  include <stdio>
#  include <alloc>
#endif

#include "error.hpp"
#include "string.hpp"
#include "counted.hpp"
#include "sorted.hpp"

int mainmethod ( void ) {

	List<int> intList;

	intList.add(3);
	intList.add(4);
	intList.add(5);
	intList.add(8);
	intList.add(1);
	intList.add(5);

	Sorted<int> copy;// = intList;

	copy.add( intList );

	if ( intList.reset() && copy.reset() )
	do {

		printf("%i %i \n\r", intList.get(), copy.get() );

		}	while ( intList.next() && copy.next() );

	return 0;
	}

int main( void ) {

	long initialmem = coreleft();

	mainmethod();

	printf("Initial mem: %li; Current mem: %li;\n\n\r", initialmem, coreleft());


	return 0;
	}

#endif //  __LIST_HARNESS


#endif // LIST_CPP

