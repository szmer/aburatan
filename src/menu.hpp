// [menu.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( MENU_HPP )
#define MENU_HPP


#include "types.hpp"
#include "enum.hpp"
#include "string.hpp"
#include "point.hpp"
#include "rectngle.hpp"
#include "list.hpp"
#include "visuals.hpp"
#include "input.hpp"
#include "global.hpp"

//--[ virtual class FloatBox ]--//
class FloatBox {

  public:
    FloatBox( menuSkin );
    virtual ~FloatBox();

    virtual void display( void ) = 0;
		void redraw( void );
    
    virtual int widest( void ) = 0;
    virtual int tallest( void ) = 0;

    void checkEOL( void );

		void setStatic();
		void setDynamic();
    
		virtual void setOrigin( const Point& ); // Dynamic mode function
		virtual void setBorder( const Rectangle& ); // Static mode function
		Point getOrigin( void );
		Rectangle getBorder( void );

		virtual void setTitle( const String& );
		const String &getTitle( void );
		void setSkin( menuSkin );

		void focus( bool f = true );
		void hide( void );

  protected:
    // Data Members

		Screen::Buffer buffer; // Saves the screen info beneath the menu

		String title;

		// For static mode:
		 Rectangle staticWin;
		// For dynamic mode:
		 Point dynamicOrigin;
		/////

		bool isStatic,
			doredraw,
			hasFocus,
			changedFocus;
    
		menuSkin skin;

    Point pre_cursor, pre_focus;

    // Member Functions
   	void dynamicBorder( void );
  };
  
//--[ class TextArea ]--//
class TextArea {
  public:
    TextArea() : color(cBlack) {}
    
    void display( const Rectangle & );

    void setBorder( const Rectangle & );
    void setColor( char );
    void setText( String );

    Rectangle getBorder( void );

  protected:
    Rectangle border;
    String text;
    char color;
  };
  

//--[ class Pane ]--//
class Pane : public FloatBox {
  public:
    Pane( menuSkin );

    virtual void display( void );

    virtual int widest( void );
    virtual int tallest( void );

    void add( TextArea& );
    
  protected:
    List<TextArea*> elements;
  };

//--[ class Menu ]--//
template <class T>
class Menu : public FloatBox {
	public:
		// Construction/Destruction:
		Menu( List<T>&, menuSkin );
		virtual ~Menu();

		//Interface:
		virtual void display( void );
    
		T &get( void );
		Command getCommand( void );

		// Shape & position:
//		virtual void setOrigin( const Point& ); // Dynamic mode function
//		virtual void setBorder( const Rectangle& ); // Static mode function

		void useLetters();
		void noLetters();

		void setLoop( bool );

		// Style:
		virtual void setTitle( const String& );
		void center( bool c = true );

		T remove( void );
		bool reset( void );
		bool toTail( void );
    bool atHead( void ) const;
    bool atTail( void ) const;
    bool next( void );
    bool previous( void );

		int numitems( void ) const { return items.numData(); }
    int getCursorRow( void ) const { return cursorRow; }

	protected://Data Members
		List<T> &items; // Every <T> class must implement menustring(char length)
										// and menuletter(void)
										// NOTE: Menus only contain a reference to the list,
										// so whenever you change the list you have to call
										// Menu::redraw()

		int cursorRow, lastRow;

    int widestText;

		bool centerText,
			letterRef,
			loopList;

		int numData, selected;

	private://Member functions
		void checkEOL( void );
    virtual int widest( void );
    virtual int tallest( void );

//		void displayStatic( void );
//		void displayDynamic( void );
//   	void dynamicBorder( void );

	};

//--[ template class Menu implementation ]--//
#include "enum.hpp"
#include "types.hpp"
#include "input.hpp"


template <class T>
Menu<T>::Menu( List<T> &l, menuSkin m )
: FloatBox(m),
  items(l), cursorRow(0), lastRow(0),
  widestText(0), centerText( false ),
	letterRef( true ),
	loopList( true ),
	numData(0),         	selected(0) {

	// Find length of longest menu string:
	long key = l.lock();
	if (l.reset())
	do {
		int len = get().menustring().formatlen();
		if (widestText < len)
			widestText = len;
		} while ( l.next() );
	l.load(key);

	// Center to mid-screen by default:
	dynamicOrigin = Screen::getFocusPt();

	}

template <class T>
Menu<T>::~Menu() {
	buffer.moveToScreen();
  Screen::locate(pre_cursor);
  Screen::setFocusPt(pre_focus);
	}

template <class T>
T Menu<T>::remove( void ) {
	doredraw = true;
	return items.remove();
	}

template <class T>
bool Menu<T>::reset( void ) {
	bool ret_val = items.reset();
	cursorRow = 0;
  return ret_val;
	}

template <class T>
bool Menu<T>::toTail( void ) {
	bool ret_val = items.toTail();
	cursorRow = staticWin.getheight() - 3;
  return ret_val;
	}

template <class T>
bool Menu<T>::atHead( void ) const {
	return items.atHead();
	}

template <class T>
bool Menu<T>::atTail( void ) const {
	return items.atTail();
	}

template <class T>
bool Menu<T>::next( void ) {
	bool ret_val = items.next();
	if (cursorRow < staticWin.getheight() - 3) ++cursorRow;
  return ret_val;
	}

template <class T>
bool Menu<T>::previous( void ) {
	bool ret_val = items.previous();
	if (cursorRow > 0) --cursorRow;
  return ret_val;
	}

/*
template <class T>
void Menu<T>::redraw( void ) {
	doredraw = true;
	display();
	}
*/
template <class T>
T &Menu<T>::get( void ) {
	return items.get();
	}

/*
template <class T>
void Menu<T>::hide( void ) {
	buffer.moveToScreen();
  Screen::locate(pre_cursor);
  Screen::setFocusPt(pre_focus);
  }
*/


template <class T>
void Menu<T>::checkEOL( void ) {

  FloatBox::checkEOL();

	if ( cursorRow > staticWin.getheight() - 3	)
		cursorRow = staticWin.getheight() - 3;

	long key = items.lock();

	for (int count = 0; count < staticWin.getheight() - 2 - cursorRow; ++count) {
		if (! items.next() ) {
			cursorRow = staticWin.getheight() - 3 - count;
			break;
			}
		}

	items.load(key);
	}

template <class T>
Command Menu<T>::getCommand( void ) {
    using namespace std;

	char count;
	int key = Keyboard::get();
	Command command = Keyboard::toCommand(key);

  // I disabled the nsew keys because it should be
  // an option (default enabled) to use them for
  // controlling menus.  When hjkl 'vi' keys are used
  // for nsew, we can't select certain items just by
  // typing their letter.

  // If we press a key that has an instant effect on the
  // menu, we must set command to kIgnore to make sure
  // we don't propagate the message and have a double
  // effect.
  
	switch (command) {
//    case kNorth:
		case kCursorUp: // Up cursor
			if (items.previous()) {
				if (cursorRow > 0) --cursorRow;
        command = kIgnore;
				}
			else
				if (!loopList)
				items.reset();
				else {
					items.toTail();
					cursorRow = staticWin.getheight() - 3;
          command = kIgnore;
					}
			break;
//    case kSouth:
		case kCursorDown: // Down cursor
			if (items.next()) {
				if (cursorRow < staticWin.getheight() - 3) ++cursorRow;
        command = kIgnore;
				}
			else
				if (!loopList)
				items.toTail();
				else {
					items.reset();
					cursorRow = 0;
          command = kIgnore;
					}
			break;
//    case kNorthWest:
		case kHome: // Home
      if (!items.atHead()) {
  			items.reset();
        command = kIgnore;
   			cursorRow = 0;
        }
			break;
//    case kSouthWest:
		case kEnd: // End
      if (!items.atTail()) {
  			items.toTail();
        command = kIgnore;
  			cursorRow = staticWin.getheight() - 3;
        }
			break;
//    case kNorthEast:
		case kPageUp: // Page up
      if (!items.atHead()) {
  			for (count = 0; count < staticWin.getheight() - 3; ++count ) {
  				items.previous();
  				}
  			if (!items.previous()) {
  				items.reset();
  				cursorRow = 0;
  				}
        command = kIgnore;
        }
			break;
//    case kSouthEast:
		case kPageDown: // Page down
      if (!items.atTail()) {
  			for (count = 0; count < staticWin.getheight() - 3; ++count ) {
  				items.next();
  				}
  			if (!items.next()) {
  				items.toTail();
  				cursorRow = staticWin.getheight() - 3;
  				}
  			else {
  				// Must check for near end of list
  				checkEOL();
  				}
        command = kIgnore;
        }
			break;

    case kSpace:
    // Treat spacebar as enter key in all menus:
      command = kEnter;
      break;

    case kBackspace:
      if (command == kBackspace && !global::options.backspace_esc)
        return command;

      // flow through to :
    case kDelete:
      if (command == kDelete && !global::options.delete_esc)
        return command;
    
      // flow through to :
    case kEscape:
      return kEscape;

		default: // Otherwise, search for quick-selection:
			if (letterRef) {

				// Start at next item and search for
				// the next item with this letter:
				int matches = 0;
				bool startOnMatch = false;
				if (items.get().menuletter().val.appearance == key ) {
					matches = 1;
					startOnMatch = true;
					}

				long i_key = items.lock();
				if ( !items.next() ) items.reset();

				while ( i_key != items.lock() ) {
					// If this item has the pressed letter:
					if (items.get().menuletter().val.appearance == key ) {
						// Select the item

						if (  ( !startOnMatch && matches == 0 )	|| matches == 1 )
							i_key = items.lock();

						matches++;

						}
					// If the end of loop, go to start:
					if ( i_key != items.lock() && !items.next() ) items.reset();
					}

				if ( ( matches == 1 )	&& isalnum(key) )
					command = kEnter;

				if (matches > 0) checkEOL();

				items.load(i_key);
				}
			break;


		}

	return command;
	}
/*
template <class T>
void Menu<T>::setStatic() {
	if (isStatic) return;
	isStatic = true;
	}

template <class T>
void Menu<T>::setDynamic() {
	if (!isStatic) return;

	dynamicOrigin.set (
		( staticWin.geta().getx() + staticWin.getb().getx() ) / 2,
		( staticWin.geta().gety() + staticWin.getb().gety() ) / 2
		);

	isStatic = false;
	}
*/
template <class T>
void Menu<T>::useLetters( void ) {
	letterRef = true;
	doredraw = true;
	}

template <class T>
void Menu<T>::noLetters( void ) {
	letterRef = false;
	doredraw = true;
	}

template <class T>
void Menu<T>::setLoop( bool b ) {
	loopList = b;
	}


/*
template <class T>
void Menu<T>::setOrigin( const Point &p ) {
  FloatBox::setOrigin(p);
	checkEOL();
	}

template <class T>
void Menu<T>::setBorder( const Rectangle& r ) {
  FloatBox::setBorder(r);
	checkEOL();
	}
*/
/*
template <class T>
Point Menu<T>::getOrigin( void ) {
	if (isStatic)
		return staticWin.getMid();
	else
		return dynamicOrigin;
	}

template <class T>
Rectangle Menu<T>::getBorder( void ) {
	if (!isStatic)
		dynamicBorder();

	return staticWin;
	}
*/

template <class T>
void Menu<T>::display( void ) {

  checkEOL();

	Screen::hidecursor();
	// Determine number of items in list:
	if ( numData != items.numData() ) doredraw = true;
	numData = items.numData();

	// Determine list position:
	long key = items.lock();
	items.reset();
	int current = 0;
	while ( key != items.lock() ) {
		current++;
		if (! items.next() ) {
			Screen::showcursor();
			return;
			}
		}

	items.load(key);


	for (int count = 0; count < cursorRow; count++)
		if (!items.previous()) {
			cursorRow = count;
			items.reset();
			}
	// Determine whether the framed portion of the list has changed:
	bool frameChanged = false;
	if ( (current-cursorRow) != (selected-lastRow) )
//		Screen::locate(staticWin.geta().getx()+1,
//			staticWin.geta().gety());
//		Screen::write(String("[")+(long)(current)+":"+(long)(cursorRow)+"-"+(long)(selected)+":"+(long)(lastRow)+"]");
    frameChanged = true;
	if ( doredraw ) frameChanged = true;


	// Determine whether cursor row has changed:
	bool changedRow = false;
	if (lastRow != cursorRow ) {
		changedRow = true;
		}

	if ( ( selected == current ) && (!changedRow) && (!frameChanged) && (!changedFocus) ) {
		items.load(key);
		Screen::showcursor();
		return;
		}

  // Draw the floating box:
  FloatBox::display();

	int y = 0;
	int height = staticWin.getheight() - 2;

	Point cursor = staticWin.geta();

	bool endOfList = false;
	String menuStr;
	String blank;
	blank.fillnchar(' ',staticWin.getwidth()-2);
	for (y = 0; y < height; y++)
	if ( frameChanged || ( y-cursorRow == 0 ) || ( y-lastRow==0 ) ) {
	if ( !endOfList ) {
		menuStr = items.get().menustring( staticWin.getwidth() - ( (letterRef)?6:4 ) );

		Screen::locate(staticWin.geta().getx()+1,
			staticWin.geta().gety()+1+y);

		char padleft, padright;

		if (centerText) {
			char temp = staticWin.getwidth() - menuStr.formatlen() - ( (letterRef)?6:4 );
			padleft = temp / 2;
			padright = temp / 2 + (temp % 2);
			}
		else {
			padleft = 0;
			padright = staticWin.getwidth() - menuStr.formatlen() - ( (letterRef)?6:4 );
			}

		Screen::writen(' ', padleft );

		if (key == items.lock() && hasFocus) {
			Screen::write( skin::Arr[skin].cursor_l );

			cursor = Screen::cursorat();

			if (letterRef) {
				Screen::write(items.get().menuletter());
				Screen::write( ' ' );
				}

			Screen::writef( menuStr, skin::Arr[skin].cursor );
			Screen::write( skin::Arr[skin].cursor_r );
			}
		else {
			Screen::write( ' ' );

			if (letterRef) {
				Screen::write(items.get().menuletter());
				Screen::write( ' ' );
				}

			Screen::writef( menuStr );
			Screen::write( ' ' );
			}

		Screen::writen(' ', padright );

		if (! items.next() ) endOfList = true;

		}
	else {
		Screen::locate(staticWin.geta().getx()+1,
			staticWin.geta().gety()+1+y);
		Screen::write(blank);

		}
	}	else if (! items.next() ) endOfList = true;


	if (selected != current) {
		selected = current;
		}
	lastRow = cursorRow;

	items.load(key);

	Screen::locate( cursor );
  Screen::setFocusPt( cursor + Point(get().menustring().formatlen() / 2,0) );
	Screen::showcursor();
  Screen::update();

	}


template <class T>
void Menu<T>::setTitle( const String &s ) {

  FloatBox::setTitle(s);

	if (title.formatlen() > widestText)
		widestText = title.formatlen();

	}

/*
template <class T>
const String &Menu<T>::getTitle( void ) {
	return title;
	}

template <class T>
void Menu<T>::setSkin( menuSkin m ) {
	skin = m;
	doredraw = true;
	}
*/

template <class T>
void Menu<T>::center( bool c ) {
	centerText = c;
	doredraw = true;
	}

/*
template <class T>
void Menu<T>::focus( bool f ) {

	if (hasFocus != f) changedFocus = true;
	hasFocus = f;

	}
*/

template <class T>
int Menu<T>::widest( void ) {
  return widestText + (letterRef?2:0);
  }

template <class T>
int Menu<T>::tallest( void ) {
  return items.numData();
  }

#endif // MENU_HPP
