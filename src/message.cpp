// [message.cpp]
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
#if !defined ( MESSAGE_CPP )
#define MESSAGE_CPP
#include "message.hpp"
#include "grammar.hpp"
#include "enum.hpp"
#include "menu.hpp"
#include "string.hpp"
#include "input.hpp"
#include "global.hpp"
#include "dialog.hpp"
#include "visuals.hpp"

#include "compiler.hpp"
#ifdef OBSOLETE_INCLUDE
#  include <ctype.h>
#else
#  include <cctype>
#endif

char Message::buffersize = BUFSIZE;

List<String> messages; // Messages that haven't been printed yet.
List<String> buffer; // Old messages.

void Message::add( const String &s, bool once_only ) {
// Adds one new message.
	using namespace std;

	String str = s;

  // Capitalise

	for (int index = 0; index < str.length(); index++)
		if ( isalnum(str[index]) ) {
			str[index] = toupper( str[index] );
			break;
			}

  // Add to recent messages list and (if important) buffer:
	::messages.add( str );
  if (!once_only)
    ::buffer.add( str );
    
	Grammar::newsentence();
	}

void Message::add( const char *ch, bool once_only ) {
// Adds one new message.
	add( String(ch), once_only );
	}

void Message::clrscr( void ) {
// clrs the message pane.
	for (int i=NUMMESSAGELINES; i >= 0; --i) {
		Screen::locate( 1, MESSAGELINE+i);
		Screen::clearline();
		}
	}


void Message::more( void ) {

	Screen::Buffer m_buffer;
	String msg = "--more--";

	Rectangle rect(Screen::maxx-msg.length(), NUMMESSAGELINES+MESSAGELINE+1,Screen::maxx,NUMMESSAGELINES+MESSAGELINE+1);

	m_buffer.setSize( rect );

	Screen::locate(Screen::maxx-msg.length(), NUMMESSAGELINES+MESSAGELINE+1);
	Screen::write(msg);

	Command key;
	do {
		key = Keyboard::getCommand();
		} while ( key != kSpace );

	}

void Message::print( void ) {
// Prints new message list then flushes messages.
	if ( messages.numData() == 0 ) {
		Grammar::clr();
		clrscr();
		}
	else {
		clrscr();

		String retStr;

		if (messages.reset())
		do {
			if ( (retStr != "") || Screen::xloc() == 1) {
				if (Screen::xloc() > 1)
					Screen::locate(1, Screen::yloc() + 1);
				if (Screen::yloc() > NUMMESSAGELINES + MESSAGELINE) {

					more();

					clrscr();
					}
				}
			if (retStr == "") {
				retStr = messages.remove();
				//buffer.add(retStr);
				}
			retStr = Screen::writef( retStr );
			if ( (retStr == "") && (Screen::xloc() < Screen::width-3) )
        Screen::write("  ");
      else if (retStr == "")
        Screen::locate(1, Screen::yloc() + 1);
        
			} while ( messages.reset() || (retStr != "") );
		flush();
		}
# ifdef RARE_PRONOUN
  	Grammar::clr();
# endif // RARE_PRONOUN
	}

void Message::flush( void ) {
// Moves new messages to old list & deletes oldest messages.
// Now merely deletes oldest and clears recent:
	if (messages.reset())
	do
		//buffer.add(
      messages.remove()
    //  )
      ;
		while ( messages.reset() );

	buffer.reset();
	while ( buffer.numData() > buffersize )
		buffer.remove();
	}

void Message::clr( void ) {
// Erases _all_ messages.
	messages.clr();
	buffer.clr();
	}


void Message::recent( void ) {
	if (buffer.numData() == 0) {
		add("No messages to display.");
		return;
		}
	List<MenuString> theList;

	buffer.reset();

	do
		theList.add( MenuString( buffer.get() ) );
		while ( buffer.next() );

	Menu<MenuString> theMenu(theList, global::skin.message);

	theMenu.setTitle("Messages");
	theMenu.noLetters();
	theMenu.toTail();
	theMenu.setLoop(false);

	theMenu.display();

	Command key = kInvalid;
	while (key != kEscape) {
		key = theMenu.getCommand();

		theMenu.display();
		}

	}


#endif // MESSAGE_CPP
