// [global.cpp]
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
#if !defined ( GLOBAL_CPP )
#define GLOBAL_CPP

#include "global.hpp"
#include "file.hpp"
#include "resource.hpp"

// Default Options values:
global::Options::Options(void)
: follow_player(false),
  backspace_esc(true),
  delete_esc(true),

  run_max(30) {}


global::QuitType global::quit = quit_false;
bool global::save = false;
bool global::load = false;

// Default Skins:                  main   invent   popup   action   quant    error   message;
global::Skins global::skin = Skins(sMain, sInvent, sPopUp, sPopUp,  sPopUp, sError, sMessage);

// Novice player info tips:
global::Info global::isNew = Info();

// Options:
global::Options global::options = Options();

// View distance:
int global::view_distance = 0;

void global::reset( void ) {

	skin = Skins(sMain, sInvent, sPopUp, sPopUp,  sPopUp, sError, sMessage);
	isNew = Info();
  options = Options();

  Resource::reset();
	}


void global::exit( void ) {

  File myFile;
  String filename = SAVEDIR + "global.dat";

  myFile[filename].write( (char*) &save, sizeof(save) );
  myFile[filename].write( (char*) &load, sizeof(load) );
  myFile[filename].write( (char*) &skin, sizeof(skin) );
  myFile[filename].write( (char*) &isNew, sizeof(isNew) );
  myFile[filename].write( (char*) &options, sizeof(options) );
  
  Resource::exit( myFile[filename] );
  }

void global::init( void ) {

  File myFile;
  String filename = "global.dat";

  if (! myFile.exists(filename,SAVEDIR) ) return;

  filename = SAVEDIR + filename;
  myFile[filename].read( (char*) &save, sizeof(save) );
  myFile[filename].read( (char*) &load, sizeof(load) );
  myFile[filename].read( (char*) &skin, sizeof(skin) );
  myFile[filename].read( (char*) &isNew, sizeof(isNew) );
  myFile[filename].read( (char*) &options, sizeof(options) );

  quit = quit_false;
  
  Resource::init( myFile[filename] );
  }

#endif // GLOBAL_CPP
