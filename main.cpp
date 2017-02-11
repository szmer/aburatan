// [main.cpp]
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
#define USE_CURSES // Sz. Rutkowski 11.02.2017

#if !defined ( MAIN_CPP )
#define MAIN_CPP

#include "main.hpp"

#include "random.hpp"
#include "global.hpp"
#include "target.hpp"
#include "rawtargt.hpp"
#include "level.hpp"
#include "message.hpp"
#include "timeline.hpp"
#include "grammar.hpp"
#include "catalog.hpp"
#include "list.hpp"
#include "function.hpp"
#include "map.hpp"
#include "food.hpp"
#include "fluid.hpp"
#include "input.hpp"
#include "file.hpp"
#include "money.hpp"
#include "name.hpp"
#include "visuals.hpp"
#include "menu.hpp"
#include "features.hpp"

#include "player.hpp"
#include "quantity.hpp"
#include "weapon.hpp"
#include "ammo.hpp"
#include "clothing.hpp"
#include "bagbox.hpp"

// including implicitly also .cpp files / Sz. Rutkowski 11.02.2017
#include "action.cpp"
#include "actor.cpp"
#include "ammo.cpp"
#include "attack.cpp"
#include "bagbox.cpp"
#include "book.cpp"
#include "catalog.cpp"
#include "clothing.cpp"
#include "cloud.cpp"
#include "compiler.cpp"
#include "counted.cpp"
#include "creature.cpp"
#include "crlist.cpp"
#include "dialog.cpp"
#include "diceroll.cpp"
#include "dir.cpp"
#include "door.cpp"
#include "dun_test.cpp"
#include "enum.cpp"
#include "error.cpp"
#include "event.cpp"
#include "feature.cpp"
#include "features.cpp"
#include "file.cpp"
#include "fluid.cpp"
#include "food.cpp"
#include "function.cpp"
#include "global.cpp"
#include "grammar.cpp"
#include "input.cpp"
#include "item.cpp"
#include "itemlist.cpp"
#include "level.cpp"
#include "list.cpp"
#include "main.cpp"
#include "map.cpp"
#include "menu.cpp"
#include "message.cpp"
#include "misc.cpp"
#include "money.cpp"
#include "monster.cpp"
#include "name.cpp"
#include "path.cpp"
#include "player.cpp"
#include "playerui.cpp"
#include "point.cpp"
#include "quantity.cpp"
#include "quest.cpp"
#include "random.cpp"
#include "rawtargt.cpp"
#include "rectngle.cpp"
#include "resource.cpp"
#include "scope.cpp"
#include "serial.cpp"
#include "skill.cpp"
#include "sorted.cpp"
#include "stairs.cpp"
#include "standard.cpp"
#include "stats.cpp"
#include "string.cpp"
#include "target.cpp"
#include "temp.cpp"
#include "terrain.cpp"
#include "tile.cpp"
#include "timeline.cpp"
#include "trap.cpp"
#include "unique.cpp"
#include "viewable.cpp"
#include "visuals.cpp"
#include "weapon.cpp"
#include "widget.cpp"

Target playercreate( void ) {
// Creates and returns a new player.
// Returns a pointer to NULL if player creation exited.

  Player::Option player_opt;
	Target player = Player::create( player_opt );

	Message::flush();

 	return player;
	}


void saveinfo( void ) {

  File myFile;
  bool saveGame = global::quit == global::quit_save;

	myFile[SAVEINFO].write( (char*) &saveGame, sizeof(saveGame));

  }

void LegacyMessage( Target ) {
// This function sets up the player's initial message
// based on class, etc.

  Pane class_pane( global::skin.main );
  class_pane.setTitle("Welcome");

  TextArea desc_title;

  desc_title.setText("You have travelled to the dead city Texarkana seeking your destiny.  You cover your nose at the noxious stench of the building nearby.      The sun is rising in the west.");
  desc_title.setBorder( Rectangle(1,0,37,4) );
  class_pane.add( desc_title );

  class_pane.display();
  Keyboard::get();
  }

void make() {

  global::reset();

  //global::load = true;
  
  	Screen::cls();

    Name::reset();
  	Timeline::reset();
  	Serial::reset();

  	// Create a player; exit if user 'q'uits:
  	Target player = playercreate();
    if (!player) return;

    SpecialRoom::loadData();
    
  	Map::create();

    // Before any maps are displayed, we must make sure that
    // the menus and maps are only drawn in allocated screen space:

    Screen::setMenuLimit( Rectangle(Screen::mapxmin, Screen::mapymin,
                                    Screen::mapxmax, Screen::mapymax)  );
                                    
    Screen::setImageLimit( Rectangle(Screen::mapxmin, Screen::mapymin,
                                     Screen::mapxmax, Screen::mapymax) );
    
    // Locate and focus on the initial level:
  	Level &myLev = *Map::focus( Map::startPoint() );

   	// Place the player:
   	int x, y;
    Point lev_pointA = myLev.getBorder().geta();
   	do {
   		x = Random::randint(lev_width)+lev_pointA.getx();
   		y = Random::randint(lev_height)+lev_pointA.gety();
   		} while ( ! myLev.getTile( Point3d(x,y) )->capable(aStartTurn, player ) );

   	myLev.getTile( Point3d(x,y) )->perform( aContain, player );

  global::load = false;
    
  LegacyMessage(player);
  Timeline::run();


  global::save = true;

    Screen::hidecursor();
    
    Screen::resetMenuLimit();
    Screen::resetImageLimit();

  	Message::clr();
  	Grammar::clr();

  	Map::save();
  	Timeline::save();

  	Catalogue::clr();

  	Serial::save();

    Name::save();

    saveinfo();

    SpecialRoom::clearData();

    Screen::cls();
    Screen::showcursor();
  global::save = false;


  global::exit();
	}

void restore() {
  global::init();

  global::load = true;
    SpecialRoom::loadData();

    Name::load();
  	Screen::cls();
  	Serial::load();

  	Map::load();

    Screen::setMenuLimit( Rectangle(Screen::mapxmin, Screen::mapymin,
                                    Screen::mapxmax, Screen::mapymax ) );
    Screen::setImageLimit( Rectangle(Screen::mapxmin, Screen::mapymin,
                                     Screen::mapxmax, Screen::mapymax) );

  	Player::loadInfo();
  	Map::get(Player::levLoc)->redraw();
    Player::get().updateStatus();
  	Timeline::load();

  global::load = false;

  assert (&Player::get() != NULL);
  Player::get().perform(aLOS,Target());

  //Map::get(Player::levLoc)->calcLights();
  //Map::calcLights();
  Screen::update();

  Timeline::run();



  global::save = true;
  
    Screen::hidecursor();

    Screen::resetImageLimit();
    Screen::resetMenuLimit();
  	Message::clr();
  	Grammar::clr();

  	Map::save();
  	Timeline::save();

  	Catalogue::clr();

  	Serial::save();

    Name::save();

    saveinfo();

    SpecialRoom::clearData();

    Screen::cls();
    Screen::showcursor();
  global::save = false;
    
  global::exit();
	}


bool quit = false;
void exit () { quit = true; }

List<FunctionItem> getoptions( void ) {

  List<FunctionItem> menuList;

  // Can always make new game:
	menuList.add( FunctionItem( &make,  "New Game", 'n' ) );

  // Check for save file:
  if ( File::exists(SAVEINFO) ) {

    File myFile;
    bool saveGame = false;

		myFile[SAVEINFO].read( (char*) &saveGame, sizeof(saveGame));

    if (saveGame)
  	  menuList.add( FunctionItem( &restore,  "Restore", 'r' ) );
    }

  // Can always quit:
	menuList.add( FunctionItem( &exit,  "Quit", 'q' ) );
  
  return menuList;
  }


void mainmenu() {

	List<FunctionItem> menuList;

  menuList = getoptions();

	Menu<FunctionItem> mainMenu( menuList, global::skin.main );

	String title = "Abura Tan";

	mainMenu.setTitle( title );
	mainMenu.display();

	Command key = kInvalid;

	while (!quit) {
		key = mainMenu.getCommand();

		switch (key) {
			case kEnter:
				mainMenu.hide();

				mainMenu.get().call();

        menuList = getoptions();
				mainMenu.redraw();

				global::quit = global::quit_false;
				break;
												default: break;
			}

		mainMenu.display();

		}

	}

/*

// Functions for Greg McIntyre's Libshadowcast:

int mapOpaque(void *map, int x, int y) {

	if ( ascending( -1, x, lev_width )
		&& ascending( -1, y, lev_height )  )
		return ((Target***)map)[x][y]->raw()->getVal( attrib_opaque );

	return true;
	}

int mapAlight(void *map, int x, int y, int dx, int dy, void *src) {

	if ( ascending( -1, x, lev_width )
		&& ascending( -1, y, lev_height )  )
		return ! ((Target***)map)[x][y]->raw()->perform( aSetVisible );

	return false;
	}*/

#define AT_MAIN
#ifdef AT_MAIN

int main() {

	Random::randomize();
	global::reset();

	Screen::init();
	Keyboard::init();

  mainmenu();

	Screen::cls();

  int ret_val = printTotalDeletes();
  
	Screen::end();

	return ret_val;
	}


#endif // AT_MAIN

#endif // MAIN_CPP

