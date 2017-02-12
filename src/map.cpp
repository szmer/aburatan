// [map.cpp]
// (C) Copyright 2003 Michael Blackney
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
#if !defined ( MAP_CPP )
#define MAP_CPP

#include "map.hpp"
#include "standard.hpp"
#include "level.hpp"
#include "file.hpp"
#include "random.hpp"
#include "stairs.hpp"
#include "visuals.hpp"
#include "features.hpp"
#include "input.hpp"
#include "player.hpp"
#include "message.hpp"
#include "target.hpp"
#include "global.hpp"


//--[ struct Map::Limbo ]--//
/*
Map::Limbo Map::limbo;

void Map::Limbo::add( TargetValue *tvp ) {
  assert(tvp);
  
  items.add(tvp);
  }
  
void Map::Limbo::remove( TargetValue *tvp ) {
  assert(tvp);

  while (items.find(tvp))
    items.remove();
  }
      

Target Map::Limbo::findTarget( targetType type, const Target &t ) {

  Target ret;
  
  if (items.reset())
  do {
  
    ret = items.get()->getData()->findTarget(type,t);
    
    } while ( items.next() && !ret );

  return ret;
  }
*/

//--[ struct Map::Bin ]--//

Map::Bin::Bin( Point3d p3d )
: location(p3d) {
  }

Map::Bin::Bin( std::iostream &stream ) {
  // --- Load and init the bin from a file.

  int num_data;
  Target read_obj;

  // location:
  stream.read( (char*)&location, sizeof(location) );

  // features:
  stream.read( (char*)&num_data, sizeof(num_data) );
  
  while ( num_data-- ) {
    stream >> read_obj;
    features.add( read_obj );
    }

  // tiles:
  stream.read( (char*)&num_data, sizeof(num_data) );
  
  while ( num_data-- ) {
    TilePoint3d tp3d(stream);
    tiles.add(tp3d);
    }


  }
  
std::iostream &operator << ( std::iostream &stream, Map::Bin &out_bin ) {
  // --- Save the bin to a file.

  int num_data;

  // location:
  stream.write( (char*)&out_bin.location, sizeof(out_bin.location) );

  // features:
  num_data = out_bin.features.numData();
  stream.write( (char*)&num_data, sizeof(num_data) );
  
  while ( out_bin.features.reset() ) {
    stream << out_bin.features.get();
    out_bin.features.remove();
    }

  // tiles:
  num_data = out_bin.tiles.numData();
  stream.write( (char*)&num_data, sizeof(num_data) );
  
  while ( out_bin.tiles.reset() ) {
    Map::Bin::TilePoint3d tp3d = out_bin.tiles.remove();
    stream << tp3d.target;
    stream.write( (char*)&tp3d.p3d, sizeof(tp3d.p3d) );
    }

    
  return stream;
  }


List<Target> &Map::Bin::featureList( void ) {
  return features;
  }
  
List<Map::Bin::TilePoint3d> &Map::Bin::tileList( void ) {
  return tiles;
  }

void Map::Bin::addTile( Point3d p3d, Target t ) {
  tiles.add( TilePoint3d(p3d,t) );
  }

void Map::Bin::addFeature( Target t ) {
  features.add(t);
  }


//--[ struct Map::Bin::TilePoint3d ]--//

Map::Bin::TilePoint3d::TilePoint3d( Point3d p, Target t )
  // Init with values:
: target(t),
  p3d(p) {
  }
  
Map::Bin::TilePoint3d::TilePoint3d( std::iostream &stream ) {
  // Init from file:
  stream >> target;
  stream.read( (char*)&p3d, sizeof(p3d) );
  }

//--[ class Req ]--//

Req::Req( void )
: type(invalid),
  location(0,0),
  area(0,0,0,0),
  material(m_invalid),
  place_by(none),
  object(NULL){
  }

Req::Req( Target obj )
: type(prefab_object),
  location(0,0),
  area(0,0,0,0),
  material(m_invalid),
  place_by(none),
  object(obj){
  }

Req::Req( Type t, Target obj )
: type(t),
  location(0,0),
  area(0,0,0,0),
  material(m_invalid),
  place_by(none),
  object(obj){
  }

Req::Req( Type t, materialClass m )
: type(t),
  location(0,0),
  area( Rectangle(1,1,lev_width-2,lev_height-2) ),
  material(m),
  place_by(rect),
  object(NULL){
  }

Req::Req( Type t, Point p, materialClass m )
: type(t),
  location(p),
  area(0,0,0,0),
  material(m),
  place_by(point),
  object(NULL){
  }

Req::Req( Type t, Rectangle r, materialClass m )
: type(t),
  location(0,0),
  area(r),
  material(m),
  place_by(rect),
  object(NULL){
  }

Req::Req( Type t, Rectangle r, Point p, materialClass m )
: type(t),
  location(p),
  area(r),
  material(m),
  place_by(both),
  object(NULL){
  }


const Rectangle &Req::getarea( void ) {
	return area;
	}

const Point &Req::getlocation( void ) {
	return location;
	}

bool Req::operator == ( const Req &rhs ) const {
	return ( (type==rhs.type)
				&& (area==rhs.area)
				&& (location==rhs.location)
				&& (place_by==rhs.place_by)
        && (object == rhs.object ) )?true:false;
	}

std::iostream &operator >> ( std::iostream &ios, Req &r ) {

	ios.read( (char*) &r.type,     sizeof(r.type) );
	ios.read( (char*) &r.location, sizeof(r.location) );
	ios.read( (char*) &r.area,     sizeof(r.area) );
	ios.read( (char*) &r.material, sizeof(r.material) );
	ios.read( (char*) &r.place_by, sizeof(r.place_by) );
  
  ios >> r.object;
  
	return ios;

	}

std::iostream &operator << ( std::iostream &ios, Req &r ) {

	ios.write( (char*) &r.type,     sizeof(r.type) );
	ios.write( (char*) &r.location, sizeof(r.location) );
	ios.write( (char*) &r.area,     sizeof(r.area) );
	ios.write( (char*) &r.material, sizeof(r.material) );
	ios.write( (char*) &r.place_by, sizeof(r.place_by) );
  
  ios << r.object;
  
	return ios;

	}
  

//bool Req::canExistIn( Map::levType lt ) {
Map::levType Req::terrain( void ) const {

  switch (type) {

    case town:
      return Map::desert;

    /*
    case asylum:
      return Map::hills;

    case fort:
    case ghost_town:
    case oasis:
    case lost_city:
      return Map::desert;
      
    case cemetery:
    case manor:
    case camp:
      return Map::plains;
      
    case ruins:
    case workshop:
      return Map::mountains;
    
    case evil_garden:
      return Map::forest;
    */
    
    default:
      return Map::any;

    }

  }


bool Req::operator < ( const Req &rhs ) {
  return (type < rhs.type);
  }

  
bool Req::operator > ( const Req &rhs ) {
  return (type > rhs.type);
  }
    
void Req::create( Level &lev ) {

  // Now that we have a level, add the level position
  // offset to our placement points:
  Rectangle lev_border = lev.getBorder();

  if (place_by != point)
    location = Random::randpoint(area);

  if (! lev_border.contains(location) ) {
    area += lev_border.geta();
    location += lev_border.geta();
    }
  
  // Make sure that the location is within the level.
  // We really don't want any errors here,
  // and this function is only called a few times
  // per level at the most:
  area.setintersect(lev_border);
  if (! lev_border.contains(location) )
    location = Random::randpoint(lev_border);

  // Now find the right requirement type and place it:
  switch (type) {

    case stairs_up :
		case stairs_down : {

      Point3d lev_loc = lev.loc();
	  	Point3d loc = Point3d(location,lev_loc.getz());
  		lev_loc.incz( type==stairs_up?1:-1 );

      if ( place_by==rect ) {
        loc.setx( Random::randint(area.getwidth())  + area.getxa() );
        loc.sety( Random::randint(area.getheight()) + area.getya() );
        // If the stairs don't have a set location, make sure that
        // we place a staircase on the adjoining level at the chosen
        // location:
        Map::get( lev_loc ).addRequirement(  Req( (type==stairs_up?stairs_down:stairs_up), loc.toPoint() )  );
        }
      Stairway::Option stairway;
      stairway.connecting = lev_loc.getz();
      stairway.level = &lev;
      stairway.location = loc;
			Stairway::create(stairway);

			return;
		  }

		case town: {

			if (place_by==point)
				area.set(location,location);

      Town::Option town;
      town.level = &lev;
      town.border = area;
			Town::create(town);
      return;
      }

    case mech_mine: {
      assert(!!object);
      MechMines::buildLev( lev );
      return;
      }

    case sewer: {
      assert(!!object);
      Sewers::buildLev( lev );
      return;
      }
      
    /*

		case clear_area: {

			if (place_by==point)
				area.set(location,location);

      Field::Option field;
      field.level = &lev;
      field.border = area;
//      field.border += Point( lev.loc().getx() * lev_width, lev.loc().gety() * lev_height );
      field.material = material;
			Field::create(field);

      return;
      }

		case basement: {

			if (place_by==point)
				area.set(location,location);

      Basement::Option basement;
      basement.level = &lev;
      basement.border = area;
			Basement::create(basement);
      return;
      }

		case sewer: {

			if (place_by==point)
				area.set(location,location);

      Sewer::Option sewer;
      sewer.level = &lev;
      sewer.border = area;
			Sewer::create(sewer);
      return;
      }

    case cemetery: {

			if (place_by==point)
				area.set(location,location);

      Churchyard::Option churchyard;
      churchyard.level = &lev;
      churchyard.border = area;
			Churchyard::create(churchyard);
      return;
     !}

    case first_lev: {

			if (place_by==point)
				area.set(location,location);

      FirstLåvel::Option lev_1st;
      lev_1st.level = &lev;
      lev_1st.border = area;
			FirstLevel::create(lev_1st);
      return;
      }
    */

    case prefab_object: {

      List<Target> obj_list;
      obj_list.add( object );
      lev.sprinkle( obj_list );
    
      return;
      }

    /*
    case lost_ciuy:
    case camp:
    case ghost_town:
    case fort:
    case manor:
    case oasis:
    case ruins:
    case asylum:
    case workshop:
    case evil_garden:
    */

    default :
			// default to have no effect:
      Message::add("Your requirements seem invalid.");
			return;
		}

	}

Req::Type Req::getType( void ) {
  return type;
  }

//--[ map structure ]--//

char **mapArr;

struct LevTypeDetails {
	char color;
	char appearance;
	Map::levType type;
	};

const LevTypeDetails levTypes [Map::num_types] = {

	// mountains
	{ cGrey,  '^', Map::mountains },
	// hills
	{ cBrown,  '~', Map::hills },

	// forest
	{ cGreen,  'T', Map::forest },
	// swamp
	{ cOrange,   ',', Map::swamp },
	// plains
	{ cGreen,  ';', Map::plains },

	// desert
	{ cYellow, '.', Map::desert },
	// waste
	{ cBrown,  ':', Map::waste },
  
	// river
	{ cBlue,   '=', Map::river },

	// ocean
	{ cBlue,   '_', Map::ocean },
	// shore
	{ cYellow, '-', Map::shore }

	};


//--[ class LevelHandle ]--//

LevelHandle::LevelHandle( const Point3d &p3d )
: lev(NULL), mapLoc(p3d),
  master(true), created(false), image(0)  {
	}

LevelHandle::LevelHandle( const LevelHandle &lh )
: lev(lh.lev),
	mapLoc(lh.mapLoc),
	master(true),
	created(lh.created) {

  requirements = lh.requirements;
  image = lh.image;

	((LevelHandle&)lh).master = false;
	}


LevelHandle::~LevelHandle() {

	if (master && lev) {
		delptr(lev);
    }

	}

bool LevelHandle::inMemory( void ) {
  // Test to see if the level is currently loaded to memory:
  return lev;
  }

bool LevelHandle::isCreated( void ) {
  return created;
  }

void LevelHandle::load( void ) {

	if (lev) {
    delptr(lev);
    }

	lev = Level::load( mapLoc );

	}

void LevelHandle::save( void ) {
	assert (master); // Error::fatal( "Attempt to save non-master LevelHandle!" );
	if (lev && master) {
    lev->packUp();
    delptr(lev);
    }
	}


Level *LevelHandle::operator->() {

	checklev();
	return lev;
	}

Level &LevelHandle::operator*() {

	checklev();
	return *lev;
	}

void LevelHandle::checklev( void ) {

  if (!lev) {
    // If no level exists yet, build one from the requirements:
    if (created)
      load();
    else {
      lev = new Level( mapLoc );
      lev->create();
      image = lev->getimage();
      created = true;
      }
    // And tell the map that this level is in memory:
    Map::openLevel(mapLoc);
    }

  }

Point3d LevelHandle::loc( void ) {
	return mapLoc;
	}

bool LevelHandle::operator > ( const LevelHandle &rhs ) const {
	return (mapLoc.getz() > rhs.mapLoc.getz());
	}

bool LevelHandle::operator < ( const LevelHandle &rhs ) const {
	return (mapLoc.getz() < rhs.mapLoc.getz());
	}

bool LevelHandle::operator == ( const LevelHandle &rhs ) const {
	return (mapLoc == rhs.mapLoc);
	}

LevelHandle &LevelHandle::operator = ( const LevelHandle &lh ) {

	if (master && lev) {
    delptr(lev);
    }

	lh.master = false;
	master = true;

	lev = lh.lev;
	mapLoc = lh.mapLoc;

	created = lh.created;

  image = lh.image;

	requirements = lh.requirements;
  
	return *this;
	}

void LevelHandle::addRequirement( const Req &req ) {
	requirements.add(req);
	}

Sorted<Req> &LevelHandle::Requirements( void ) {

	return requirements;
	}

LevelHandle::LevelHandle( std::iostream &inFile )
: lev(NULL) {

	// read Point3d mapLoc
	inFile.read( (char*) &mapLoc, sizeof(mapLoc) );

	// read bool master
	inFile.read( (char*) &master, sizeof(master) );

	// read bool created
	inFile.read( (char*) &created, sizeof(created) );

	// read Image image
	inFile.read( (char*) &image, sizeof(image) );

	// read List<Req> requirements
	int size;
	inFile.read( (char*) &size, sizeof(size) );

	for (int count = 0; count < size; count++) {

		Req load;
    inFile >> load;
//		inFile.read( (char*) &load, sizeof(load) );

		requirements.add( load );

		}

	}

void LevelHandle::toFile( std::iostream &outFile ) {

	// write Point3d mapLoc
	outFile.write( (char*) &mapLoc, sizeof(mapLoc) );

	// write bool master
	outFile.write( (char*) &master, sizeof(master) );

	// write bool created
	outFile.write( (char*) &created, sizeof(created) );

	// write Image image
	outFile.write( (char*) &image, sizeof(image) );

	// write List<Req> requirements
	int size = requirements.numData();
	outFile.write( (char*) &size, sizeof(size) );

	if ( requirements.reset() )
	do {

		Req save( requirements.get() );
//		outFile.write( (char*) &save, sizeof(save) );
    outFile << save;

		} while ( requirements.next() );

	}

/*
void LevelHandle::setImage( Image i ) {
  image = i;
  }
*/
Image LevelHandle::getImage( void ) const {
  return image;
  }


//--[ class Map ]--//

List<Target> Map::creatures;
//List<Target> Map::lights;

Sorted<LevelHandle> ***Map::levels = NULL;
Point3d Map::start_point(OFF_MAP);

Rectangle Map::limits( 0, 0, map_width * lev_width - 1, map_height * lev_height - 1 );
Point3d Map::focus_pt(OFF_MAP);
Point3d Map::player_lev_pt(OFF_MAP);
Point3d Map::vis_offset(OFF_MAP);

List<Point3d> Map::open_levels;
LevelHandle ***Map::quick_level = NULL;
Rectangle Map::open_limit;
int Map::open_z = 0;

Map::orthoRec *Map::ortho = NULL;

List<Map::Bin*> Map::bin_list;

void Map::binAdd( Map::Bin *bin_ptr ) {

  if (bin_ptr)
    bin_list.add( bin_ptr );
  }
  
Map::Bin *Map::binRemove( Point3d p3d ) {

  if (bin_list.reset())
  do {

    if ( bin_list.get()->getLocation() == p3d )
      return bin_list.remove();

    } while ( bin_list.next() );

  return NULL;

  }


void Map::allocate( void ) {
	int x, y;
	levels = new Sorted<LevelHandle> **[ map_width ];

	for (x = 0; x < map_width; ++x)
		levels[x] = new Sorted<LevelHandle> *[ map_height ];

	for (y = 0; y < map_height; ++y)
		for (x = 0; x < map_width; ++x)
			levels[x][y] = NULL;
	}


LevelHandle &Map::get( const Point3d &p ) {

  int x = p.getx(), y = p.gety();

  assert( x >= 0 && x < map_width );
  assert( y >= 0 && y < map_height );

	if (!levels) {
		// Allocate levels storage:
		allocate();
		}


  // Test for short-circuit:
  if (quick_level &&
      p.getz() == open_z &&
      open_limit.contains(p) &&
      quick_level[x-open_limit.getxa()][y-open_limit.getxb()])
      return *quick_level[x-open_limit.getxa()][y-open_limit.getya()];

  
	if ( ! levels[x][y] ) {
		levels[x][y] = new Sorted<LevelHandle>;
		}

	LevelHandle *lhp = NULL;

	if ( levels[x][y]->reset() )
	do {

		if ( levels[x][y]->get().loc() == p )
			lhp = &levels[x][y]->get();

		} while ( levels[x][y]->next() );

	if (!lhp) {
		levels[x][y]->add( LevelHandle( p ) );

		if ( levels[x][y]->reset() )
		do {

			if ( levels[x][y]->get().loc() == p )
				lhp = &levels[x][y]->get();

			} while ( levels[x][y]->next() );

		assert (lhp); //  Error::fatal("Error found in Map::get()!");

		}

	return *lhp;

	}

bool Map::created( const Point3d &p ) {

	if (!levels)
		return false;

	if ( ! levels[p.getx()][p.gety()] )
		return false;

	if ( ! levels[p.getx()][p.gety()]->reset() )
		return false;

	do {

		if ( levels[p.getx()][p.gety()]->get().loc() == p )
			return true;

		} while ( levels[p.getx()][p.gety()]->next() );

	return false;
	}


// Scale between the two units of measurement: Tile and Level

Point3d Map::LevScale( const Point3d &p3d ) {
// Scale Tile up to Level
  int x = p3d.getx() / lev_width,
      y = p3d.gety() / lev_height;

  return Point3d(x,y,p3d.getz());
  }
  
Point3d Map::TileScale( const Point3d &p3d ) {
// Scale Level down to Tile (cell 0,0 of the level)
  int x = p3d.getx() * lev_width,
      y = p3d.gety() * lev_height;

  return Point3d(x,y,p3d.getz());
  }


// Moved ORTHO_WIDTH etc. to .hpp so that other classes have
// access to them, specifically PlayerUI.  Could add a Map::findnearest
// type class, but I'm not sure I want the Map class to have that type
// of power :/
#define LOADED_RECT Rectangle(-EW_LEV_BORDER,-NS_LEV_BORDER,+EW_LEV_BORDER,+NS_LEV_BORDER)
    
#define VIS_OFFSET_SET \
    vis_offset.set( (focus_pt.getx()-EW_LEV_BORDER)* lev_width,\
                    (focus_pt.gety()-NS_LEV_BORDER)* lev_height,\
                     0 )


#define MAX_RIVER   50
#define MAX_MOUNT   50
#define MAX_FOREST 200
#define MAX_SWAMP   25
#define MAX_PLAINS 200
#define START_REQ Req::sewer

void Map::create( void ) {

  ortho = NULL;
  start_point = player_lev_pt = focus_pt = vis_offset = OFF_MAP;

	int x, y, i;
  Rectangle borderRect(0,0,map_width-1,map_height-1);

  assert (!mapArr); // Error::fatal("mapArr not NULL!");

	// Allocate storage:
	mapArr = new char* [map_height];

	for (y = 0; y < map_height; ++y)
		mapArr[y] = new char [map_width];


	// Fill with desert:

	for (y = 0; y < map_height; ++y)
		for (x = 0; x < map_width; ++x)
			mapArr[y][x] = desert;


  // create rivers:

  int totalLen = 0;

  do {

    int startX = Random::randint(map_width/2)+map_width/4,
        startY = Random::randint(map_height/2)+map_height/4;
      
    bool done = false;
    Point dir, startDir;
    int length = 0;
  

    // Starts on a vertical edge:
    if (Random::randbool()) {
      startY = 0;
      dir = Point(0,1);
      }
    else {
      startY = map_height-1;
      dir = Point(0,-1);
      }

    startDir = dir;

    x = startX;
    y = startY;
  
    if (borderRect.contains(Point(x,y)))
      mapArr[y][x] = river;

    do {

      int newx=dir.getx(), newy=dir.gety();

      switch(Random::randint(4)) {

        case 0:
          // left:
          if (dir.getx()!=dir.gety())
            newx += dir.gety();
          if (dir.getx()!=-dir.gety())
            newy -= dir.getx();
          dir.set( newx, newy );

          // Make sure we are not looping back on ourself:
          if (startDir.getx() == -dir.getx() && startDir.gety() == -dir.gety() )
            dir.set( -dir.gety(),dir.getx() );
          
          break;
        case 1:
          // right:
          if (dir.getx()!=-dir.gety())
            newx -= dir.gety();
          if (dir.getx()!=dir.gety())
            newy += dir.getx();
          dir.set( newx, newy );

          // Make sure we are not looping back on ourself:
          if (startDir.getx() == -dir.getx() && startDir.gety() == -dir.gety() )
            dir.set( dir.gety(),-dir.getx() );
          break;

        default: break;
    
        }

      length ++;
      x += dir.getx();
      y += dir.gety();

      if ( (x == 0) || (x == map_width) || (y == 0) || (y == map_height) )
        done = true;
      
      if (borderRect.contains(Point(x,y))) {
        if (mapArr[y][x] == river) done=true;
        mapArr[y][x] = river;
        }
      else {
        done = true;
        break;
        }

      if (Random::randint(length) > 20) done = true;
    
      } while (!done);

    totalLen += length;
    } while (totalLen < MAX_RIVER);
  

	for (y = 0; y < map_height; ++y)
		for (x = 0; x < map_width; ++x)
      if ((x < map_width-1 && mapArr[y][x+1] == river)
      && ( (y < map_height-1 &&mapArr[y+1][x] == river)&&(x < map_width-1 && y < map_height-1 && mapArr[y+1][x+1] != river)
         ||(y > 0 && mapArr[y-1][x] == river)&&(x < map_width-1 && y > 0 && mapArr[y-1][x+1] != river) ) )
        mapArr[y][x] = river;


  // alter the plains to suit:

  // create mountains + hills:
  totalLen = 0;

  do {

    int size = 0;
    bool done = false;

    x = Random::randint(map_width),
    y = Random::randint(map_height);
    levType ltype = hills;


    do {

      x += Random::randint(3)-1;
      y += Random::randint(3)-1;

      if (!borderRect.contains(Point(x,y)) )
        done = true;

      else {

        if (mapArr[y][x] == desert) {
          size++;
          mapArr[y][x] = ltype;
          }

        if (size >= 10) done = true;
      
        }
    
      } while (!done);

    totalLen += size;
  
    } while (totalLen < MAX_MOUNT);

	for (y = 0; y < map_height; ++y)
		for (x = 0; x < map_width; ++x) {

      int surr = 0;
      if (y+1 < map_height)
        surr += mapArr[y+1][x] == hills;
      if (y-1 > 0)
        surr += mapArr[y-1][x] == hills;
      if (x+1 < map_width)
        surr += mapArr[y][x+1] == hills;
      if (x-1 > 0)
        surr += mapArr[y][x-1] == hills;
//      int surr = (mapArr[y+1][x] == hills)+(mapArr[y-1][x] == hills)+(mapArr[y][x+1] == hills)+(mapArr[y][x-1] == hills); // this code segfaults

      if (mapArr[y][x] == desert && surr > 2)
        mapArr[y][x] = hills;

      if (surr > 3)
        mapArr[y][x] = mountains;
      }


  // create plains:
  totalLen = 0;

  do {

    int size = 0;
    bool done = false;

    x = Random::randint(map_width),
    y = Random::randint(map_height);
    levType ltype = plains;


    do {

      x += Random::randint(3)-1;
      y += Random::randint(3)-1;

      if (!borderRect.contains(Point(x,y)) )
        done = true;

      else {

        if (mapArr[y][x] == desert) {
          size++;
          mapArr[y][x] = ltype;
          }
      
        }
    
      if (size >= MAX_PLAINS) done = true;
      
      } while (!done);

    totalLen += size;
  
    } while (totalLen < MAX_PLAINS);

	for (y = 0; y < map_height; ++y)
		for (x = 0; x < map_width; ++x)
      if (mapArr[y][x] == desert
      && ((y < map_height-1 && mapArr[y+1][x] == plains)+(y > 0 && mapArr[y-1][x] == plains)
         +(x < map_width-1 && mapArr[y][x+1] == plains)+(x > 0 && mapArr[y][x-1] == plains ) > 2) )
        mapArr[y][x] = desert;

  // create forests:
  totalLen = 0;

  do {

    int size = 0;
    bool done = false;

    x = Random::randint(map_width),
    y = Random::randint(map_height);
    levType ltype = forest;


    do {

      x += Random::randint(3)-1;
      y += Random::randint(3)-1;

      if (!borderRect.contains(Point(x,y)) )
        done = true;

      else {

        if (mapArr[y][x] == desert) {
          size++;
          mapArr[y][x] = ltype;
          }

        if (size > 12) done = true;
      
        }
    
      } while (!done);

    totalLen += size;
  
    } while (totalLen < MAX_FOREST);

  // create swamp:
  // For the moment, we don't want swamp.
  // Not sure how swamp will fit in with the
  // rest of the theme.
  /*
  totalLen = 0;

  do {

    int size = 0;
    bool done = false;

    x = Random::randint(map_width),
    y = Random::randint(map_height);
    levType ltype = swamp;


    do {

      x += Random::randint(3)-1;
      y += Random::randint(3)-1;

      if (!borderRect.contains(Point(x,y)) )
        done = true;

      else {

        if (mapArr[y][x] == desert) {
          size++;
          mapArr[y][x] = ltype;
          }
      
        }

      if (size >= MAX_SWAMP) done = true;
    
      } while (!done);

    totalLen += size;
  
    } while (totalLen < MAX_SWAMP);
  */


  // === Place guaranteed features:

  // --- Place Gilead:
  //  Gilead is a walled city, beneath of which lie the
  // catacombs.  The residents of Gilead are lawful and
  // honest.  Gilead houses the last gunslingers.
  // Praise the Imperium!

  // Place town:

  // Gilead is self-sufficient... within the walls, and
  // nearby without, it must contain one of each type of
  // store, enough residents and plenty of farms.

  // -> a reasonable amount of nearby land is fertile
  // -> no empty houses within the walls
  // -> has cemetery w/ access to catacombs
  // -> Lawful inhabitants


  // --- Place Texarkana:
  //  Texarkana is an old city that used to house great
  // libraries, but now is the home of freeholders.  The
  // nearby Mechanical Mines keep Texarkanans with enough
  // reason to stay, despite the lack of farmland.

  // -> surrounding land is infertile
  // -> nearby is entrance to Mechanical Mines
  // -> Lawless inhabitants

  start_point = Point3d(40,10,0);
  Point3d no_feature = start_point;
  
  Sewers::Option s_opt;
  s_opt.entrance = start_point;
  Sewers::create(s_opt);

  start_point.getx() *= lev_width;
  start_point.gety() *= lev_height;

  // --- Place Abura Tan:
  //  Abura Tan was a citadel of the old world.

  // -> ghost city w/ no residents though good chance of mech widgets
  // -> citadel


  // === Place all the requirements for the tunnels, caves, etc.

  // For now just put them all over the place:
  

  // === Set up random level requirements:

  // A list for requirements we want to place randomly:
  List<Req> req_list;

  // Some towns:
  for (i = 0; i < 50; ++i)
    req_list.add( Req::town );

  // Some cemeteries:
  // Make sure that the outer edges are not included:
  //Rectangle cem_border = Rectangle(	8,3,map_width-9,map_height-4);
  //for (i = 0; i < 50; ++i)
  //  req_list.add( Req(Req::cemetery,cem_border) );

/*
  // Three camps:
  for (i = 0; i < 5; ++i)
    req_list.add( Req::camp );


  // A number of interesting places:
  req_list.add( Req::lost_city );
  req_list.add( Req::ghost_town );
  req_list.add( Req::fort );
  req_list.add( Req::manor );
  req_list.add( Req::oasis );
  req_list.add( Req::ruins );
  req_list.add( Req::asylum );
  req_list.add( Req::workshop );
  req_list.add( Req::cemetery );
  req_list.add( Req::evil_garden );
*/

  // Find locations for all the items in the list:
  // First make a list of all of the locations of each type of terrain:
  List<Point> terr_loc[num_types];
    
  for (y = 0; y < map_height; ++y)
    for (x = 0; x < map_width; ++x)
      if ( Point3d(x,y,0) != no_feature )
        terr_loc[ (levType)mapArr[y][x] ].add( Point(x,y) );
        
  // Continue while there are requirements left in the list:
  while ( req_list.reset() ) {

    levType terrain = req_list.get().terrain();
    Point loc;
    int index = Random::randint( terr_loc[terrain].numData() );

    assert (terr_loc[terrain].reset());
    //  Error::fatal(String()+"Map has insufficient space for requirements! type="+(long)terrain);
    terr_loc[terrain].reset();
      
    for (i = 0; i < index; ++i)
      terr_loc[terrain].next();

    loc = terr_loc[terrain].remove();
    if (start_point==OFF_MAP && req_list.get().getType() == START_REQ) {
      start_point = Point3d(loc);
      start_point.getx() *= lev_width;
      start_point.gety() *= lev_height;
      }
    
    get( Point3d(loc) ).addRequirement( req_list.remove() );
    
    }
    
//  get( Point(40,10,-1) ).addRequirement( Req::first_lev );
//  start_point = Point(40,10,-1);

  // Convert array from types to image types:

	for (y = 0; y < map_height; ++y)
		for (x = 0; x < map_width; ++x)
			mapArr[y][x] = levTypes[mapArr[y][x]].appearance;


  // Ensure we have placed the player:
  assert ( start_point != OFF_MAP );


  }


Rectangle Map::getVisible( void ) {
  // Calculate visible screen size:
  Rectangle screen(0, 0, map_width-1, map_height-1);
  
  // Add the focus offset to determine which tiles are visible:
  screen -= Point(Screen::levxoff,Screen::levyoff);

  return screen;
  
  
  }


void Map::load( void ) {

	int x, y;

  delarr(ortho);

	assert (!levels); // Error::fatal("Attempt to load an unsaved map!");

	// Allocate levels array:
	allocate();

	File file;
	String filename = SAVEDIR+"LEVEL.MAP";

	std::fstream &myFile = file[filename];

  // -- Read Bin list:
  // First read the size of the list:
  int num_data;
  myFile.read( (char*) &num_data, sizeof(num_data) );

  // Then read all the elements:
  while ( num_data-- )
    bin_list.add( new Bin(myFile) );


  // Read focus_pt:
	myFile.read( (char*) &focus_pt, sizeof(focus_pt) );
  // Read open_z
	myFile.read( (char*) &open_z, sizeof(open_z) );
  // Read player_lev_pt
	myFile.read( (char*) &player_lev_pt, sizeof(player_lev_pt) );
  // Read vis_offset
	myFile.read( (char*) &vis_offset, sizeof(vis_offset) );

  // Read Screen::levxoff and Screen::levyoff
	myFile.read( (char*) &Screen::levxoff, sizeof(Screen::levxoff) );
	myFile.read( (char*) &Screen::levyoff, sizeof(Screen::levyoff) );
  
	// Read list location:
	Point loc(0,0);
	myFile.read( (char*) &loc, sizeof(loc) );


  while ( loc != Point(-1,-1) ) {

		// Read list size:
		int size = 0;
		myFile.read( (char*) &size, sizeof(size) );

 		levels[loc.getx()][loc.gety()] = new Sorted<LevelHandle>;
      
		// Read list elements:
		for (int count = 0; count < size; count++) {

      LevelHandle readLH(myFile);
			levels[loc.getx()][loc.gety()]->add( readLH );
			}

		// Read next list location:
		myFile.read( (char*) &loc, sizeof(loc) );
		}


	mapArr = new char* [map_height];

	for (y = 0; y < map_height; ++y)
		mapArr[y] = new char [map_width];

	for (y = 0; y < map_height; ++y)
		for (x = 0; x < map_width; ++x)
			myFile.read( (char*) &mapArr[y][x], sizeof(mapArr[y][x]) );

	}


void Map::save( void ) {

	int x, y;

  // Close all the levels:
  keepOpen( Rectangle(-1,-1,-1,-1), 0 );

	File file;
	String filename = SAVEDIR+"LEVEL.MAP";

	std::fstream &myFile = file[filename];

  // -- Write Bin list:
  // First write the size of the list:
  int num_data = bin_list.numData();
  myFile.write( (char*) &num_data, sizeof(num_data) );

  // Then write all the elements:
  while ( bin_list.reset() ) {
    Bin *bin_remove = bin_list.remove();
    assert (bin_remove);
    myFile << ( *bin_remove );
    delptr( bin_remove );
    }
  

  // Write focus_pt:
	myFile.write( (char*) &focus_pt, sizeof(focus_pt) );
  // Write open_z
	myFile.write( (char*) &open_z, sizeof(open_z) );
  // Write player_lev_pt
	myFile.write( (char*) &player_lev_pt, sizeof(player_lev_pt) );
  // Write vis_offset
	myFile.write( (char*) &vis_offset, sizeof(vis_offset) );

  // Write Screen::levxoff and Screen::levyoff
	myFile.write( (char*) &Screen::levxoff, sizeof(Screen::levxoff) );
	myFile.write( (char*) &Screen::levyoff, sizeof(Screen::levyoff) );

	for (y = 0; y < map_height; ++y)
		for (x = 0; x < map_width; ++x) {

			if ( levels[x][y] ) {

				// Write list location:
				Point loc(x,y);
				myFile.write( (char*) &loc, sizeof(loc) );

				// Write list size:
				int size = levels[x][y]->numData();
				myFile.write( (char*) &size, sizeof(size) );

				assert ( levels[x][y]->reset() );// Error::fatal("Empty list in Map::save!");
        levels[x][y]->reset();

				// Write list elements:
				do {

          //levels[x][y]->get().save();
					levels[x][y]->get().toFile( myFile );

					} while( levels[x][y]->next() );

				levels[x][y]->clr();
				}

			delptr( levels[x][y] );

			}

	// Write end of map marker to file:
	Point end(-1,-1);
	myFile.write( (char*) &end, sizeof(end) );


  // --- Cleanup
  
	// Continue to delete map storage:
	for (x = 0; x < map_width; ++x)
		delarr( levels[x] );

	delarr( levels );

  // Destroy mapArr:
	for (y = 0; y < map_height; ++y)
		for (x = 0; x < map_width; ++x)
			myFile.write( (char*) &mapArr[y][x], sizeof(mapArr[y][x]) );

	for (y = 0; y < map_height; ++y)
		delarr(mapArr[y]);

	delarr(mapArr);

  // Ensure quick_level is destroyed:
  if (quick_level) {
    
    for (x = 0; x < EW_LEV_BORDER * 2 + 1; ++x)
      delarr(quick_level[x]);
      
    delarr(quick_level);
      

    }

  delarr(ortho);

  creatures.clr();
  }

////////////////////////////////
// START LOS, LIGHTING FUNCTIONS

                                         
void Map::allocOrtho( void ) {

  int x, y, z;
  if (!ortho)
    ortho = new orthoRec[ ORTHO_HEIGHT * ORTHO_WIDTH ];

  Point3d world_point;

  z = focus_pt.getz();

  for (y = 0; y < ORTHO_HEIGHT; y++)
    for (x = 0; x < ORTHO_WIDTH; x++) {
      
      world_point.set( x,y,z );
      world_point += vis_offset;
      
      if ( limits.contains( world_point.toPoint() ) )
        ortho[x+y*ORTHO_WIDTH].opacity = tileVal( world_point, attrib_opaque );
      else
        ortho[x+y*ORTHO_WIDTH].opacity = 100;

      //ortho[x+y*ORTHO_WIDTH].lit.reset();
      ortho[x+y*ORTHO_WIDTH].visible = 0;
        
      }
    }
    
void Map::shiftMap( Point offset ) {
  // --- shiftMap now redraws the newly visible tiles whenever
  //     the screen scrolls.  So we need to know the direction
  //     that the screen scrolls, the distance, and the screen
  //     limits.

  // If ortho isn't allocated, don't bother:
  if (!ortho)
    return allocOrtho();

  // If the screen didn't scroll, don't bother:
  if (offset == Point(0,0))
    return;

  // Standard vars:
  int x, y, z = focus_pt.getz();
  Point3d world_point;

  // Calculate the screen limits:
  //#define POINT_VISIBLE(x,y) ( ascending(Screen::mapxmin-1,(x)+Screen::levxoff+Screen::mapxoff+1,Screen::mapxmax+1)
  //                          && ascending(Screen::mapymin-1,(y)+Screen::levyoff+Screen::mapyoff+1,Screen::mapymax+1) )


  Point screen_a ( Screen::mapxmin-(Screen::levxoff+Screen::mapxoff+1),
                   Screen::mapymin-(Screen::levyoff+Screen::mapyoff+1) ),

        screen_b ( Screen::mapxmax-(Screen::levxoff+Screen::mapxoff),
                   Screen::mapymax-(Screen::levyoff+Screen::mapyoff) );

  int min_x = limits.getxa(),   min_y = limits.getya(),
      max_x = limits.getxb()+1, max_y = limits.getyb()+1;

  // For all new empty cells: (horizontal band)
  if (offset.gety() != 0)
    for (y = max(min_y, ( (offset.gety()<0 )?screen_b.gety()+offset.gety():screen_a.gety())  );
         y < min(max_y, ( (offset.gety()<0 )?screen_b.gety():screen_a.gety()+offset.gety())  ); y++)
      for (x = max(min_x,  min(offset.getx(),0)+screen_a.getx() );
           x < min(max_x,  max(offset.getx(),0)+screen_b.getx() ); x++) {

        // Draw:
        world_point.set(x,y,z);
        tileDraw(world_point);
        }

  // For all new empty cells: (vertical band)
  if (offset.getx() != 0)
    for (x = max(min_x, ( (offset.getx()<0 )?screen_b.getx()+offset.getx():screen_a.getx())  );
         x < min(max_x, ( (offset.getx()<0 )?screen_b.getx():screen_a.getx()+offset.getx())  ); x++)
      for (y = max(min_y,  min(offset.gety(),0)+screen_a.gety() );
           y < min(max_y,  max(offset.gety(),0)+screen_b.gety() ); y++) {

        // Draw:
        world_point.set(x,y,z);
        tileDraw(world_point);
        }



  // Code from when this function tried to move ortho:
  // // Shift the ortho array in 'offset' direction:
  // //for (y = max(offset.gety(),0); y < min(offset.gety(),0)+ORTHO_HEIGHT; y++)
  // //  for (x = max(offset.getx(),0); x < min(offset.getx(),0)+ORTHO_WIDTH; x++) {
  // //    ortho[x+y*ORTHO_WIDTH] = ortho[x-offset.getx()+(y-offset.gety())*ORTHO_WIDTH];
  // //    }
  //
  //      if ( limits.contains( world_point.toPoint() ) )
  //        ortho[x+y*ORTHO_WIDTH].opacity = tileVal( world_point, attrib_opaque );
  //      else
  //        ortho[x+y*ORTHO_WIDTH].opacity = 100;
  //
  //      ortho[x+y*ORTHO_WIDTH].lit.reset();
  //      ortho[x+y*ORTHO_WIDTH].visible.reset();
  //


  }

void Map::clearOrtho( void ) {
  if (ortho)
    delarr(ortho);
  }

#define TracePath(grid,startX,startY,endX,endY) { \
	int longestLength, \
			lengthX = (endX - startX)*MULT, \
			lengthY = (endY - startY)*MULT, \
			divX, divY, \
			posX = startX*MULT, \
			posY = startY*MULT, \
			posXmod, posYmod, \
			posXdiv, posYdiv; \
	bool tall = false; \
	char plusX = 0, plusY = 0; \
	if (abs(lengthX) > abs(lengthY)) { \
		longestLength = abs (endX - startX); \
		tall = false; \
		} \
	else { \
		longestLength = abs (endY - startY); \
		tall = true; \
		} \
	if (longestLength == 0) break; \
	divX = lengthX / longestLength;\
	divY = lengthY / longestLength;\
	int tile=-1;\
	char opacity = 0;\
	bool secondpoint = false;\
	int tile2=-1;\
	int opacity1 = 0, opacity2 = 0;\
  bool do_break=false;\
  int i; \
	for (i = 1; i <= longestLength; i++) {\
    do_break = false;\
		posX += divX;                             \
		posY += divY;\
		posXmod = posX % 256;\
		posYmod = posY % 256;\
		plusX = posXmod > 63;\
		plusY = posYmod > 63;\
		assert ( (plusX != 1) || (plusY != 1) ); /* Error with LOS antialiasing algorithm! */\
		if (tall)\
			secondpoint = (plusX && (posXmod < 192 ) )?true:false;\
		else\
			secondpoint = (plusY && (posYmod < 192 ) )?true:false;\
		posXdiv = posX/MULT;\
		posYdiv = posY/MULT;\
		/*if ( !ascending( -1, posXdiv+plusX, lev_width )*/\
		/*	|| !ascending( -1, posYdiv+plusY, lev_height )  )*/\
		if ( !ascending( -1, posXdiv+plusX, ORTHO_WIDTH )\
			|| !ascending( -1, posYdiv+plusY, ORTHO_HEIGHT )  )\
				break;\
		/*tile = (posXdiv+plusX)+lev_width* (posYdiv+plusY);*/\
		tile = (posXdiv+plusX)+ORTHO_WIDTH* (posYdiv+plusY);\
		if (!secondpoint)\
			grid[tile].setvisible = i;\
		if (secondpoint) {\
			/*tile2 = (posXdiv)+lev_width*(posYdiv);*/\
			tile2 = (posXdiv)+ORTHO_WIDTH*(posYdiv);\
			opacity1  += grid[tile].opacity;\
			opacity2 += grid[tile2].opacity;\
			}\
		else\
		opacity += grid[tile].opacity;\
		if ( (opacity >= OPACITY_BLOCK) || ((opacity1 >= OPACITY_BLOCK) && (opacity2 >= OPACITY_BLOCK)) ) {\
      if (secondpoint) {\
        if ( (plusX||plusY) && (tall&&divX < 0||!tall&&divY < 0) )\
          grid[tile].setvisible = i;/*max(i,grid[tile].setvisible);*/\
        else\
          grid[tile2].setvisible = i;\
        \
        }\
      break;\
      }\
		if (secondpoint) {\
			grid[tile].setvisible = i;\
			grid[tile2].setvisible = i;\
			}\
    do_break = true;\
		}\
  if (do_break) {\
  	/*tile = endX+lev_width*endY;*/\
  	tile = endX+ORTHO_WIDTH*endY;\
  	grid[tile].setvisible = i;\
    }\
	}

void Map::LOS( const Target &player ) {

	int x, y, i;

	Point3d pl = player->getlocation();
  //int z = focus_pt.getz();
  int z = pl.getz();
  
//  Point3d vis_offset( focus_pt.getx() * lev_width,
//                      focus_pt.gety() * lev_height,
//                      focus_pt.getz() );

	int px = pl.getx() - vis_offset.getx(),
      py = pl.gety() - vis_offset.gety();


  if (!ortho) allocOrtho();

  gridRec *grid = new gridRec[ ORTHO_HEIGHT * ORTHO_WIDTH ];

	for (y = 0; y < ORTHO_HEIGHT; y++)
		for (x = 0; x < ORTHO_WIDTH; x++) {
      i = x+y*ORTHO_WIDTH;
			grid[i].opacity = ortho[i].opacity;
			grid[i].setvisible = 0;
			}

  // Set the player's location to brightest:
	grid[px+py*ORTHO_WIDTH].setvisible = 1;


	for (x = px-80, y = py-40; x <= px+80; x++)
		TracePath(grid, px,py, x, y);
	for (x = px-80, y = py-40; y <= py+40; y++)
		TracePath(grid, px,py, x, y);
	for (x = px-80, y = py+40; x <= px+80; x++)
		TracePath(grid, px,py, x, y);
	for (x = px+80, y = py-40; y <= py+40; y++)
		TracePath(grid, px,py, x, y);


  Point3d grid_pt;
  grid_pt.setz(z);


	for (grid_pt.sety(0); grid_pt.gety() < ORTHO_HEIGHT; grid_pt.incy())
		for (grid_pt.setx(0); grid_pt.getx() < ORTHO_WIDTH; grid_pt.incx()) {

      i = grid_pt.X_plus_Y_times(ORTHO_WIDTH);
      
			if (grid[i].setvisible) {
        //if (ortho[i].visible != grid[i].setvisible) {
          tileView( grid_pt+vis_offset,
            ortho[i].visible = grid[i].setvisible,
            (  (grid_pt.getx() - px ) * (grid_pt.getx() - px )
             + (grid_pt.gety() - py ) * (grid_pt.gety() - py )
              )
            );
          }
        //}
      else {
        tileUnview( grid_pt+vis_offset );
        ortho[i].visible = 0;
        }
      }


  Screen::update();
	delarr( grid );
	}

void Map::calcOpacity( const Point3d &p3d ) {

  if (!ortho) allocOrtho();

  if (p3d.getz() == focus_pt.getz()) {
    // These offsets will have to be updated when we display
    // more than one level per screen:
    Point3d vis_pos(p3d);
    vis_pos -= vis_offset;
    ortho[vis_pos.X_plus_Y_times(ORTHO_WIDTH)].opacity = tileVal( p3d, attrib_opaque );
    }
  }
        
void Map::setOpacity( const Point3d &p3d, int o ) {

  if (p3d.getz() == focus_pt.getz()) {
    if (!ortho) allocOrtho();
    ortho[p3d.X_plus_Y_times(ORTHO_WIDTH)].opacity = o;
    }
    
  }


/*
void Map::setLit( int x, int y, const Target &t ) {

  Orthog dir;
  Point location = t->getlocation().toPoint();

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
//    getTile( Point3d(x,y,z)+vis_offset )->light(dir);
    ortho[x+y*lev_width].lit = dir;
//    }
    
  }
*/

/*
void Map::sunrise( void ) {
//  int x, y;

//  for (y = 0; y < lev_height; y++)
//  	for (x = 0; x < lev_width; x++)
//			tiles[x][y]->raw()->perform(aSunrise);
      
  clearOrtho();
  //calcLights();
  redraw();
  Screen::update();
  }

void Map::sunset( void ) {
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

void Map::addCreature( const Target &c ) {
  assert ( c->istype( tg_creature) );
	creatures.add( c );
	}

void Map::removeCreature( const Target &c ) {
	assert ( c->istype( tg_creature) );
	if ( creatures.find(c))
		creatures.remove();
	else {
    //assert(0);
    //Message::add("Map::removeCreature failed!");
    }
	}

List<Target> &Map::creatureList( void ) {
  return creatures;
  }

/*
void Map::addLight( const Target &l ) {
	//assert ( !lights.find(l) );
  
	lights.add( l );
  l->doPerform( aLight );
	}
*/
/*
void Map::removeLight( const Target &l ) {

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
    //assert(0);
    Message::add("Level::removeLight failed!");
    }
  }

void Map::calcLights( void ) {

  //light cells:
  if ( lights.reset() )
  do {

    lights.get()->doPerform( aLight );
    
    } while ( lights.next() );

  Screen::update();
    
  }
*/

void Map::redraw( void ) {
// Re-draw the whole map

  int z = focus_pt.getz();
  
	File file;
	for (int y = 0; y < lev_height; y++)
		for (int x = 0; x < lev_width; x++) {
			//getTile( Point3d(x,y,z)+vis_offset )->draw();
      tileDraw( Point3d(x,y,z)+vis_offset );
			}
	}

// END LOS, LIGHTING FUNCTIONS
//////////////////////////////

Map::levType Map::type( Point p ) {

  if ( (p.getx() >= 0) && (p.getx() < map_width)
    && (p.gety() >= 0) && (p.gety() < map_height) ) {

  	int i;
  	char app = mapArr[p.gety()][p.getx()];

  	for ( i = 0; i < num_types; ++i )
  		if ( app == levTypes[i].appearance )
  			return (levType) i;
    }
    
	return (levType)0;
	}

void Map::display( void ) {

	Screen::Buffer buffer;

	buffer.setSize( Rectangle(  Screen::mapxoff+1,Screen::mapyoff+1,
															map_width+Screen::mapxoff,map_height+Screen::mapyoff )  );

  Screen::hidecursor();

  Point3d player_loc = Player::levLoc;

	int x, y;
	Image img;

	for (y = 0; y < map_height; ++y)
		for (x = 0; x < map_width; ++x) {

			levType t = type( Point(x,y) );
      // If player is on surface, default image is
      // that for the level terrain:
      if (player_loc.getz() == 0)
  			img = Image( levTypes[t].color, levTypes[t].appearance );
      else
        img = 0;

      bool is_created = created(Point3d(x,y,player_loc.getz()));
			if ( ! is_created || ! get(Point3d(x,y,player_loc.getz())).isCreated() ) {
        img.val.color = cUnexplored;
        }
      Image g = 0;
      if (is_created)
        g = get(Point3d(x,y,player_loc.getz())).getImage();
      if (g.i) img = g;

			MAP_DRAW(x,y, img );
			}

	MAP_DRAW( player_loc.getx(),player_loc.gety(), Image(cWhite,'@') );

	Screen::locate( Player::levLoc.toPoint() + Point(Screen::mapxoff+1,Screen::mapyoff+1) );

  Screen::showcursor();
  Screen::update();

  // The map is now on screen...
  // Take this opportunity
  // to cache some map:
  streamData();

	Keyboard::get();

	buffer.toScreen();
	}



Target Map::getTile( const Point3d &world_loc ) {

  assert( limits.contains( world_loc.toPoint() ) );

  return get( Point3d( world_loc.getx() / lev_width,
                       world_loc.gety() / lev_height,
                       world_loc.getz() )
             )->getTile(world_loc);
  }


long Map::tileVal( const Point3d &world_loc, StatType stat ) {
// Equivalent to tile->getVal(s)
  assert( limits.contains( world_loc.toPoint() ) );

  return get( Point3d( world_loc.getx() / lev_width,
                       world_loc.gety() / lev_height,
                       world_loc.getz() )
             )->tileVal(world_loc, stat);
  }
  
void Map::tileDraw( const Point3d &world_loc ) {
// Equivalent to tile->draw()
  assert( limits.contains( world_loc.toPoint() ) );

  get( Point3d( world_loc.getx() / lev_width,
                world_loc.gety() / lev_height,
                world_loc.getz() )
     )->tileDraw(world_loc);
  }
  
void Map::tileView( const Point3d &world_loc, int visible, int hypotenuse ) {
// Equivalent to tile->view(o)()
  if ( limits.contains( world_loc.toPoint() ) )
    get( Point3d( world_loc.getx() / lev_width,
                  world_loc.gety() / lev_height,
                  world_loc.getz() )
       )->tileView(world_loc,visible, hypotenuse);
  }
  
void Map::tileUnview( const Point3d &world_loc ) {
// Equivalent to tile->unview()
  if ( limits.contains( world_loc.toPoint() ) )

    get( Point3d( world_loc.getx() / lev_width,
                  world_loc.gety() / lev_height,
                  world_loc.getz() )
       )->tileUnview(world_loc);
  }
  
bool Map::tileCapable( const Point3d &world_loc, Action a, const Target &t ) {
// Equivalent to tile->capable(a,t)
  assert( limits.contains( world_loc.toPoint() ) );

  return get( Point3d( world_loc.getx() / lev_width,
                       world_loc.gety() / lev_height,
                       world_loc.getz() )
             )->tileCapable(world_loc, a,t );
  }
  
bool Map::tilePerform( const Point3d &world_loc, Action a, const Target &t ) {
// Equivalent to tile->perform(a,t)
  assert( limits.contains( world_loc.toPoint() ) );

  return get( Point3d( world_loc.getx() / lev_width,
                       world_loc.gety() / lev_height,
                       world_loc.getz() )
             )->tilePerform(world_loc, a, t );
  }
  
Action Map::tileAutoAction( const Point3d &world_loc ) {
// Equivalent to tile->autoAction()
  assert( limits.contains( world_loc.toPoint() ) );

  return get( Point3d( world_loc.getx() / lev_width,
                       world_loc.gety() / lev_height,
                       world_loc.getz() )
             )->tileAutoAction(world_loc);
  }

int Map::openLevel( const Point3d &p3d ) {

  if (p3d != OFF_MAP )
    open_levels.add( p3d );
    
  return open_levels.numData();
  }

void Map::keepOpen( const Rectangle &limit, int z ) {
// close all open levels besides those within limit

  //File Retain;

  bool remove = false;

  // Close all levels outside the limit:
  if (open_levels.reset())
  do {

    remove = ! ( limit.contains( open_levels.get().toPoint() )
             && z == open_levels.get().getz() );
    if (remove) {
      get( open_levels.remove() ).save();
      }
    
    } while ( remove && open_levels.numData()
             || open_levels.next() );

  // If this limit isn't the same as we already had,
  if ( (open_limit != limit || open_z != z)&&
        limit != Rectangle(-1,-1,-1,-1)  ) {
    // Set up the short-circuit pointers:

    int max_x = EW_LEV_BORDER * 2 + 1,
        max_y = NS_LEV_BORDER * 2 + 1,
        x, y;

    // Ensure quick_level is allocated:
    if (!quick_level) {
    
      quick_level = new LevelHandle **[max_x];
      
      for (x = 0; x < max_x; ++x)
        quick_level[x] = new LevelHandle *[max_y];

      }
    
    // First clear all short circuits so we don't try to
    // quick-link to the wrong level when we soon call Map::get()
    for (y = 0; y < max_y; ++y)
      for (x = 0; x < max_x; ++x)
        quick_level[x][y] = NULL;


    open_limit = Rectangle(-1,-1,-1,-1);
    // Find and point to each level within the open limit:
    for (y = 0; y < max_y; ++y)
      for (x = 0; x < max_x; ++x)
        if ( x+limit.getxa() >= 0 && x+limit.getxa() < map_width &&
             y+limit.getya() >= 0 && y+limit.getya() < map_height )
          quick_level[x][y] = &get( Point3d(x+limit.getxa(),y+limit.getya(),z) );



    }

  open_limit = limit;
  open_z = z;
  }

LevelHandle &Map::focus( const Point3d &p3d ) {
// This function keeps track of which level is considered
// to be the center of the action, i.e. which level the player
// is on.  It also adjusts the display when we move out of the
// center of the screen.

  bool instant_update = global::options.follow_player;
  Point3d old_vis_offset = vis_offset;

  Point3d lev_pt = p3d;
  lev_pt.getx() /= lev_width;
  lev_pt.gety() /= lev_height;
  open_z = p3d.getz();

  // If we don't want instant updates,
  // Only change the focus if we've moved to a new level:
  if (!instant_update && player_lev_pt == lev_pt)
  	return get( player_lev_pt );
  
  // Is this the first focus?
  bool first_focus = (player_lev_pt == OFF_MAP);
  Point3d delta = lev_pt - player_lev_pt;
  
  player_lev_pt = lev_pt;

  if ( first_focus ) focus_pt = lev_pt;

  // This method does a lot of visual manipulation so make
  // sure we don't update the screen until we're done:
  Screen::hidecursor();
  
  // Update visual offset:
  int new_xoff, new_yoff;
  if (instant_update) {
    new_xoff = -p3d.getx() +(map_width)/ 2;
    new_yoff = -p3d.gety() +(map_height)/ 2;
    }
  else {
    new_xoff = lev_pt.getx() * -lev_width + (map_width - lev_width)/ 2;
    new_yoff = lev_pt.gety() * -lev_height +(map_height-lev_height)/ 2;
    }

  // If we haven't focused yet, focus on this tile:
  if ( first_focus ) {
        
    Screen::levxoff = new_xoff;
    Screen::levyoff = new_yoff;

    // Call the macro that sets the new visual offset:
    VIS_OFFSET_SET;
    }
  // otherwise, adjust the offset when required:
  else {

    // Preserve to calculate the unit-scale delta:
    Point offsets( Screen::levxoff, Screen::levyoff );
    
    if (instant_update) {
      // This section deals with when the screen follows the
      // player each turn.
      Screen::levxoff = new_xoff;
      Screen::levyoff = new_yoff;
      // Update the focus point if we moved a whole level:
      focus_pt.getx() += delta.getx();
      focus_pt.gety() += delta.gety();
      focus_pt.getz() += delta.getz();

      VIS_OFFSET_SET;
      }

    else {
  
    /* We want the screen to follow the player loosely, not keep him
     * centered at all times.  The follow mock-up shows the boundary
     * we should let the player roam about within:
     *
     * ::||||::::||||::  Where '|' and ':' represent off-limits tiles
     * ::||||::::||||::    and '/' and '%' represent tiles that the
     * ||////%%%%////||                  player may walk within and
     * ||/@//%%%%////||                  not worry about scrolling.
     * ||////%%%%////||  We should always have half a level as a border
     * ||////%%%%////||  surrounding the playfield.
     * ::||||::::||||::   ( I used two images for each type to demonstrate
     * ::||||::::||||::     how the levels are partitioned on-screen.)
     *
     *  The '@' in the diagram shows where the player might be;
     *  player_lev_pt keeps track of this level's coordinates.
     *
     *  The '%'s show the level in the center of the screen;
     *  focus_pt keeps track of this level's coordinates.
     *
     */

      // If we have moved levels horizontally and we are more than
      // one level's width from the center of the screen, update:
      if (delta.getx() != 0 && abs(Screen::levxoff-new_xoff) > lev_width ) {
        Screen::levxoff -= lev_width * 3 * delta.getx();
        focus_pt.getx() += 3 * delta.getx();
        VIS_OFFSET_SET;
        //vis_offset.set( (focus_pt.getx()-1) * lev_width,
        //                (focus_pt.gety()) * lev_height,
        //                focus_pt.getz() );
        }
      
      // Whenever we have moved vertically we update:
      if (delta.gety() != 0) {
        Screen::levyoff = new_yoff;
        focus_pt.gety() += delta.gety();
        VIS_OFFSET_SET;
        //vis_offset.set( (focus_pt.getx()-1) * lev_width,
        //                focus_pt.gety() * lev_height,
        //                focus_pt.getz() );
        }

      // Whenever we have changed altitudes, we must update the z coord:
      focus_pt.getz() += delta.getz();
      
      }

    // Determine the unit-scale delta:
    offsets -= Point( Screen::levxoff, Screen::levyoff );
    
    // Now if we've moved we should shift the screen contents to
    // give that nice 'scrolling' effect:

    if ( offsets != Point(0,0) ) {
      Screen::Buffer buffer;
      buffer.setSize( Rectangle( Screen::mapxmin, Screen::mapymin,
                                 Screen::mapxmax, Screen::mapymax ) );
      buffer.shiftData( -offsets );
      }

    // shift the ortho buffer so we don't have to recalculate all of
    // the opacity values:
    //size_t struct_size = sizeof(orthoRec);
    //int width  = ORTHO_WIDTH;
    //int height = ORTHO_HEIGHT;
    //Screen::Buffer::ShiftBuffer( ortho, width, height, struct_size, -offsets);

    //recalculate ortho values:
    if (vis_offset != old_vis_offset && delta.getz() == 0)
      allocOrtho();
      
    // And draw the newly visible parts:
    if (!first_focus && delta.getz() == 0)
      shiftMap( -offsets );

      //shiftMap( Point3d(vis_offset - old_vis_offset).toPoint() );


    // The ortho values should really only be recalculated for
    // tiles that now have invalid values.  This can't be done with
    // a buffer shift ATM as for some reason it fails.  Can't find the
    // bug.  Damn.  For later as a speed tweak.

    }
  
  // If we've moved inter-level,
  if ( delta != Point3d(0,0,0) ) {

    // Don't do this here or you'll get stutter whenever
    // the player tries to walk between levels!  Since we're
    // trying to stream levels to and from disk, we should clean
    // up during convenient user-interface moments.
    
    // // close levels out of local area:
    // keepOpen( Rectangle(focus_pt.toPoint(),focus_pt.toPoint())
    //          +LOADED_RECT,
    //           focus_pt.getz() );

    // if we've moved up or down levels,
    if (delta.getz() != 0 &&
    // and it's not our first focus,
        ! first_focus ) {
      // close excess open levels:
      keepOpen( Rectangle(focus_pt.toPoint(),focus_pt.toPoint())
               +LOADED_RECT,
                focus_pt.getz() );
      // and clear the screen:
      Screen::cls();
      }
    }

  // Now if we've moved up or down levels,
  // refresh the ortho array:
  if (delta.getz() != 0)
    allocOrtho();

  // We're done with visuals, updating the screen is OK:
  Screen::showcursor();
  
	return get( player_lev_pt );
  }


void Map::streamData( void ) {
// Saves all levels not within the immediate area
// of the player so that we're using as little memory as possible.

// We want this to be called any time the user presses any key
// besides a cursor key (including the 'm'ovement key) so that
// we can minimise jerky movement.

  keepOpen( Rectangle(focus_pt.toPoint(),focus_pt.toPoint())
           +LOADED_RECT,
            focus_pt.getz() );
  }
  
/*
void Map::setImage( Point p, char h, Image i ) {

	if (!levels)
    Error::fatal("Map::setImage error type 1");

	if ( ! levels[p.getx()][p.gety()] )
    Error::fatal("Map::setImage error type 2");

	if ( ! levels[p.getx()][p.gety()]->reset() )
    Error::fatal("Map::setImage error type 3");

	do {

		if ( levels[p.getx()][p.gety()]->get().height() == h ) {
      levels[p.getx()][p.gety()]->get().setImage(i);
      return;
      }

		} while ( levels[p.getx()][p.gety()]->next() );

  Error::fatal("Map::setImage error type 4");
  }
*/

#endif // MAP_CPP

