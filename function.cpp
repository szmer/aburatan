// [function.cpp]
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
#if !defined ( FUNCTION_CPP )
#define FUNCTION_CPP

#include "function.hpp"

#include "visuals.hpp"
#include "compiler.hpp"
#ifdef OBSOLETE_INCLUDE
#  include <ctype.h>
#else
#  include <cctype>
#endif


String FunctionItem::menustring( char len ) {
	String retStr = mstring;

	retStr.abbrev(len);

	return retStr;
	}

Image FunctionItem::menuletter( void ) {
	return Image(cWhite, letter);
	}

bool FunctionItem::operator ==( const FunctionItem &rfi ) {
	return ( vfptr == rfi.vfptr )? true:false;
	}

bool FunctionItem::operator <( const FunctionItem &rfi ) {
	return (  ( std::toupper(letter) < std::toupper(rfi.letter) ) && ( letter < rfi.letter )  )? true:false;
	}

bool FunctionItem::operator >( const FunctionItem &rfi ) {
	return (  ( std::toupper(letter) > std::toupper(rfi.letter) ) && ( letter > rfi.letter )  )? true:false;
	}


#endif // FUNCTION_CPP
