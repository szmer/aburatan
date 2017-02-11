// [level.cpp]
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
#if !defined ( LEVEL_CPP )
#define LEVEL_CPP

#define LEVEL__HARNESS

#include "compiler.hpp"

#ifdef OBSOLETE_INCLUDE
#  include <stdlib.h>
#else
#  include <cstdlib>
#endif

#include "define.hpp"
#include "level.hpp"
#include "standard.hpp"
#include "visuals.hpp"
#include "random.hpp"
#include "file.hpp"
#include "enum.hpp"
#include "timeline.hpp"
#include "file.hpp"
#include "input.hpp"
#include "catalog.hpp"
#include "message.hpp"
#include "trap.hpp"
#include "tile.hpp"
#include "quantity.hpp"
#include "features.hpp"
#include "door.hpp"
#include "global.hpp"
#include "grammar.hpp"
#include "dun_test.hpp"
#include "monster.hpp"
#include "player.hpp"

//Rectangle Level::borderRect(0, 0, lev_width-1, lev_height-1);

const struct TileDetails {

  const char *name;
  char appearance;
  int opacity;
  bool adj;
  bool block;


  } tile_details[tt_num] = {
    {"ground", '.',   0, false, false },
    {"floor",  '.',   0,  true, false },
    {"river",  ',',   0, false, false },
    {"tree",   'T',  10, false, false },
    {"cactus", 'Y',  10, false, false },
    {"bush",   'T',  10, false, false },
    {"wall",   '#', 100,  true, true },
    {"fence",  ':',  20,  true, true },

    };

Level::Level( const Point3d &p3d ) {
	mapCoord = p3d;
  borderRect += Point(p3d.getx()*lev_width, p3d.gety()*lev_height);
  borderRect.getb() += Point(lev_width-1,lev_height-1);


	int x, y;

	//tiles = new Target **[ lev_width ];

	//for (x = 0; x < lev_width; ++x)
	//	tiles[x] = new Target *[ lev_height ];

  // Clear the tiles arrays:
  // Simple:
  memset( (void*) tile_simple, 0, sizeof( TileSimple ) * lev_width * lev_height );

  // Complex
	for (y = 0; y < lev_height; ++y)
		for (x = 0; x < lev_width; ++x) {
      tile_complex[x][y].inventory = NULL;
      tile_complex[x][y].tile = Target(NULL);
      //tile_complex[x][y].feature = Target(NULL);
      }

  //ortho = NULL;
  
  odor = od_none;
	}

Level::~Level( void ) {

  // At the moment, the reason why loading and saving
  // Levels takes so damn long is the serialisation
  // method.  Rather than make those parts faster
  // (since that might mean a total restructuring of
  // that code) we will make the Level save code move
  // all serialised objects to the Map class, into
  // in-memory storage bins.  These bins can be saved
  // when moving up or downstairs but won't be saved
  // when moving laterally.

  //Message::add(String("Close (")<<mapCoord.getx()<<","<<mapCoord.gety()<<","<<mapCoord.getz()<<")");

	File file;

	String filename = SAVEDIR;
	filename += (long) mapCoord.getx();
	filename += (char) '_';
	filename += (long) mapCoord.gety();
	if (mapCoord.getz() >= 0) filename += (char) '_';
	filename += (long) mapCoord.getz();
	filename += ".LEV";

	std::fstream &myFile = file[filename];

	myFile.seekp(0);

  // Save odor:
  myFile.write( (char*) &odor, sizeof(odor) );

  // Save visuals:
	int x, y;
	//int numData;

  int crNum;
  // Sleep clouds:
  if ( clouds.reset() && !global::save )
  do {
    crNum = clouds.numData();
		clouds.get()->doPerform(aSleep);
		} while ( (clouds.numData() < crNum && crNum > 1) || clouds.next() );

  // The level is closed, so we can't see the tiles:
  // Clear the visibility status for all tiles:
  for (y = 0; y < lev_height; ++y)
	 	for (x = 0; x < lev_width; ++x)
      tile_simple[x][y].visible = 0;//.viewDir.reset();


  // Simple :
  myFile.write( (char*) &tile_simple[0][0], sizeof( TileSimple ) * lev_width * lev_height );

  // Map bin:
  Map::Bin *map_bin = new Map::Bin(mapCoord);
  Map::binAdd( map_bin );
  
  // Complex :
  for (y = 0; y < lev_height; ++y)
	 	for (x = 0; x < lev_width; ++x) {
    
      //TileSimple &tile_s = tile_simple[x][y];
      TileComplex &tile_c = tile_complex[x][y];

      // Check to see if the name needs to be saved:
      //if (tile_s.graffiti.generated())
      //  tile_s.graffiti.toFile();
        
      // There's at least one reference to this tile (i.e. this one)
      // but if there are no more, we no longer need the tile:
      if (  !tile_c.inventory
        && !!tile_c.tile
        &&   tile_c.tile.numRefs() <= 1)
        tile_c.tile = Target(NULL);

      if (!!tile_c.tile) {
        ( (Tile*) tile_c.tile.raw() )->level = NULL;
        map_bin->addTile( Point3d(x,y,mapCoord.getz()) + Point3d(borderRect.geta()), tile_c.tile );
        }
        
      if ( tile_c.inventory )
      while ( tile_c.inventory->reset() ) {

        tile_c.tile->allocInventory();
        tile_c.inventory->get()->doPerform(aSleep);
        if ( tile_c.inventory->canGet() )
          tile_c.tile->inventory->add( tile_c.inventory->remove() );
      
        }
        
      delptr(tile_c.inventory);
      }
    
	// Save creatures list:
  //numData = creatures.numData();

	//myFile.write( (char *) &numData, sizeof(numData) );
  /*
	if ( creatures.reset() )
	do {
    long key = creatures.lock();
		//myFile << creatures.get();
    creatures.load(key);
		} while ( creatures.next() );
  */

	// Save features:
	//numData = features.numData();

	//myFile.write( (char *) &numData, sizeof(numData) );
	if ( features.reset() )
	do {
    map_bin->addFeature( features.get() );
		} while ( features.next() );

  //if (ortho)
  //  delarr(ortho);

  /* // OLD VERSION:::
	File file;

	String filename = SAVEDIR;
	filename += (long) mapCoord.getx();
	filename += (char) '_';
	filename += (long) mapCoord.gety();
	if (mapCoord.getz() >= 0) filename += (char) '_';
	filename += (long) mapCoord.getz();
	filename += ".LEV";

	std::fstream &myFile = file[filename];

	myFile.seekp(0);

  // Save odor:
  myFile.write( (char*) &odor, sizeof(odor) );


  // Save visuals:
	int x, y;
	int numData;

  int crNum;
  // Sleep clouds:
  if ( clouds.reset() && !global::save )
  do {
    crNum = clouds.numData();
		clouds.get()->perform(aSleep);
		} while ( (clouds.numData() < crNum && crNum > 1) || clouds.next() );

	// Save lights list:
  numData = lights.numData();

	myFile.write( (char *) &numData, sizeof(numData) );
	if ( lights.reset() )
	do {
    long key;
    key = lights.lock();
		myFile << lights.get();
    lights.load(key);
		} while ( lights.next() );

  // Sleep Creatures:
  if ( creatures.reset() && !global::save )
  do {
    crNum = creatures.numData();
    assert(! creatures.get()->istype(tg_player)); // Error::fatal("Player saved!");
		creatures.get()->perform(aSleep);
		} while ( (creatures.numData() < crNum && crNum > 1) || creatures.next() );


  // Simple :
  myFile.write( (char*) &tile_simple[0][0], sizeof( TileSimple ) * lev_width * lev_height );
  
  // Complex :
  for (y = 0; y < lev_height; ++y)
	 	for (x = 0; x < lev_width; ++x) {
    
      TileComplex &tile_c = tile_complex[x][y];

      // There's at least one reference to this tile (i.e. this one)
      // but if there are no more, we no longer need the tile:
      if (  !tile_c.inventory
        && !!tile_c.tile
        &&   tile_c.tile.numRefs() <= 1)
        tile_c.tile = Target(NULL);
        
      myFile << tile_c.tile;
      tile_c.tile.save();
        
      int num_data = 0;
      if ( tile_c.inventory )
        num_data = tile_c.inventory->numData();

      myFile.write( (char*) &num_data, sizeof(num_data) );
        
      if ( num_data )
      while ( tile_c.inventory->reset() ) {

        myFile << tile_c.inventory->get();
        tile_c.inventory->remove().save();
      
        }
        
      delptr(tile_c.inventory);
      }
    
	// Save creatures list:
  numData = creatures.numData();

	myFile.write( (char *) &numData, sizeof(numData) );
	if ( creatures.reset() )
	do {
    long key = creatures.lock();
		myFile << creatures.get();
    creatures.load(key);
		} while ( creatures.next() );

	// Save features:
	numData = features.numData();

	myFile.write( (char *) &numData, sizeof(numData) );
	if ( features.reset() )
	do {
		myFile << features.get();
		} while ( features.next() );

  if (ortho)
    delarr(ortho);
  */
  }


void Level::packUp( void ) {
// Prepare the level for putting away
	int x, y;

	for ( y = 0; y < lev_height; ++y )
		for ( x = 0; x < lev_width; ++x ) {
//			tiles[x][y]->raw()->unlight();
//			tiles[x][y]->raw()->unview();
			}
  }
  

Level *Level::load( const Point3d &mapLoc ) {

	Level *lev = new Level( mapLoc );
	lev->loadLevel();
	return lev;

 	}


void Level::loadLevel( void ) {
//~
  //Message::add(String("Open (")<<mapCoord.getx()<<","<<mapCoord.gety()<<","<<mapCoord.getz()<<")");

	File file;

	String filename = SAVEDIR;
	filename += (long) mapCoord.getx();
	filename += (char) '_';
	filename += (long) mapCoord.gety();
	if (mapCoord.getz() >= 0) filename += (char) '_';
	filename += (long) mapCoord.getz();
	filename += ".LEV";

	std::fstream &myFile = file[filename];

	assert (!! myFile); // Error::fatal(  "Level load error: Error opening file " + filename);

	Target read;

	int x, y;
	//int numData
  //int i;

  // Load odor:
  myFile.read( (char*) &odor, sizeof(odor) );
  
	// load visuals:
  Screen::hidecursor();
  

	// Load lights:
  /*
	myFile.read( (char *) &numData, sizeof(numData) );
	for (i = 0; i < numData; i++) {
		myFile >> read;
		lights.add( read );
		}
  */

  // Map bin:
  Map::Bin *map_bin = Map::binRemove(mapCoord);

  // Load Tiles:
  // Simple :
  myFile.read( (char*) &tile_simple[0][0], sizeof( TileSimple ) * lev_width * lev_height );
  
  // Complex :
  for (y = 0; y < lev_height; ++y)
	 	for (x = 0; x < lev_width; ++x)
      tile_complex[x][y].inventory = NULL;
      
  // Add back all tile objects:
  if (map_bin && map_bin->tileList().reset())
  do {
    Point3d p3d = map_bin->tileList().get().p3d;

    assert( borderRect.contains(p3d) );
    p3d -= Point3d(borderRect.geta());

    TileComplex &tile_c = tile_complex[p3d.getx()][p3d.gety()];
    tile_c.tile = map_bin->tileList().get().target;

    if ( tile_c.tile->inventory && tile_c.tile->inventory->reset() ) {
      tile_c.inventory = new Sorted<Target>;

      do {

        tile_c.inventory->add( tile_c.tile->inventory->remove() );
        tile_c.inventory->get()->doPerform(aWake);

        } while (tile_c.tile->inventory->reset());
      }
    
    } while ( map_bin->tileList().next() );


	// Load creatures:
	//myFile.read( (char *) &numData, sizeof(numData) );
	//for (i = 0; i < numData; i++) {
		//myFile >> read;
		//creatures.add( read );
		//}

  //if (!global::load && creatures.reset() )
  //do {
	//	creatures.get()->perform(aWake);
	//	} while ( creatures.next() );

	// Load features:
	//myFile.read( (char *) &numData, sizeof(numData) );

	//for ( i = 0; i < numData; i++) {
		//myFile >> read;
		//features.add( read );
		//read->gettype(); // Will load feature and fix links
		//}

	if ( map_bin && map_bin->featureList().reset() )
	do {
    features.add( map_bin->featureList().get() );
		} while ( map_bin->featureList().next() );


  Screen::showcursor();
  Screen::update();


  // free:
  delptr(map_bin);
  
/* // OLD VERSION::::
	File file;

	String filename = SAVEDIR;
	filename += (long) mapCoord.getx();
	filename += (char) '_';
	filename += (long) mapCoord.gety();
	if (mapCoord.getz() >= 0) filename += (char) '_';
	filename += (long) mapCoord.getz();
	filename += ".LEV";

	std::fstream &myFile = file[filename];

	assert (!! myFile); // Error::fatal(  "Level load error: Error opening file " + filename);
        

	Target read;

	int x, y;
	int numData, i;

  // Load odor:
  myFile.read( (char*) &odor, sizeof(odor) );
  
	// load visuals:
  Screen::hidecursor();
  
// ATM Saving of greyed-out level visuals is broken:
//	for ( y = 0; y < lev_height; ++y )
//		for ( x = 0; x < lev_width; ++x ) {
//
//			Image i;
//			myFile.read( (char*) &i, sizeof(i) );
//			MAPDRAW(x,y,i);
//			}

	// Load lights:
	myFile.read( (char *) &numData, sizeof(numData) );
	for (i = 0; i < numData; i++) {
		myFile >> read;
		lights.add( read );
		}

	// Load tiles:
	//for (y = 0; y < lev_height; ++y)
	//	for (x = 0; x < lev_width; ++x) {
	//		myFile >> read;
	//		*tiles[x][y] = read;
	//		read->perform(aInitLOS);
  //  	tiles[x][y]->raw();
	//		}
	//tiles[59][10]->raw()->gettype();
  
  // Load Tiles:
  // Simple :
  myFile.read( (char*) &tile_simple[0][0], sizeof( TileSimple ) * lev_width * lev_height );
  
  // Complex :
  for (y = 0; y < lev_height; ++y)
	 	for (x = 0; x < lev_width; ++x) {

      TileComplex &tile_c = tile_complex[x][y];
      
      myFile >> tile_c.tile;
      //myFile >> tile_c.feature;
      
      int num_data = 0;
      myFile.read( (char*) &num_data, sizeof(num_data) );

      // If there's an inventory, read it:
      if (num_data) {
        // Allocate an inventory list object:
        tile_c.inventory = new Sorted<Target>;

        assert(!!tile_c.tile);
        
        Target read_in;
        for (int i = 0; i < num_data; ++i ) {
          myFile >> read_in;
          tile_c.inventory->add( read_in );
          read_in->parent = tile_c.tile->THIS;
          }
          
        }
      else
        tile_c.inventory = NULL;
      
      }


	// Load creatures:
	myFile.read( (char *) &numData, sizeof(numData) );
	for (i = 0; i < numData; i++) {
		myFile >> read;
		creatures.add( read );
		}

  if (!global::load && creatures.reset() )
  do {
		creatures.get()->perform(aWake);
		} while ( creatures.next() );

	// Load features:
	myFile.read( (char *) &numData, sizeof(numData) );

	for ( i = 0; i < numData; i++) {
		myFile >> read;
		features.add( read );
		read->gettype(); // Will load feature and fix links
		}

  Screen::showcursor();
  Screen::update();
*/
	}


void Level::redraw( void ) {
// Re-draw the whole level

//	File file;
//	for (int y = 0; y < lev_height; y++)
//		for (int x = 0; x < lev_width; x++)
//      tileDraw( Point3d(x,y,mapCoord.getz()) );

	}

void Level::incOdor( int delta ) {
  odor = (Odor) (odor + delta);
  }

/*
void Level::allocOrtho( void ) {


  
  int x, y;
  if (!ortho) {
    ortho = new orthoRec[ lev_height * lev_width ];

  	for (y = 0; y < lev_height; y++)
  		for (x = 0; x < lev_width; x++) {
//        ortho[x+y*lev_width].opacity = tiles[x][y]->raw()->getVal( attrib_opaque );
        ortho[x+y*lev_width].lit.reset();
//        ortho[x+y*lev_width].visible.reset();
        }
    }
    
  }
*/
/*
void Level::clearOrtho( void ) {
  if (ortho)
    delarr(ortho);
  }
*/
/*
void Level::sunrise( void ) {
  int x, y;

  for (y = 0; y < lev_height; y++)
  	for (x = 0; x < lev_width; x++)
//			tiles[x][y]->raw()->perform(aSunrise);
      
  clearOrtho();
  calcLights();
  redraw();
  Screen::update();
  }

void Level::sunset( void ) {
  int x, y;

  for (y = 0; y < lev_height; y++)
  	for (x = 0; x < lev_width; x++) {
//			tiles[x][y]->raw()->unlight();
//			tiles[x][y]->raw()->perform(aSunset);
      }

  clearOrtho();
  calcLights();
  redraw();
  Screen::update();
  }
*/

/*
void Level::calcOpacity( Point ) {

  assert(0);

  if (!ortho) allocOrtho();
  
  int x = p.getx()%lev_width, y = p.gety()%lev_height;
  ortho[x+y*lev_width].opacity = tiles[x][y]->raw()->getVal( attrib_opaque );

  }
*/
/*
void Level::setOpacity( int, int, int ) {

  assert(0);

  if (!ortho) allocOrtho();
  
  ortho[(x%lev_width)+(y%lev_height)*lev_width].opacity = o;

  }
*/
/*
int Level::getOpacity( int x, int y ) {
//  assert(0);

  if (!ortho) allocOrtho();
  
  return ortho[(x%lev_width)+(y%lev_height)*lev_width].opacity;

//  return 0;
  return tileVal( Point3d(x,y)+Point3d(borderRect.geta(),mapCoord.getz()), attrib_opaque );
  }
*/
/*
void Level::setLit( int x, int y, const Target &t ) {

  // We have to trim the ints used for location
  // so that we can use them as array indicies:
  x %= lev_width;
  y %= lev_height;
  
  Orthog dir;
  Point location = t->getlocation().toPoint();
  
  // Trim this location too:
  location.set( location.getx()%lev_width, location.gety()%lev_height );

  if (!ortho) allocOrtho();

  if ( Point(x,y) == location )
    dir.self = true;
  else {
    if ( x < location.getx() ) dir.e = true;
    else if ( x > location.getx() ) dir.w = true;
    
    if ( y < location.gety() ) dir.s = true;
    else if ( y > location.gety() ) dir.n = true;
    }
  
//  if ( ortho[x+y*lev_width].lit != dir || dir.self) {
//    tiles[x][y]->raw()->light(dir);
    ortho[x+y*lev_width].lit = dir;
//    }
    
  }
*/

void Level::addFeature( const Target &f ) {
	assert ( f->istype( tg_feature) ); // Error::fatal("Must only add features to Level::addFeature!");
	features.add( f );
	}
/*
void Level::addCreature( const Target &c ) {
	assert ( c->istype( tg_creature) ); // Error::fatal("Must only add creatures to Level::addCreature!");
	creatures.add( c );
	}
*/
void Level::removeFeature( const Target &f ) {
	assert ( f->istype( tg_feature) ); // Error::fatal("Must only remove features from Level::removeFeature!");
	if ( features.find(f))
		features.remove();
	else {
    assert(0);
    Message::add("Level::removeFeature failed!");
    }
	}

/*
void Level::removeCreature( const Target &c ) {
	assert ( c->istype( tg_creature) ); // Error::fatal("Must only remove creatures from Level::removeCreature!");
	if ( creatures.find(c))
		creatures.remove();
	else {
    //assert(0);
    Message::add("Level::removeCreature failed!");
    }
	}
*/
void Level::addCloud( const Target &c ) {
	assert ( c->istype( tg_cloud ) ); //Error::fatal("Must only add clouds to Level::addCloud!");
	clouds.add( c );
	}
  
void Level::removeCloud( const Target &c ) {
	assert ( c->istype( tg_cloud ) ); // Error::fatal("Must only remove clouds from Level::removeCloud!");
	if ( clouds.find(c))
		clouds.remove();
	else {
    assert(0);
    Message::add("Level::removeCloud failed!");
    }
	}

/*
void Level::addLight( const Target &l ) {

	//assert ( !lights.find(l) );
  
	lights.add( l );
  l->perform( aLight );
	}

void Level::removeLight( const Target &l ) {
	if ( lights.find(l)) {

		lights.remove();

    //set all cells within radius to unlit:
    int x,y;
    int radius = l->getVal(attrib_light_radius);
    Point lLoc = l->getlocation().toPoint();

  	for ( y = max(0,lLoc.gety()-radius); y < min((int)lev_height,lLoc.gety()+radius+1); ++y )
   		for ( x = max(0,lLoc.getx()-radius); x < min((int)lev_width,lLoc.getx()+radius+1); ++x ) {
      
//   			tiles[x][y]->raw()->unlight();
        assert (ortho); // Error::fatal("Light removed before LOS calculated!");

//        ortho[x+y*lev_width].lit.reset( tiles[x][y]->raw()->getVal(attrib_sunlit) );

        }
        
    calcLights();

    }

	else {
//		Error::fatal(
    //assert(0);
    Message::add("Level::removeLight failed!");
    }
  }

void Level::calcLights( void ) {

  //light cells:
  if ( lights.reset() )
  do {

    lights.get()->perform( aLight );
    
    } while ( lights.next() );

  Screen::update();
    
  }
*/

void Level::setBorder( Rectangle b ) { borderRect = b; }
Rectangle Level::getBorder( void ) { return borderRect; }

Point3d Level::loc( void ) {
	return mapCoord;
	}


void Level::create( void ) {

	placeTerrain();

	// Check the level requirements:
	List<Req> &req = Map::get(mapCoord).Requirements();

	if ( req.reset() )
	do {
  
		req.get().create( * this );

		} while ( req.next() );

	req.clr();

	// place monsters:
	placeMons();
	}

List<Target> &Level::featureList( void ) {
	return features;
	}

//List<Target> &Level::creatureList( void ) {
//	return creatures;
//	}


void Level::placeDoors( void ) {
	// place doors:
  /*
	int x, y;

	for ( y = 1; y < lev_height-1; ++y )
		for ( x = 1; x < lev_width-1; ++x )
			if (tiles[x][y]->raw()->istype(tg_floor) &&
					tiles[x][y]->raw()->getVal(attrib_empty) ) {

				bool nsWall = ( tiles[x][y-1]->raw()->istype(tg_wall)
										 && tiles[x][y+1]->raw()->istype(tg_wall) )?true:false;
				bool ewWall = ( tiles[x-1][y]->raw()->istype(tg_wall)
										 && tiles[x+1][y]->raw()->istype(tg_wall) )?true:false;

				bool nsCoor = ( tiles[x][y-1]->raw()->istype(tg_floor)
                     && tiles[x][y-1]->raw()->getVal(attrib_empty)
										 && tiles[x][y+1]->raw()->istype(tg_floor)
                     && tiles[x][y+1]->raw()->getVal(attrib_empty) )?true:false;
				bool ewCoor = ( tiles[x-1][y]->raw()->istype(tg_floor)
                     && tiles[x-1][y]->raw()->getVal(attrib_empty)
										 && tiles[x+1][y]->raw()->istype(tg_floor)
                     && tiles[x+1][y]->raw()->getVal(attrib_empty) )?true:false;

				int diagWall = 0, diagCoor = 0;

				if ( tiles[x-1][y-1]->raw()->istype(tg_floor) ) diagCoor++; else diagWall++;
				if ( tiles[x-1][y+1]->raw()->istype(tg_floor) ) diagCoor++; else diagWall++;
				if ( tiles[x+1][y-1]->raw()->istype(tg_floor) ) diagCoor++; else diagWall++;
				if ( tiles[x+1][y+1]->raw()->istype(tg_floor) ) diagCoor++; else diagWall++;

				// Doors in ns walls:
				if ( ( ( nsWall && ewCoor ) || ( nsCoor && ewWall ) )
						&& ( diagCoor > 0 ) && Random::randbool()

						 ) {
           Door::Option door_opt;
           door_opt.hidden = (Random::randint(5) == 0);
           door_opt.material = door_opt.hidden?m_stone:m_wood;
					 tiles[x][y]->raw()->perform( aContain, Door::create( door_opt ) );
					 }

				}

  */
	}


void Level::placeMons( void ) {

  // See if the level features want to place their own monsters:
  bool placed = false;

	if ( features.reset() )
	do {
    if ( features.get()->doPerform(aPlaceMons) )
      placed = true;
		} while ( features.next() );
  
  if (placed) return;
  

  // 1/3 chance of a monster per level:
  int num_mons = Random::randint(3) == 0;

  Map::levType type;
	type = Map::type( mapCoord.toPoint() );
  if (mapCoord.getz() < 0) {
    // Underground monster generation should still be influenced by location
    // but if you uncomment the next line it won't be:
    //type = Map::any;
    
    // More monsters underground:
    //num_mons += Random::randint(4);
    
    // At the moment since monsters are not to be created in
    // levels with all rock and no tunnel, we supress natural
    // monster generation.  Monsters will be created by
    // special features only.
    return;
    }


  List<Target> monsters;

  Monster::Option monst_opt;
  monst_opt.odor = odor;
  monst_opt.habitat = type;
  monst_opt.max_xp = 100; // Only generate puny monsters
  monst_opt.theme = th_wild; // Only _wild mons in wilderness
  for (int i = 0; i < num_mons; ++i) {
  	monsters.add( Monster::create(monst_opt) );
    }
    
  sprinkle(monsters);

  assert(monsters.numData() == 0);

  }


void Level::sprinkle( List<Target> &monsters ) {
// Take a list of monsters; apply liberally.
// Can be used to place a list of Items of any type.

  int x, y, index,
      z = mapCoord.getz();

  if (!monsters.reset())
    return;
  
	Target mon = monsters.get();
  // Make a list of empty squares:

  // Assumes first monster in list shares same placement
  // rules as the entire list.
  
  List<Point3d> empties;
  for (y = borderRect.getya(); y <= borderRect.getyb(); ++y)
    for (x = borderRect.getxa(); x <= borderRect.getxb(); ++x)
      if ( tileCapable( Point3d(x,y,z), aStartTurn, mon ) )
        empties.add( Point3d(x,y,z) );


	while (monsters.reset()) {

    assert(empties.reset());

    index = Random::randint(empties.numData());
    empties.load(index);
    
    tileAdd( getTile(empties.get()), monsters.get() );
    // If the 'monster' item is a blocker, remove this tile:
    if (monsters.remove()->isCapable(aBlock))
      empties.remove();
		}

	};


void Level::placeTerrain( void ) {

//  Field::create( borderRect, *this, m_grass );
//  return;
  
	if ( mapCoord.getz() == 0 ) {

		int i;

		Map::levType type[5];

		type[0] = Map::type( mapCoord.toPoint() );

		if (mapCoord.gety() > 0)
			type[1] = Map::type( mapCoord.toPoint() + Point( 0,-1) );
		else
			type[1] = type[0];

		if (mapCoord.gety() < lev_height-1)
			type[2] = Map::type( mapCoord.toPoint() + Point( 0, 1) );
		else
			type[2] = type[0];

		if (mapCoord.getx() < lev_width-1)
			type[3] = Map::type( mapCoord.toPoint() + Point( 1, 0) );
		else
			type[3] = type[0];

		if (mapCoord.getx() > 0)
			type[4] = Map::type( mapCoord.toPoint() + Point(-1, 0) );
		else
			type[4] = type[0];

		materialClass m [5];

		for ( i = 0; i < 5; ++i ) {

			switch( type[i] ) {

				case Map::plains :    m[i] = m_grass; break;
				case Map::river :     m[i] = m_sand; break;
				case Map::hills :     m[i] = m_grass; break;
				case Map::mountains : m[i] = m_snow; break;
				case Map::forest :    m[i] = m_grass; break;
				case Map::swamp :     m[i] = m_water; break;
				case Map::waste :     m[i] = m_dirt; break;
				case Map::desert :    m[i] = m_sand; break;
				case Map::ocean :     m[i] = m_grass; break;
				case Map::shore :     m[i] = m_grass; break;

				default:
					Message::add("Error determining level type.");
					m[i] = m_grass; break;
				}

			}

    FourField::Option ff_opt;
    ff_opt.level = this;
    ff_opt.border = borderRect.quadrant(0);
    ff_opt.north = m[1];    ff_opt.east  = m[0];
    ff_opt.south = m[0];    ff_opt.west  = m[4];
		FourField::create( ff_opt );
    
    ff_opt.border = borderRect.quadrant(1);
    ff_opt.north = m[1];    ff_opt.east  = m[3];
    ff_opt.south = m[0];    ff_opt.west  = m[0];
		FourField::create( ff_opt );
    
    ff_opt.border = borderRect.quadrant(2);
    ff_opt.north = m[0];    ff_opt.east  = m[0];
    ff_opt.south = m[2];    ff_opt.west  = m[4];
		FourField::create( ff_opt );
    
    ff_opt.border = borderRect.quadrant(3);
    ff_opt.north = m[0];    ff_opt.east  = m[3];
    ff_opt.south = m[2];    ff_opt.west  = m[0];
		FourField::create( ff_opt );

    Forest::Option forest_opt;
    forest_opt.level = this;
    
		if (type[0] == Map::forest) {
      forest_opt.border = borderRect;
			Forest::create( forest_opt );
      }
    else {
    	int x, y, i;
    	int forests = DiceRoll( 1, 12 ).roll() / 6;
    	for (i = 0; i < forests; ++i) {
    		x = Random::randint(lev_width)+borderRect.getxa();
    		y = Random::randint(lev_height)+borderRect.getya();
        forest_opt.border = Rectangle(x,y,x,y);
    		forest_opt.border+= Rectangle ( -Random::randint(6)-2, -Random::randint(4)-1,
    														        +Random::randint(6)+2, +Random::randint(4)+1);
    		forest_opt.border.setintersect( borderRect );
    		Forest::create( forest_opt );
    		}
      }

    if (type[0] == Map::river) {

      Road::Option river;
      river.border = borderRect;
      river.orthog.reset(0);
      river.orthog.n = ( mapCoord.gety() == 0 || Map::type( mapCoord.toPoint() + Point( 0,-1) ) == Map::river );
      river.orthog.s = ( mapCoord.gety() == map_height-1 || Map::type( mapCoord.toPoint() + Point( 0, 1) ) == Map::river );
      river.orthog.e = ( mapCoord.getx() == map_width -1 || Map::type( mapCoord.toPoint() + Point( 1, 0) ) == Map::river );
      river.orthog.w = ( mapCoord.getx() == 0 || Map::type( mapCoord.toPoint() + Point(-1, 0) ) == Map::river );
      river.material = m_water;
      river.level = this;

      Road::create( river );
    
      }

		return;

		}

	if ( mapCoord.getz() < 0 ) {
		// If the level is underground just fill it with the
		// solid stone we will carve the passages from.
    Solid::Option solid_opt;
    solid_opt.border = borderRect;
    solid_opt.level = this;
    solid_opt.material = m_stone;
		Solid::create( solid_opt );
		return;
		}

  assert(0);
	// Error::fatal("Error: placeTerrain: Upstairs not yet implemented!");

	};


Image Level::getimage( void ) {

  Image ret=0;
  // ooo-factor is how Out of Ordinary a feature is.
  // The feature with the highest ooo-factor gets displayed on the map.
  int max_ooo = 0;

  if (features.reset())
  do {

    if (features.get()->getVal(attrib_ooo_ness) > max_ooo) {
      max_ooo = features.get()->getVal(attrib_ooo_ness);
      ret = features.get()->getimage();
      }
  
    } while (features.next());

  return ret;

  }

String Level::name( void ) {

  String ret;
  int max_ooo = 0;

  if (features.reset())
  do {

    if (features.get()->getVal(attrib_ooo_ness) > max_ooo) {
      max_ooo = features.get()->getVal(attrib_ooo_ness);
      ret = features.get()->name();
      }
  
    } while (features.next());

  return ret;

  }

void Level::identify( IDmask id_mask ) {

  if ( id_mask & id_name ) {

    // Identify the name of the most ooo feature:
    int max_ooo = 0;

    if ( features.reset() ) {
    
      do {
        if (features.get()->getVal(attrib_ooo_ness) > max_ooo)
          max_ooo = features.get()->getVal(attrib_ooo_ness);
        } while (features.next());

      do {
        if (features.get()->getVal(attrib_ooo_ness) == max_ooo)
          features.get()->identify(id_name);
        } while (features.next());
        
      }

    }

  }

Odor Level::getOdor( void ) {
  return odor;
  }

//////////////////////////////////////////////////////
//                                                  //
// Hereafter are all the functions required for the //
// shift from a solid level tile class to the array //
// of simple structs:                               //
//                                                  //
//////////////////////////////////////////////////////


Target Level::getTile( const Point3d &p ) {
/*
	if ( borderRect.contains(p) )
  	return * tiles[p.getx()%lev_width][p.gety()%lev_height];

  // Out of bounds level element requested:
  assert(0);

	// In theory this will never occur:
	return * tiles[0][0];
*/
  assert ( borderRect.contains(p) );
  assert( p.getz() == mapCoord.getz() );

  Point3d arr_pt = p - Point3d(borderRect.geta());
  TileComplex &tile_c = tile_complex[arr_pt.getx()][arr_pt.gety()];

  // If we've already created a temporary tile, use it:
  if (!!tile_c.tile)
    return tile_c.tile;

  // Otherwise create a temporary tile:

  Tile::TempOption tile_opt;

  tile_opt.location = p;
  tile_opt.level = this;

  // Set our tile to this tile so we don't ever create more than one
  // object per tile:
  tile_c.tile = Tile::create(tile_opt);
  
  return tile_c.tile;
	}

void Level::tileCreate( Tile::Option t_opt ) {
// Deletes current tile contents and sets the tile
// attributes to those denoted in t_opt.  If necessary,
// might add a function TilePoly(t_opt) that retains the
// contents of the tile and only changes specific attributes.

  assert( borderRect.contains( t_opt.location.toPoint() ) );
  assert( t_opt.location.getz() == mapCoord.getz() );
  
  Point3d arr_pt = t_opt.location - Point3d(borderRect.geta());
  TileSimple  &tile_s = tile_simple[arr_pt.getx()][arr_pt.gety()];
  // TileComplex &tile_c = tile_complex[arr_pt.getx()][arr_pt.gety()];

  // Do a check of the options:

  // Liquid is never floor, but river:
  if ( Material::data[t_opt.floor].state == liquid
    && (t_opt.type == tt_floor || t_opt.type == tt_ground)  )
    t_opt.type = tt_river;

  tile_s.f_material = t_opt.floor;
  tile_s.o_material = t_opt.object;
  tile_s.type       = t_opt.type;
  tile_s.last_image = 0;

  /*
  // Graffiti shouldn't be an attribute of a tile, since
  // it's going to be less frequent than one per level.
  if (t_opt.graffiti != "")
    tile_s.graffiti = t_opt.graffiti;
    
  tile_s.etching = t_opt.etching;
  tile_s.e_material = t_opt.etch_material;
  */
  
  //tile_c.feature    = t_opt.feature;
  assert(!!t_opt.feature);
  //tile_s.litDir.reset( t_opt.feature->getVal(attrib_sunlit) );
  
  // The following variables remain the same:
  // Orthog viewDir, litDir;
  // Target tile;

  // Delete contents ??
  // // delptr(tile_c.inventory);

  }
  
long Level::tileVal( const Point3d &p3d, StatType s ){

  assert( borderRect.contains( p3d.toPoint() ) );
  assert( p3d.getz() == mapCoord.getz() );

  Point3d arr_pt = p3d - Point3d(borderRect.geta());
  TileSimple  &tile_s = tile_simple[arr_pt.getx()][arr_pt.gety()];
  TileComplex &tile_c = tile_complex[arr_pt.getx()][arr_pt.gety()];

  switch (s) {
    case attrib_pool:
      return Material::data[tile_s.f_material].state == liquid;

		case attrib_opaque: {

      // Calculate the tile opacity:
			long opacity = tile_details[tile_s.type].opacity;

      if (!tile_c.inventory || !tile_c.inventory->reset() )
				return opacity;

			do {
        assert( !!tile_c.inventory->get() );
				opacity += ( tile_c.inventory->get()->getVal( attrib_opaque ) );
				} while ( tile_c.inventory->next() );

  		return opacity;
      }

		case attrib_person :
			return Grammar::p_third_n;

    case attrib_on_level :
      //return (!!tile_c.feature);
      return true;

    case attrib_sunlit: {
      Target t_feature = tileGetTarget(p3d,tg_feature);
      if (!!t_feature)
        return t_feature->getVal(attrib_sunlit);
      return false;
      }

		case attrib_empty :
			if (tile_c.inventory && tile_c.inventory->reset()) return false;
			return true;
      
    case attrib_empty_nodoor:
    case attrib_non_wall:

      // Blocking squares are not considered empty:
      if (tile_details[tile_s.type].block)
        return false;
      
			if (tile_c.inventory && tile_c.inventory->reset())
      do {

        if (!tile_c.inventory->get()->getVal(s)) return false;
      
        } while(tile_c.inventory->next());

      return true;

    case attrib_has_door:
    case attrib_has_stairs:
			if (tile_c.inventory && tile_c.inventory->reset())
      do {

        if (tile_c.inventory->get()->getVal(s)) return true;
      
        } while(tile_c.inventory->next());

      return false;

    case attrib_vis_enemy:
		case attrib_vis_mon :

			if (!tileVal(p3d,attrib_visible)) return false;

			if ( tileCapable(p3d,aSeeInv) ) {

				if (!tile_c.inventory || !tile_c.inventory->reset())
					return false;

				do {

					if ( tile_c.inventory->get()->getVal(s) )
						return true;

					} while ( tile_c.inventory->next() );

				}
			return false;

    case attrib_visible :
      return tileCapable(p3d,aView);

    case attrib_material:
      return tile_s.f_material;


  
    default: return 0;
    }
  }
  
bool Level::tileAdd( const Target &dest, const Target &object ) {

  // Must make sure we add dest object to our complex tiles arr:
  Point3d dest_loc = dest->getlocation();
  // Check the dest_point is within level limits:
  assert( borderRect.contains( dest_loc.toPoint() ) );
  assert( dest_loc.getz() == mapCoord.getz() );

  Point3d arr_pt = dest_loc - Point3d(borderRect.geta());
  TileSimple  &tile_s = tile_simple[arr_pt.getx()][arr_pt.gety()];
  TileComplex &tile_c = tile_complex[arr_pt.getx()][arr_pt.gety()];

  assert( !!dest );
  assert( !!object );

  // It's an error to not have this version of the tile in memory:
  assert ( tile_c.tile == dest );
  // tile_c.tile = dest; // <- never should be required

  // Special case for dropping fluid in a location
	// made of the same substance:
  if (object->istype(tg_fluid) && ( object->getVal(attrib_material) == tile_s.f_material )) {
    object->detach();
    return true;
    }

  // Now add the object to our inventory:
  bool new_loc = false;
  // Check to see if item is in a new location:
  if (object->istype(tg_creature)) {
    if ( object->getVal(attrib_on_level) && object->getTarget(tg_feature) != tileGetTarget(dest_loc,tg_feature) )
      new_loc = true;
    }
  else
    if ( object->getVal(attrib_on_level) ) {
      if( object->getTarget(tg_possessor) != tileGetTarget(dest_loc,tg_feature)->getTarget(tg_possessor) )
        new_loc = true;
    }
  else
    if ( !object->getVal(attrib_on_level) )
      new_loc = true;
    
  // Now add the object:
  //object->setlocation( dest_loc );

  // *now* the item should be able to be told:
  object->setParent(dest->THIS);
  if (!object) return false;

  // Special case for floors made of liquid (ie pools of water)
  if (tileVal(dest_loc, attrib_pool) &&
      // presently flying creatures are exempt to drown damage:
      ! object->getVal(attrib_flying) ) {

    if (!ActionEvent::exists(object, aSwim))
      ActionEvent::create( object, aSwim, -1 );

    /*
    // If the object isn't swimming, it takes damage:
    int strength = 20 * ( !object->getVal(attrib_canswim) );

    Target THIS = getTile(dest_loc);
		Attack wet( CombatValues(pen_vapor,strength,dam_drown,10), THIS, THIS, "drowned" );
    
    if (strength)
      object->takeDamage( wet );
      
    if (object->istype(tg_player))
      if (strength)
        Message::add("You are drowning!");
      else
        Message::add("You swim.");
    */
      
    }


  // Let's put this *after* the item has been aded to the list...
  //object->setParent(dest);

	if (!tile_c.inventory)
		tile_c.inventory = new Sorted<Target>;

	// Check to see if items can stack:
	bool combined = false;

	if ( tile_c.inventory->reset() )
	do {
		if (  tile_c.inventory->get()->capable( aCombine, object )  ) {
			tile_c.inventory->get()->perform( aCombine, object );
			combined = true;
			}
		} while ( !combined && tile_c.inventory->next() );

	if (!combined) tile_c.inventory->add(object);

  // Alert all other items in tile of t's arrival:
	if ( tile_c.inventory && tile_c.inventory->reset() )
	do {
		if ( tile_c.inventory->get() != object )
			tile_c.inventory->get()->perform( aAlertShareSquare, object );
		} while ( tile_c.inventory->next() );

  // If t has moved from a different feature,
  if ( new_loc )
    // alert our feature of t's arrival:
    tileGetTarget(dest_loc,tg_feature)->perform(aAlertTrespass, object);

  // Update the screen and return:
	tileDraw(dest_loc);
  return true;
  }
  
bool Level::tileRemove( const Point3d &p3d, const Target &t ) {
  assert( borderRect.contains( p3d.toPoint() ) );
  assert( p3d.getz() == mapCoord.getz() );

  Point3d arr_pt = p3d - Point3d(borderRect.geta());
  TileComplex &tile_c = tile_complex[arr_pt.getx()][arr_pt.gety()];

	if (tile_c.inventory) {
    long i_key = tile_c.inventory->lock();
  	if ( tile_c.inventory->reset() ) {
  		if ( tile_c.inventory->find(t) ) {
  			tile_c.inventory->remove();

  			if (tile_c.inventory->numData() == 0) {
  				delptr(tile_c.inventory);
          // Must make sure we remove dest object from our complex
          // tiles arr, if necessary:
          // No, now we keep the tile object until
          // the level is saved, then we check to see whether we
          // require it.
          // tile_c.tile = Target(NULL);
          }

  			if (tile_c.inventory) tile_c.inventory->load(i_key);
        tileDraw(p3d);
  			return true;
  			}

  		tile_c.inventory->load(i_key);
  		}
    }
	return false;
  }
  
void Level::tileDraw( const Point3d &p3d ){

//	if ( tileVal(p3d, attrib_visible) )
  if ( p3d.getz() == Map::open_z )
  	LEV_DRAW( p3d.getx(),
             p3d.gety(),
             //Image(cYellow-p3d.getx()%2-p3d.gety()%2,'$'));
             tileImage(p3d) );
  
  }
  
void Level::tileView( const Point3d &p3d, int visible, int hypotenuse ){
  assert( borderRect.contains( p3d.toPoint() ) );
  assert( p3d.getz() == mapCoord.getz() );

  Point3d arr_pt = p3d - Point3d(borderRect.geta());
  TileSimple  &tile_s = tile_simple[arr_pt.getx()][arr_pt.gety()];

  tile_s.visible = visible;
  tile_s.hypotenuse = hypotenuse;
  
	tileDraw(p3d);
  }

/*
void Level::tileLight( const Point3d &p3d, const Orthog &dir ){
  assert( borderRect.contains( p3d.toPoint() ) );
  assert( p3d.getz() == mapCoord.getz() );

  Point3d arr_pt = p3d - Point3d(borderRect.geta());
  TileSimple  &tile_s = tile_simple[arr_pt.getx()][arr_pt.gety()];

  if (!dir.total()) {
    tileUnlight(p3d);
    return;
    }

  tile_s.litDir = dir;
	tileDraw(p3d);
  }
*/
void Level::tileUnview( const Point3d &p3d ){
  assert( borderRect.contains( p3d.toPoint() ) );
  assert( p3d.getz() == mapCoord.getz() );

  Point3d arr_pt = p3d - Point3d(borderRect.geta());
  TileSimple  &tile_s = tile_simple[arr_pt.getx()][arr_pt.gety()];

  // If we were visible, draw a grey char:
  if (tileVal(p3d, attrib_visible)) {
    Image i = tileImage(p3d);
    i.val.color = cDGrey;
    LEV_DRAW( p3d.getx(), p3d.gety(), i );
    }

  // No longer visible:
  tile_s.visible = 0;//.viewDir.reset();

  }
/*
void Level::tileUnlight( const Point3d &p3d ){
  assert( borderRect.contains( p3d.toPoint() ) );
  assert( p3d.getz() == mapCoord.getz() );

  Point3d arr_pt = p3d - Point3d(borderRect.geta());
  TileSimple  &tile_s = tile_simple[arr_pt.getx()][arr_pt.gety()];
  //TileComplex &tile_c = tile_complex[arr_pt.getx()][arr_pt.gety()];

  Target t_feature = tileGetTarget(p3d,tg_feature);
  // If not sunlit and we were visible, draw a grey char:
  if (tileVal(p3d, attrib_visible) && !t_feature->getVal(attrib_sunlit)) {
    Image i = tileImage(p3d);
    i.val.color = cDGrey;
    LEV_DRAW( p3d.getx(), p3d.gety(), i );
    }

  // Otherwise, reset the lit state based on whether we're in the sun:
  if (!!t_feature)
    tile_s.litDir.reset( t_feature->getVal(attrib_sunlit) );
  
  }
*/
bool Level::tileCapable( const Point3d &p3d, Action a, const Target &t ){
  assert( borderRect.contains( p3d.toPoint() ) );
  assert( p3d.getz() == mapCoord.getz() );

  Point3d arr_pt = p3d - Point3d(borderRect.geta());
  TileSimple  &tile_s = tile_simple[arr_pt.getx()][arr_pt.gety()];
  TileComplex &tile_c = tile_complex[arr_pt.getx()][arr_pt.gety()];

	switch (a) {

    case aStartTurn:
      // Is the tile a safe place to start?
      // It's got to be able to contain t:
      if ( !!t && ! tileCapable(p3d, aContain, t) )
        return false;

      // And it can't be a fluid tile:
      if ( tile_s.type == tt_river )
        return false;

      // Otherwise it's safe to start here:
      return true;


    case aSetController:
    case aSetOwner:
      return true;

    case aRelease:
      return true;
      
    case aContain:
    
      // Blocking squares cannot contain:
      // Todo: unless t is insubstantial
      if (tile_details[tile_s.type].block)
        return false;

			if (!tile_c.inventory || !tile_c.inventory->reset() )
				return true;

			do {
				if ( tile_c.inventory->get()->capable(aBlock, t) ) return false;
				} while ( tile_c.inventory->next() );

			return true;

		case aBlock:
      // Blocking squares block:
      // Todo: unless t is insubstantial
      if (tile_details[tile_s.type].block)
        return true;
      
      // Normal tiles block based on contents:
      if (tile_c.inventory && tile_c.inventory->reset())
      do {
        if (tile_c.inventory->get()->capable(a,t)) return true;
        } while (tile_c.inventory->next());

			return false;

		case aSeeInv:
			return true;

		case aSpMessage:
			// Special message:
      // check the tiletypes arr for a non-null string.

      // Check for graffiti:
      //if (tile_s.graffiti.generated()) {
      //  return true;
      //  }
        

			return false;

    case aBeAttacked:
		case aBeKicked:
      // Blocking squares can be attacked:
      if (tile_details[tile_s.type].block)
        return true;
      return false;

    case aView: {
      //return true;

      // This whole sorry mess is from the
      // old lighting code.  We don't want
      // lighting any more because it turns
      // out that lighting is a big wank.

      // I'm not here to show how awesome a
      // programmer I am - lighting was a
      // relic of older games and an annoying
      // resource in the author's opinion.

      // So fuck it.  No more lights.

      // That said, we do like limiting the
      // fov to add to mood.  That's another
      // kettle of fish...

      bool ret;
      long key = 0;
      // Save the inventory location in case another
      // process requires it:
      if (tile_c.inventory)
        key = tile_c.inventory->lock();
      // Tile is visible if:
      ret =  tile_s.visible
          // player orthogonal range is less than view distance,
          && tile_s.visible < global::view_distance
          // and any of:
          && (
            // tile can block travel:
            tileCapable( p3d, aBlock, Target() )
            // tile can block sight:
            || tileVal( p3d, attrib_opaque )
            // or player pythagorean range is less than view distance:
            || tile_s.hypotenuse < global::view_distance
                                 * global::view_distance
            );
            
      // Unlock the inventory:
      if (tile_c.inventory)
        tile_c.inventory->load(key);

      return ret;
      
      /*
      // If tile not lit nor visible,
      if (tile_s.litDir.toPoint() == Point(0,0) || tile_s.viewDir.toPoint() == Point(0,0))
        return false;

      // If tile not within the visible tile range,
      if ( !Map::getVisible().contains(p3d) )
        return false;

      if (tile_s.litDir.self && tile_s.viewDir.self) return true;
      if (tile_s.litDir.ground && tile_s.viewDir.ground) return false;

      if (  tile_s.litDir.n && tile_s.viewDir.n
         || tile_s.litDir.s && tile_s.viewDir.s
         || tile_s.litDir.e && tile_s.viewDir.e
         || tile_s.litDir.w && tile_s.viewDir.w) {

        if (( !tile_s.litDir.n && tile_s.viewDir.n )||( !tile_s.litDir.s && tile_s.viewDir.s )) {
          if (tile_s.viewDir.e && tileCapable( p3d + Point3d(1,0), aBlock))
            return !( tile_s.litDir.ground = tile_s.viewDir.ground = true);
          else if (tile_s.viewDir.w && tileCapable( p3d + Point3d(-1,0), aBlock))
            return !( tile_s.litDir.ground = tile_s.viewDir.ground = true);

          }
    
        if (( !tile_s.litDir.w && tile_s.viewDir.w )||( !tile_s.litDir.e && tile_s.viewDir.e )) {
          if (tile_s.viewDir.s)
            if (tileCapable( p3d + Point3d(0,1), aBlock) )
            return !( tile_s.litDir.ground = tile_s.viewDir.ground = true);
            else {
              tile_s.litDir.self = tile_s.viewDir.self = true;
              return true;
              }
          else if (tile_s.viewDir.n)
            if (tileCapable( p3d + Point3d(0,-1), aBlock) ) {
              tile_s.litDir.ground = tile_s.viewDir.ground = true;
              return false;
              }
            else {
              tile_s.litDir.self = tile_s.viewDir.self = true;
              return true;
              }
          if (tileCapable( p3d + Point3d(tile_s.viewDir.toPoint()), aBlock) )
            return false;
          else {
            tile_s.litDir.self = tile_s.viewDir.self = true;
            return true;
            }

          }
          
        tile_s.litDir.self = tile_s.viewDir.self = true;
        return true;
        }

      return false;
      */
      }



    default: return false;
    }
  }
  
bool Level::tilePerform( const Point3d &p3d, Action a, const Target &t ){
  assert( borderRect.contains( p3d.toPoint() ) );
  assert( p3d.getz() == mapCoord.getz() );

  Point3d arr_pt = p3d - Point3d(borderRect.geta());
  //TileSimple  &tile_s = tile_simple[arr_pt.getx()][arr_pt.gety()];
  TileComplex &tile_c = tile_complex[arr_pt.getx()][arr_pt.gety()];

  switch (a) {
    case aLookAround: {
      // special messages based on tile type:
      String message;

      Target t_feature = tileGetTarget(p3d,tg_feature);
      message = "You stand in "+Grammar::plural(t_feature,Grammar::det_possessive)+".";

      Message::add(message);

      return true;
      }

    case aSniffAround: {
      // How rotten this level smells on account of the monsters:
      String message;

      if (odor > od_none) {
        message = String("The air here smells ")+OdorStr::Type[odor].odor+".";
        Message::add(message);
        }
      
      return true;
      }

    case aSpMessage:
      // special messages based on tile type:
      /*
      if (tile_s.graffiti.generated()) {

        if (tile_s.e_material != m_invalid) {
          String material_str = Material::data[tile_s.e_material].name;
          Message::add("A message is here - scrawled in "+material_str+": \""+tile_s.graffiti.toString()+"\"");
          }
        else
          Message::add("A message here: "+tile_s.graffiti.toString());
        }
      */
      return true;

    /*
    case aSunrise: {
      Target t_feature = tileGetTarget(p3d,tg_feature);
      if (!!t_feature && t_feature->getVal(attrib_sunlit))
        tile_s.litDir.reset( true );
      return true;
      }
      
    case aSunset:
      tile_s.litDir.reset( false );
      return true;
    */
//    case aSetFeature:
//      assert (!!t);
//      assert ( t->istype(tg_feature) );
//
//      tile_c.feature = t;
//
//      return true;

    case aDescribe: {
      // Give a description of the topmost item (or the tile if empty):
      
			if (! tileCapable(p3d,aView) ) {
        Message::add("You cannot see this location.");
        return false;
        }
      
			if ( tile_c.inventory && tile_c.inventory->reset() )
			do {
				if (  tile_c.inventory->get()->getVal(attrib_visible)  )
					return tile_c.inventory->get()->doPerform(aDescribe);
				}	while ( tile_c.inventory->next() );
        
			Message::add(Grammar::plural( tileName(p3d,n_menu),
                                    1,
                                    tileVal(p3d,attrib_abstract),
                                    false )+".");
      
      /*
      // This code for lighting / LOS debugging:
      
      String strLitVis;
      strLitVis += " Lit:";
      if (tile_s.litDir.n) strLitVis += "N";
      if (tile_s.litDir.w) strLitVis += "W";
      if (tile_s.litDir.e) strLitVis += "E";
      if (tile_s.litDir.s) strLitVis += "S";
      if (tile_s.litDir.self) strLitVis += ".";
      if (tile_s.litDir.ground) strLitVis += ">";

      strLitVis += " Vis:";
      if (tile_s.viewDir.n) strLitVis += "N";
      if (tile_s.viewDir.w) strLitVis += "W";
      if (tile_s.viewDir.e) strLitVis += "E";
      if (tile_s.viewDir.s) strLitVis += "S";
      if (tile_s.viewDir.self) strLitVis += ".";
      if (tile_s.viewDir.ground) strLitVis += ">";
      
			Message::add(tileName(p3d,n_menu)+"."+strLitVis);
      */
			return true;
      }

    case aContain:
    case aRelease:
      // This should never occur since all calls to
      // Tile::perform(aContain || aRelease,Target) have been redirected
      // directly to Level::tileAdd() and Level::tileRemove()!
      assert(0);
      return false;
    
    /*
    // Now taken care of by setlocation() recursion:
    case aEnterLev:
    case aLeaveLev:
      // Notify children:
      if ( tile_c.inventory && tile_c.inventory->reset() )
      do {
        long i_key = tile_c.inventory->lock();
        tile_c.inventory->get()->perform(a,t);
        tile_c.inventory->load(i_key);
        } while (tile_c.inventory->next());
        
      return true;
    */

    case aSleep:

      if (tile_c.inventory && tile_c.inventory->reset())
      do {

        tile_c.inventory->get()->doPerform(aSleep);
      
        } while ( tile_c.inventory->next() );
      
      return true;

    default:
			// If we can't deal with the action, see if any
			// item in the inventory can deal with it:
			if ( tile_c.inventory && tile_c.inventory->reset() )
      do {
				if ( tile_c.inventory->get()->perform( a, t ) )
          return true;
        } while ( tile_c.inventory->next() );
			// Nope.  False.
			return false;
    }
  }
  
void Level::tileTakeDamage( const Point3d &p3d, const Attack &a ){
  assert( borderRect.contains( p3d.toPoint() ) );
  assert( p3d.getz() == mapCoord.getz() );

  Point3d arr_pt = p3d - Point3d(borderRect.geta());
  TileComplex &tile_c = tile_complex[arr_pt.getx()][arr_pt.gety()];

  // If a level element takes damage, all items in the tile take the damage:
  // - Blow that.  It's annoying to have potions bubble and boil when
  // you didn't know they were there.  Now on, only the topmost item takes
  // damage!

  if ( tile_c.inventory && tile_c.inventory->reset() )
    tile_c.inventory->get()->takeDamage(a);

  /*
  do {

    long key = tile_c.inventory->lock();
    tile_c.inventory->get()->takeDamage(a);
    if (tile_c.inventory) tile_c.inventory->load(key);
  
    } while ( tile_c.inventory && tile_c.inventory->next()
      // If one of the items in the tile was the player, and he died,
      // don't bother with the other items in the tile:
      && global::quit == global::quit_false);
  */

/*
  // Special case for trees:
  // Chance for fruit to fall is based on bash/chop strength:
  int fruit_chance = a.c_values.penetration[ pen_bash ]
                   + a.c_values.penetration[ pen_chop ];

  // Only cutting and chopping attacks can draw water from cactus.
  if ( material == m_sand )
    fruit_chance = a.c_values.penetration[ pen_cut ]
                 + a.c_values.penetration[ pen_chop ];


  if (Random::randint(10) < fruit_chance) {
    Target fruit;
    switch (material) {
      case m_sand: {
        Fluid::Option fluid_opt;
        fluid_opt.material = m_water;
        fruit = Fluid::create(fluid_opt);
        break;
        }
      case m_dirt:
        // Todo: What should bushes contain?  Nuts/berries?
        // Todo: And where do bananas grow? Coconuts?
      default:
        if (Random::randint(5)) {
         Food::Option food_opt;
         food_opt.special = foodStats::fruit;
         fruit = Food::create(food_opt);
         }
        else {
         Monster::Option mon_opt;
         mon_opt.form = crSnake;
         fruit = Monster::create(mon_opt);
         }
        break;
      }

    fruit->perform(aWarn,a.deliverer);

    String fall = " fall";
    if (fruit->istype(tg_fluid)) fall = " pour";
      
    if (RawTarget::getVal(attrib_visible)) {
      String subject = Grammar::Subject(fruit);
      Message::add(subject+fall+(fruit->getVal(attrib_plural)?"":"s")+" from "+Grammar::Object(Target(THIS),Grammar::det_definite)+"!");
      }

    getlevel().getTile(location.toPoint())->perform(aContain,fruit);
    }

  // Todo: react to fire damage by... burning.
  
  // Chance for tree to be chopped/pushed down:
  if (Random::randint(20) < fruit_chance ) {

    Floor::Option floor_opt;
    floor_opt.location = location;
    floor_opt.floor = material;
    Target replace = Floor::create(floor_opt);
    replace->perform(aSetFeature,feature);

    if (inventory && inventory->reset())
    do {
      replace->perform(aContain,inventory->get());
      } while (inventory && inventory->reset());

    // Todo:  add a log to the replace tile.

    getlevel().getTile(location.toPoint()) = replace;
    if (lit) replace->light(Orthog());
    if (visible) replace->view(Orthog());

    // Todo: damage items in new tile as if a tree fell on them...
    //       using a.deliverer as a deliverer so you get exp!
    Screen::update();

    if (RawTarget::getVal(attrib_visible))
      Message::add(Grammar::Subject(THIS,Grammar::det_definite)+" creaks, then falls.");
    else
      Message::add("You hear a loud creak.");
    }
*/
  
  }

const Image &setLast( Image &dest, const Image &source ) {

  dest = source;
  switch (dest.val.appearance) {
    case '<': dest.val.color = cBlue; break; // Upstairs in Blue
    case '>': dest.val.color = cRed; break; // Downstairs in Red
    case '^': break; // Traps retain their colour
    default: dest.val.color = cDGrey; break; // Other elements just darkened
    }

  return source;

  }
  
Image Level::tileImage( const Point3d &p3d ) {
  assert( borderRect.contains( p3d.toPoint() ) );
  assert( p3d.getz() == mapCoord.getz() );

  Point3d arr_pt = p3d - Point3d(borderRect.geta());
  TileSimple  &tile_s = tile_simple[arr_pt.getx()][arr_pt.gety()];
  TileComplex &tile_c = tile_complex[arr_pt.getx()][arr_pt.gety()];

	if ( !tileVal(p3d, attrib_visible) )
    return tile_s.last_image;


  char mask_color = cBlack;
  char mask_appearance = 0;
  
  // Generated from the tile array:
  Image ret_img = Image(Material::data[tile_s.f_material].color,
                        tile_details[tile_s.type].appearance);

  // Special appearance for etched/scrawled tiles:
  /*
  switch (tile_s.etching) {
    case e_graffiti:
        // FALL THROUGH //
    case e_engraved:
         ret_img.val.appearance = '-';
         break;
         
    case e_smear:
         ret_img.val.appearance = '-';
         break;
         
    case e_prints:
         ret_img.val.appearance = '-';
         break;
         
    default:
         // NOTHING //
         break;
    }
  */
    
  // Special colour for etched/scrawled:
  //if ( tile_s.etching != e_none )
  //  ret_img.val.color = Material::data[tile_s.e_material].color;

  // Special colour overlay for liquid tiles:
  if (Material::data[tile_s.f_material].state == liquid) {

  	if ( tile_c.inventory && tile_c.inventory->reset() )
  	do {
  		if ( !tile_c.inventory->get()->getVal(attrib_invisible) ) {

        if ( tile_c.inventory->get()->getVal(attrib_flying) || tile_c.inventory->get()->getVal(attrib_floats) )
          ret_img = tile_c.inventory->get()->getimage();
        else
          ret_img.val.appearance = tile_c.inventory->get()->getimage().val.appearance;
          
  			return setLast(tile_s.last_image,ret_img);
        }
  		}	while ( tile_c.inventory->next() );

    return setLast(tile_s.last_image,ret_img);

    }
  
  
	if ( tile_c.inventory && tile_c.inventory->reset() )
	do {
    if ( tile_c.inventory->get()->getVal(attrib_mask_color) ) {
      ret_img = tile_c.inventory->get()->getimage();
      if (mask_color == cBlack)
        mask_color = ret_img.val.color;
      }
    else if ( tile_c.inventory->get()->getVal(attrib_mask_appearance) ) {
      ret_img = tile_c.inventory->get()->getimage();
      if (mask_appearance == 0)
        mask_appearance = ret_img.val.appearance;
      }
		else if ( !tile_c.inventory->get()->getVal(attrib_invisible) ) {
      ret_img = tile_c.inventory->get()->getimage();
      break;
      }
		}	while ( tile_c.inventory->next() );

  if (mask_color != cBlack) ret_img.val.color = mask_color;
  if (mask_appearance != 0) ret_img.val.appearance = mask_appearance;
  
	return setLast(tile_s.last_image,ret_img);

  }


  
String Level::tileName( const Point3d &p3d, NameOption n_opt ){
  assert( borderRect.contains( p3d.toPoint() ) );
  assert( p3d.getz() == mapCoord.getz() );

  Point3d arr_pt = p3d - Point3d(borderRect.geta());
  TileSimple  &tile_s = tile_simple[arr_pt.getx()][arr_pt.gety()];

  bool is_ground = (tile_s.type == tt_ground);
  
  switch (n_opt) {
    case n_menu_noprice:
    case n_menu: {
    	String retstr;

      if ( tile_details[tile_s.type].adj || is_ground )
        if ( Material::data[ tile_s.f_material ].adj )
        	retstr += Material::data[ tile_s.f_material ].adj;
        else
        	retstr += Material::data[ tile_s.f_material ].name;

      if (!is_ground) {

        if (retstr != "")
          retstr += " ";
          
        retstr += tile_details[tile_s.type].name;
        }

    	return retstr;
    	}
    default:
      if (is_ground) return String("#ground~");
    	return "#"+tileName(p3d,n_menu)+"~";
  	}
    
  }
  
Action Level::tileAutoAction( const Point3d &p3d ){
  assert( borderRect.contains( p3d.toPoint() ) );
  assert( p3d.getz() == mapCoord.getz() );

  Point3d arr_pt = p3d - Point3d(borderRect.geta());
  TileComplex &tile_c = tile_complex[arr_pt.getx()][arr_pt.gety()];

// Default autoAction for Tile is aNone for
// empty elements, but asks the inventory, return
// autoaction for first item that has one (note:
// there is no implied guarantee that this item
// is visible on the screen... you have been warned!)

  Action ret_val = aNone;
  
	if ( tile_c.inventory && tile_c.inventory->reset() )
  do {
		ret_val = tile_c.inventory->get()->autoAction();
    } while ( ret_val == aNone && tile_c.inventory->next() );

	return ret_val;

  return aNone;
  }
  
Target Level::tileGetTarget( const Point3d &p3d, targetType type ){

  switch( type ) {
    case tg_top_blocker: {
      assert( borderRect.contains( p3d.toPoint() ) );
      assert( p3d.getz() == mapCoord.getz() );

      Point3d arr_pt = p3d - Point3d(borderRect.geta());
      TileComplex &tile_c = tile_complex[arr_pt.getx()][arr_pt.gety()];

      if (tile_c.inventory && tile_c.inventory->reset())
      do {
        if (tile_c.inventory->get()->isCapable(aBlock))
          return tile_c.inventory->get();
        } while (tile_c.inventory->next());

      return getTile(p3d);
      }
  
    case tg_feature:
    case tg_parent: {
      assert( borderRect.contains( p3d.toPoint() ) );
      assert( p3d.getz() == mapCoord.getz() );

      Rectangle f_border;

      if (features.toTail())
      do {

        if ( features.get()->GET_STRUCT( s_border, f_border )
          && f_border.contains( p3d ) )

          return features.get();

        } while ( features.previous() );

      return Target(NULL);
      }

    default:
      return Target(NULL);
    }
  }
  
List<Target> *Level::tileItemList( const Point3d &p3d ) {
  assert( borderRect.contains( p3d.toPoint() ) );
  assert( p3d.getz() == mapCoord.getz() );

  Point3d arr_pt = p3d - Point3d(borderRect.geta());
  TileComplex &tile_c = tile_complex[arr_pt.getx()][arr_pt.gety()];
  
  return tile_c.inventory;
  }

void Level::tileSetTHIS( const Point3d& p3d, TargetValue *tvp ) {
  assert( borderRect.contains( p3d.toPoint() ) );
  assert( p3d.getz() == mapCoord.getz() );

  Point3d arr_pt = p3d - Point3d(borderRect.geta());
  TileComplex &tile_c = tile_complex[arr_pt.getx()][arr_pt.gety()];

  if ( (tile_c.inventory) && (tile_c.inventory->reset() )  )
  do {
    tile_c.inventory->get()->setParent(tvp);
    } while ( tile_c.inventory->next() );


  }

/*
Target Level::findTarget( const Point3d &p3d, targetType type, const Target &t ) {
  switch( type ) {
    case tg_parent:
    case tg_possessor: {
    
      assert( borderRect.contains( p3d.toPoint() ) );
      assert( p3d.getz() == mapCoord.getz() );
      assert( !!t );
      assert( p3d == t->getlocation() );

      Point3d arr_pt = p3d - Point3d(borderRect.geta());
      TileComplex &tile_c = tile_complex[arr_pt.getx()][arr_pt.gety()];

      Target found;

    	if ( (tile_c.inventory) && (tile_c.inventory->reset() )  )
    	do {
    		if (tile_c.inventory->get() == t)
          return getTile(p3d);

        found = tile_c.inventory->get()->findTarget(tg_parent,t);
        if (!!found)
          return found;

    		} while ( tile_c.inventory->next() );

      return Target(NULL);
      }

    default:
      return Target(NULL);
    }
  }
*/

#endif // LEVEL_CPP
