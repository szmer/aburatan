// [menu.cpp]
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
#if !defined ( MENU_CPP )
#define MENU_CPP

#include "compiler.hpp"

//#define MENU__HARNESS

#include "menu.hpp"

//--[ class FloatBox ]--//

FloatBox::FloatBox( menuSkin m )
: isStatic( false ),
  doredraw( true ),
  hasFocus(true),
  changedFocus(false),
  skin(m),
  pre_cursor(Screen::cursorat()),
  pre_focus(Screen::getFocusPt()) {}

FloatBox::~FloatBox( void ) {
  }

void FloatBox::checkEOL( void ) {
  if (!isStatic) dynamicBorder();
  }

void FloatBox::dynamicBorder( void ) {

	if (doredraw) {

		int num = tallest();
    int widest_text = widest();
		staticWin.set (
			dynamicOrigin.getx() - widest_text / 2 - 2,
			dynamicOrigin.gety() - num / 2 - 1,

			dynamicOrigin.getx() + widest_text / 2 + 1 + widest_text % 2,
			dynamicOrigin.gety() + num / 2 + num % 2

			);

		while (staticWin.getxa() < Screen::getMenuLimit().getxa()) {
			staticWin.getxa()++;	staticWin.getxb()++;
			}
		while (staticWin.getxb() > Screen::getMenuLimit().getxb()) {
			staticWin.getxa()--;	staticWin.getxb()--;
			}
		while (staticWin.getya() < Screen::getMenuLimit().getya()) {
			staticWin.getya()++;	staticWin.getyb()++;
			}
		while (staticWin.getyb() > Screen::getMenuLimit().getyb()) {
			staticWin.getya()--;	staticWin.getyb()--;
			}

		staticWin.setintersect ( Screen::getMenuLimit() );

		}

	}

void FloatBox::display( void ) {
	// If we need to redraw, redraw the window:
	if (doredraw || changedFocus) {
		buffer.setSize( staticWin );
		Screen::drawwin(staticWin, title, skin, hasFocus);

		// Now redrawed:
		doredraw = false;
		changedFocus = false;
  	}
  }

void FloatBox::focus( bool f ) {

	if (hasFocus != f) changedFocus = true;
	hasFocus = f;

	}


void FloatBox::redraw( void ) {
	doredraw = true;
	display();
	}

void FloatBox::hide( void ) {
	buffer.moveToScreen();
  doredraw = true;
  Screen::locate(pre_cursor);
  Screen::setFocusPt(pre_focus);
  }

void FloatBox::setStatic() {
	if (isStatic) return;
	isStatic = true;
	}

void FloatBox::setDynamic() {
	if (!isStatic) return;

	dynamicOrigin.set (
		( staticWin.geta().getx() + staticWin.getb().getx() ) / 2,
		( staticWin.geta().gety() + staticWin.getb().gety() ) / 2
		);

	isStatic = false;
	}

void FloatBox::setOrigin( const Point &p ) {
	isStatic = false;
	dynamicOrigin = p;
	doredraw = true;
  checkEOL();
	}

void FloatBox::setBorder( const Rectangle& r ) {
	isStatic = true;
	staticWin = r;
	doredraw = true;
  checkEOL();
	}

Point FloatBox::getOrigin( void ) {
	if (isStatic)
		return staticWin.getMid();
	else
		return dynamicOrigin;
	}

Rectangle FloatBox::getBorder( void ) {
	if (!isStatic)
		dynamicBorder();

	return staticWin;
	}

void FloatBox::setTitle( const String &s ) {
	title = s;

	doredraw = true;

  checkEOL();
	}

const String &FloatBox::getTitle( void ) {
	return title;
	}

void FloatBox::setSkin( menuSkin m ) {
	skin = m;
	doredraw = true;
	}

//--[ class Pane ]--//

Pane::Pane( menuSkin m )
: FloatBox(m) {

	// Center to mid-screen by default:
	setOrigin( Screen::getFocusPt() );

  checkEOL();
  }


void Pane::display( void ) {

  Screen::hidecursor();
  // Draw border:
  FloatBox::display();

  // Fill in gaps:
  int x = staticWin.getxa()+1, y;
  String str = String().fillnchar(' ',staticWin.getwidth()-2);
  
  for ( y = staticWin.getya() + 1; y < staticWin.getyb(); ++y )
    Screen::writeat( x, y, str.toChar() );

  // Draw elements:
  if (elements.reset()) {

    Rectangle extents( staticWin.geta()+Point(1,1), staticWin.getb()+Point(-1,-1) );
  
    do {

      elements.get()->display( extents );
  
      } while (elements.next());
      
    }

  Screen::showcursor();
  }

int Pane::widest( void ) {
  int ret_val = 6;

  if ( ret_val < title.formatlen() + 2 )
    ret_val = title.formatlen() + 2;

  if ( elements.reset() )
  do {

    if ( ret_val < elements.get()->getBorder().getwidth() + elements.get()->getBorder().getxa() )
      ret_val = elements.get()->getBorder().getwidth() + elements.get()->getBorder().getxa()-3;

    } while ( elements.next() );
  
  return ret_val;
  }
  
int Pane::tallest( void ) {
  int ret_val = 1;

  if ( elements.reset() )
  do {

    if ( ret_val < elements.get()->getBorder().getheight() + elements.get()->getBorder().getya() )
      ret_val = elements.get()->getBorder().getheight() + elements.get()->getBorder().getya();

    } while ( elements.next() );
  
  return ret_val;
  }

void Pane::add( TextArea &t_a ) {
  elements.add(&t_a);
  checkEOL();
  }


//--[ class TextArea ]--//

void TextArea::display( const Rectangle &extents ) {

  Rectangle limits( extents.geta()+border.geta(), extents.geta()+border.getb() );

  limits.setintersect( extents );

  Screen::setFormatLimit( limits );

  Point cursor = limits.geta();
  String next_line = text;

  do {
  
    Screen::locate( cursor );
    next_line = Screen::writef( next_line, color );

    cursor.incy();
    
    } while (next_line.formatlen() && limits.contains(cursor) ) ;

  Screen::resetFormatLimit();

  }

void TextArea::setBorder( const Rectangle &b ) {
  border = b;
  }

Rectangle TextArea::getBorder( void ) {
  return border;
  }
  
void TextArea::setColor( char c ) {
  color = c;
  }
  
void TextArea::setText( String t ) {
  text = t;
  }



//--[ MENU harness ]--//
#ifdef MENU__HARNESS


#include "global.hpp"
#include "string.hpp"
#include "point.hpp"
#include "rectngle.hpp"
#include "visuals.hpp"
#include "input.hpp"
#include "sorted.hpp"
#include "random.hpp"
#include "counted.hpp"
#include "enum.hpp"
#include "error.hpp"
#include "function.hpp"

bool gQuit = false;


Menu<FunctionItem> *mainMenuPtr;


void WriteName() { Screen::locate(1,1); Screen::write("Michael!"); }
void ClearScreen() { mainMenuPtr->hide(); Screen::cls(); mainMenuPtr->redraw(); }
void RandomResize() {
	mainMenuPtr->setBorder(
		Rectangle(
			Random::randint(35)+1, Random::randint(12)+1,
			Random::randint(35)+45,Random::randint(12)+13
			).setintersect( Screen::getMenuLimit() )
		);
	}
void RandomPos() {
	mainMenuPtr->setOrigin(
		Point( Random::randint(80)+1, Random::randint(25)+1 )
		);
	}
void ChangeSkin() {
	mainMenuPtr->setSkin( (menuSkin) Random::randint(NumSkins) );
	}
void SwitchLoop() {
	static bool loop=false;

	mainMenuPtr->setLoop( loop );

	loop = (bool) !loop;
	}
void SwitchCenter() {
	static bool center=true;

	mainMenuPtr->center(center);

	center = (bool) !center;
	}
void SwitchLetters() {
	static bool letters=false;

	if (letters) mainMenuPtr->useLetters();
	else mainMenuPtr->noLetters();

	letters = (bool) !letters;
	}



void mainroutine( void ) {
	Sorted<FunctionItem> menuList;

	menuList.add( FunctionItem( &WriteName,    "Write my name",      'w' ) );
	menuList.add( FunctionItem( &ClearScreen,  "Clear the screen",   'c' ) );
	menuList.add( FunctionItem( &RandomResize, "Static Resize",      'r' ) );
	menuList.add( FunctionItem( &RandomPos,    "Dynamic Reposition", 'R' ) );
	menuList.add( FunctionItem( &ChangeSkin,   "Random Menu Skin",   's' ) );
	menuList.add( FunctionItem( &SwitchLoop,   "List Loop <11On>/Off",   'l' ) );
	menuList.add( FunctionItem( &SwitchCenter, "Centering On/Off",   'C' ) );
	menuList.add( FunctionItem( &SwitchLetters,"Quick Keys On/Off",  'l' ) );


	Menu<FunctionItem> mainMenu( menuList, global::skin.inventory );
	mainMenuPtr = &mainMenu;


	mainMenu.setTitle("Main Menu");
	mainMenu.display();

	Command key = kInvalid;
	while ( (key != kQuitGame) && (key != kEscape) ) {
		key = mainMenu.getCommand();

		switch (key) {
			case kEnter:
				mainMenu.get().call();
				break;
      default:
        break;
			}

		mainMenu.display();
		}


  Pane myPane( global::skin.main );
  myPane.setTitle("A Pane");

  TextArea text_area, desc_area;

  desc_area.setText("<!Description:>");
  desc_area.setBorder( Rectangle(0,0,22,0) );
  myPane.add( desc_area );

  text_area.setText("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30");
  text_area.setBorder( Rectangle(1,1,22,6) );
  myPane.add( text_area );
  
  myPane.display();
  
  key = kInvalid;
  while (key != kQuitGame && key != kEscape) {
    key = Keyboard::getCommand();
    }

  desc_area.setText("<~This is cool:>");
  myPane.display();
  
  key = kInvalid;
  while (key != kQuitGame && key != kEscape) {
    key = Keyboard::getCommand();
    }

	mainMenuPtr = NULL;

	}


int main() {

	Random::randomize();
	global::reset();

	Screen::init();
	Keyboard::init();

	mainroutine();

	Screen::cls();

  int ret_val = printTotalDeletes();
  
	Screen::end();

	return ret_val;
	}

#endif // MENU__HARNESS



#endif // MENU_CPP

