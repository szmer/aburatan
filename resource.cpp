// [resource.cpp]
// (C) Copyright 2004 Michael Blackney
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
#if !defined ( RESOURCE_CPP )
#define RESOURCE_CPP

#include "resource.hpp"
#include "features.hpp"
#include "player.hpp"

void Resource::reset( void ) {
  // Go through all the data arrays and change:
  // - num_killed and num_generated to 0
  // - id_form to false

  // Reset all Misc. static classes:
  MechMines::tTHIS = Target(NULL);
  MechMines::connections.clr();

  Sewers::tTHIS = Target(NULL);
  Sewers::wet_levels.clr();
  Sewers::dry_levels.clr();
  Sewers::to_connect.clr();
  Sewers::water_track.clr();
  Sewers::stair_locs.clr();
  Sewers::entrance = OFF_MAP;
  Sewers::levels_planned = 0;
  Sewers::level_remainder = 0;
  Sewers::finished = false;

  }
  
void Resource::init( std::fstream &istream ) {
  // Load all num_killed, num_generated and id_flags from file:

  // --- Load all Misc. static classes:
  // Mech Mines:
  istream >> MechMines::tTHIS;

  // Sewers:
  istream >> Sewers::tTHIS;
    
  LIST_LOAD(istream,Point3d,Sewers::wet_levels);
  LIST_LOAD(istream,Point3d,Sewers::dry_levels);
  LIST_LOAD(istream,Point3d,Sewers::stair_locs);
  LIST_LOAD(istream,Point3d,Sewers::to_connect);
  LIST_LOAD(istream,Point3d,Sewers::water_track);

  ITEM_LOAD(istream,Sewers::levels_planned);
  ITEM_LOAD(istream,Sewers::level_remainder);
  ITEM_LOAD(istream,Sewers::entrance);
  ITEM_LOAD(istream,Sewers::finished);
  }
  
void Resource::exit( std::fstream &ostream ) {
  // Save all num_killed, num_generated and id_flags to file:

  // --- Save all Misc. static classes:
  // MechMines:
  ostream << MechMines::tTHIS;

  // Sewers:
  ostream << Sewers::tTHIS;
    
  LIST_SAVE(ostream,Sewers::wet_levels);
  LIST_SAVE(ostream,Sewers::dry_levels);
  LIST_SAVE(ostream,Sewers::stair_locs);
  LIST_SAVE(ostream,Sewers::to_connect);
  LIST_SAVE(ostream,Sewers::water_track);
  
  ITEM_SAVE(ostream,Sewers::levels_planned);
  ITEM_SAVE(ostream,Sewers::level_remainder);
  ITEM_SAVE(ostream,Sewers::entrance);
  ITEM_SAVE(ostream,Sewers::finished);

  // Save the player: (to its own file)
  Player::save();
  
  reset();
  }
  
Target Resource::wish( String ) {
  // Look for an item with this name from all the
  // item classes, and generate if found.

  // ...

  // Otherwise, return NULL target:
  return Target();
  }



#endif // RESOURCE_CPP
