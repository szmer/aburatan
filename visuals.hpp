// [visuals.hpp]
// (C) Copyright 2000 Michael Blackney
#if !defined ( VISUALS_C_H )
#define VISUALS_C_H
#include "types.hpp"
#include "point.hpp"
#include "rectngle.hpp"
#include "string.hpp"
#include "enum.hpp"
#include "stddef.h"

// Color values:
const enum Color {

 // Normal Colors:
 cBlack,
 
 cBlue,
 cGreen,
 cOrange,
 cRed,
 cIndigo,
 cBrown,
 cGrey,

 cDIM=cGrey,

 cDGrey,
 cLBlue,
 cLGreen,
 cLGrey,
 cLRed,

 cViolet,
 cYellow,
 cWhite,

 cNorm,

 // Shimmering Colors:
 cFire,    // Red, Orange + Yellow
 cIce,     // White + LGrey + LBlue
 cWater,   // Blue + LBlue
 cHaze,    // DGrey + LGrey
 cRainbow  // ROYGBIV

 }
// Redefinable Colors:    (per compile only)
 cUnexplored = cDGrey,
 cDefault    = cGrey,
 cNotSeen    = cBlack;

#ifdef COLOR_REDEF
struct RGBType {
  int r, g, b;
  };

const RGBType colorTable[16] = {
	{0x00,0x00,0x00},    // 0 cBlack
	{0x55,0x55,0xa0},	   // 1 cBlue
	{0x44,0xa0,0x44},	   // 2 cGreen
	{0xee,0x88,0x00},	   // 3 cOrange
	{0xa0,0x44,0x44},	   // 4 cRed
	{0xa0,0x00,0xdf},	   // 5 cIndigo
	{0xaa,0x88,0x00},	   // 6 cBrown
	{0xa0,0xa0,0xa0},	   // 7 cGrey
  
	{0x80,0x80,0x80},	   // 8 cDGrey
	{0x68,0xa0,0xdf},	   // 9 cLBlue
	{0x68,0xdf,0xa0},	   // 0 cLGreen
	{0xc0,0xc0,0xc0},	   // 1 cLGrey
	{0xdf,0x68,0x68},	   // 2 cLRed
	{0xcf,0x40,0xc8},	   // 3 cViolet
	{0xcf,0xc8,0x00},	   // 4 cYellow
	{0xe5,0xe5,0xe5} 	   // 5 cWhite
	};
#endif // COLOR_REDEF

// Image constants
#define BLANK 0x0000



class Screen {
	public:
	 // Interface methods: ( OS Dependant )
	 static void cls( void ); // Clears the screen
	 static void clearline( void ); // Clears the rest of the screen row

	 static void  put( const int, const int, const Image ); // Display one integer
	 static Image get( const int, const int ); // return one integer
   static void update( void );

   static int  fixcolor( int c ); // Assign color if of random type
	 static void setcolor( int c ); // Change the current drawing color
   
	 static void locate( int x, int y ); // Move the cursor (absolute)
	 static void hidecursor( void );
	 static void showcursor( void );

	 static int xloc( void ); // Cursor x location
	 static int yloc( void ); // Cursor y location
	 static int linespace( void ); // Number of intacters until eol

	 class Buffer {
		 public:
			 Buffer() : buffer(NULL), bufferRect() {};
			 ~Buffer();
			 void setSize( const Rectangle &r );
       void shiftData( const Point &p );
			 const Rectangle &getSize( void ) const;
			 int toBuffer();
			 int toScreen();
			 void moveToScreen();
       
       // A multipurpose function for these buffers and
       // other types (buffers of structs for example)
       static void ShiftBuffer( void *, int, int, size_t, Point );

		 private:
			 void *buffer;
			 Rectangle bufferRect;
		 };

   static void nextFrame( void );
   static void mSleep( unsigned );

	 // Regular interface methods:

	 static Point cursorat( void ); // Point version of wherex,y
	 static void locate( const Point &p ); // Move the cursor (absolute)
	 static void writeprocessed( const char*, int c = cDefault );
	 static void writeat( int, int, const char*, int c = cDefault );
	 static void write( const char*, int c = cDefault );
	 static void write( const String&, int c = cDefault );
	 static String writef( const String&, int c = cBlack );
	 static void writen( char, int, int c = cDefault );
	 static void write( char, int c = cDefault );
	 static void write( Image );
	 static void drawrect( const Rectangle&, menuSkin, bool focus = true );
	 static void drawwin( const Rectangle&, const String&, menuSkin, bool focus = true );

	 static void init( void );
	 static void end( void );

   static void focusCursor( void );
   static Point getFocusPt( void );
   static void  setFocusPt( Point );
   static void  setFocusX ( int );
   static void  setFocusY ( int );

   static void    resetMenuLimit( void );
   static Rectangle getMenuLimit( void );
   static void      setMenuLimit( Rectangle );

   static void    resetImageLimit( void );
   static Rectangle getImageLimit( void );
   static void      setImageLimit( Rectangle );

   static void    resetFormatLimit( void );
   static Rectangle getFormatLimit( void );
   static void      setFormatLimit( Rectangle );

	 // Static data:
	 static int size;
	 static int width;
	 static int height;
	 static int maxx;
	 static int maxy;
	 static int mapxoff;
	 static int mapxmin;
	 static int mapxmax;
	 static int mapyoff;
	 static int mapymin;
	 static int mapymax;
	 static int levxoff;
	 static int levyoff;

   static int cursor_off;

  private:
   static Point focus_pt;
   static Rectangle menu_limit;
   static Rectangle image_limit;
   static Rectangle format_limit;
   
	};

#define MAP_DRAW(x,y,c) Screen::put((x)+Screen::mapxoff+1,(y)+Screen::mapyoff+1,c)
#define MAP_LOCATE(x,y) Screen::locate((x)+Screen::mapxoff+1,(y)+Screen::mapyoff+1)
#define MAP_GET(x,y) Screen::get((x)+Screen::mapxoff+1,(y)+Screen::mapyoff+1)

#define LEV_DRAW(x,y,c) Screen::put((x)+Screen::levxoff+Screen::mapxoff+1,(y)+Screen::levyoff+Screen::mapyoff+1,c)
#define LEV_LOCATE(x,y) Screen::locate((x)+Screen::levxoff+Screen::mapxoff+1,(y)+Screen::levyoff+Screen::mapyoff+1)
#define LEV_GET(x,y) Screen::get((x)+Screen::levxoff+Screen::mapxoff+1,(y)+Screen::levyoff+Screen::mapyoff+1)

#define POINT_VISIBLE(x,y) ( ascending(Screen::mapxmin-1,(x)+Screen::levxoff+Screen::mapxoff+1,Screen::mapxmax+1) \
                          && ascending(Screen::mapymin-1,(y)+Screen::levyoff+Screen::mapyoff+1,Screen::mapymax+1) )

#endif // VISUALS_C_H
