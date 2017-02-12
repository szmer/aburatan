// [dialog.cpp]
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
#if !defined ( DIALOG_CPP )
#define DIALOG_CPP

#include "dialog.hpp"
#include "input.hpp"

#include <values.h>

#include "compiler.hpp"
#ifdef OBSOLETE_INCLUDE
#  include <ctype.h>
#else
#  include <cctype>
#endif

Dialog::Dialog( const String &t, const String &c, menuSkin ms )
:
  Title(t),
	content(c),
	origin( Screen::getFocusPt() ),
	skin(ms),
	dorefresh(true) {
	}

Dialog::~Dialog( void ) {
	buffer.toScreen();
	}

void Dialog::calcWin( void ) {

	int widestText = max( Title.length() + 2, content.length() );

	staticWin.set (
		origin.getx() - widestText / 2 - 2,
		origin.gety() - 1,

		origin.getx() + widestText / 2 + 1 + widestText % 2,
		origin.gety() + 1

		);

	while (staticWin.getxa() < 1) {
		staticWin.getxa()++;	staticWin.getxb()++;
		}
	while (staticWin.getxb() > Screen::width) {
		staticWin.getxa()--;	staticWin.getxb()--;
		}
	while (staticWin.getya() < 1) {
		staticWin.getya()++;	staticWin.getyb()++;
		}
	while (staticWin.getyb() > Screen::maxy) {
		staticWin.getya()--;	staticWin.getyb()--;
		}

	staticWin.setintersect ( Rectangle(1,1,80,25) );

	}


void Dialog::display( void ) {

	if (dorefresh) {

		calcWin();

		buffer.setSize( staticWin );
		buffer.toBuffer();
		Screen::drawwin(staticWin, Title, skin);

		// Now refreshed:
		dorefresh = false;
		}

	Screen::locate( staticWin.getxa() + 1, staticWin.getya() + 1 );
  Screen::focusCursor();

	Screen::write( ' ' );
	Screen::write( content, skin::Arr[skin].cursor );
	Screen::write( ' ' );

	}


int Dialog::getKey( void ) {
	return Keyboard::get();
	}

void Dialog::setSkin( menuSkin s ) {
	if (skin != s)
		dorefresh = true;
	skin = s;
	}

void Dialog::setOrigin( const Point &p ) {
	if (origin != p)
		dorefresh = true;
	origin = p;
	}

void Dialog::setTitle( const String &t ) {
	if ( Title != t )
		dorefresh = true;
	Title = t;
	}

void Dialog::setContent( const String &c ) {
	if ( content.length() != c.length() )
		dorefresh = true;
	content = c;
	}

void Dialog::hide( void ) {
	buffer.toScreen();
	}


//-[ StringDialog ]------//

StringDialog::StringDialog( const String &t, const String &c, int len, alnum al_n, menuSkin ms )
: Dialog(t, c, ms),
	align(left),
	inputlen(len),
	inputtype(al_n),
	initialInput(true) {
	}

void StringDialog::setAlign( Hand al ) {
	if (align != al)
		dorefresh = true;
	align = al;
	}

void StringDialog::display( void ) {

	if (dorefresh) {

		calcWin();

		buffer.setSize( staticWin );
		buffer.toBuffer();
		Screen::drawwin(staticWin, Title, skin);

		// Now refreshed:
		dorefresh = false;
		}

	Screen::locate( staticWin.getxa() + 1, staticWin.getya() + 1 );

	Screen::write( ' ' );
	Screen::write( content, skin::Arr[skin].option );
	Screen::write( ' ' );

	String underscore;
	underscore.fillnchar( '_', inputlen - input.length() );
	if (align == right) Screen::write( underscore, skin::Arr[skin].option );

	Screen::write( input, (initialInput)? skin::Arr[skin].option : skin::Arr[skin].cursor );
	Point cursor = Screen::cursorat();
	if (align == right) cursor.decx();

	if (align == left) Screen::write( underscore, skin::Arr[skin].option );

	Screen::write( ' ' );


	Screen::locate( cursor );
  Screen::focusCursor();

	}

String StringDialog::readString( bool clearinput = true ) {
    using namespace std;

	if (clearinput) input = "";

	display();

	int key = 0;
	Command commandkey = kInvalid;

	do {
		key = getKey();
		commandkey = Keyboard::toCommand(key);

#   ifdef USE_CURSES
    if (key < KEY_MIN) {
#   endif // USE_CURSES
    
    if (key == '%') {
			if (initialInput) { initialInput = false; input = ""; }
      input += "/";
      key = 0;
      }
    
		if (  (inputtype == alphanumeric) || (inputtype == alpha) ) {
			if (isalpha(key)) {
				if (initialInput) { initialInput = false; input = ""; }
				input += (char*)&key;
				}
			if (ispunct(key)) {
				if (initialInput) { initialInput = false; input = ""; }
				input += (char*)&key;
				}
			if ( commandkey==kSpace )	{
				if (initialInput) { initialInput = false; input = ""; }
				input += " ";
				}
			}
		if (  (inputtype == alphanumeric) || (inputtype == numeric) ) {
			if (isdigit(key)) {
				if (initialInput) { initialInput = false; input = ""; }
				input += (char*)&key;
				}
			}

#   ifdef USE_CURSES
    }
#   endif // USE_CURSES

		if ( commandkey==kBackspace ) {
			initialInput = false;
			input.trunc( input.length() - 1 );
			}

		input.trunc( inputlen );

		display();

		}	while ( commandkey != kEnter );

	return input;

	}

void StringDialog::setInput( const String &str ) {
	input = str;
	input.trunc( inputlen );
	}


void StringDialog::calcWin( void ) {

	int widestText = max( Title.length() + 2, content.length() + 1 + inputlen );

	staticWin.set (
		origin.getx() - widestText / 2 - 2,
		origin.gety() - 1,

		origin.getx() + widestText / 2 + 1 + widestText % 2,
		origin.gety() + 1

		);

	while (staticWin.getxa() < 1) {
		staticWin.getxa()++;	staticWin.getxb()++;
		}
	while (staticWin.getxb() > Screen::width) {
		staticWin.getxa()--;	staticWin.getxb()--;
		}
	while (staticWin.getya() < 1) {
		staticWin.getya()++;	staticWin.getyb()++;
		}
	while (staticWin.getyb() > Screen::maxy) {
		staticWin.getya()--;	staticWin.getyb()--;
		}

	staticWin.setintersect ( Rectangle(1,1,80,25) );

	}

void StringDialog::setnumeric( void ) {
	inputtype = numeric;
	}
void StringDialog::setalpha( void ) {
	inputtype = alpha;
	}
void StringDialog::setalphanumeric( void ) {
	inputtype = alphanumeric;
	}


////////////////////////////
#ifdef DIALOG__HARNESS


#include "menu.hpp"
#include "string.hpp"
#include "point.hpp"
#include "rectngle.hpp"
#include "visuals.hpp"
#include "input.hpp"
#include "counted.hpp"
#include "enum.hpp"
#include "standard.hpp"

int main () {

	////////////////////

	Dialog dialog( "Hello", "This is a Dialog", Point(40,10) );

	dialog.setSkin( sWater );
	dialog.display();

	dialog.getKey();


	////////////////////

	StringDialog stringdialog( "New Character", "Name?", Point(40,13), 12 );

	stringdialog.setInput("Michael");

	stringdialog.readString(false);


	////////////////////

	StringDialog numericdialog( "Drop Items", "Quantity? [0..40]", Point(40,16), 2, numeric );

	numericdialog.setSkin( sFire );
	numericdialog.setAlign( right );
	numericdialog.setInput("40");

	String number;
	do {
		number = numericdialog.readString(false);
		} while (  !ascending( -1, number.toInt(), 41 )  );


	return 0;

	}

#endif // DIALOG__HARNESS
////////////////////////////

#endif // DIALOG_CPP

