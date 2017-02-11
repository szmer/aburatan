// [serial.cpp]
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
#if !defined ( SERIAL_CPP )
#define SERIAL_CPP

#include "serial.hpp"
#include "file.hpp"

long Serial::next = 1;

Serial::Serial() {
	number = 0;
	}

void Serial::init( long l ) {
	if (l == 0) {	if (number == 0) number = next++; }

	else number = l;
	}

void Serial::save( void ) {

	File file;

	String filename = SAVEDIR+"SERIAL.NUM";

	file[filename].write( (char*) &next, sizeof(next) );

	}

void Serial::load( void ) {
	File file;

	String filename = SAVEDIR+"SERIAL.NUM";

	file[filename].read( (char*) &next, sizeof(next) );

	}

void Serial::reset( void ) {
	next = 1;
	}

String Serial::toFileName( void ) const {
	return toFileName(number);
	}

String Serial::toFileName( long number ) {
	long temp = number;

	if (number < 0) return "ERROR";

	String retval = "\0";
	char modulo, append;

	do {
		modulo = temp % 36;
		assert ( !(modulo < 0) && !(modulo > 35) );
    // Error::fatal( String("Modulo ") + modulo + " is invalid!" );

		if ( modulo < 10 ) append = '0' + modulo;
		else append = 'a' + modulo - 10;

		retval += append;

		temp /= 36;

		} while (temp > 0);

	return retval;
	}

long Serial::toLong( void ) const {
	return number;
	}

bool Serial::operator ==( long l ) const {
	return (number == l)?true:false;
	}

std::iostream &operator << ( std::iostream &ios, Serial &s ) {

	s.init();

	ios.write( (char*) &s.number, sizeof(s.number) );

	return ios;

	}

std::iostream &operator >> ( std::iostream &ios, Serial &s ) {
	long read;

	ios.read( (char*) &read, sizeof(read) );

	s.init( read );

	return ios;

	}


//#define SERIAL__HARNESS

#ifdef SERIAL__HARNESS

#include "visuals.hpp"
#include "rectngle.hpp"
#include "point.hpp"
#include "string.hpp"
#include "counted.hpp"
#include "enum.hpp"
#include "random.hpp"

int main ( void ) {
	Screen::cls();
	Screen::locate(1,1);

	Serial *serial;

	for (long count = 1; count > 0 ; count += 1000000 ) {

		serial = new Serial();
		serial->init(count);

		Screen::write( serial->toFileName() + " == " + serial->toLong() + "\n\r" );

		delptr( serial );

		}


	}

#endif // SERIAL__HARNESS

#endif // SERIAL_CPP
