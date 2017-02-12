// [input.cpp]
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
#if !defined ( INPUT_CPP )
#define INPUT_CPP

#include "file.hpp"
#include "compiler.hpp"
#include "input.hpp"
#include "visuals.hpp"
#include "types.hpp"
#include "dialog.hpp"
#include "global.hpp"
#include <values.h>

#ifdef OBSOLETE_INCLUDE
#  include <stdio.h>
#else
#  include <cstdio>
#endif

#ifdef DOS_CONIO
#  ifdef OBSOLETE_INCLUDE
#    include <conio.h>
#  else
#    include <conio>
#  endif
#endif

int Keyboard::hit( void ) {
//  Returns true if a key has been hit.
# if defined( DOS_CONIO )
  return kbhit();
# elif defined( USE_CURSES )
  return halfdelay(1);
# else
#   error You must define Keyboard::hit (input.cpp) for your OS
# endif
	}

char hex_arr[17] = "0123456789ABCDEF";

String hex_str( unsigned char uchar ) {
  String ret_str;

  ret_str += hex_arr[uchar/16];
  ret_str += hex_arr[uchar%16];
  
  return ret_str;
  }

int Keyboard::get( void ) {
//  Reads and returns a key from stdin.
# if defined( DOS_CONIO )
	int key = getch();
	if (key == '\0') key = (getch() << BITSPERBYTE) | '^'; // Extended keys in DOS
																						 // (such as cursor keys)
																						 // are of the type '^M'
	return key;
# elif defined( USE_CURSES )
  int key = getch();

# ifdef OS_DOS
  Point cursor = Screen::cursorat();
  while ( key == KEY_F(7) || key == KEY_F(8) ) {

    Screen::hidecursor();

    bool HTML = ( key == KEY_F(8) );
    // Save Screenshot
    int x, y, currColor = -1;
    String appearance;
    Image image;
    File file;
    // Find a filename
    int i = 0;
    String filename;
    do {
      filename = "";
      filename += (long)i;
      if (HTML)
        filename += ".html";
      else
        filename += ".txt";
      i++;
      } while (File::exists(filename,SHOTDIR));

 	  std::fstream &myFile = file[SHOTDIR+filename];
    myFile.flush();

    if (HTML)
      myFile << "<html><head><title>Abura Tan Screenshot</title></head>"
             << "<body style=\"background-color:#E6E6E6;\">"
             << "<font size=\"+2\" color=\"black\">Abura Tan</font>"
             << "<table><tr><td style=\"background-color:'black'\">"
             << "<tt>";

    int *width = new int [Screen::height];

    for (y = 1; y <= Screen::height; ++y) {

      width[y] = 0;
    
      for (x = 1; x <= Screen::width; ++x) {

        Image image = Screen::get(x,y);
        if (image.val.color != cBlack
         && image.val.appearance != ' '
         && image.val.appearance != '\0')
          width[y] = x;

        }
      }

    if (HTML)
      width[Screen::height] = Screen::width;

    for (y = 1; y <= Screen::height; ++y) {

      myFile << std::endl;
      
      for (x = 1; x <= width[y]; ++x) {

        // Get appearance:
        image = Screen::get(x,y);
        appearance = "";
        appearance += (char) image.val.appearance;
        if (HTML) {
          if (image.val.color == cBlack || appearance == " " || appearance == "\0") appearance = "&nbsp;";
          if (appearance == "<") appearance = "&lt;";
          if (appearance == ">") appearance = "&gt;";
          }

        // Get color:
        if ( HTML && image.val.color != currColor && image.val.color != cBlack) {

          if (currColor != -1) myFile << "</font>";

          myFile << "<font color=\"";

#         ifdef COLOR_REDEF
          String color_string = "#";

          RGBType rgb = colorTable[image.val.color];

          color_string += hex_str(rgb.r);
          color_string += hex_str(rgb.g);
          color_string += hex_str(rgb.b);
          
          myFile << color_string.toChar();
#         else
          switch (image.val.color) {

            case cBlue:     myFile << "#0000AA"; break;
            case cRed:      myFile << "#AA0000"; break;
            case cCyan:     myFile << "#00AAAA"; break;
            case cGreen:    myFile << "#00AA00"; break;
            case cPurple:   myFile << "#AA00AA"; break;
            case cUmber:    myFile << "#AAAA00"; break;
            case cSlate:    myFile << "#AAAAAA"; break;
            case cLBlack:   myFile << "#555555"; break;
            case cLBlue:    myFile << "#5555FF"; break;
            case cLRed:     myFile << "#FF5555"; break;
            case cLCyan:    myFile << "#55FFFF"; break;
            case cLGreen:   myFile << "#55FF55"; break;
            case cViolet:   myFile << "#FF55FF"; break;
            case cLUmber:   myFile << "#FFFF55"; break;
            default   :     myFile << "white"; break;

            }
#         endif // COLOR_REDEF
          myFile << "\">";
          }

        currColor = image.val.color;

        // Write to file:
        myFile << appearance.toChar();
        }

      if (HTML)
        myFile << "<br />";
      }

    delarr(width);
    if (currColor != -1) myFile << "</font>";
    if (HTML)
      myFile << "</tt></td></tr></table></body>";
    
    Screen::showcursor();

    String title;
    if (HTML) title =      "HTML Screen Dump "+filename;
    else title =           "Text Screen Dump "+filename;
    Dialog anyKey ( title, "Press any key to continue", global::skin.main );
    anyKey.display();
    anyKey.getKey();
    anyKey.hide();

    Screen::locate(cursor);
    key = getch();

    }
# endif
  
# ifdef PADENTER
    if (key == PADENTER) key = AT_KEY_ENTER;
# endif // PADENTER
  return key;
# else
#   error You must define Keyboard::get (input.cpp) for your OS
# endif
	}

Command Keyboard::toCommand( int key ) {
// Translate a keyboard input to a command.

# ifdef KEYDEBUG
	gotoxy(1,25); printf("%c%c", key % 256, key >> BITSPERBYTE);
# endif // KEYDEBUG


	for (int count = 0; count < kNum; ++count) {
		if (keyMap[count].dkey != '\0') {
			if (keyMap[count].dkey == key)
				return keyMap[count].command;
			}
		else if (keyMap[count].key == key)
				return keyMap[count].command;

		}

# ifdef KEYDEBUG
	printf("%c%i\n", key, key);
# endif // KEYDEBUG
	return kInvalid;
	}


Command Keyboard::getCommand( void ) {
// Return a command from the key/command list.
	return toCommand( get() );
	}

Command Keyboard::getDirection( void ) {
// Returns when input is any of the following:
//  - a cursor key
//  - escape
//  - a down/up key (by default '>' and '<')
//  - the 'self' or 'wait' keys (by default '.' and '5')

	Command key;

	do {

		key = getCommand();
		switch( key ) {

		//  - a cursor key
			case kSouthWest:
			case kSouth:
			case kSouthEast:
			case kWest:
			case kEast:
			case kNorthWest:
			case kNorth:
			case kNorthEast:
			case kCursorUp:
			case kCursorDown:
			case kCursorLeft:
			case kCursorRight:
			case kPageUp:
			case kPageDown:
			case kHome:
			case kEnd:

		//  - escape
			case kEscape:

		//  - a down/up key (by default '>' and '<')
			case kAscend:
			case kDescend:

		//  - the 'self' or 'wait' keys (by default '.' and '5')
			case kWait:
			case kSelf: return key;
                        default: break;
			}

		} while ( true );

	}

Dir Keyboard::toDir( Command c ) {

  Dir dir;
	switch( c ) {

		case kSouthWest:	case kEnd:
      dir.sw = true;
      break;

		case kSouth:		  case kCursorDown:
      dir.s = true;
      break;

		case kSouthEast:	case kPageDown:
      dir.se = true;
      break;

		case kWest:     	case kCursorLeft:
      dir.w = true;
      break;

		case kEast:     	case kCursorRight:
      dir.e = true;
      break;

		case kNorthWest:	case kHome:
      dir.nw = true;
      break;

		case kNorth:    	case kCursorUp:
      dir.n = true;
      break;

		case kNorthEast:  case kPageUp:
      dir.ne = true;
      break;

	//  - a down/up key (by default '>' and '<')
		case kAscend:
		case kDescend:
      dir.ground = true;
      break;

	//  - the 'self' or 'wait' keys (by default '.' and '5')
		case kWait:
		case kSelf:
      dir.self = true;
      break;

		default:
      // Silly key passed
      assert(0);
      break;
		}

  return dir;
  }
  

void Keyboard::init( void ) {
# ifdef USE_CURSES
  keypad(stdscr,TRUE);
  noecho();
# endif
	}

String Keyboard::keytostr( int i ) {

  if (i == AT_KEY_TAB)
    return String("Tab");
  if (i == AT_KEY_ESCAPE)
    return String("Esc");
  if (i == AT_KEY_ENTER)
    return String("Enter");
  if (i == AT_KEY_BACKSPACE)
    return String("Backspace");
    
	String ret = (char*)(&i);
	return ret;

	}


String Keyboard::keyname( int i ) {
  String ret = keyMap[toCommand(i)].name;
  return ret;
  }

String Keyboard::keydesc( int i ) {
  return String(keyMap[toCommand(i)].desc);
  }


#endif // INPUT_CPP
