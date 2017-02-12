// [visuals.cpp]
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
#if !defined ( VISUALS_CPP )
#define VISUALS_CPP

#include "compiler.hpp"
#include "visuals.hpp"
#include "string.h"
#include "enum.hpp"
#include "stdio.h"
#include "stdlib.h"
#include "list.hpp"
#include "random.hpp"

#ifdef OBSOLETE_INCLUDE
#  include <ctype.h>
#else
#  include <cctype>
#  include <ctime>
#endif

#ifdef DOS_CONIO
#  include "conio.h" // includes some io used by DOS
#  include "dos.h" // ditto
#endif


#ifdef USE_CURSES
#  include <curses.h>
#endif


//#define _VISUALS_HARNESS

int Screen::size = 2000;

int Screen::width = 80;
int Screen::height = 25;

int Screen::maxx = 79;
int Screen::maxy = 24;

int Screen::mapxoff = 0;
int Screen::mapxmin = mapxoff + 1;
int Screen::mapxmax = mapxoff + map_width;

int Screen::mapyoff = 2;
int Screen::mapymin = mapyoff + 1;
int Screen::mapymax = mapyoff + map_height;

int Screen::levxoff = 0;
int Screen::levyoff = 0;
int Screen::cursor_off = 0;

Point Screen::focus_pt(Screen::width / 2 , Screen::height / 2);

Rectangle Screen::menu_limit(1,1,Screen::width,Screen::height);
Rectangle Screen::image_limit(1,1,Screen::width,Screen::height);
Rectangle Screen::format_limit(1,1,Screen::width,Screen::height);

void Screen::cls( void ) {
//  Clears the text screen.
# ifdef OS_DOS
#  ifdef INLINE_ASSEMBLY
		asm {
			Mov  Ax, 0xB800
			Mov  Es, Ax
			Xor  Di, Di
			Mov  Al, 0x20
			Mov  Ah, 0x07
			Mov  Cx, 2000
			Rep  Stosw
			}
#  elif defined( DOS_CONIO )
		clrscr();
#  elif defined( USE_CURSES )
	clear();
  update();
#  else
#    error You need to define Screen::cls() for your OS
#  endif // INLINE_ASSEMBLY
#  elif defined( USE_CURSES )
	clear();
  update();
# else
#   error You need to define Screen::cls() for your OS
# endif // OS_DOS
	}
void Screen::clearline( void ) {
// Clears the rest of the screen row
# ifdef DOS_CONIO
	clreol();
#  elif defined( USE_CURSES )
	clrtoeol();
  update();
# else
#   error You need to define Screen::clearline() for your OS
# endif
	}
void Screen::put( const int x, const int y, const Image i ) {
//  Puts a text image to the text screen.
	#if ( defined( OS_DOS ) && defined( INLINE_ASSEMBLY ) )
		asm {
			Mov  Ax, 0xB800
			Mov  Es, Ax
			Xor  Di, Di
			Mov  Ax, y
			Xor  Ah, Ah
			Sub  Al, 1
			Mov  Di, Ax
			Shl  Di, 5
			Shl  Ax, 7
			Add  Di, Ax
			Mov  Ax, x
			Xor  Ah, Ah
			Sub  Al, 1
			Shl  Ax, 1
			Add  Di, Ax
			Mov  Ax, i
			Stosw
			}
	#elif defined( USE_CURSES )
   // Only print to screen when within the screen borders:
   if (image_limit.contains( Point( x, y ) )) {
  	 move(y-1,x-1);
  	 setcolor((Color)i.val.color);
     // Make sure that curses doesn't print the image
     // as some queer escape sequence:
  	 addch( (i.val.appearance >= 32)
              ? i.val.appearance
              : ' ' );
     // The above code would suit being put into
     // the Image union.  Todo: as a possible speed increase
     update();
     }
	#else
	 locate(x, y);
	 write( (int) (i.i & 0x00FF ), (char) i.i >> BITSPERBYTE );
	#endif
	}
void Screen::update( void ) {
# if defined( USE_CURSES )
  if (!cursor_off)
    refresh();
# endif
  }
  
Image Screen::get( const int x, const int y ) {
//  Gets a text image from the text screen.
# ifdef DOS_CONIO
	int *p = (int*) MK_FP(0xB800,(x-1)*2+(y-1)*160);
	return *p;
#  elif defined( USE_CURSES )
	locate(x,y);
	chtype ch = inch();
	return Image(PAIR_NUMBER(ch)-1,ch);
# else
#   error You need to define Screen::get (visuals unit) for your OS/compiler
# endif
	}

void Screen::locate( int x, int y ) {
//  Moves the cursor to specified area.
# ifdef DOS_CONIO
	gotoxy( x, y );
#  elif defined( USE_CURSES )
  move(y-1,x-1);
  update();
# else
#   error You need to define Screen::locate() for your OS
# endif
	}

void Screen::hidecursor( void ) {
# ifdef DOS_CONIO
	_setcursortype(_NOCURSOR);
#  elif defined( USE_CURSES )
  cursor_off ++;
# else
#   error You need to define Screen::hidecursor() for your OS
# endif
	}
void Screen::showcursor( void ) {
# ifdef DOS_CONIO
	_setcursortype(_NORMALCURSOR);
#  elif defined( USE_CURSES )
  cursor_off --;
  // Guard against too many calls to showcursor:
  assert (cursor_off >= 0);
# else
#   error You need to define Screen::showcursor() for your OS
# endif
	}


Screen::Buffer::~Buffer() {
	if (buffer)
		moveToScreen();
	}

void Screen::Buffer::moveToScreen( void ) {
	toScreen();
	free(buffer);
	buffer = NULL;
	}

void Screen::Buffer::setSize( const Rectangle &r ) {

	if (bufferRect != r) {
		if (buffer)
			moveToScreen();

		bufferRect = r;
		}

	if (!buffer) toBuffer();
	}

void Screen::Buffer::shiftData( const Point &p ) {
// Shifts the contents of the buffer in direction p
  size_t struct_size = sizeof(int);
  int width  = bufferRect.getwidth();
  int height = bufferRect.getheight();
  ShiftBuffer( buffer, width, height, struct_size, p);
  }

void Screen::Buffer::ShiftBuffer( void *buffer, int width, int height, size_t struct_size, Point shift ) {
// Shifts the contents of any buffer in direction p

  int x, y, y_inc,
      x_shift = shift.getx(),
      y_shift = shift.gety(),
      min_x = 0, copy_width = width,
      min_y = 0, max_y = height-1;

  if ( y_shift > 0 ) {
    max_y -= y_shift;
    // If we're shifting down, we shift rows from bottom to top:
    y_inc = -1;

    // kludge, instead of swap() from standard.hpp that gets confused with functions from standard lib 
    // Sz. Rutkowski 11.02.2017
    int temp = max_y;
    max_y = min_y;
    min_y = temp;
    }
  else {
    min_y -= y_shift;
    y_inc = 1;
    }

  if ( x_shift < 0 )
    min_x -= x_shift;

  copy_width -= abs(x_shift);



  for (y = min_y; y != max_y+y_inc; y += y_inc ) {

    // Copy the data:
    memmove( (void*)( (long)buffer + ( min_x + x_shift + (y+y_shift) * width ) * struct_size),
             (void*)( (long)buffer + ( min_x + y * width ) * struct_size),
             copy_width * struct_size );
    // Zero the section that didn't just receive data:
    if (x_shift < 0)
      x = copy_width;
    else
      x = 0;

    memset ( (void*)( (long)buffer + ( x + y * width ) * struct_size), 0,
             ( abs(x_shift) ) * struct_size );

    }

  // If we're shifting down, swap the min/max values before we finish:
  if (y_inc == -1) {
    // kludge, instead of swap() from standard.hpp that gets confused with functions from standard lib 
    // Sz. Rutkowski 11.02.2017
    int temp = max_y;
    max_y = min_y;
    min_y = temp;
  }

  // Then zero the horizontal band (if any) above/below the moved section:
  if (y_shift < 0)
    y = (max_y+y_shift+1) * width * struct_size;
  else
    y = 0;
    
  memset ( (void*)( (long)buffer + y ), 0,
           ( abs(y_shift) * width ) * struct_size );

  }

const Rectangle &Screen::Buffer::getSize( void ) const {
	return bufferRect;
	}


int Screen::Buffer::toBuffer() {
	if (!buffer) buffer = (void *) malloc(bufferRect.getheight() * bufferRect.getwidth() * sizeof(int) );
	assert (buffer);
# ifdef DOS_CONIO
	return gettext(bufferRect.geta().getx(), bufferRect.geta().gety(),
		bufferRect.getb().getx(), bufferRect.getb().gety(), buffer);
# elif defined( USE_CURSES )
  hidecursor();
  
  int x, y,
      ax=bufferRect.geta().getx(),
      bx=bufferRect.getb().getx(),
      ay=bufferRect.geta().gety(),
      by=bufferRect.getb().gety(),
      width = bufferRect.getwidth();
  for (y = ay; y <= by; ++y)
    for (x = ax; x <= bx; ++x)
			((int*)buffer)[x-ax + (y-ay)*width] = get(x,y).i;

  showcursor();
  
	return 1;
    
# else
#   error You need to define Screen::Buffer::toBuffer() for your OS
# endif
	}

int Screen::Buffer::toScreen() {
  if (buffer) {
# ifdef DOS_CONIO
  	return puttext(bufferRect.getxa(), bufferRect.getya(),
  		bufferRect.getxb(), bufferRect.getyb(), buffer);
#  elif defined( USE_CURSES )
    hidecursor();
    
    int x, y,
        ax=bufferRect.geta().getx(),
        bx=bufferRect.getb().getx(),
        ay=bufferRect.geta().gety(),
        by=bufferRect.getb().gety(),
        width = bufferRect.getwidth();
    for (y = ay; y <= by; ++y)
      for (x = ax; x <= bx; ++x)
  			put(x, y, ((int*)buffer)[x-ax + (y-ay)*width] );

    showcursor();
    
  	return 1;
# else
#   error You need to define Screen::Buffer::toScreen() for your OS
# endif
    }
  else return 0;
  }


Point Screen::cursorat( void ) {
// Point version of wherex,y
	return Point(xloc(),yloc());
	}

void Screen::locate( const Point &p ) {
	locate(p.getx(),p.gety());
	}

int Screen::fixcolor( int c ) {
// Assign color if of random type
// Called for level elements so they don't `shimmer'.
  int col = c;
  if (c > cNorm) {
    switch(c) {
      case cFire   :
        switch (Random::randint(3)) {
          case 1:  col = cRed; break;
          case 2:  col = cOrange; break;
          default: col = cYellow; break;
          }
        break;
      case cIce    :
        switch (Random::randint(3)) {
          case 1:  col = cWhite; break;
          case 2:  col = cLGrey; break;
          default: col = cLBlue; break;
          }
        break;
      case cWater  :
        switch (Random::randbool()) {
          case true: col = cBlue; break;
          default:   col = cLBlue; break;
          }
        break;
      case cHaze   :
        switch (Random::randbool()) {
          case true: col = cDGrey; break;
          default:   col = cGrey; break;
          }
        break;
      case cRainbow: default:
        switch (Random::randint(7)) {
          case 1:  col = cRed; break;
          case 2:  col = cOrange; break;
          case 3:  col = cYellow; break;
          case 4:  col = cGreen; break;
          case 5:  col = cBlue; break;
          case 6:  col = cIndigo; break;
          default: col = cViolet; break;
          }
      }
    }
  return col;
  }

#define TO_CURSES_STYLE(c) COLOR_PAIR(c+1)|(c<=cDIM?A_DIM:A_BOLD)
void Screen::setcolor( int c ) {
//  Sets the text screen color attributes.
# ifdef USE_COLOR
#   ifdef DOS_CONIO
		textattr(c);
#  elif defined( USE_CURSES )
  int col = fixcolor(c);
	attrset( TO_CURSES_STYLE(col) );
#   else
#     error You need to define Screen::setcolor() for your OS
#   endif
# endif // USE_COLOR

	}

int Screen::xloc( void ) {
// Cursor x location
# if defined( DOS_CONIO )
	return wherex();
# elif defined( USE_CURSES )
  int x, y;
  getyx(stdscr, y, x);
  return x+1;
# else
#   error You need to define Screen::xloc() for your OS
# endif
	}
int Screen::yloc( void ) {
// Cursor y location
# if defined( DOS_CONIO )
	return wherey();
#  elif defined( USE_CURSES )
  int x, y;
  getyx(stdscr, y, x);
  return y+1;
# else
#   error You need to define Screen::yloc() for your OS
# endif
	}
int Screen::linespace( void ) {
// Number of intacters until eol
	return width-xloc();
	}

void Screen::writeat( int x, int y, const char* text, int c ) {
//  Writes a text string to a screen location in a color.
	locate(x, y);
	write(text, c);
	}

void Screen::write( const char* text, int c ) {
//  Writes a text string to screen.
	setcolor(c);
	#if defined( DOS_CONIO )
	# ifdef USE_COLOR
		cprintf(text);  // Color output for DOS screens.
	# else
		printf(text);
	# endif // USE_COLOR
	#elif defined( USE_CURSES )
		printw((char*)text);
    update();
	#else
	printf(text);
	#endif
	}
void Screen::write( const String &text, int c ) {
	write( text.toChar(), c );
	}

String Screen::writef( const String &strParam, int c ) {
//
// Writes a formatted string, using the following format chars:
//
// \    - Display the next character (next char is *not* a format char)
// <x   - Begin new color section where x is one of:
//        + - Good message
//        - - Bad message
//        ? - Help Message
//        ! - Warning
// <xx  - Begin new color section where xx is a number
//        from 00 to 15, representing those defined colors.
// >    - End current color section.
//
//
// Color sections can be nested, as in Plain<01Blue<04Red>Blue>Plain
//

	using namespace std;

  hidecursor();

	String str = strParam + ' ';

	String buffer;
	List<int> *colors = NULL;
	int currCol = c?c:cGrey;
	bool eol = false;
	char chr;
	int index;
  bool force = false;

  int max_x = format_limit.getxb();

	for ( index = 0; index < str.length() && !eol; index++ ) {

		chr = str[index];

    // A character preceeded by a backslash should be forced
    // to be displayed exactly, so no check to see if it is a format char.
    if ( !force && chr == '\\' )
      force = true;
    
		else if ( !force && chr == '<' ) {

			if (!colors) colors = new List<int>;

			colors->add( currCol );

			if (buffer.length() + xloc() <= max_x) {
				write(buffer, c?c:currCol);
				buffer = "";
				if (xloc() == max_x) eol = true;
				}
			else {
				eol = true;
				break;
				}

			int get = str[index+1];
			switch (get) {

				case '+' : currCol = cGreen; break; // + is green:'good'
				case '-' : currCol = cLRed; break;  // - is red:  'bad'
 
        case 'r' : currCol = cRed; break;
        case 'R' : currCol = cLRed; break;
        case 'o' : currCol = cOrange; break;
        case 'y' : currCol = cBrown; break; // 'y' because it is dark yellow...?
        case 'Y' : currCol = cYellow; break;
        case 'g' : currCol = cGreen; break;
        case 'G' : currCol = cLGreen; break;
        case 'b' : currCol = cBlue; break;
        case 'B' : currCol = cLBlue; break;
        case 'i' : currCol = cIndigo; break;
        case 'v' : currCol = cViolet; break;

				case '?' : currCol = cYellow; break; // Help messages
				case '!' : currCol = cRed; break;    // red - warning

        case '_' : currCol = cDGrey; break; // Bottom grey
        case '~' : currCol = cLGrey; break; // Higher grey
        case '^' : currCol = cWhite; break; // Peak grey :)

				default:
					currCol = (get-'0')*10 + (str[index+2]-'0');
					index++;
					break;
				}

      force = false;
			index ++;

			}
		else if ( !force && chr == '>' ) {

			if (colors)	{
				colors->toTail();

				if (buffer.length() + xloc() <= max_x) {
					write(buffer, c?c:currCol);
					buffer = "";
					if (xloc() == max_x) eol = true;
					}
				else {
					colors->add( currCol );
					eol = true;
					break;
					}

				currCol = colors->remove();
				if (colors->numData() == 0) delptr( colors );
				}
			else {

				if (buffer.length() + xloc() <= max_x) {
					write(buffer, c?c:currCol);
					buffer = "";
					if (xloc() == max_x) eol = true;
					}
				else {
					eol = true;
					}

				currCol = c?c:cGrey;

				}

      force = false;
			}
		else {
			if (
				isspace(chr) || chr==0 ) {
				if (buffer.length() + xloc() <= max_x) {
					write(buffer, (c?c:currCol));
					buffer = "";
					if (xloc() == max_x) eol = true;
				  if (index < str.length()-1)
					  if (yloc() < max_x-1)
              write(' ');
					}
				else {
					eol = true;
					if (!colors) colors = new List<int>;
					colors->add( currCol );
					index --;
					}
				}
			else {
				if (index < str.length()-1)
				buffer += chr;
        // since all printf calls from here are without parameters,
        // all '%'s are meant for display.
        if (chr == '%')
          buffer += '%';
				}

      force = false;
			}

		}

	if (buffer.length() + xloc() <= max_x) {
		write(buffer, c?c:currCol);
		buffer = "";
		}

	for ( ; index < str.length(); index++ ) {
		buffer += str[index];
		}

	char chbuf[5];

  if (!colors) colors = new List<int>;
	colors->add( currCol );

  if (colors && colors->toTail() && buffer != "" )
	while (colors->numData() > 0) {
		sprintf( chbuf, "<%02i", colors->remove() );
		buffer = String(chbuf) + buffer;
		}

	delptr( colors );

  showcursor();

	return buffer;

	}

void Screen::writen( char ch, int n, int c ) {

  setcolor(c);

  assert (n < 80);

  for (int count = 0; count < n; count++) {
#   if defined( DOS_CONIO )
#     ifdef USE_COLOR
	cprintf("%c", ch);
#     else
	printf("%c", ch);
#   endif // USE_COLOR
#   elif defined( USE_CURSES )
      addch(ch);
#   else
      printf("%c", ch);
#   endif
      }
      
# if defined( USE_CURSES )
  update();
# endif //USE_CURSES
  }
void Screen::write( char ch, int c ) {
//  Writes a character to screen.
  setcolor(c);
# ifdef DOS_CONIO
#   ifdef USE_COLOR
    cprintf("%c", ch);
#   else
    printf("%c", ch);
#   endif // USE_COLOR
# elif defined( USE_CURSES )
  addch(ch);
  update();
# else
  printf("%c", ch);
# endif
  }
/*
void Screen::write( int i, int c ) {
//  Writes a integer to screen.
	setcolor(c);
	#ifdef DOS_CONIO
	# ifdef USE_COLOR
		cprintf("%i", i);
	# else
		printf("%i", i);
	# endif // USE_COLOR
	#else
	printf("%i", i);
	#endif
	}
void Screen::write( long l, int c ) {
//  Writes a long to screen.
	setcolor(c);
	#ifdef DOS_CONIO
	# ifdef USE_COLOR
		cprintf("%li", l);
	# else
		printf("%li", l);
	# endif // USE_COLOR
	#else
	printf("%li", l);
	#endif
	}
*/
void Screen::writeprocessed( const char* text, int c ) {
// Processes a text string before writing it to the screen to
// avoid end of line overflow.  Removes extraneous whitespace.
	const char*offset= text;
	char  word[80], *wordoff = word;
	while (true) {
		// If char is whitespace, print the word.
		if ( (*offset == ' ')||(*offset == '\0') ) {
			*wordoff = '\0'; // Add end of string char.

			if ((int) strlen(word) <= (int) linespace())	// If the word will fit,
				write(word, c);  // write the word.
			else {
				locate(1,yloc()+1); // else write the word
				write(word, c);       // on the next line.
				}

			// Move cursor if required:
			if ( ascending(1, xloc(), 80) ) locate(xloc()+1,yloc());

			wordoff = word;  // New word.
			if (*offset == '\0') break; // If end of text, finish.
			while (*offset == ' ') ++offset; // Next char.
			}
		// else add the char to the word.
		else {
			*wordoff = *offset;
			++wordoff;
			++offset; // Next char.
			}
		}
	}

void Screen::write( Image i ) {
	write( i.val.appearance, i.val.color );
	}

void Screen::drawrect( const Rectangle& r, menuSkin m, bool focus ) {
// Draws a fancy rectangle on the screen.
// Used by windows/menus.
	skin theSkin = skin::Arr[m];
	if (!focus) {
		if (theSkin.corner_11.val.color == cWhite) theSkin.corner_11.val.color = cGrey;
		if (theSkin.corner_12.val.color == cWhite) theSkin.corner_12.val.color = cGrey;
		if (theSkin.corner_21.val.color == cWhite) theSkin.corner_21.val.color = cGrey;
		if (theSkin.corner_22.val.color == cWhite) theSkin.corner_22.val.color = cGrey;
		if (theSkin.north.val.color == cWhite) theSkin.north.val.color = cGrey;
		if (theSkin.south.val.color == cWhite) theSkin.south.val.color = cGrey;
		if (theSkin.east.val.color == cWhite) theSkin.east.val.color = cGrey;
		if (theSkin.west.val.color == cWhite) theSkin.west.val.color = cGrey;
		if (theSkin.bracket_l.val.color == cWhite) theSkin.bracket_l.val.color = cGrey;
		if (theSkin.bracket_r.val.color == cWhite) theSkin.bracket_r.val.color = cGrey;
		if (theSkin.cursor_l.val.color == cWhite) theSkin.cursor_l.val.color = cGrey;
		if (theSkin.cursor_r.val.color == cWhite) theSkin.cursor_r.val.color = cGrey;

		if (theSkin.title == cWhite) theSkin.title = cGrey;
		if (theSkin.cursor == cWhite) theSkin.cursor = cGrey;
		if (theSkin.option == cWhite) theSkin.option = cGrey;
		}

	locate( r.geta() );
	// Top Left Corner:
	write(theSkin.corner_11);
	// Top Bar:
        writen(theSkin.north.val.appearance, r.getwidth()-2, theSkin.north.val.color);
/*	while (xloc() < r.getb().getx()) {
		write(theSkin.north);
		}
*/
	// Top Right Corner:
	write(theSkin.corner_12);

	// Left & Right Bars:
	int y = r.geta().gety() + 1;
	while (y < r.getb().gety()) {
		locate( r.geta().getx(), y );
		write(theSkin.west);

		locate( r.getb().getx(), y );
		write(theSkin.east);
		++y;
		}


	// Bottom Left Corner:
	locate( r.geta().getx(), r.getb().gety() );
	write(theSkin.corner_21);
	// Bottom Bar:
        writen(theSkin.south.val.appearance, r.getwidth()-2, theSkin.south.val.color);
/*	while (xloc() < r.getb().getx()) {
		write(theSkin.south);
		}
*/
	// Bottom Right Corner:
	write(theSkin.corner_22);

	}

void Screen::drawwin( const Rectangle& r, const String &str, menuSkin m, bool focus ) {
// Draws a rectangular window with a title
  String s = str;

	skin theSkin = skin::Arr[m];
	if (!focus) {
		if (theSkin.corner_11.val.color == cWhite) theSkin.corner_11.val.color = cGrey;
		if (theSkin.corner_12.val.color == cWhite) theSkin.corner_12.val.color = cGrey;
		if (theSkin.corner_21.val.color == cWhite) theSkin.corner_21.val.color = cGrey;
		if (theSkin.corner_22.val.color == cWhite) theSkin.corner_22.val.color = cGrey;
		if (theSkin.north.val.color == cWhite) theSkin.north.val.color = cGrey;
		if (theSkin.south.val.color == cWhite) theSkin.south.val.color = cGrey;
		if (theSkin.east.val.color == cWhite) theSkin.east.val.color = cGrey;
		if (theSkin.west.val.color == cWhite) theSkin.west.val.color = cGrey;
		if (theSkin.bracket_l.val.color == cWhite) theSkin.bracket_l.val.color = cGrey;
		if (theSkin.bracket_r.val.color == cWhite) theSkin.bracket_r.val.color = cGrey;
		if (theSkin.cursor_l.val.color == cWhite) theSkin.cursor_l.val.color = cGrey;
		if (theSkin.cursor_r.val.color == cWhite) theSkin.cursor_r.val.color = cGrey;

		if (theSkin.title == cWhite) theSkin.title = cGrey;
		if (theSkin.cursor == cWhite) theSkin.cursor = cGrey;
		if (theSkin.option == cWhite) theSkin.option = cGrey;
		}

	drawrect(r, m);

	int width = r.getwidth();

	if (s.length() > width-6) {
		s.trunc(width-8);
		s += "..";
		}
	// +-[Title]----+
	// +-[Long T..]-+
	// +-[Very L..]-+

	locate( r.getxa()+2, r.getya() );

	write(theSkin.bracket_l);
	write(s, theSkin.title);
	write(theSkin.bracket_r);

	}


char colorIndex[16] = {
	0, 1, 2, 3, 4, 5, 20, 7, 56, 57, 58, 59, 60, 61, 62, 63
	};

void SetPalRGB( unsigned char c, RGBType rgb ) {
# ifdef OS_DOS
	outportb(0x03c8, c);
	outportb(0x03c9, rgb.r >> 2);
	outportb(0x03c9, rgb.g >> 2);
	outportb(0x03c9, rgb.b >> 2);
# else
// Colors may look a little off for non-dos
# endif // MS_DOS
  }

void setColors( void ) {
	for (char b = 0; b < 16; b++) {
		SetPalRGB( colorIndex[b], colorTable [b] );
		}
	}
  
void Screen::init( void ) {
# ifdef USE_CURSES
  
	initscr();
  raw();

  if (has_colors()) {

    start_color();

    // Black on black only for possible future use:
    init_pair( cBlack +1, COLOR_BLACK | A_DIM,   COLOR_BLACK);
    
    init_pair( cRed   +1, COLOR_RED | A_DIM,     COLOR_BLACK);
    init_pair( cOrange+1, COLOR_CYAN | A_DIM,    COLOR_BLACK);
    init_pair( cYellow+1, COLOR_YELLOW | A_BOLD, COLOR_BLACK);
    init_pair( cGreen +1, COLOR_GREEN | A_DIM,   COLOR_BLACK);
    init_pair( cBlue  +1, COLOR_BLUE | A_DIM,    COLOR_BLACK);
    init_pair( cIndigo+1, COLOR_MAGENTA | A_DIM, COLOR_BLACK);
    init_pair( cViolet+1, COLOR_MAGENTA | A_BOLD,COLOR_BLACK);

    init_pair( cBrown +1, COLOR_YELLOW | A_DIM,  COLOR_BLACK);

    init_pair( cLRed  +1, COLOR_RED | A_BOLD,    COLOR_BLACK);
    init_pair( cLGreen+1, COLOR_GREEN | A_BOLD,  COLOR_BLACK);
    init_pair( cLBlue +1, COLOR_BLUE | A_BOLD,   COLOR_BLACK);

    init_pair( cDGrey +1, COLOR_BLACK | A_BOLD,  COLOR_BLACK);
    init_pair( cGrey  +1, COLOR_WHITE | A_DIM,   COLOR_BLACK);
    init_pair( cLGrey +1, COLOR_CYAN | A_BOLD,   COLOR_BLACK);
    init_pair( cWhite +1, COLOR_WHITE | A_BOLD,  COLOR_BLACK);

#   ifdef COLOR_REDEF
      setColors();
#   endif // COLOR_REDEF
    }
    
# endif
	cls();
	}

void Screen::end( void ) {
# ifdef USE_CURSES
	endwin();
# endif
	}



void Screen::mSleep( unsigned time )  {
	clock_t limit = time + clock();
	while ( limit > clock()) {}
  }

void Screen::nextFrame( void ) {
// Pause to make sure fast computers see the animation:
  mSleep(5);
  update();
  }

void Screen::focusCursor( void ) {
  focus_pt = cursorat();
  }
  
Point Screen::getFocusPt( void ) {
  return focus_pt;
  }

void  Screen::setFocusPt( Point pt ) {
  focus_pt = pt;
  }
  
void  Screen::setFocusX ( int x ) {
  focus_pt.setx(x);
  }

void  Screen::setFocusY ( int y ) {
  focus_pt.sety(y);
  }

void    Screen::resetMenuLimit( void ) {
  menu_limit.set(1,1,width,height);
  }
  
Rectangle Screen::getMenuLimit( void ) {
  return menu_limit;
  }
  
void      Screen::setMenuLimit( Rectangle limit ) {
  menu_limit = limit;
  }

void    Screen::resetImageLimit( void ) {
  image_limit.set(1,1,width,height);
  }
  
Rectangle Screen::getImageLimit( void ) {
  return image_limit;
  }
  
void      Screen::setImageLimit( Rectangle limit ) {
  image_limit = limit;
  }

void    Screen::resetFormatLimit( void ) {
  format_limit.set(1,1,width,height);
  }
  
Rectangle Screen::getFormatLimit( void ) {
  return format_limit;
  }
  
void      Screen::setFormatLimit( Rectangle limit ) {
  format_limit = limit;
  }

#ifdef _VISUALS_HARNESS

#include "point.hpp"
#include "string.hpp"
#include "counted.hpp"
#include "rectngle.hpp"
#include "enum.hpp"
#include "error.hpp"
#include "input.hpp"
#include "random.hpp"

int main() {
        Screen::init();
        Keyboard::init();

	Random::randomize();

	Screen::cls();

	for (int count = 0; count < 5; count++) {
		Screen::locate(20 + Random::randint(59), count*2 + 5);

		String retStr = Screen::writef("This {14String} {02is} {03somewhat} long {04(for {12what} it's worth)} and uses {09colors!}");

		if (retStr != "") {
			Screen::locate(1,Screen::yloc()+1);
			Screen::writef( retStr );
			}
		}

        Keyboard::get();

        Screen::end();

	return 0;
	}

#endif //_VISUALS_HARNESS

#endif // VISUALS_CPP

