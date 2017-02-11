// [input.hpp]
// (C) Copyright 2000 Michael Blackney
#if !defined ( INPUT_HPP )
#define INPUT_HPP

#include "enum.hpp"
#include "config.hpp"
#include "string.hpp"
#include "dir.hpp"

#ifdef USE_CURSES
#  include <curses.h>
#endif

#ifdef border
#  undef border // CURSES defines border but we want it, damnit!
#endif

enum Command {
	kSouthWest,
	kSouth,
	kSouthEast,
	kWest,
	kWait,
	kEast,
	kNorthWest,
	kNorth,
	kNorthEast,
  kMove,
	kOpen,
	kClose,
  kApply,
  kEtch,
  kBuild,
	kTarget,
	kAttack,
	kKick,
	kVolley,
	kInventory,
	kLoad,
	kUnload,
	kWield,
	kSwitch,
	kWear,
	kTakeOff,
	kPickUp,
	kDrop,
	kDropMany,
	kEat,
	kQuaff,
  kRead,
  kView,
	kChat,
	kAdjust,
	kDisarm,
	kSearch,
  kPray,
  kSkills,
	kCharInfo,
	kMap,
	kLook,
  kLookAround,
	kDescend,
	kAscend,
	kSelf,
	kRest,
	kQuitGame,
	kHelp,
	kMessages,

	kCursorUp,
	kCursorDown,
	kCursorLeft,
	kCursorRight,
	kEnter,
	kSpace,
	kEscape,
	kTab,
	kBackspace,
	kPageUp,
	kPageDown,
	kHome,
	kEnd,
  kInsert,
  kDelete,
	kMemory,

  kNum,
  kIgnore, // Ignore this keystroke - it's been dealt with
	kInvalid = kNum
	};

class Keyboard {
	public:
	 // Interface methods: ( OS Dependant )
	 static int hit( void );
	 // Regular interface methods:
	 static int get( void );
	 static Command toCommand( int );
	 static Command getCommand( void );
	 static Command getDirection( void );
   static Dir     toDir( Command );
	 static void init( void );
	 static String keytostr( int );
	 static String keyname( int );
	 static String keydesc( int );
	};

struct KeyDesc {
	int key, dkey;
	Command command;
	const char *name;
	const char *desc;
  bool help;
	};


# if !defined(USE_CURSES)
#   define KEY_UP     '^'|'H'<<8
#   define KEY_DOWN   '^'|'P'<<8
#   define KEY_LEFT   '^'|'K'<<8
#   define KEY_RIGHT  '^'|'M'<<8
#   define KEY_PPAGE  '^'|'I'<<8
#   define KEY_NPAGE  '^'|'Q'<<8
#   define KEY_HOME   '^'|'G'<<8
#   define KEY_END    '^'|'O'<<8
# endif

// The following keys are defined poorly by curses
// so we have to hard-code them in.
// The enter key is the select key in menus,
// the escape key is to cancel menus.
// Tab switches between menus and backspace
// is used when typing strings.
# if defined( OS_DOS ) or defined( OS_WIN )
#   define AT_KEY_ENTER     (char)13
#   define AT_KEY_ESCAPE    (char)27
#   define AT_KEY_TAB       (char)9
#   define AT_KEY_BACKSPACE (char)8
# elif defined( OS_LINUX )
#   define AT_KEY_ENTER     (char)10
#   define AT_KEY_ESCAPE    (char)27
#   define AT_KEY_TAB       (char)9
#   define AT_KEY_BACKSPACE (char)127
# elif defined( OS_UNIX )
#   define AT_KEY_ENTER     (char)10
#   define AT_KEY_ESCAPE    KEY_CANCEL
#   define AT_KEY_TAB       KEY_STAB
#   define AT_KEY_BACKSPACE KEY_BACKSPACE
# else
// The following are pretty lousy key choices and
// should only be used temporarily to test a new OS.
#   define AT_KEY_ENTER     ' '
#   define AT_KEY_ESCAPE    '`'
#   define AT_KEY_TAB       '\\'
#   define AT_KEY_BACKSPACE '\\'
# endif


// Command list.  Includes extended keys
// such as cursor keys and control/alt keys.

const KeyDesc	keyMap [kNum] = {

		{	'\0','b', kSouthWest,
			"south west",
			 "Movement key.", true},
		{	'\0','j', kSouth,
			"south",
			 "Movement key.", true},
		{	'\0','n', kSouthEast,
			"south east",
			 "Movement key.", true},
		{	'\0','h', kWest,
			"west",
			 "Movement key.", true},
		{	'\0','5', kWait,
			"wait",
			 "Movement key.", true},
		{	'\0','l', kEast,
			"east",
			 "Movement key.", true},
		{	'\0','y', kNorthWest,
			"north west",
			 "Movement key.", true},
		{	'\0','k', kNorth,
			"north",
			 "Movement key.", true},
		{	'\0','u', kNorthEast,
			"north east",
			 "Movement key.", true},

		{	'\0','m', kMove,
			"move",
			 "Move repeatedly in a given direction.", true},

		{	'\0','o', kOpen,
			"open",
			 "Open something.", true},
		{	'\0','c', kClose,
			"close",
			 "Close something.", true},
    { '\0','a', kApply,
			"apply",
			 "Apply a tool.", true},
    { '\0','E', kEtch,
			"etch",
			 "Write or draw on the ground.", false},
    { '\0','B', kBuild,
			"build",
			 "Improve weapons by combining them with items.", true},

       
		{	'\0','t', kTarget,
			"target",
			 "Fire a wielded missile weapon.", true},

		{	'\0','f', kAttack,
			"fight",
			 "Attack something.", true},
		{	'\0','K', kKick,
			"kick",
			 "Kick something.", true},

		{	'\0','v', kVolley,
			"volley",
			 "Throw an item.", true},
		{	'\0','i', kInventory,
			"inventory",
			 "Access items in the inventory.", true},

		{	'\0',';', kLoad,
			"load",
			 "Add to the contents of an item.", true},
		{	'\0',':', kUnload,
			"unload",
			 "Remove the contents of an item.", true},

		{	'\0','w', kWield,
			"wield",
			 "Assign an item to be held in the hands.", true},
		{	'\0','x', kSwitch,
			"switch",
			 "Swap primary and secondary weapons.", true},

		{	'\0','W', kWear,
			"wear",
			 "Put on clothing or apparel.", true},
		{	'\0','T', kTakeOff,
			"take off",
			 "Remove clothing or apparel.", true},


		{	'\0',',', kPickUp,
			"pick up",
			 "Pick up an item from the floor.", true},
		{	'\0','d', kDrop,
			"drop",
			 "Drop an item on the floor.", true},
		{	'\0','D', kDropMany,
			"drop multiple",
			 "Drop multiple items on the floor.", true},

		{	'\0','e', kEat,
			"eat",
			 "Eat something.", true},
		{	'\0','q', kQuaff,
			"quaff",
			 "Drink something.", true},
    { '\0','r', kRead,
      "read",
       "Read something (usually a book)", true},

    { '\0','V', kView,
      "view",
       "View the contents of an adjacent location.", false},
       
		{	'\0','C', kChat,
			"Chat",
			 "Talk to others.", true },

		{	'\0','A', kAdjust,
			"adjust",
			 "Adjust the menu key for an item.", false },

		{	'\0','U', kDisarm,
			"disarm trap",
			 "Attempt to disarm a trap.", true},
		{	'\0','s', kSearch,
			"search",
			 "Search for secret doors and traps.", true},
		{	'\0','p', kPray,
			"prayer",
			 "Begin or conclude a holy prayer.", true},
       
		{	'\0','S', kSkills,
			"skills",
			 "Activate a skill.", true},

		{	'\0','@', kCharInfo,
			"character info",
			 "View a summary of your character.", true },

		{	'\0','M', kMap,
			"map",
			 "View the wilderness map.", true},
		{	'\0','/', kLook,
			"look",
			 "Identify characters on the screen.", true},
		{	'\0','L', kLookAround,
			"look around",
			 "Describe the immediate area.", true },

		{	'\0','>', kDescend,
			"descend",
			 "Descend a staircase; Select the ground.", true},
		{	'\0','<', kAscend,
			"ascend",
			 "Ascend a staircase.", true},
		{	'\0','.', kSelf,
			"self",
			 "Stand still; Select yourself.", true},
		{	'\0','R', kRest,
			"rest",
			 "Remain still for a short while.", true},


		{	'\0','Q', kQuitGame,
			"quit game",
			 "Stop playing and write game data to disk.", true},
		{	'\0','?', kHelp,
			"help",
			 "Use the help system.", true},
		{	'\0','P', kMessages,
			"messages",
			 "View recent messages.", true },
		{	'\0',KEY_UP, kCursorUp,
			"north",
			 "Cursor key.", false},
		{	'\0',KEY_DOWN, kCursorDown,
			"down",
			 "Cursor key.", false},
		{	'\0',KEY_LEFT, kCursorLeft,
			"left",
			 "Cursor key.", false},
		{	'\0',KEY_RIGHT, kCursorRight,
			"right",
			 "Cursor key.", false},

		{	'\0',AT_KEY_ENTER, kEnter,
			"enter",
			 "Selection key.", false},
		{	'\0',' ', kSpace,
			"space",
			 "Selection key.", false},
		{	'\0',AT_KEY_ESCAPE, kEscape,
			"escape",
			 "Cancellation key.", false},
		{	'\0',AT_KEY_TAB, kTab,
			"tab",
			 "Cursor key.", false},
		{	'\0',AT_KEY_BACKSPACE, kBackspace,
			"backspace",
			 "Deletion key.", false},

		{	'\0',KEY_PPAGE, kPageUp,
			"page up",
			 "Cursor key.", false},
		{	'\0',KEY_NPAGE, kPageDown,
			"page down",
			 "Cursor key.", false},
		{	'\0',KEY_HOME, kHome,
			"home",
			 "Cursor key.", false},
		{	'\0',KEY_END, kEnd,
			"end",
			 "Cursor key.", false},
		{	'\0',KEY_IC, kInsert,
			"insert",
			 "Cursor key.", false},
		{	'\0',KEY_DC, kDelete,
			"delete",
			 "Cursor key.", false},

		{	'\0','~', kMemory,
			"memory",
			 "Display memory information.", false}



	};


#endif // INPUT_HPP
