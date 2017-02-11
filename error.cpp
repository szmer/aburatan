// [error.cpp]
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
#if !defined ( ERROR_CPP )
#define ERROR_CPP

#include "error.hpp"

#include "compiler.hpp"

#ifdef OBSOLETE_INCLUDE
#  include <stdio.h>
#  include <stdlib.h>
#else
#  include <cstdio>
#  include <cstdlib>
#endif



void Error::fatal( const char *const str ) {
// Writes string to screen then exits.
	fprintf(stderr, str);
	std::exit(1);
	}

void Error::fatal( const String &str ) {
	fatal( str.toChar() );
	}
#endif // ERROR_CPP
