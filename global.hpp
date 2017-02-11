// [global.hpp]
// (C) Copyright 2001 Michael Blackney
#if !defined ( GLOBAL_HPP )
#define GLOBAL_HPP

#include "types.hpp"
#include "enum.hpp"
#include "standard.hpp"

struct global {

  // Member functions:
	static void reset( void );
  static void init( void );
  static void exit( void );


  // Data members:
	static enum QuitType {
    quit_dead,
    quit_save,
    
    quit_false
    } quit;
  static bool save;
  static bool load;
  
	static struct Skins {

		menuSkin main;
		menuSkin inventory;
    menuSkin popup;
		menuSkin action;
		menuSkin quantity;
		menuSkin error;
		menuSkin message;

		Skins( menuSkin m, menuSkin i, menuSkin p, menuSkin a, menuSkin q, menuSkin e, menuSkin ms )
		: main(m), inventory(i), popup(p), action(a), quantity(q), error(e), message(ms) {}
		} skin;

	static struct Info {

		bool map;

		bool rest;

		bool upstairs;
		bool downstairs;

		bool search;
		bool untrap;

		bool reload;
		bool wield;

		bool bottle;
		bool pickup;
		bool inventory;

    bool chat;
    bool shop;

		bool getDir;

    bool help;

		Info( void )
		: map(true), rest(true),
			upstairs(true), downstairs(true),
			search(true), untrap(true),
			reload(true), wield(true),
			bottle(true), pickup(true), inventory(true),
      chat(true),   shop(true),
			getDir(true), help(true) {}

		} isNew;

  static struct Options {
    bool follow_player;
    bool backspace_esc;
    bool delete_esc;
    int  run_max;

    Options( void );
    
    } options;

  // How far the player can see:
  static int view_distance;

	};

#endif // GLOBAL_HPP
