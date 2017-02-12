// [dialog.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( DIALOG_HPP )
#define DIALOG_HPP


#include "string.hpp"
#include "point.hpp"
#include "menu.hpp"
#include "visuals.hpp"
#include "enum.hpp"

/*
	class Dialog

	+-[Adjust]----------------+
	| Adjust to which letter? |
	+-------------------------+

*/


class Dialog {
	public:
		Dialog( const String&, const String&, menuSkin );
		~Dialog( void );

		void display( void );
		int getKey( void );

		void setOrigin( const Point& );

		void setTitle( const String& );
		void setContent( const String& );

		void setSkin( menuSkin );
		void hide( void );


	protected:
		Screen::Buffer buffer;

		Rectangle staticWin;
		String Title, content;
		Point origin;
		menuSkin skin;

		bool dorefresh;

		void calcWin( void );
	};





/*
	class StringDialog

	+-[New Character]-+
	| Name? ________  |
	+-----------------+


	StringDialog::numeric

	+-[Drop Items]---+
	| Quantity? ____ |
	+----------------+

*/

class StringDialog : public Dialog {
	public:
		StringDialog( const String&, const String&, int, alnum, menuSkin );

		void setInput( const String& );
		void setAlign( Hand );
		void display( void );

		void setnumeric( void );
		void setalpha( void );
		void setalphanumeric( void );

		String readString( bool );

	private:
		Hand align;  // Left or right alignment
		int inputlen;
		alnum inputtype;

		String input;

		bool initialInput;

		void calcWin( void );
	};


#endif // DIALOG_HPP
