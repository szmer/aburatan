// [features.cpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( FEATURES_CPP )
#define FEATURES_CPP

#include "features.hpp"
#include "random.hpp"
#include "tile.hpp"
#include "door.hpp"
#include "trap.hpp"
#include "stairs.hpp"
#include "message.hpp"
#include "path.hpp"

#include "dun_test.hpp"
#include "fluid.hpp"
#include "bagbox.hpp"
#include "food.hpp"
#include "ammo.hpp"
#include "weapon.hpp"
#include "clothing.hpp"
//#include "light.hpp"
#include "quantity.hpp"
#include "monster.hpp"
#include "scope.hpp"
#include "book.hpp"
#include "terrain.hpp"
#include "widget.hpp"
#include "money.hpp"
#include "file.hpp"

//-[ class Field ]-----------
Field::Field( const Option *o )
: Feature(o), type(o->material) {
	}

void Field::build( const Option &o ) {
  o.level->addFeature( THIS );

  int z = levelloc.getz();
	for (int y = border.getya(); y <= border.getyb(); ++y)
		for (int x = border.getxa(); x <= border.getxb(); ++x) {
      Tile::Option tile_opt;
      tile_opt.location = Point3d(x,y,z);
      tile_opt.floor = type;
      tile_opt.type = tt_ground;
      tile_opt.feature = THIS;
			o.level->tileCreate( tile_opt );
			}
	}

Field::Field( std::iostream &ios, TargetValue *TVP )
: Feature( ios, TVP ) {

	// Read char type
	ios.read( (char*) &type, sizeof(type) );

	}

void Field::toFile( std::iostream &ios ) {

	// Write Feature base information:
	Feature::toFile( ios );

	// Write char type
	ios.write( (char*) &type, sizeof(type) );

	}

targetType Field::gettype( void ) const {
	return tg_field;
	}

bool Field::istype( targetType t ) const {

	if ( t == tg_field )
		return true;
	else return Feature::istype(t);

	}

String Field::name( NameOption ) {
  String fname;
  if (levelloc.getz() < 0)  fname = " cavern~";
  if (levelloc.getz() == 0) fname = " field~";
  if (levelloc.getz() > 0)  fname = " area~";
  return String("#") + Material::data[type].name	+ fname;
	}

//-[ class FourField ]-----------
FourField::FourField( const Option *o  )
: Feature(o) {
	}

void FourField::build( const Option &o ) {
  o.level->addFeature( THIS );
  
	materialClass type;

	int width = border.getwidth(),
			height = border.getheight();
  int z = levelloc.getz();

	for (int y = border.getya(); y <= border.getyb(); ++y)
		for (int x = border.getxa(); x <= border.getxb(); ++x) {


      int
       chance_n = (height - (y - border.getya()) ) * width,
       chance_s = (y - border.getya() ) * width,
       chance_e = (x - border.getxa() ) * height,
       chance_w = (width - (x - border.getxa()) ) * height,
       chance_total = chance_n+chance_s+chance_e+chance_w;

      int choice = Random::randint(chance_total);

      if ( choice < chance_n )
        type = o.north;
      else if ( choice < chance_n + chance_s)
        type = o.south;
      else if ( choice < chance_n + chance_s + chance_e )
        type = o.east;
      else
        type = o.west;


      /*
      if (Random::randbool())
				type = (Random::randint(height)+border.getya() > y )
								 ?type_n:type_s;
			else
				type = (Random::randint(width)+border.getxa() > x )
								 ?type_w:type_e;
      */

      Tile::Option tile_opt;
      tile_opt.location = Point3d(x,y,z);
      tile_opt.floor = type;
      tile_opt.type = tt_ground;

			tile_opt.feature = THIS;
			o.level->tileCreate( tile_opt );
			}

	}

FourField::FourField( std::iostream &ios, TargetValue *TVP )
: Feature( ios, TVP ) {

	}

void FourField::toFile( std::iostream &ios ) {

	// Write Feature base information:
	Feature::toFile( ios );

	}

targetType FourField::gettype( void ) const {
	return tg_fourfield;
	}

bool FourField::istype( targetType t ) const {

	if ( t == tg_fourfield )
		return true;
	else return Feature::istype(t);

	}

String FourField::name( NameOption ) {
	return String("#field~");
	}

long FourField::getVal( StatType s ) {

  switch(s) {
  
    case attrib_ooo_ness:
      return 1;

    default:
      return Feature::getVal(s);
    }
  }

//-[ class Solid ]-----------
Solid::Solid( const Option *o )
: Feature(o), type(o->material) {
	}

void Solid::build( const Option &o ) {
  o.level->addFeature( THIS );

  int z = levelloc.getz();
	for (int y = border.getya(); y <= border.getyb(); ++y)
		for (int x = border.getxa(); x <= border.getxb(); ++x) {

      Tile::Option tile_opt;
      tile_opt.location = Point3d(x,y,z);
      tile_opt.floor = tile_opt.object = type;
      tile_opt.type = tt_wall;
      // Todo: call when opt_nethack_corridor is true
      //tile_opt.image.val.appearance = ' ';

			tile_opt.feature = THIS;
			o.level->tileCreate( tile_opt );
			}
	}

Solid::Solid( std::iostream &ios, TargetValue *TVP )
: Feature( ios, TVP ) {

	// Read char type
	ios.read( (char*) &type, sizeof(type) );

	}

void Solid::toFile( std::iostream &ios ) {

	// Write Item base information:
	Feature::toFile( ios );

	// Write char type
	ios.write( (char*) &type, sizeof(type) );

	}

targetType Solid::gettype( void ) const {
	return tg_solid;
	}

bool Solid::istype( targetType t ) const {

	if ( t == tg_solid )
		return true;
	else return Feature::istype(t);

	}

String Solid::name( NameOption ) {
	return String("#solid") + Material::data[type].name	+ "~";
	}

//-[ class House ]-----------
House::House( const Option *o )
: Feature(o), shop( o->shop ) {
	}

void House::build( const Option &o ) {
  int x, y;
  int z = levelloc.getz();
	bool created = false;

	if ( o.split && (border.getwidth() > 8) && !Random::randint( 10 - border.getwidth() ) ) {

		x = border.getxa() + 4 + Random::randint( border.getwidth() - 8 );

		Rectangle newHouse = border;
		newHouse.setxb( x - 1 );
		if ( Random::randint(6) < newHouse.getwidth() ) {
      Option new_opt;
      new_opt.border = newHouse;
      new_opt.level = o.level;
      new_opt.shop = o.shop;
      new_opt.max_shops = o.max_shops/2 + Random::randint(o.max_shops%2+1);
			House::create( new_opt );
			created = true;
			}

		newHouse = border;
		newHouse.setxa( x + 1 );
		if ( Random::randint(6) < newHouse.getwidth() || !created ) {
      Option new_opt;
      new_opt.border = newHouse;
      new_opt.level = o.level;
      new_opt.shop = o.shop;
      new_opt.max_shops = o.max_shops/2 + Random::randint(o.max_shops%2+1);
			House::create( new_opt );
			}

		return;
		}
	else if ( o.split && (border.getheight() > 8) && !Random::randint( 10 - border.getheight() ) ) {

		y = border.getya() + 4 + Random::randint( border.getheight() - 8 );

		Rectangle newHouse = border;
		newHouse.setyb( y - 1 );
		if ( Random::randint(6) < newHouse.getheight() ) {
      Option new_opt;
      new_opt.border = newHouse;
      new_opt.level = o.level;
      new_opt.shop = o.shop;
      new_opt.max_shops = o.max_shops/2 + Random::randint(o.max_shops%2+1);
			House::create( new_opt );
			created = true;
			}

		newHouse = border;
		newHouse.setya( y + 1 );
		if ( Random::randint(6) < newHouse.getheight() || !created) {
      Option new_opt;
      new_opt.border = newHouse;
      new_opt.level = o.level;
      new_opt.shop = o.shop;
      new_opt.max_shops = o.max_shops/2 + Random::randint(o.max_shops%2+1);
			House::create( new_opt );
			}

		return;

		}

  o.level->addFeature( THIS );

  shop = o.shop;
  if (shop == shop_random)
    shop = (Shop) Random::randint(shop_num);

  if (o.max_shops <= 0)
    shop = shop_none;

	//  0
	// 3 2    <- Doorsides NSEW
	//  1

	// Randomly shrink the house size:
	// Width:
	if (Random::randint(3) + 4 < border.getwidth() )
		if ( Random::randint(2) )
			border.geta().incx();
		else
			border.getb().decx();

	// Height:
	if (Random::randint(3) + 4 < border.getheight() )
		if ( Random::randint(2) )
			border.geta().incy();
		else
			border.getb().decy();

	// Create the house:

  /** House quality **
   *
   *         floor      walls
   *
   * poor    dirt       brick
   * low     wood       brick
   * medium  stone      brick
   * high    tile       stone
   *
   */

  enum hqTemp {
    hq_poor,
    hq_low,
    hq_medium,
    hq_high,
    
    hq_num
    } house_quality = (hqTemp) Random::randint(hq_num);
   
  enum ftTemp {
    ft_dirt,
    ft_wood,
    ft_stone,
    ft_brick,
    ft_tile,
    
    ft_num
    } floor_type;

  materialClass wall_type;

  switch (house_quality) {
    case hq_low:    floor_type = ft_wood;  wall_type = m_brick; break;
    case hq_medium: floor_type = ft_stone; wall_type = m_brick; break;
    case hq_high:   floor_type = ft_tile;  wall_type = m_stone; break;
    default:        floor_type = ft_dirt;  wall_type = m_brick; break;
    }

	char doorSide = Random::randint(4);
	int doorPos;
	if (doorSide < 2)
		doorPos = Random::randint( border.getwidth()-2 ) + border.getxa()+1;
	else
		doorPos = Random::randint( border.getheight()-2 ) + border.getya()+1;

  // Place the floor:
  ftTemp f_type = floor_type;
    
	for (y = border.getya(); y <= border.getyb(); ++y)
		for (x = border.getxa(); x <= border.getxb(); ++x) {

      if (floor_type == ft_tile)
        f_type = (x%2==y%2)?ft_stone:ft_brick;

      Tile::Option tile_opt;
      tile_opt.location = Point3d(x,y,z);
      tile_opt.type = tt_floor;

			switch (f_type) {
        case ft_dirt:  tile_opt.floor = m_dirt; break;
        case ft_wood:  tile_opt.floor = m_wood; break;
        case ft_stone: tile_opt.floor = m_stone; break;
        case ft_brick: tile_opt.floor = m_brick; break;
        default: assert(0); // Error::fatal("Invalid house floor type!");
        }

      // Blood scrawl here was for debugging.
      // Small chance of blood scrawl:
      /*
      if (Random::randint(100) < 2) {
        tile_opt.etching = e_graffiti;
        tile_opt.etch_material = m_blood;
        tile_opt.graffiti = Random::graffiti();
        }
      */

      
			tile_opt.feature = THIS;
			o.level->tileCreate( tile_opt );
			}

  // Houses only have one door ATM so it should never be hidden:
  Door::Option door_opt;
  door_opt.hidden = false; // Random::randbool();
	door_opt.material = door_opt.hidden?wall_type:m_wood;

  Tile::Option tile_opt;
  tile_opt.type = tt_wall;
  tile_opt.floor = tile_opt.object = wall_type;

	for (y = border.getya(); y <= border.getyb(); ++y) {
		x = border.getxa();
		if ( (doorSide == 3) && (y == doorPos) )
			o.level->getTile( Point3d(x,y,z) )->perform( aContain, Door::create(door_opt) );
		else {
      tile_opt.location = Point3d(x,y,z);
			tile_opt.feature = THIS;
			o.level->tileCreate( tile_opt );
			}

		x = border.getxb();
		if ( (doorSide == 2) && (y == doorPos) )
			o.level->getTile( Point3d(x,y,z) )->perform( aContain, Door::create(door_opt) );
		else {
      tile_opt.location = Point3d(x,y,z);
			tile_opt.feature = THIS;
			o.level->tileCreate( tile_opt );
			}
		}

	for (x = border.getxa()+1; x < border.getxb(); ++x) {
		y = border.getya();
		if ( (doorSide == 0) && (x == doorPos) )
			o.level->getTile( Point3d(x,y,z) )->perform( aContain, Door::create(door_opt) );
		else {
      tile_opt.location = Point3d(x,y,z);
			tile_opt.feature = THIS;
			o.level->tileCreate( tile_opt );
			}

		y = border.getyb();
		if ( (doorSide == 1) && (x == doorPos) )
			o.level->getTile( Point3d(x,y,z) )->perform( aContain, Door::create(door_opt) );
		else {
      tile_opt.location = Point3d(x,y,z);
			tile_opt.feature = THIS;
			o.level->tileCreate( tile_opt );
			}
		}


  
    // Place home owner:
  // At the moment all residents begin within their house:
  Human::Option human_opt;
      
  switch (shop) {
    case shop_military:
    case shop_hunting:
    case shop_hardware:
    case shop_finewear:
    case shop_workwear:
      human_opt.job = job_sales; break;

    case shop_stable: human_opt.job = job_stablehand; break;
    case shop_pharmacy: human_opt.job = job_pharmacist; break;
    case shop_library:human_opt.job = job_librarian; break;
    case shop_saloon: human_opt.job = job_bartender; break;
    case shop_market: human_opt.job = job_blackmarket; break;
    //case shop_church: human_opt.job = job_clergy; break;
    default: {}
    }


  Target human = Human::create(human_opt);

  Point3d p( Random::randint(border.getwidth()-2)+border.getxa()+1,
             Random::randint(border.getheight()-2)+border.getya()+1 );
  getlevel().getTile(p)->perform( aContain,human );

  // Set this human as the house owner:
  if (shop != shop_church)
    perform( aSetOwner, human );

  Target shop_item;
  switch (shop) {
    // Most shop types are simply furnished:
    // Just cover the floor in items.
    case shop_military:
    case shop_hunting:
    case shop_pharmacy:
    case shop_hardware:
    case shop_finewear:
    case shop_workwear:
    case shop_library:
    case shop_market:
      for (y = border.getya()+1; y < border.getyb(); ++y)
        for (x = border.getxa()+1; x < border.getxb(); ++x) {
          shop_item = Random::shopitem( shop );
          shop_item->perform( aSetOwner, human );
          o.level->getTile( Point3d(x,y,z) )->perform( aContain, shop_item );
          }

    case shop_stable:
    case shop_saloon:
    case shop_church:
    default: break;
    }



	// 25% of medium and 50% of high quality houses have down-stairs:
//  int quartile = Random::randint(4);
//	if (shop == shop_none
//  && (    quartile > 1 && house_quality > hq_medium
//      ||  quartile > 2 && house_quality == hq_medium )  ) {
  // But for the moment, all houses have them:
  if (true) {
		x = Random::randint( border.getwidth() -2 ) + border.getxa() + 1;
		y = Random::randint( border.getheight()-2 ) + border.getya() + 1;

    Stairs::Option stairs_opt;
    stairs_opt.connecting = z-1;
		o.level->getTile( Point3d(x,y,z) )->perform( aContain, Stairs::create( stairs_opt ) );

		// Make sure lower level has a room/staircase at this position
		Map::get( o.level->loc() - Point3d(0,0,1) ).addRequirement( Req(Req::stairs_up,Point(x,y)) );

		}

	}

House::House( std::iostream &ios, TargetValue *TVP )
: Feature( ios, TVP ) {
	// Read enum shop
	ios.read( (char*) &shop, sizeof(shop) );
  }

void House::toFile( std::iostream &ios ) {
	// Write Feature base information:
	Feature::toFile( ios );
	// Write enum shop
	ios.write( (char*) &shop, sizeof(shop) );
	}


targetType House::gettype( void ) const {
	return tg_house;
	}

bool House::istype( targetType t ) const {

	if ( t == tg_house )
		return true;
	else return Feature::istype(t);

	}

String House::name( NameOption ) {
  switch( shop ) {
    case shop_military: return String("#military surplus store~");
    case shop_hunting: return String("#hunting goods store~");
    case shop_stable: return String("#stable~");
    case shop_pharmacy: return String("#pharmacy~");
    case shop_hardware: return String("#hardware store~");
    case shop_finewear: return String("#fine clothing store~");
    case shop_workwear: return String("#workwear store~");
    case shop_library: return String("#library~");
    case shop_saloon: return String("#saloon~");
    case shop_church: return String("#church~");
    case shop_market: return String("#black market~");
    default:
    	return String("#house~");
    }
    
	}

long House::getVal( StatType s ) {

  switch(s) {
  
    case attrib_ooo_ness:
      return 2;

    // The following lines make houses always lit, even at night:
    //case attrib_sunlit:
      // return 1;
      
    default:
      return Feature::getVal(s);
    }
  }

bool House::perform( Action a, const Target &t ) {

  switch (a) {

    case aAlertTrespass:
      if (!!owner) return owner->perform(aAlertTrespass, t);
      return false;

    case aPlaceMons: {
      // Home owners are placed in the build() routine
      return true;
      }

    default:
      return Feature::perform(a,t);
      
    }

  }

//-[ class Forest ]-----------
Forest::Forest( const Option *o )
: Feature(o) {
	}

void Forest::build( const Option &o ) {
  o.level->addFeature( THIS );

	int x, y;
  int z = levelloc.getz();
	int startX = border.getxa(), startY = border.getya();
	int width = border.getwidth(),
			height =  border.getheight();

	for (y = border.getya(); y <= border.getyb(); ++y)
		for (x = border.getxa(); x <= border.getxb(); ++x) {

			bool mx = (Random::randint (abs((x-startX)-width/2) * 100 /width ) > 15  )?true:false,
					 my = (Random::randint (abs((y-startY)-height/2) * 100 /height ) > 15)?true:false;

			if (! ( (mx) || (my) ) ) {

					materialClass ground = (materialClass) o.level->getTile( Point3d(x,y,z) )->getVal(attrib_material);

          Tile::Option tile_opt;
          tile_opt.type = tt_tree;
					bool placeTree = true;

					if ( ground == m_sand ) {
            placeTree = (Random::randint(8))?false:true;
            tile_opt.type = tt_cactus;
            }
					else if ( ground == m_dirt ) {
            placeTree = (Random::randint(5))?false:true;
            tile_opt.type = tt_bush;
            }
					else if ( ground == m_water ) {
            placeTree = false;
            }

					if (placeTree) {
            tile_opt.floor = ground;
            tile_opt.object = m_wood;
            tile_opt.location = Point3d(x,y,z);
						tile_opt.feature = THIS;
            o.level->tileCreate( tile_opt );
						}

					}
			else
				// 1% chance of a trap in open:
				if ( !Random::randint(100) )
					o.level->getTile( Point3d(x,y,z) )->perform( aContain, Trap::create() );

			}

	}

Forest::Forest( std::iostream &ios, TargetValue *TVP )
: Feature( ios, TVP ) {	}


targetType Forest::gettype( void ) const {
	return tg_forest;
	}

bool Forest::istype( targetType t ) const {

	if ( t == tg_forest )
		return true;
	else return Feature::istype(t);

	}

String Forest::name( NameOption ) {
	return String("#forest~");
	}

long Forest::getVal( StatType s ) {

  switch(s) {
  
    case attrib_ooo_ness:
      return (border.getwidth() + border.getheight() > 50)?2:0;

    default:
      return Feature::getVal(s);
    }
  }
  

//-[ class Stairway ]-----------
Stairway::Stairway( const Option *o )
: Feature(o), connecting(o->connecting) {

  if (o->connecting > levelloc.getz()) image.val.appearance = '<';
  
  border = Rectangle(o->location.toPoint(),o->location.toPoint());
  
	}

long Stairway::getVal( StatType s ) {

  switch(s) {
  
    case attrib_ooo_ness:
      return 3;
      
    default:
      return Feature::getVal(s);
    }

  }

void Stairway::build( const Option &o ) {
  o.level->addFeature( THIS );

	int x = border.getxa(), y = border.getya();
  int z = levelloc.getz();

  Tile::Option tile_opt;
  tile_opt.location = Point3d(x,y,z);
  tile_opt.floor = m_stone;
  tile_opt.type = tt_floor;
	tile_opt.feature = THIS;
  o.level->tileCreate( tile_opt );

  Stairs::Option stair_opt;
  stair_opt.connecting = connecting;
	o.level->getTile( Point3d(x,y,z) )->perform( aContain, Stairs::create( stair_opt ) );

	}

Stairway::Stairway( std::iostream &ios, TargetValue *TVP )
: Feature( ios, TVP ) {

	// Read char connecting
	ios.read( (char*) &connecting, sizeof(connecting) );
	}

void Stairway::toFile( std::iostream &ios ) {

	// Write Feature base information:
	Feature::toFile( ios );

	// Write char type
	ios.write( (char*) &connecting, sizeof(connecting) );

	}

targetType Stairway::gettype( void ) const {
	return tg_stairway;
	}

bool Stairway::istype( targetType t ) const {

	if ( t == tg_stairway )
		return true;
	else return Feature::istype(t);

	}

String Stairway::name( NameOption ) {
	return String("#stairway~");
	}

//-[ class Corridor ]-----------
Corridor::Corridor( const Option *o )
: Feature(o), finish(o->finish), type(o->material) {

  border = Rectangle(o->start,o->finish);
	}

void Corridor::build( const Option &o ) {
  // Don't add the corridor to the feature list
  // because it's so trivial:
  //o.level->addFeature( THIS );

	int width = border.getwidth(),
			height =  border.getheight();

  // the bool L denotes whether the corridor
  // is L/F shaped or otherwise
  //
  // L: |   F: |~   otherwise: ~|  |
  //    |_     |                | _|
  
	bool L = Random::randbool();
	Point3d h1, h2, v1, v2;
  
  int z = o.level->loc().getz();

  Tile::Option tile_opt;
  tile_opt.floor = type;
  tile_opt.type = tt_ground;
  tile_opt.feature = THIS;

  bool h_done = false;
  
	if ( width > 1 ) {

		if (L) {
			h1 = Point3d( border.getxa(), finish.gety(), z );
			h2 = Point3d( finish.getx(),  finish.gety(), z );
			}
		else {
			h1 = Point3d(border.getxa(), border.getya(), z );
			h2 = Point3d( finish.getx(), border.getya(), z );
			}

		Path h (h1,h2);
		h.calculate();

		if ( h.reset() )
		do {

			if ( ! o.level->getTile( h.get() )->getVal(attrib_non_wall) ) {
        tile_opt.location = Point3d(h.get().getx(),h.get().gety(),levelloc.getz());
        o.level->tileCreate( tile_opt );
				}
      else if (o.stop_on_floor)
        h_done = true;

			} while ( h.next() && !h_done );

    if ( o.include_ends ) {
      tile_opt.location = h1;
      o.level->tileCreate( tile_opt );
      // Only do the end point if we aren't doing stop_on_floor
      // or we didn't stop on floor:
      if ( !o.stop_on_floor || ! h_done ) {
        tile_opt.location = h2;
        o.level->tileCreate( tile_opt );
        }
      }
    
		}

  bool v_done = false;
  
  if ( height > 1 ) {

		if (L) {
			v1 = Point3d(border.getxa(), border.getya(), z);
			v2 = Point3d( border.getxa(), finish.gety(), z);
			}
		else {
			v1 = Point3d( finish.getx(), border.getya(), z);
			v2 = Point3d( finish.getx(), finish.gety(), z);
			}


		Path v (v1,v2);
		v.calculate();

		if ( v.reset() )
		do {

			if ( ! o.level->getTile( v.get() )->getVal(attrib_non_wall) ) {
        tile_opt.location = Point3d(v.get().getx(),v.get().gety(),levelloc.getz() );
        o.level->tileCreate( tile_opt );
				}
      else if (o.stop_on_floor)
        v_done = true;

			} while ( v.next() && !v_done );

    if ( o.include_ends ) {
      tile_opt.location = v1;
      o.level->tileCreate( tile_opt );
      // Only do the end point if we aren't doing stop_on_floor
      // or we didn't stop on floor:
      if ( !o.stop_on_floor || ! v_done ) {
        tile_opt.location = v2;
        o.level->tileCreate( tile_opt );
        }
      }
      
    }

	}

Corridor::Corridor( std::iostream &ios, TargetValue *TVP )
: Feature( ios, TVP ) {

	// Read Point finish
	ios.read( (char*) &finish, sizeof(finish) );

	// Read char type
	ios.read( (char*) &type, sizeof(type) );
	}

void Corridor::toFile( std::iostream &ios ) {

	// Write Feature base information:
	Feature::toFile( ios );

	// Write Point finish
	ios.write( (char*) &finish, sizeof(finish) );

	// Write char type
	ios.write( (char*) &type, sizeof(type) );

	}


targetType Corridor::gettype( void ) const {
	return tg_corridor;
	}

bool Corridor::istype( targetType t ) const {

	if ( t == tg_corridor )
		return true;
	else return Feature::istype(t);

	}

String Corridor::name( NameOption ) {
	return String("#") + Material::data[type].name	+ " corridor~";
	}


//-[ class Town ]-----------
Town::Town( const Option *o )
: Feature(o) {

  f_name = Random::townname(true);

	}

long Town::getVal( StatType s ) {

  switch(s) {
  
    case attrib_ooo_ness:
      return 4;
      
    default:
      return Feature::getVal(s);
    }

  }
void Town::build( const Option &o ) {
  o.level->addFeature( THIS );
  Rectangle borderRect = o.level->getBorder();

  House::Option house_opt;
  house_opt.border = borderRect + Rectangle(1,1,-1,-1);
  house_opt.level = o.level;
  house_opt.shop = shop_random;
  house_opt.max_shops = 2;
  House::create(house_opt);

/*//------------

	int x, y, i,
      z = levelloc.getz();

	int houses = DiceRoll( 1,3,+1).roll();
	Rectangle houseLimit( borderRect.getxa()+3,borderRect.getya()+3,borderRect.getxb()-3,borderRect.getyb()-3);
	Rectangle houseBorder;

	for (i = 0; i < houses; ++i) {
		bool done = true;
		int attempts = 0;
		do {
			done = true;
			x = Random::randint(lev_width-6)+6+borderRect.getxa();
			y = Random::randint(lev_height-6)+6+borderRect.getya();
			houseBorder.set( x-Random::randint(6)-houses+i,y-Random::randint(4)-houses+i,
											 x+Random::randint(6)+houses-i,y+Random::randint(4)+houses-i);
			houseBorder.setintersect( houseLimit );

			for (y = houseBorder.getya()-1; y <= houseBorder.getyb()+1; ++y)
				for (x = houseBorder.getxa()-1; x <= houseBorder.getxb()+1; ++x)
					if ( o.level->getTile(Point3d(x,y,z))->getTarget(tg_parent)->istype( tg_house ) ) done = false;

			attempts ++;

			} while (
							(
							 ( houseBorder.getwidth() < 3 )
								||
							 ( houseBorder.getheight() < 3 )
								||
							 !done
							)
							&&
							(  attempts < NUM_ABORT )
					);
		if (attempts < NUM_ABORT) {
      House::Option house_opt;
      house_opt.border = houseBorder;
      house_opt.level = o.level;
      house_opt.shop = shop_random;
      house_opt.max_shops = i;
      House::create(house_opt);
      }
		}


  //int house_width = DiceRoll(3,6, 12).roll(),
  //    house_height= DiceRoll(2,3,  8).roll();
  //Point a( Random::randint(lev_width-6-house_width)+3,
  //         Random::randint(lev_height-6-house_height)+3);
  //House::Option house_opt;
  //house_opt.border.set(a,a+Point(house_width,house_height));
  //house_opt.level = o.level;
  //House::create(house_opt);

  //*///

  // Towns have basement levels:
  //Map::get( o.level->loc() - Point3d(0,0,1) ).addRequirement( Req(Req::basement,borderRect) );


  // Make a perimeter wall / fence?
  List<Target>   &feat_list = o.level->featureList();
  Perimeter::Option per_opt;
  per_opt.level = o.level;
  per_opt.largest_gap = 8;

  // All houses to be fenced:
  Rectangle yard(-2,-2,+2,+2), house;
  if ( feat_list.reset() )
  do {
    if (feat_list.get()->istype(tg_house)) {
      house = feat_list.get()->getborder();
      int max_len = max(house.getwidth(),house.getheight()),
          min_len = min(house.getwidth(),house.getheight());
      if (Random::randint( max_len ) >= 4 && min_len >= 4
         || min_len > 6 )
        per_opt.rect_list.add( house );
      else
        per_opt.rect_list.add( house+yard );
      }
    } while (feat_list.next());

  // All entrances should be fenced:
//	for (y = border.getya(); y <= border.getyb(); ++y)
//for (x = border.getxa(); x <= border.getxb(); ++x)
//      if ( o.level->getTile( Point3d(x,y,z) )->getVal(attrib_has_door) )
//        per_opt.rect_list.add( Rectangle(x,y,x,y)+Rectangle(-1,-1,1,1) );
  

  // Large towns get a stone wall:
  //if ( per_opt.rect_list.numData() > 5 ) {
  //	Perimeter::create( per_opt.rect_list, l, m_stone, true );
  //	o.level->placeDoors();
  //  }
  // Medium towns get an iron fence:
  //else
//  if ( per_opt.rect_list.numData() > 4 ) {
//    per_opt.material = m_wood;
//  	Perimeter::create( per_opt );
//    }

	}

/*
bool Town::perform( Action a, const Target &t ) {

  switch (a) {

    case aPlaceMons: {

      List<Target> to_place;
      int i, population = 0;

      List<Target> &feat_list = getlevel().featureList();

      if ( feat_list.reset() )
      do {
        if (feat_list.get()->istype(tg_house))
          ++population;

        } while (feat_list.next());
      

      for (i = 0; i < population; ++i ) {
        Target human = Human::create();
        to_place.add( human );
        }

      Map::get( levelloc,levelheight )->sprinkle( to_place );

      return true;
      }

    default:
      return Feature::perform(a,t);
      
    }

  }
*/

Town::Town( std::iostream &ios, TargetValue *TVP )
: Feature( ios, TVP ) {

	}

void Town::toFile( std::iostream &ios ) {

	// Write Feature base information:
	Feature::toFile( ios );

	}

targetType Town::gettype( void ) const {
	return tg_town;
	}

bool Town::istype( targetType t ) const {

	if ( t == tg_town )
		return true;
	else return Feature::istype(t);

	}

String Town::name( NameOption ) {

  if ( id_mask & id_name )
    return f_name.toString();
    
	return String("#town~");
  
	}

//-[ class Churchyard ]-----------
Churchyard::Churchyard( const Option *o )
: Feature(o) {

  //  f_name = Random::churchname(true);
	}

Churchyard::Churchyard( std::iostream &ios, TargetValue *TVP )
: Feature( ios, TVP ) {}

long Churchyard::getVal( StatType s ) {

  switch(s) {
  
    case attrib_ooo_ness:
      return 3;
      
    default:
      return Feature::getVal(s);
    }

  }

  
void Churchyard::build( const Option &o ) {
  o.level->addFeature( THIS );

/**
 *
 * Example Churchyard:
 *
 *
 * ::::::::::::::::::::
 * :..................: t - vicar or nun    ~ - candles
 * :.########.+...+.+.: - - pews            ++- double doors
 * :.#.~~~~.#.-...-.-.: _ - altar
 * :.#.._t..#...+.+.+.:
 * :.#--..--#...-.-.-.:   + - headstone
 * :.#--..--#.+.+...+.:   - - grave
 * :.#--..--#.-.-...-.:
 * :.#......#.+...+.+.: : - iron fence
 * :.###++###.-...-.-.: \\- iron gate
 * :..................:
 * :::::\\:::::::::::::
 *
 * Made up of:
 *  -Iron Fence Perimeter
 *  -Church
 *  -Graveyard
 *
 */
  int x, y, dx, dy,
      z = levelloc.getz();
  Rectangle active = border;
  Point p_11 = Point(1,1);
  
  active.geta() += p_11;
  active.getb() -= p_11;

  Rectangle church, graveyard;

  dx = DiceRoll(1,3,4).roll();
  dy = DiceRoll(1,2,3).roll();
  x = Random::randint(active.getwidth()-dx-2)+active.getxa()+1;
	y = Random::randint(active.getheight()-dy-2)+active.getya()+1;
	church.set( x, y, x+dx, y+dy );
  church.setintersect( active );

  dx = DiceRoll(1,8,6).roll();
  dy = DiceRoll(1,4,4).roll();
  x = Random::randint(active.getwidth()-dx-2)+active.getxa()+1;
	y = Random::randint(active.getheight()-dy-2)+active.getya()+1;
	graveyard.set( x, y, x+dx, y+dy );
  graveyard.setintersect( active );


  Rectangle yard(-2,-2,+2,+2);
  
  Graveyard::Option gravey_opt;
  gravey_opt.border = graveyard;
  gravey_opt.level = o.level;
  Graveyard::create( gravey_opt );

  House::Option church_opt;
  church_opt.border = church;
  church_opt.level = o.level;
  if (Random::randbool())
    church_opt.split = false;
  church_opt.max_shops = 8;
  church_opt.shop = shop_church;
  House::create( church_opt );

  Perimeter::Option per_opt;
  per_opt.level = o.level;
  per_opt.rect_list.add (church);
  graveyard += yard;
  per_opt.rect_list.add (graveyard);

  // All entrances should be fenced:
	for (y = border.getya(); y <= border.getyb(); ++y)
		for (x = border.getxa(); x <= border.getxb(); ++x)
      if ( o.level->getTile( Point3d(x,y,z) )->getVal(attrib_has_door) )
        per_opt.rect_list.add( Rectangle(x,y,x,y)+Rectangle(-1,-1,1,1) );
  
	Perimeter::create( per_opt );
	}

targetType Churchyard::gettype( void ) const {
	return tg_churchyard;
	}

bool Churchyard::istype( targetType t ) const {

	if ( t == tg_churchyard )
		return true;
	else return Feature::istype(t);

	}

String Churchyard::name( NameOption ) {

  if ( id_mask & id_name )
    return f_name.toString();
    
	return String("#cemetery~");
  
	}

//-[ class Graveyard ]-----------
Graveyard::Graveyard( const Option *o )
: Feature(o) {

  //  f_name = Random::churchname(true);
	}

Graveyard::Graveyard( std::iostream &ios, TargetValue *TVP )
: Feature( ios, TVP ) {}

long Graveyard::getVal( StatType s ) {

  switch(s) {
  
    case attrib_ooo_ness:
      return 2;
      
    default:
      return Feature::getVal(s);
    }

  }

  
void Graveyard::build( const Option &o ) {
  o.level->addFeature( THIS );

  int x, y,
      z = levelloc.getz();

  enum {
    yard_small,
    yard_medium,
    yard_large
    
    } size = ((border.getheight() < 4)||(border.getwidth() < 4))? yard_small
             :
             ((border.getheight() < 7)||(border.getwidth() < 7))? yard_medium
             : yard_large;

  bool has_grave;
  for ( y = border.getya(); y < border.getyb(); y += 2 )
    for ( x = border.getxa(); x <= border.getxb(); x += 2 ) {

      if (size == yard_small)
        has_grave = !Random::randint(2);
      else if (size == yard_medium)
        has_grave = !Random::randint(3);
      else
        has_grave = !Random::randint(4);
      
      if (has_grave) {
        o.level->getTile( Point3d(x,y,z)  )->perform( aContain, Headstone::create() );
        o.level->getTile( Point3d(x,y+1) )->perform( aContain, Grave::create() );
        }

      }

  for ( y = border.getya(); y <= border.getyb(); y ++ )
    for ( x = border.getxa(); x <= border.getxb(); x ++ )
      o.level->getTile(  Point3d(x,y,z)  )->perform(aSetFeature,THIS);

	}

targetType Graveyard::gettype( void ) const {
	return tg_graveyard;
	}

bool Graveyard::istype( targetType t ) const {

	if ( t == tg_graveyard )
		return true;
	else return Feature::istype(t);

	}

String Graveyard::name( NameOption ) {

  if ( id_mask & id_name )
    return f_name.toString();
    
	return String("#graveyard~");
  
	}

//-[ class Perimeter ]-----------
Perimeter::Perimeter( const Option *o )
: Feature(o) {

  //  f_name = Random::churchname(true);
  type = o->material;
  is_wall = o->is_wall;
	}

Perimeter::Perimeter( std::iostream &ios, TargetValue *TVP )
: Feature( ios, TVP ) {
	ios.read( (char*) &is_wall, sizeof(is_wall) );
	ios.read( (char*) &type, sizeof(type) );
	}

void Perimeter::toFile( std::iostream &ios ) {

	// Write Feature base information:
	Feature::toFile( ios );

	ios.write( (char*) &is_wall, sizeof(is_wall) );
	ios.write( (char*) &type, sizeof(type) );

	}


long Perimeter::getVal( StatType s ) {

  switch(s) {
  
    case attrib_ooo_ness:
      return 1;
      
    default:
      return Feature::getVal(s);
    }

  }

void Perimeter::build( const Option &o ) {
  o.level->addFeature( THIS );
/* Given a list of rectangles, builds a perimeter wall around them
 * and unsures all sections within the wall can be accessed through
 * perimeter doors.
 *
 *                           --.
 *  ##                      |## --+---.
 *  #=                      |#=       |
 *     #= #=#        ->     |   #= #=#|
 *     ## ###                ---##+###'
 *        ###                      ###
 *
 */

  if (o.rect_list.numData() < 1)
    return;
    
  // Build a char map
  int x, y;
  int z = levelloc.getz();
	char ** testgrid;
	testgrid = new char*[lev_width];

	for ( x = 0; x < lev_width; ++x )
		testgrid[x] = new char[lev_height];

	// Initialise grid:
	for ( y = 0; y < lev_height; ++y )
		for ( x = 0; x < lev_width; ++x )
			testgrid[x][y] = 0;

  // Fill in rectangular sections:
  o.rect_list.reset();
  border = o.rect_list.get();
  do {
    Rectangle rect = o.rect_list.get();
    border.setunion(rect);
    
  	for ( y = rect.getya(); y <= rect.getyb(); ++y )
    	for ( x = rect.getxa(); x <= rect.getxb(); ++x )
        testgrid[x%lev_width][y%lev_height] = 2;

    } while ( o.rect_list.next() );

  // Connect all rectangles:
  bool finished;
  do {
    finished = true;

    // Horizontal connection:
  	for ( y = 0; y < lev_height; ++y ) {

      int recent = -1,
          space  = -1;
          
      x = 0;
      do {

        if ( testgrid[x][y] ) {
          if (space >= 0) {
            finished = false;
            for (int i = recent+1; i < x; ++i)
              testgrid[i][y] = 2;
            }
          else
            recent = x;
          }
          
        else if (recent >= 0) {
          if (++space > o.largest_gap) {
            recent = -1;
            space = -1;
            }
          }


        } while (++x < lev_width );

      /*
      if (first > -1) {
        x = lev_width-1;
        do {
          if ( testgrid[x][y] )
            last = x;
          } while (--x >= 0 && last == -1);
        }


      if (first != -1 && last != -1)
        for (x = first+1; x < last; ++x)
          if (!testgrid[x][y]) {
            finished = false;
            testgrid[x][y] = 2;
            }
      */
      }

    // Vertical connection:
  	for ( x = 0; x < lev_width; ++x ) {

      int first = -1,
          last  = -1;
          
      y = 0;
      do {
        if ( testgrid[x][y] )
          first = y;
        } while (++y < lev_height && first == -1);
        
      if (first > -1) {
        y = lev_height-1;
        do {
          if ( testgrid[x][y] )
            last = y;
          } while (--y >= 0 && last == -1);
        }


      if (first != -1 && last != -1)
        for (y = first+1; y < last; ++y)
          if (!testgrid[x][y]) {
            finished = false;
            testgrid[x][y] = 2;
            }
      }

  
    } while (!finished);

  // Flag any square at the edge of a rectangle as perimeter
	for ( y = 0; y < lev_height; ++y )
 		for ( x = 0; x < lev_width; ++x )
      if (testgrid[x][y] == 2) {
        if (x > 0 && y > 0)
          if ( !testgrid[x-1][y-1] ) testgrid[x][y] = 1;//testgrid[x-1][y-1] = 1;
        if (y > 0)
          if ( !testgrid[ x ][y-1] ) testgrid[x][y] = 1;//testgrid[ x ][y-1] = 1;
        if (x < lev_width-1 && y > 0)
          if ( !testgrid[x+1][y-1] ) testgrid[x][y] = 1;//testgrid[x+1][y-1] = 1;
        if (x > 0)
          if ( !testgrid[x-1][ y ] ) testgrid[x][y] = 1;//testgrid[x-1][ y ] = 1;
        if (x < lev_width-1)
          if ( !testgrid[x+1][ y ] ) testgrid[x][y] = 1;//testgrid[x+1][ y ] = 1;
        if (x > 0 && y < lev_height-1)
          if ( !testgrid[x-1][y+1] ) testgrid[x][y] = 1;//testgrid[x-1][y+1] = 1;
        if (y < lev_height-1)
          if ( !testgrid[ x ][y+1] ) testgrid[x][y] = 1;//testgrid[ x ][y+1] = 1;
        if (x < lev_width-1 && y < lev_height-1)
          if ( !testgrid[x+1][y+1] ) testgrid[x][y] = 1;//testgrid[x+1][y+1] = 1;
        }

  // Remove all flagged squares if they surround nothing:
  //
  //        ::::                ::::
  //        :  :                :  :
  //  ::::::::::      ->  ::::  ::::
  //  :  :                :  :
  //  ::::                ::::
  //
	for ( y = 0; y < lev_height; ++y )
 		for ( x = 0; x < lev_width; ++x )
      if (testgrid[x][y] == 1) {
        if (
          (x == 0 || y == 0  || testgrid[x-1][y-1] != 2 )
        && (y == 0           || testgrid[ x ][y-1] != 2 )
        && (x == lev_width-1 || y == 0 || testgrid[x+1][y-1] != 2 )
        && (x == 0           || testgrid[x-1][ y ] != 2 )
        && (x == lev_width-1 || testgrid[x+1][ y ] != 2 )
        && (x == 0 || y == lev_height-1|| testgrid[x-1][y+1] != 2 )
        && (y == lev_height-1|| testgrid[ x ][y+1] != 2 )
        && (x == lev_width-1 || y == lev_height-1 || testgrid[x+1][y+1] != 2 )
         )

        testgrid[x][y] = 0;
        }
  

  // Build the wall
	for (y = border.getya(); y <= border.getyb(); ++y)
		for (x = border.getxa(); x <= border.getxb(); ++x)
      // Allow currently existing walls to be a part of the perimeter
      if (testgrid[x%lev_width][y%lev_height] == 1 && o.level->getTile(Point3d(x,y,z))->getVal(attrib_non_wall) ) {
        Tile::Option tile_opt;
        tile_opt.type = tt_wall;
        if (o.is_wall) {
          tile_opt.location = Point3d(x,y,z);
          tile_opt.floor = (materialClass)o.level->getTile(Point3d(x,y,z))->getVal(attrib_material);
          tile_opt.object = type;
          }
        else {
          tile_opt.location = Point3d(x,y,z);
          tile_opt.floor = (materialClass)o.level->getTile(Point3d(x,y,z))->getVal(attrib_material);
          tile_opt.object = type;
          }

      	tile_opt.feature = THIS;
        o.level->tileCreate( tile_opt );

        }

      else
        testgrid[x%lev_width][y%lev_height] = 0;

  // Make sure that each fence segment walls in only one
  // section, ie. not the following:
  //
  //    ::::::::      // Fixed by making sure a fence
  //    : #    :      // segment ends if any part has
  //    : #    +      // fewer than 2 orthogonally
  //    ::::::::      // adjacent empty spaces.
  border.setintersect(o.level->getBorder()+Rectangle(+1,+1,-1,-1) );
  
	for (y = border.getya(); y <= border.getyb(); ++y)
		for (x = border.getxa(); x <= border.getxb(); ++x) {
      int num_spaces =   o.level->getTile(Point3d(x+1,y))->getVal(attrib_non_wall)
                       + o.level->getTile(Point3d(x,y+1))->getVal(attrib_non_wall)
                       + o.level->getTile(Point3d(x-1,y))->getVal(attrib_non_wall)
                       + o.level->getTile(Point3d(x,y-1))->getVal(attrib_non_wall);

      if (num_spaces < 2) testgrid[x%lev_width][y%lev_height] = 0;
      }

  // Make sure each unbroken wall section contains a door
	Dungeon_Tester dt(testgrid,lev_width,lev_height);
	dt.find_regions();

	int regions = dt.get_num_regions(), i;

  Point3d lev_offset( o.level->getBorder().geta(), levelloc.getz() );
  
	for ( i = 0; i < regions; ++i ) {
    // Choose a point at random along the fence:
    bool corner = true;
    int attempts = 0;
    Point3d gate;
    do {
      gate = Point3d(dt[i]) + lev_offset;
      corner = (!o.level->getTile(gate+Point3d(1,0))->getVal(attrib_non_wall) || !o.level->getTile(gate+Point3d(-1,0))->getVal(attrib_non_wall))
            && (!o.level->getTile(gate+Point3d(0,1))->getVal(attrib_non_wall) || !o.level->getTile(gate+Point3d(0,-1))->getVal(attrib_non_wall));
      } while ( ++attempts < NUM_ABORT && corner );
    if (!corner) {
      Tile::Option tile_opt;
      tile_opt.location = gate;
      tile_opt.floor = m_dirt;
      tile_opt.type = tt_ground;
    	tile_opt.feature = THIS;
      o.level->tileCreate( tile_opt );
      Door::Option door_opt;
      if (!o.is_wall) {
        door_opt.gate = true;
        door_opt.material = m_iron;
        }
      o.level->getTile( gate )->perform( aContain, Door::create( door_opt ) );
      }
    //else
    //  Error::fatal("Error placing door in Perimeter code!");
    }


  // Dealloc test grid:
	for ( x = 0; x < lev_width; ++x )
		delarr( testgrid[x]);

	delarr( testgrid );
  }


targetType Perimeter::gettype( void ) const {
	return tg_perimeter;
	}

bool Perimeter::istype( targetType t ) const {

	if ( t == tg_perimeter )
		return true;
	else return Feature::istype(t);

	}

String Perimeter::name( NameOption ) {

  if ( id_mask & id_name )
    return f_name.toString();

  if (is_wall)
  	return String("#perimeter wall~");
  else
  	return String("#perimeter fence~");
  
	}

//-[ class Basement ]-----------
Basement::Basement( const Option *o )
: Feature(o) {
	}

#define HASH_ROOM_BORDER(b)   for (y = b.getya()-1; y <= b.getyb()+1; ++y)\
    for (x = b.getxa()-1; x <= b.getxb()+1; ++x)\
      o.level->getTile( Point3d(x,y,z) )->setimage( Image(o.level->getTile( Point3d(x,y,z) )->getimage().val.color,'#') );
      
void Basement::build( const Option &o ) {

  o.level->addFeature( THIS );

  // Basements are at least stale-smelling:
  o.level->incOdor( od_stale );

  //*
  // Create a basement special room:
  SpecialRoom::Option sp_opt;
  sp_opt.name = "basement";
  sp_opt.border = o.level->getBorder();
  sp_opt.level = o.level;
  SpecialRoom::create(sp_opt);
  //*/

  // We don't want this here.  We want lev 1 areas to be
  // defined on map creation, then again for each level
  // to be defined when the previous level has been reached.
  //
  // // And set up all the surrounding levels:
  // //Map::get( o.level->loc() + Point3d(+1,0,0) ).addRequirement( Req(Req::sewer,border) );
  // //Map::get( o.level->loc() + Point3d(0,+1,0) ).addRequirement( Req(Req::sewer,border) );
  // //Map::get( o.level->loc() + Point3d(-1,0,0) ).addRequirement( Req(Req::sewer,border) );
  // //Map::get( o.level->loc() + Point3d(0,-1,0) ).addRequirement( Req(Req::sewer,border) );


/*//-----------------------
  // Old basement code:



  int x, y, i = 0,
      z = levelloc.getz();

	int rooms = DiceRoll( 2,4,+4).roll();
//  Rectangle borderRect = o.level->getBorder();
	Rectangle roomLimit( border.getxa()+1,border.getya()+1,border.getxb()-1,border.getyb()-1);
  Field::Option field_opt;
	Target room, lastRoom;

	x = Random::randint(lev_width-6)+3+border.getxa();
	y = Random::randint(lev_height-6)+3+border.getya();
	field_opt.border.set( x-Random::randint(3+(rooms-i)/4)-1,y-Random::randint(2+(rooms-i)/5)-1,
									x+Random::randint(3+(rooms-i)/4)+1,y+Random::randint(2+(rooms-i)/5)+1);
	field_opt.border.setintersect( roomLimit );
  field_opt.level = o.level;
  field_opt.material = m_stone;

  // Todo: Make sure room borders are '#':
  //HASH_ROOM_BORDER(field_opt.border);

//	room = Field::create( field_opt );
  SpecialRoom::Option sp_opt;
  sp_opt.name = "sewer entrance";
  sp_opt.border = roomLimit;
  sp_opt.level = o.level;
  room = SpecialRoom::create(sp_opt);

  // Place a staircase down in the first room:
//  x = Random::randint( field_opt.border.getwidth() -2 ) + field_opt.border.getxa() + 1;
//	y = Random::randint( field_opt.border.getheight()-2 ) + field_opt.border.getya() + 1;

//  Stairs::Option stairs_opt;
//  stairs_opt.connecting = o.level->height()-1;
//	o.level->getTile( Point3d(x,y,z) )->perform( aContain, Stairs::create( stairs_opt ) );
	// Make sure lower level has a room/staircase at this position
//	Map::get( o.level->loc(), o.level->height()-1 ).addRequirement( Req(Req::stairs_up,Point(x,y)) );

	for (i = 1; i < rooms; ++i) {
		bool done = true;
		lastRoom = room;
		int attempts = 0;
		do {
			done = true;
			x = Random::randint(lev_width-6)+3+border.getxa();
			y = Random::randint(lev_height-6)+3+border.getya();
			field_opt.border.set( x-Random::randint(3+(rooms-i)/4)-1,y-Random::randint(2+(rooms-i)/5)-1,
											x+Random::randint(3+(rooms-i)/4)+1,y+Random::randint(2+(rooms-i)/5)+1);
			field_opt.border.setintersect( roomLimit );

			for (y = field_opt.border.getya()-1; y <= field_opt.border.getyb()+1; ++y)
				for (x = field_opt.border.getxa()-1; x <= field_opt.border.getxb()+1; ++x)
					if ( o.level->getTile( Point3d(x,y,z))->istype( tg_floor ) ) done = false;

			attempts ++;

			} while (
							(
							 ( field_opt.border.getwidth() < 3 )
								||
							 ( field_opt.border.getheight() < 3 )
								||
							 !done
							)
							&&
							(  attempts < NUM_ABORT )
					);

		if (attempts < NUM_ABORT) {
      //HASH_ROOM_BORDER(field_opt.border);
			room = Field::create( field_opt );
			}
		}

	// Check for inaccessible parts:
	char ** testgrid;
	testgrid = new char*[lev_width];

	for ( x = 0; x < lev_width; ++x )
		testgrid[x] = new char[lev_height];

	// Initialise test grid:
	for (int y = border.getya(); y <= border.getyb(); ++y)
		for (int x = border.getxa(); x <= border.getxb(); ++x) {
			testgrid[x%lev_width][y%lev_height] = ( o.level->getTile( Point3d(x,y,z))->getVal(attrib_non_wall) );
//      Screen::put(x+1,y+3,Image(cGrey,'0'+testgrid[x][y]));
      }

	Dungeon_Tester dt(testgrid,lev_width,lev_height);
	dt.find_regions();
  Tile::Option tile_opt;
  tile_opt.floor = m_stone;
  tile_opt.type = tt_ground;
  Point origin = border.geta();

  while (dt.get_num_regions() > 1) {
    List<Point> route = dt.dig_from(0);

    if (route.reset())
    do {

			if ( ! o.level->getTile( Point3d( route.get()+origin, levelloc.getz() ) )->istype( tg_floor ) ) {

        tile_opt.location = Point3d( route.get()+origin, levelloc.getz() );
  			tile_opt.feature = THIS;
        o.level->tileCreate( tile_opt );

        }

      } while ( route.next() );

    else Message::add("Error digging path in Basement::build()");
      
    }
    
//	int regions = dt.get_num_regions();

//  Corridor::Option cor_opt;
//  cor_opt.material = m_stone;
//  cor_opt.level = o.level;
  
//	for ( i = 0; i < regions-1; ++i ) {
//    cor_opt.start = dt[i];
//    cor_opt.finish = dt[i+1];
//		Corridor::create( cor_opt );
//    }



	// Dealloc test grid:
	for ( x = 0; x < lev_width; ++x )
		delarr( testgrid[x]);

	delarr( testgrid );

  // Place a few items:
  Target item;
  int numitems = DiceRoll(3,4).roll();
  for ( i = 0; i < numitems; ++i ) {

    switch(Random::randint(12)) {
      case 0: item = Clothing::create(); break;
      case 1: item = Light::create(); break;
      case 2: item = Ranged::create(); break;
      case 3: {
              Quantity::Option q_opt;
              q_opt.quantity = DiceRoll(2,12).roll();
              q_opt.target = Ammo::create();
              item = Quantity::create( q_opt ); break;
              }
      case 4: item = Food::create(); break;
      case 5: item = Bottle::create(); break;
      case 6: item = Bag::create(); break;
      case 7: item = Fluid::create(); break;
      case 8: item = Scope::create(); break;
      case 9: item = Book::create(); break;
    
      default:
        Melee::Option mel_opt;
        mel_opt.theme = th_toolshed;
        item = Melee::create(mel_opt); break;
      }
    Point3d place_loc;
   	do {
      place_loc = Point3d( Random::randpoint( border ), levelloc.getz() ) ;
   		} while ( ! o.level->getTile(place_loc)->capable(aContain, item ) );
    o.level->getTile(place_loc)->perform(aContain, item );
    }

	o.level->placeDoors();

  // Sewer network:
  Map::get( o.level->loc() - Point3d(0,0,1) ).addRequirement( Req(Req::sewer,border) );
//*/
	}

Basement::Basement( std::iostream &ios, TargetValue *TVP )
: Feature( ios, TVP ) {}

void Basement::toFile( std::iostream &ios ) {

	// Write Feature base information:
	Feature::toFile( ios );

	}

targetType Basement::gettype( void ) const {
	return tg_basement;
	}

bool Basement::istype( targetType t ) const {

	if ( t == tg_basement )
		return true;
	else return Feature::istype(t);

	}

String Basement::name( NameOption ) {
	return String("#basement~");
	}

long Basement::getVal( StatType s ) {

  switch(s) {
  
    case attrib_ooo_ness:
      return 3;
    default:
      return Feature::getVal(s);
    }
  }


//-[ class Road ]-----------
Road::Road( const Option *o )
: Feature(o), exits(o->orthog), type(o->material) {
	}

void Road::build( const Option &o ) {
  o.level->addFeature( THIS );

  Point n = Point(border.getxa()+lev_width/2, border.getya()),
        s = Point(border.getxa()+lev_width/2, border.getyb()),
        e = Point(border.getxb(),border.getya()+lev_height/2),
        w = Point(border.getxa(),border.getya()+lev_height/2);

  Point offset = border.geta();
  
  Point mid = offset + Point(lev_width/2,lev_height/2);
  if (exits.n) mid += n;
  if (exits.s) mid += s;
  if (exits.e) mid += e;
  if (exits.w) mid += w;

  mid.setx( mid.getx() / (exits.total()+1) );
  mid.sety( mid.gety() / (exits.total()+1) );

  if (exits.n) Trace( n-offset, mid-offset, *o.level);
  if (exits.s) Trace( s-offset, mid-offset, *o.level);
  if (exits.e) Trace( e-offset, mid-offset, *o.level);
  if (exits.w) Trace( w-offset, mid-offset, *o.level);


	}

#define TRACE_N 3
#define TRACE_S 2
#define TRACE_W 5
#define TRACE_E 4
void Road::Trace( Point start, Point fin, Level &lev ) {

  if (start.getx() < TRACE_W) start.setx(TRACE_W);
  if (start.getx() > lev_width-TRACE_E-1) start.setx(lev_width-TRACE_E-1);
  if (start.gety() < TRACE_N) start.sety(TRACE_N);
  if (start.gety() > lev_height-TRACE_S-1) start.sety(lev_height-TRACE_S-1);

  if (fin.getx() < TRACE_W) fin.setx(TRACE_W);
  if (fin.getx() > lev_width-TRACE_E-1) fin.setx(lev_width-TRACE_E-1);
  if (fin.gety() < TRACE_N) fin.sety(TRACE_N);
  if (fin.gety() > lev_height-TRACE_S-1) fin.sety(lev_height-TRACE_S-1);

  start += border.geta();
  fin += border.geta();

  Path tracePath;
  tracePath.setStart( Point3d(start) );
  tracePath.setFinish( Point3d(fin) );
	tracePath.calculate();

  int x, y;
  int z = levelloc.getz();

  Tile::Option tile_opt;
  tile_opt.floor = type;
  tile_opt.type = tt_ground;
  if ( tracePath.reset() )
	do {

    Point curr = tracePath.get().toPoint();

    for (y = curr.gety()-TRACE_N; y <= curr.gety()+TRACE_S; ++y )
      for (x = curr.getx()-TRACE_W; x <= curr.getx()+TRACE_E; ++x ) {
//        if ( abs(y-curr.gety())+abs(x-curr.getx()) <  ) {
          tile_opt.location = Point3d(x,y,z);
  				tile_opt.feature = THIS;
          lev.tileCreate( tile_opt );
          }
      

		} while ( tracePath.next() );

  for (y = start.gety()-TRACE_N; y <= start.gety()+TRACE_S; ++y )
    for (x = start.getx()-TRACE_W; x <= start.getx()+TRACE_E; ++x ) {
      tile_opt.location = Point3d(x,y,z);
			tile_opt.feature = THIS;
      lev.tileCreate( tile_opt );
      }

  for (y = fin.gety()-TRACE_N; y <= fin.gety()+TRACE_S; ++y )
    for (x = fin.getx()-TRACE_W; x <= fin.getx()+TRACE_E; ++x ) {
      tile_opt.location = Point3d(x,y,z);
			tile_opt.feature = THIS;
      lev.tileCreate( tile_opt );
      }

  }

Road::Road( std::iostream &ios, TargetValue *TVP )
: Feature( ios, TVP ) {

	// Read Orthog exits
	ios.read( (char*) &exits, sizeof(exits) );

	// Read char type
	ios.read( (char*) &type, sizeof(type) );
	}

void Road::toFile( std::iostream &ios ) {

	// Write Feature base information:
	Feature::toFile( ios );

	// Write Orthog exits
	ios.write( (char*) &exits, sizeof(exits) );

	// Write char type
	ios.write( (char*) &type, sizeof(type) );

	}


targetType Road::gettype( void ) const {
	return tg_road;
	}

bool Road::istype( targetType t ) const {

	if ( t == tg_road )
		return true;
	else return Feature::istype(t);

	}

String Road::name( NameOption ) {

  if (type == m_water)
    return String("#river~");
    
	return String("#") + Material::data[type].name	+ " road~";
	}

long Road::getVal( StatType s ) {

  switch(s) {
  
    case attrib_ooo_ness:
      return 2;
    default:
      return Feature::getVal(s);
    }
  }


//-[ class SpecialRoom ]--
//#define AT_FEATURES

#ifdef AT_FEATURES
#  include "input.hpp"
#  include "visuals.hpp"
#endif // AT_FEATURES

/* Special Room Default Legends:
 *
 * # - Stone Wall           Space - Always means Unaffected tile
 * + - Wooden Door
 * . - Stone Floor
 * ~ - Floor with Candle
 * U,D - Up/Down stairs
 *
 */
/*
char *test_file [] = {

// Reserved Symbols:
// All of: '=', ':', '+', '<', '>', '%', and whitespace cannot be redefined.

//    "Map: #######      #######",
//    "Map: #~....#      #....~#",
//    "Map: #.....###++###.....#",
//    "Map: #.......~..~.......#",
//    "Map: #..^^....&.........#",
//    "Map: #.......~..~../....#",
//    "Map: #..................#",
//    "Map:##.#.#.#.~..~.#.#.#.##",
//    "Map:#...................U#",
//    "Map:##..&.............&.##",
//    "Map: ####...~####~...####",
//    "Map:    ######  ######",
//    "Legend:/=FLOOR+MELEE(a machete)+ITEM",
//    "Legend:&=MONSTER",
//    "Legend:^=TRAP",

    "Name:caved-in stairwell",
    "Map:#########",
    "Map:#......U#",
    "Map:#.....###",
    "Map:#....##",
    "Map:+....#",
    "Map:#....#",
    "Map:#...##",
    "Map:#..##",
    "Map:####",
    
    "Name:portal chamber",
    "Map:#+#",
    "Map:+.+",
    "Map:#+#",

    NULL
    };*/

int SpecialRoom::Data::width( void ) {

  int ret_val = 0, get_val;
  if (map.reset())
  do {

    get_val = map.get().length();
    if (get_val > ret_val)
      ret_val = get_val;
      
    } while (map.next());

  return ret_val;
  }
  
int SpecialRoom::Data::height( void ) {

  return map.numData();
  }


List<SpecialRoom::Data> SpecialRoom::data_list;

struct StringToType {
  const char *string;
  targetType type;
  } string_to_type [] = {


  { "MONSTER",  tg_monster },
  { "HUMAN",    tg_human },

  { "ITEM",     tg_item },


  { "TRAP",     tg_trap },

  { "MACHINE",  tg_machine_part },

  { "MELEE",    tg_melee },
  { "RANGED",   tg_ranged },

  { "AMMO",     tg_ammo },
  { "CLOTHING", tg_clothing },

  { "FOOD",     tg_food },

  { "BOOK",     tg_book },

//  { "LIGHT",    tg_light },
  { "SCOPE",    tg_scope },

  { "BOTTLE",   tg_bottle },
  { "QUIVER",   tg_quiver },
  { "BAG",      tg_bag },

  { "MONEY",    tg_money },

  { "FLUID",    tg_fluid },

//  { "SEAT",     tg_seat },
//  { "ALTAR",    tg_altar },
  { "HEADSTONE",tg_headstone },
  { "GRAVE",    tg_grave },
    

  { "DOOR",     tg_door },
  { "FLORA",    tg_flora },
  { "WALL",     tg_wall },
  { "FENCE",    tg_fence },

  { "FLOOR",    tg_floor },


  { NULL, tg_invalid }

  };


/*
struct StringToReq {
  char *string;
  Req::Type type;
  } string_to_req [] = {

  { "SEWER",      Req::sewer },
  { "BASEMENT",   Req::basement },
  { "MINE",       Req::mech_mine },

  { NULL, Req::invalid }

  };
*/

void SpecialRoom::loadData( void ) {

  Data temp;
  bool first = true;

	std::fstream myFile;

  String file_name = INFODIR + "feature.txt";
	myFile.open( file_name.toChar(), std::ios::in & ~std::ios::binary );
  
	assert (!! myFile); // Error::fatal(  "Feature load error: Error opening file " + file_name);

  for ( int i = 0; !myFile.eof()/*test_file[i] != NULL*/; ++i ) {

    char buffer[80];
    myFile.getline( buffer, 80 );
    
    /*char *line = test_file[i];*/
    char *line = buffer;

    // Skip lines beginning with hash:
    if (line[0] == '#') continue;
    
    char *colon_pos = strchr( line, ':' );
    
    int typestr_len = colon_pos-line;
    String typestr = line;
    typestr.trunc(typestr_len);

    if (colon_pos)
      line = colon_pos+1;

    typestr = typestr.upcase();
    
    if ( typestr == "NAME" ) {
      if (!first) {
        data_list.add(temp);
        temp.name = "";
        temp.map.clr();
        }
      
      first = false;
      temp.name = line;
      }
    else if ( typestr == "MAP" ) {
      temp.map.add( line );
      }
    else if ( typestr == "LEGEND" ) {

      char *equals_pos = strchr( line, '=' );
      
      String symbol_str = line;
      symbol_str.trunc(equals_pos-line);

      if (equals_pos)
        line = equals_pos + 1;

      List<String> values;

      char *plus_pos = NULL;

      do {
        plus_pos = strchr( line, '+' );

        if (plus_pos) {

          String str = line;
          str.trunc(plus_pos-line);
          
          values.add(str);
        
          line = plus_pos + 1;
          }
          
        } while (plus_pos);

      if (*line)
        values.add(line);


      // The first character in the symbol string is used:
      Data::Legend legend;
      legend.symbol = 0;
      int index = 0;
      do {
        legend.symbol = symbol_str[index];
        } while ( isspace(legend.symbol) && ++index < symbol_str.length() );

      assert( ! isspace(legend.symbol) ); // Error::fatal("Error with legend.");

      if (values.reset())
      do {

        legend.type = tg_invalid;
        legend.param = "";

        String obj_str = values.get();

        const char *bracket_pos = strchr( obj_str.toChar(), '(' );

        if (bracket_pos) {

          int trunc_len = bracket_pos-obj_str.toChar();

          // replaced with +/-1 below, so we can have a const char*
          // - Sz. Rutkowski 11.02.2017
          //++bracket_pos;

          const char *close_bracket_pos = strchr( bracket_pos+1, ')' );

          String force_str = bracket_pos;
          
          if (close_bracket_pos)
            force_str.trunc( close_bracket_pos-bracket_pos-1 );

          legend.param = (force_str);
          
          obj_str.trunc(trunc_len);
          }

        index = 0;
        legend.type = tg_invalid;
        do {
        
          if (obj_str.upcase() == string_to_type[index].string)
            legend.type = string_to_type[index].type;
            
          ++index;
          }
          while ( string_to_type[index].string && legend.type == tg_invalid );

        temp.legend.add( legend );
        
#       ifdef AT_FEATURES
        Screen::write(String(legend.symbol)+"=\""+obj_str+"\" ("+(long)legend.type+") :\""+legend.param+"\"\n\r");
#       endif // AT_FEATURES

        } while ( values.next() );
      
        
//      Screen::write(String(temp.legend.get().symbol)+"\n\r");
//      if (temp.legend.reset())
//      do {
//
//        Screen::write("\""+values.get()+"\" ("+(long)temp.legend.get().type+") :\""+temp.legend.get().param+"\"\n\r");
//        }
//        while ( temp.legend.next() );
        
#     ifdef AT_FEATURES
      Keyboard::get();
#     endif // AT_FEATURES
    
      }
      
    }

  if (!first)
    data_list.add(temp);

# ifdef AT_FEATURES
  Screen::write( String("Number of special rooms: ") + (long)data_list.numData() + "\n\r" );
  
  if (data_list.reset())
  do {
    Screen::write( String("--[ ") + data_list.get().name + " ]--\n\r" );

    data_list.get().map.reset();
    do {
      Screen::write( data_list.get().map.get() + "\n\r" );
      } while (data_list.get().map.next());
    
    Screen::write( String("Width=") + (long)data_list.get().width() + " Height=" + (long)data_list.get().height() + "\n\r" );
    Screen::update();

    Keyboard::get();
    Screen::cls();
    } while (data_list.next());
# endif // AT_FEATURES

  }
 
void SpecialRoom::clearData( void ) {
  data_list.clr();
  }

SpecialRoom::SpecialRoom( const Option *o )
: Feature(o) {
	}

void SpecialRoom::build( const Option &o ) {
  int x, y,
     start_x, start_y,
     dx,dy,
     z = levelloc.getz();

  // Find the right room:

  bool found = false;
  if (data_list.reset())
  do {

    if (data_list.get().name.upcase() == o.name.upcase())
      found = true;

    } while (!found && data_list.next());

  // If we can't find the room, return;
  if (!found)
    return;

  // Now add the room to the level:
  o.level->addFeature( THIS );

  // Name the room:
  f_name = data_list.get().name;
  
  // Fit the room into the level somewhere:
  dx = data_list.get().width();
  dy = data_list.get().height();

  start_x = border.getxa() + Random::randint( border.getwidth()-dx );
  start_y = border.getya() + Random::randint( border.getheight()-dy );

  // Place the room:

  Target new_targ;
  //List<Target> lights;
  y = start_y;
  if (data_list.get().map.reset())

  do {

    String line = data_list.get().map.get();
    
    for ( x = 0; x < line.length(); ++x ) {
      new_targ = o.level->getTile( Point3d(x+start_x,y,z) );

      bool use_legend = false;
      // Set up floor to a default state:
      Tile::Option tile_opt;
      tile_opt.location = Point3d(x+start_x,y, levelloc.getz());
      tile_opt.floor = m_stone;
      tile_opt.type = tt_floor;
      tile_opt.feature = THIS;
      
      List<Target> items;
      
      if (data_list.get().legend.reset()) {

        
        do {

          if (data_list.get().legend.get().symbol == line[x])
          switch ( data_list.get().legend.get().type ) {

            case tg_floor: {
              use_legend = true;
              tile_opt.floor = m_stone;
              tile_opt.type = tt_floor;
              break;
              }
            case tg_flora: {
              use_legend = true;
              tile_opt.type = tt_bush;
              tile_opt.floor = m_dirt;
              tile_opt.object = m_wood;
              break;
              }
            case tg_wall: {
              use_legend = true;
              tile_opt.type = tt_wall;
              tile_opt.floor = m_stone;
              tile_opt.object = m_stone;
              break;
              }
            case tg_fence: {
              use_legend = true;
              tile_opt.type = tt_fence;
              tile_opt.floor = m_stone;
              tile_opt.object = m_iron;
              break;
              }

#    define ITEMCASE(tg_type,ClassName)\
            case tg_type:\
              use_legend = true;\
              if (data_list.get().legend.get().param != "")\
                items.add( ClassName::create()->Force(data_list.get().legend.get().param) );\
              else\
                items.add( ClassName::create() );\
              break;

            ITEMCASE(tg_melee,Melee);
            ITEMCASE(tg_monster,Monster);
            ITEMCASE(tg_trap,Trap);
            ITEMCASE(tg_human,Human);
            ITEMCASE(tg_machine_part,MachinePart);
            ITEMCASE(tg_ranged,Ranged);
            ITEMCASE(tg_ammo,Ammo);
            ITEMCASE(tg_clothing,Clothing);
            ITEMCASE(tg_food,Food);
            ITEMCASE(tg_book,Book);
            //ITEMCASE(tg_light,Light);
            ITEMCASE(tg_scope,Scope);
            ITEMCASE(tg_bottle,Bottle);
            ITEMCASE(tg_quiver,Quiver);
            ITEMCASE(tg_bag,Bag);
            ITEMCASE(tg_money,Money);
            ITEMCASE(tg_fluid,Fluid);
            ITEMCASE(tg_headstone,Headstone);
            ITEMCASE(tg_grave,Grave);
            ITEMCASE(tg_door,Door);

            case tg_item:
              use_legend = true;
              items.add( Random::shopitem(shop_random) );
              break;

            default:
              break;
          
            }
          
          }
          while (data_list.get().legend.next());

        }

      if (use_legend) {
   			o.level->tileCreate( tile_opt );

        Target floor = o.level->getTile( Point3d(x+start_x,y,z) );

        if (items.reset())
        do {

          floor->perform(aContain,items.get());
          }
          while ( items.next() );
        }
      
      else switch ( (char)line[x] ) {

        case '#': { // Stone Wall

          tile_opt.type = tt_wall;
          tile_opt.floor = tile_opt.object = m_stone;
          o.level->tileCreate( tile_opt );
          break;
          }

        case '~': // Floor with Candle
        case '+': // Wooden Door
        case 'U': // Stairs Up
        case 'D': // Stairs Up
        case '.': { // Stone Floor

          tile_opt.floor = m_stone;
          tile_opt.type = tt_floor;
          o.level->tileCreate( tile_opt );

          if (line[x] == '+') {
            Door::Option door_opt;
            door_opt.hidden = false;
        	  door_opt.material = m_wood;
            assert(!!new_targ);
    			  new_targ->perform( aContain, Door::create(door_opt) );
            }
          /*
          else if (line[x] == '~') {
            Light::Option light_opt;
            light_opt.radius = 2;
        	  //light_opt.material = m_wood;
            Target light = Light::create(light_opt);
            lights.add(light);
            assert(!!new_targ);
    			  new_targ->perform( aContain, light );
            }
          */
          else if (line[x] == 'U') {
            Stairs::Option stairs_opt;
            stairs_opt.connecting = z+1;
            assert(!!new_targ);
    			  new_targ->perform( aContain, Stairs::create( stairs_opt ) );
        		// Make sure upper level has a room/staircase at this position
        		Map::get( o.level->loc() + Point3d(0,0,1) ).addRequirement( Req(Req::stairs_down,Point(x+start_x,y)) );
            }
          else if (line[x] == 'D') {
            Stairs::Option stairs_opt;
            stairs_opt.connecting = z-1;
            assert(!!new_targ);
    			  new_targ->perform( aContain, Stairs::create( stairs_opt ) );
        		// Make sure lower level has a room/staircase at this position
        		Map::get( o.level->loc() - Point3d(0,0,1) ).addRequirement( Req(Req::stairs_up,Point(x+start_x,y)) );
            }
            
          break;
          }

        default:break;
      
        }
    
      }

    ++y;
    } while (data_list.get().map.next());

  /*if (lights.reset())
  do {

    lights.get()->doPerform(aBeApplied);

  
    } while (lights.next());
    */

//  o.level->getTile( Point3d(x,y,z) )->perform( aContain,  );
	}

SpecialRoom::SpecialRoom( std::iostream &ios, TargetValue *TVP )
: Feature( ios, TVP ) {
	// Read enum shop
	//ios.read( (char*) &shop, sizeof(shop) );
  }

void SpecialRoom::toFile( std::iostream &ios ) {
	// Write Feature base information:
	Feature::toFile( ios );
	// Write enum shop
	//ios.write( (char*) &shop, sizeof(shop) );
	}


targetType SpecialRoom::gettype( void ) const {
	return tg_special_room;
	}

bool SpecialRoom::istype( targetType t ) const {

	if ( t == tg_special_room )
		return true;
	else return Feature::istype(t);

	}

String SpecialRoom::name( NameOption ) {

  if (f_name.generated())
    return "#" + f_name.toString() + "~";
    
 	return String("#special room~");
	}

long SpecialRoom::getVal( StatType s ) {

  switch(s) {

    case attrib_sort_category:
      // SpecialRooms are generated first
      return 0;
    
    case attrib_ooo_ness:
      return 2;

    default:
      return Feature::getVal(s);
    }
  }

bool SpecialRoom::perform( Action a, const Target &t ) {

  switch (a) {

    case aPlaceMons: {
      return false;
      }

    default:
      return Feature::perform(a,t);
      
    }

  }

//-[ class FirstLevel ]-----------
FirstLevel::FirstLevel( const Option *o )
: Feature(o) {
	}

void FirstLevel::build( const Option &o ) {
  o.level->addFeature( THIS );

  SpecialRoom::Option sp_opt;
  sp_opt.name = "Mysterious Chamber";
  sp_opt.border = o.border;
  sp_opt.level = o.level;
  SpecialRoom::create(sp_opt);

	}

FirstLevel::FirstLevel( std::iostream &ios, TargetValue *TVP )
: Feature( ios, TVP ) {

	}

void FirstLevel::toFile( std::iostream &ios ) {

	// Write Feature base information:
	Feature::toFile( ios );

	}

targetType FirstLevel::gettype( void ) const {
	return tg_first_level;
	}

bool FirstLevel::istype( targetType t ) const {

	if ( t == tg_first_level )
		return true;
	else return Feature::istype(t);

	}

String FirstLevel::name( NameOption ) {
	return String("#foul shrine~");
	}

long FirstLevel::getVal( StatType s ) {

  switch(s) {
  
    case attrib_ooo_ness:
      return 3;
    default:
      return Feature::getVal(s);
    }
  }


//--[ class MechMines ]--//
struct MechMines::MineInfo MechMines::mine_info [map_width][map_height];
int MechMines::mine_depth = 10;
int MechMines::levels_created = 0;
Point3d MechMines::entrance = OFF_MAP;
Target MechMines::tTHIS;
List<Point3d> MechMines::connections;

MechMines::MechMines( const Option *o )
: Feature(o) {
  border = Rectangle( 0,0,map_width*lev_width-1,map_height*lev_height-1 );
	}

void MechMines::buildLev( Level &level ) {
  level.addFeature( tTHIS );
  
  Point3d location = level.loc();
  int z = location.getz();
  int x, y;
  Rectangle border = level.getBorder();

  // The Mech Mines are at least stale-smelling:
  if (location.getz() < 0)
    level.incOdor( od_stale );

  // If it's the entrance level:
  if (location == entrance) {
  
    // Create a MechMines special entrance room:
    SpecialRoom::Option sp_opt;
    sp_opt.name = "Mine Entrance";
    sp_opt.border = border+Rectangle(5,3,-5,-3);
    sp_opt.level = &level;
    SpecialRoom::create(sp_opt);
    // Add all lower stairs tiles to the connections list:
    for (y = sp_opt.border.getya(); y < sp_opt.border.getyb(); ++y )
      for (x = sp_opt.border.getxa(); x < sp_opt.border.getxb(); ++x )
        if (level.getTile( Point3d(x,y,0) )->getVal(attrib_has_stairs) )
          connections.add( Point3d(x,y,-1) );

    return;
    }

  // --- Otherwise, build the mine:

  List<Point3d> to_connect;

  // Get the mine info for this level:
  MineInfo lev_info = mine_info[location.getx()][location.gety()];
      /*
      char num_corridors:5,
           has_workshop:1,
           has_repairshop:1,
           has_leeching_pit:1;
      */
  
  // Check the connections list for any points within this level:
  bool next;
  if (connections.reset())
  do {

    next = false;
    // Place connection, and add to our to_connect list
    if (border.contains( connections.get() )) {

      to_connect.add(connections.remove());
      next = connections.canGet();
      }
    
    } while ( next || connections.next() );


  // Place our num_corridors:
  int i, num_corridors = lev_info.num_corridors;
  // ensure that there are enough corridors to connect all points:
  if (num_corridors * 2 < to_connect.numData() )
    num_corridors = to_connect.numData();
  

  Tile::Option tile_opt;
  tile_opt.floor = m_stone;
  tile_opt.type = tt_ground;
	tile_opt.feature = tTHIS;

  for (i = 0; i < num_corridors; ++i) {

    Point tile;
    // If there are any connections left, use one:
    if (to_connect.reset())
      tile = to_connect.remove().toPoint();

    else
      // or just get a random edge point:
      tile = Random::randpoint(border);


    // Make an empty space at each point:
    tile_opt.location = Point3d(tile,location.getz());
    level.tileCreate( tile_opt );

    }


  // Add rooms if required:
  
  bool workshop = lev_info.has_workshop,
       repairshop = lev_info.has_repairshop,
       leeching_pit = lev_info.has_leeching_pit;

  // workshop:
  if (workshop) {
    // Create a MechMines special entrance room:
    SpecialRoom::Option sp_opt;
    sp_opt.name = "workshop";
    sp_opt.border = border;
    sp_opt.level = &level;
    SpecialRoom::create(sp_opt);
    }

  // repairshop:
  if (repairshop) {
    // Create a MechMines special entrance room:
    SpecialRoom::Option sp_opt;
    sp_opt.name = "repairshop";
    sp_opt.border = border;
    sp_opt.level = &level;
    SpecialRoom::create(sp_opt);
    }
    
  // leeching pit:
  if (leeching_pit) {
    // Create a MechMines special entrance room:
    SpecialRoom::Option sp_opt;
    sp_opt.name = "leeching pit";
    sp_opt.border = border;
    sp_opt.level = &level;
    SpecialRoom::create(sp_opt);
    }

  // Now check the borders and add points to the list
  // if any corridors hit the level edges:

  bool north_new = !Map::get(location+Point3d(0,-1,z)).isCreated(),
       south_new = !Map::get(location+Point3d(0,+1,z)).isCreated(),
        west_new = !Map::get(location+Point3d(-1,0,z)).isCreated(),
        east_new = !Map::get(location+Point3d(+1,0,z)).isCreated();

  // North & south:
  for (x = border.getxa(); x < border.getxb(); ++x) {
    y = border.getya();
    // if tile is non wall
    if (level.getTile( Point3d(x,y,z) )->getVal(attrib_non_wall)
      // and level hasn't been created
      && north_new )
      // then add connecting tile to list:
      connections.add(Point3d(x,y-1,z));

    y = border.getyb();
    if (level.getTile( Point3d(x,y,z) )->getVal(attrib_non_wall)
      && south_new )
      connections.add(Point3d(x,y+1,z));
    }

  // East & west:
  for (y = border.getya(); y < border.getyb(); ++y) {

    x = border.getxa();
    if (level.getTile( Point3d(x,y,z) )->getVal(attrib_non_wall)
      && west_new )
      connections.add(Point3d(x-1,y,z));

    x = border.getxb();
    if (level.getTile( Point3d(x,y,z) )->getVal(attrib_non_wall)
      && east_new )
      connections.add(Point3d(x+1,y,z));
    }

  // Now check that all corridors are connected:
  ((Feature*)tTHIS.raw())->connectAll(&level);

  // Place some doors:
	level.placeDoors();
  
  // Place some stuff:
  List<Target> sp_list;
  
  // Place some items:
  
  
  // Place some monsters:
  Monster::Option mon_opt;
  mon_opt.theme = th_mines;
  
  for (i = 0; i < 5; ++i)
    sp_list.add( Monster::create(mon_opt) );

  level.sprinkle(sp_list);
  
  }
      
void MechMines::build( const Option & ) {

  int x, y;

  for (y = 0; y < map_height; ++y )
    for (x = 0; x < map_width; ++x ) {
      mine_info[x][y].num_corridors = 0;
      mine_info[x][y].has_workshop = 0;
      mine_info[x][y].has_repairshop = 0;
      mine_info[x][y].has_leeching_pit = 0;
      }
    
  // Make up a map of all levels (from depth 1 down) that
  // will have mines:
  
  mine_info[entrance.getx()][entrance.gety()].num_corridors = 15;
  mine_info[entrance.getx()][entrance.gety()].has_workshop = 1;
  mine_info[entrance.getx()+1][entrance.gety()].num_corridors = 9;
  mine_info[entrance.getx()+1][entrance.gety()].has_leeching_pit = 1;
  mine_info[entrance.getx()-1][entrance.gety()].num_corridors = 9;
  mine_info[entrance.getx()-1][entrance.gety()].has_repairshop = 1;
  mine_info[entrance.getx()][entrance.gety()+1].num_corridors = 9;
  mine_info[entrance.getx()][entrance.gety()-1].num_corridors = 9;

  // Add the entrance requirement:
  Map::get( entrance ).addRequirement( Req(Req::mech_mine, THIS) );

  // Add the mines requirements for dlevel 1:
  for (y = 0; y < map_height; ++y )
    for (x = 0; x < map_width; ++x )
      if (mine_info[x][y].num_corridors)
        Map::get( Point3d(x,y,-1) ).addRequirement( Req(Req::mech_mine, THIS) );


	}

MechMines::MechMines( std::iostream &ios, TargetValue *TVP )
: Feature( ios, TVP ) {}

void MechMines::toFile( std::iostream &ios ) {

	// Write Feature base information:
	Feature::toFile( ios );

	}

targetType MechMines::gettype( void ) const {
	return tg_mech_mines;
	}

bool MechMines::istype( targetType t ) const {

	if ( t == tg_mech_mines )
		return true;
	else return Feature::istype(t);

	}

String MechMines::name( NameOption ) {
	return String("#Mechanical Mines~");
	}

long MechMines::getVal( StatType s ) {

  switch(s) {
  
    case attrib_ooo_ness:
      return 3;
    case attrib_unique:
      return true;
    default:
      return Feature::getVal(s);
    }
  }


//--[ class Sewers ]--//
List<Point3d> Sewers::wet_levels;
List<Point3d> Sewers::dry_levels;
List<Point3d> Sewers::stair_locs;
List<Point3d> Sewers::to_connect;
List<Point3d> Sewers::water_track;
int Sewers::levels_planned = 0;
int Sewers::level_remainder = 0;
Target Sewers::tTHIS;
bool Sewers::finished = false;
Point3d Sewers::entrance = OFF_MAP;

Sewers::Sewers( const Option *o )
: Feature(o) {
  border = Rectangle( 0,0,map_width*lev_width-1,map_height*lev_height-1 );
	}


void Sewers::buildLev( Level &level ) {
  level.addFeature( tTHIS );
  
  Point3d location = level.loc();
  int z = location.getz();
  int x, y;
  Rectangle border = level.getBorder();

  // The Sewers are at least stale-smelling:
  if (location.getz() < 0)
    level.incOdor( abs(location.getz()) );

  // If it's the entrance level:
  if (location == entrance) {
  
    // Create a Sewers special entrance room:
    SpecialRoom::Option sp_opt;
    sp_opt.name = "sewer entrance";
    sp_opt.border = border+Rectangle(5,3,-5,-3);
    sp_opt.level = &level;
    SpecialRoom::create(sp_opt);
    // Add all stairs tiles to the connections list:
    for (y = sp_opt.border.getya(); y < sp_opt.border.getyb(); ++y )
      for (x = sp_opt.border.getxa(); x < sp_opt.border.getxb(); ++x )
        if (level.getTile( Point3d(x,y,0) )->getVal(attrib_has_stairs) )
          stair_locs.add( Point3d(x,y,0) );

    }
  else {


    // --- Otherwise, build the sewer:
    
    // Work out what type of sewer level to build:
    // Is it a wet level?
    if ( wet_levels.reset()
      && wet_levels.find(location) ) {
      // We're a wet level:

      // Remove the wet level point:
      wet_levels.remove();

      List<Point3d> this_water_track,
                    this_to_connect,
                    this_stair_locs;

      // Step 1: Find tiles from Point lists.
      // Water track:
      if (water_track.reset())
      do {
      
        if ( border.contains(water_track.get())
          && water_track.get().getz() == z  ) {
          this_water_track.add(water_track.remove());
          water_track.previous();
          }

        } while ( water_track.next() );
      
      // Floor:
      if (to_connect.reset())
      do {
      
        if ( border.contains(to_connect.get())
          && to_connect.get().getz() == z ) {
          this_to_connect.add(to_connect.remove());
          to_connect.previous();
          }

        } while ( to_connect.next() );
      
      // Stairs:
      if (stair_locs.reset())
      do {
      
        if ( border.contains(stair_locs.get())
          && stair_locs.get().getz() == z  ) {
          this_stair_locs.add(stair_locs.remove());
          stair_locs.previous();
          }

        } while ( stair_locs.next() );
      
      // Step 2: Place inner edge tiles for adjacent, unbuilt levels.
      // Check adjacent levels for wet levels:
      int xloc, yloc;
      Point3d delta;

      // West
      delta.set(-1,+0,+0);
      if ( wet_levels.find( location + delta ) ) {
        xloc = border.getxa();
        yloc = border.getya() + Random::randint( border.getheight()-4 ) + 2;
        // Add water track:
        this_water_track.add( Point3d(xloc, yloc, z) );
        water_track.add( Point3d(xloc, yloc, z) + delta );
        // Add path on either side:
        this_to_connect.add( Point3d(xloc, yloc-1, z) );
        to_connect.add( Point3d(xloc, yloc-1, z) + delta );
        this_to_connect.add( Point3d(xloc, yloc+1, z) );
        to_connect.add( Point3d(xloc, yloc+1, z) + delta );
        }

      // North
      delta.set(+0,-1,+0);
      if ( wet_levels.find( location + delta ) ) {
        xloc = border.getxa() + Random::randint( border.getwidth()-4 ) + 2;
        yloc = border.getya();
        // Add water track:
        this_water_track.add( Point3d(xloc, yloc, z) );
        water_track.add( Point3d(xloc, yloc, z) + delta );
        // Add path on either side:
        this_to_connect.add( Point3d(xloc-1, yloc, z) );
        to_connect.add( Point3d(xloc-1, yloc, z) + delta );
        this_to_connect.add( Point3d(xloc+1, yloc, z) );
        to_connect.add( Point3d(xloc+1, yloc, z) + delta );
        }

      // East
      delta.set(+1,+0,+0);
      if ( wet_levels.find( location + delta ) ) {
        xloc = border.getxb();
        yloc = border.getya() + Random::randint( border.getheight()-4 ) + 2;
        // Add water track:
        this_water_track.add( Point3d(xloc, yloc, z) );
        water_track.add( Point3d(xloc, yloc, z) + delta );
        // Add path on either side:
        this_to_connect.add( Point3d(xloc, yloc-1, z) );
        to_connect.add( Point3d(xloc, yloc-1, z) + delta );
        this_to_connect.add( Point3d(xloc, yloc+1, z) );
        to_connect.add( Point3d(xloc, yloc+1, z) + delta );
        }


      // South
      delta.set(+0,+1,+0);
      if ( wet_levels.find( location + delta ) ) {
        xloc = border.getxa() + Random::randint( border.getwidth()-4 ) + 2;
        yloc = border.getyb();
        // Add water track:
        this_water_track.add( Point3d(xloc, yloc, z) );
        water_track.add( Point3d(xloc, yloc, z) + delta );
        // Add path on either side:
        this_to_connect.add( Point3d(xloc-1, yloc, z) );
        to_connect.add( Point3d(xloc-1, yloc, z) + delta );
        this_to_connect.add( Point3d(xloc+1, yloc, z) );
        to_connect.add( Point3d(xloc+1, yloc, z) + delta );
        }

      // Now check adjacent levels for dry levels:
      
      
      // Step 3: Build the water track and surrounding path.
      // Build water track tiles:
      bool one_room = this_water_track.numData() == 1;

      assert( this_water_track.reset() );
      while ( this_water_track.reset() ) {

        // Remove the head point from the track list:
        Point3d first_wt = this_water_track.remove();

        // If there's a point to connect it to:
        if ( this_water_track.reset() ) {
          // Connect the two points with water corridor:
          Point3d last_wt = this_water_track.get();
          
          // First, find the mid-point we'll use to connect them:
          Point3d mid_point;
          // The mid-point is calculated by averaging the
          // x and y values of both points, ignoring any edge values:
          int x_1, x_2, y_1, y_2;
          x_1=x_2=y_1=y_2 = 0;

          if ( !border.edgePointX(first_wt) )
            x_1 = first_wt.getx();
          if ( !border.edgePointX(last_wt) )
            x_2 = last_wt.getx();
            
          if ( !border.edgePointY(first_wt) )
            y_1 = first_wt.gety();
          if ( !border.edgePointY(last_wt) )
            y_2 = last_wt.gety();

          // We now have the values of both points (ignoring edge),
          // so now we average them:

          if (x_1 && x_2)
            mid_point.setx( (x_1+x_2) / 2 );
          else if (x_1)
            mid_point.setx( x_1 );
          else if (x_2)
            mid_point.setx( x_2 );
          else  // Both are edge tiles.  Use mid level:
            mid_point.setx( border.getxa() + border.getwidth() / 2 );

          if (y_1 && y_2)
            mid_point.sety( (y_1+y_2) / 2 );
          else if (y_1)
            mid_point.sety( y_1 );
          else if (y_2)
            mid_point.sety( y_2 );
          else // Both are edge tiles.  Use mid level:
            mid_point.sety( border.getya() + border.getheight() / 2 );

          // Now we do two corridors: one from first to mid,
          // then one from mid to last:
          
          Corridor::Option cor_opt;
          cor_opt.material = m_water;
          cor_opt.level = &level;
          cor_opt.include_ends = true;
  
          cor_opt.start = first_wt.toPoint();
          cor_opt.finish = mid_point.toPoint();
      		Corridor::create( cor_opt );

          cor_opt.start = mid_point.toPoint();
          cor_opt.finish = last_wt.toPoint();
      		Corridor::create( cor_opt );
          
          }
        // Otherwise, if there was only one room:
        else if (one_room) {
          // Place the water tile:
          Tile::Option tile_opt;
          tile_opt.location = first_wt;
          tile_opt.floor = m_water;
          tile_opt.type = tt_ground;
          tile_opt.feature = tTHIS;
    			level.tileCreate( tile_opt );
          }
      
        }
      
      // Build path tiles for all floor tiles adjacent to a water track:
      assert( this_to_connect.reset() );
      while( this_to_connect.reset() ) {
        // Get a track point:
        Point3d track_point = this_to_connect.remove(),
                end_point = track_point;
        // Trace a line toward the centre of the level,
        // stopping when we reach a non-wall tile:
        
        // Make sure that the track point is an edge tile:
        assert( border.contains(track_point) );

        if ( border.edgePointX(track_point) ) {
          // The end point must be at the same y but
          // at the opposite EW end of the level:
          end_point.setx( border.getxa() + border.getxb() - end_point.getx() );

          // Make the corridor:
          Corridor::Option cor_opt;
          cor_opt.material = m_stone;
          cor_opt.level = &level;
          cor_opt.include_ends = true;
          cor_opt.stop_on_floor = true;
  
          cor_opt.start = track_point.toPoint();
          cor_opt.finish = end_point.toPoint();
      		Corridor::create( cor_opt );
          }
          
        else if ( border.edgePointY(track_point) ) {
          // The end point must be at the same x but
          // at the opposite NS end of the level:
          end_point.sety( border.getya() + border.getyb() - end_point.gety() );

          // Make the corridor:
          Corridor::Option cor_opt;
          cor_opt.material = m_stone;
          cor_opt.level = &level;
          cor_opt.include_ends = true;
          cor_opt.stop_on_floor = true;
  
          cor_opt.start = track_point.toPoint();
          cor_opt.finish = end_point.toPoint();
      		Corridor::create( cor_opt );
          }
          
        else {
          assert(0); }
        
        }
      
      // Step 4: Fill unused space with rooms/tunnels and place stairs:

      // Place stairs:
      if ( this_stair_locs.reset() )
      do {

        Point3d this_stair = this_stair_locs.remove();

        // Prepare the tile to contain a staircase:
        Tile::Option tile_opt;
        tile_opt.location = this_stair;
        tile_opt.floor = m_stone;
        tile_opt.type = tt_ground;
        tile_opt.feature = tTHIS;
  			level.tileCreate( tile_opt );

        // Place down-stairs on this level:
        Stairs::Option stairs_opt;
        stairs_opt.connecting = z-1;
    		level.getTile( this_stair )
          ->perform( aContain, Stairs::create( stairs_opt ) );
        
    		// Make sure lower level has an up-stairs at this position:
    		Map::get( level.loc() - Point3d(0,0,1) )
          .addRequirement( Req(Req::stairs_up,this_stair.toPoint()) );

        //Message::add("STAIRS CREATED!");

        }  while ( this_stair_locs.next() );


      // Step 5: Connect all non-water-track-adjacent tiles and rooms.

     	// Check for inaccessible parts:
    	char ** testgrid;
    	testgrid = new char*[lev_width];

    	for ( x = 0; x < lev_width; ++x )
    		testgrid[x] = new char[lev_height];

    	// Initialise test grid:
    	for (int y = border.getya(); y <= border.getyb(); ++y)
    		for (int x = border.getxa(); x <= border.getxb(); ++x) {
    			testgrid[x%lev_width][y%lev_height]
            = ( level.getTile( Point3d(x,y,z))->getVal(attrib_non_wall)
             || level.getTile( Point3d(x,y,z))->itemList() != NULL  );
          // Screen::put(x+1,y+3,Image(cGrey,'0'+testgrid[x][y]));
          }
          
    	Dungeon_Tester dt(testgrid,lev_width,lev_height);
    	dt.find_regions();
      Tile::Option tile_opt;
      tile_opt.floor = m_stone;
      tile_opt.type = tt_ground;
      Point origin = border.geta();

      while (dt.get_num_regions() > 1) {
        List<Point> route = dt.dig_from(0);

        if (route.reset())
        do {

    			if ( ! level.getTile( Point3d( route.get()+origin, z ) )->istype( tg_floor ) ) {
    
            tile_opt.location = Point3d( route.get()+origin, z );
      			tile_opt.feature = tTHIS;
            level.tileCreate( tile_opt );

            }
    
          } while ( route.next() );

        else Message::add("Error digging path in Sewer creation.");
      
        }
    
    	// Dealloc test grid:
    	for ( x = 0; x < lev_width; ++x )
    		delarr( testgrid[x]);

    	delarr( testgrid );
      
      
      // -----------------
      /*
      SpecialRoom::Option sp_opt;

      switch (Random::randint(3)) {
        case 0:  sp_opt.name = "filthy sewer";
                 break;
        case 1:  sp_opt.name = "putrid sewer";
                 break;

        default: sp_opt.name = "stinky sewer";
        }

      sp_opt.border = border;
      sp_opt.level = &level;
      SpecialRoom::create(sp_opt);
      */
      
      // Place some stuff:
      List<Target> sp_list;

      // Place some items:
      for (int i = DiceRoll(2,4,-4).roll(); i >= 0; --i)
        sp_list.add( Random::themeitem( th_sewers ) );

      // Place some monsters:
      Monster::Option mon_opt;
      mon_opt.theme = th_sewers;
      mon_opt.odor = level.getOdor();

      for (int i = DiceRoll(2,3,-3).roll(); i >= 0; --i)
        sp_list.add( Monster::create(mon_opt) );

      level.sprinkle(sp_list);


      }
    else if (dry_levels.reset()
          && dry_levels.find(location) ) {
      // We're a dry level:
      // ATM doesn't exist:
      assert(0);
      }
    else {
      // We shouldn't be a level at all!
      assert(0);
      }

    
    /*

    List<Point3d> to_connect;

    // Get the info for this level:
    //...

    // Check the stair locations list for any points within this level:
    bool next;
    if (stair_locs.reset())
    do {

      next = false;
      // Place connection, and add to our to_connect list
      if (border.contains( stair_locs.get() )) {

        to_connect.add(stair_locs.remove());
        next = stair_locs.canGet();
        }

      } while ( next || stair_locs.next() );


    // Now check that all corridors are connected:
    ((Feature*)tTHIS.raw())->connectAll(&level);

    // Place some doors:
  	level.placeDoors();

    // Place some stuff:
    List<Target> sp_list;

    // Place some items:


    // Place some monsters:
    Monster::Option mon_opt;
    mon_opt.theme = th_sewers;

    for (int i = 0; i < 5; ++i)
      sp_list.add( Monster::create(mon_opt) );

    level.sprinkle(sp_list);
    */
    }

  // Make sure that the level below has been
  // designed already:
  createMap( location.getz() -1 );

  }

// This structure is needed for the createMap
// method, but since it is used in a templated
// List, we need it a non-loca type:
struct PlanStruct {
  Point3d location;
  int exits;
  };

#include "input.hpp"

void Sewers::createMap( int depth ) {
  // --- Make up a map of all levels (from depth 1 down) that
  // will have sewers.

  if (finished) return;
  // Make sure we only plan the map for unplanned dlevels:
  if (levels_planned <= depth)
    return;

  // Set up our values:
  int remainder = 0;
  int total_exits = 50 + level_remainder;
  
  // Make up the list of points to place levels:
  List<PlanStruct> new_levels;
  
  // -- First, make up some wet levels:
  // Find all the locations of stairs that enter this
  // level from above:
  if (stair_locs.reset())
  do {

    // if we find stairs one level above this one:
    if (stair_locs.get().getz() == depth+1) {
      // Add an initial room to this level:
      PlanStruct initial;
      initial.location = stair_locs.get();
      initial.location.sety( initial.location.gety() / lev_height );
      initial.location.setx( initial.location.getx() / lev_width );
      initial.location.setz(depth);
      initial.exits = total_exits;
      new_levels.add(initial);

      //Message::add( String("New Level: ") << initial.location.getx() << ","
      //                                    << initial.location.gety() << ","
      //                                    << initial.location.getz() << "." );
      }

  
    } while (stair_locs.next());
  
  // Now keep popping a location from the stack until done:

  // We can't continue if we've found no new stair locations:
  if (!new_levels.reset() ) {
    finished = true;
    Message::add("You feel you've reached the end of the sewers.");
    return;
    }
  
  while ( new_levels.reset() ) {
    PlanStruct current = new_levels.remove();

    // Find out which adjacent areas already have a level:
    Dir deltas_exist;
    deltas_exist.reset(false);
    
    // This can be either from the new_levels list:
    if (new_levels.reset())
    do {
      Point3d delta = new_levels.get().location - current.location;
      
      if ( max(abs(delta.getx()),abs(delta.gety())) == 1
        && delta.getz() == 0 )
          deltas_exist[delta.toPoint()] = true;
      
      } while (new_levels.next());
      
    // Or the wet_levels list:
    if (wet_levels.reset())
    do {
      Point3d delta = wet_levels.get() - current.location;
      
      if ( max(abs(delta.getx()),abs(delta.gety())) == 1
        && delta.getz() == 0 )
          deltas_exist[delta.toPoint()] = true;
      
      } while (wet_levels.next());


    assert(deltas_exist[Point(0,0)] == false);
    
    // Now decide what to do with this tile based on
    // where adjacent tiles are:

    //  *  We can have up to 3 orthogonally
    // *#  adjacent levels and still continue
    //  *  to dig.
    if ( deltas_exist.totalOrthog() <= 3

    // *   We can have up to 2 diagonally
    //  #  adjacent levels and still continue
    // *   to dig.
      && deltas_exist.totalDiag() <= 2 ) {

      // Make a dir that represents all possible
      // directions the digging can progress:
      Dir next_dir = deltas_exist.invert();
      
      next_dir.n &= next_dir.nw && next_dir.ne;
      next_dir.s &= next_dir.se && next_dir.sw;
      next_dir.e &= next_dir.ne && next_dir.se;
      next_dir.w &= next_dir.nw && next_dir.sw;
      next_dir.nw = false;
      next_dir.ne = false;
      next_dir.sw = false;
      next_dir.se = false;
      next_dir.self = next_dir.ground = false;

      List<Point> next_deltas = next_dir.allPoints();
      
      // Now choose a direction from the possibilities:
      Point next;
      int num = 0;
      int num_exits = 1;

      // When there are few exits from this level, there's
      // a chance to have two or more exits:
      while (Random::randint(4) > deltas_exist.total())
        ++num_exits;

      if (num_exits > next_dir.total())
        num_exits = abs(next_dir.total());

      if (num_exits > current.exits)
        num_exits = current.exits;

      // (for safety, check that we have possibilities)
      if (!next_deltas.reset())
        remainder += current.exits;
        
      else {

        int this_exit = current.exits - 1;
#       ifdef SEWERS_DEBUG
        int color = cBlue;
#       endif // SEWERS_DEBUG

        for (int i = 1; i <= num_exits; ++i) {
          num = 0;
          // Choose a direction from the list:
          if (next_deltas.reset())
          do {
            if ( Random::randint(++num) == 0 )
              next = next_deltas.get();
            } while (next_deltas.next());

          // Now add the next level to the new levels list:
          if ( Rectangle(1,1,map_width-2,map_height-2).contains(Point3d(next) + current.location) ) {

            PlanStruct next_level;
            next_level.location = Point3d(next) + current.location;

            if (i != num_exits)
              next_level.exits = this_exit / 2;
            else
              next_level.exits = this_exit;

            new_levels.add(next_level);
#           ifdef SEWERS_DEBUG
            MAP_DRAW(next_level.location.getx(),
                     next_level.location.gety(),
                     Image(color++ % 16,'#'));
#           endif // SEWERS_DEBUG
            }
          else
            remainder += this_exit / 2;

          // Each exit gets a portion of the exit points:
          this_exit -= this_exit / 2;

          // Remove the chose point so it's not chosen again:
          if (next_deltas.find(next))
            next_deltas.remove();
          else assert(0);
          }
        }
        
      }
      
    else {
      // Add all unused tiles to the next level's total:
      remainder += current.exits;
      // If we haven't placed any down stairs,
      if (levels_planned > depth) {
        // place one on this level:
        Point3d down_stairs;
        
        // Set the level coords:
        down_stairs.set( current.location.getx() * lev_width,
                         current.location.gety() * lev_height,
                         current.location.getz() );
                         
        // Locate somewhere mid-level:
        down_stairs.getx() += Random::randint( lev_width - 4 ) + 2;
        down_stairs.gety() += Random::randint( lev_height - 4 ) + 2;

        stair_locs.add( down_stairs );
        
        levels_planned = depth;
        }
        

      }

    // This location is a new wet level:
    wet_levels.add(current.location);
    // And don't forget to add a Map hook so this level
    // is actually created:
    Map::get( current.location ).addRequirement( Req(Req::sewer, tTHIS) );
    
    
    }

  // Now give the debugging programmer a look at the sewer:
# ifdef SEWERS_DEBUG
  Keyboard::get();
# endif // SEWERS_DEBUG

  // -- Now place some dry levels:
  
//List<Point3d> Sewers::dry_levels;
//List<Point3d> Sewers::to_connect;
//List<Point3d> Sewers::stair_locs;



  // Now we've planned this level:
  levels_planned = depth;
  level_remainder = remainder;
  }
  
void Sewers::build( const Option & ) {

  // Add the entrance requirement:
  Map::get( entrance ).addRequirement( Req(Req::sewer, THIS) );

  }

Sewers::Sewers( std::iostream &ios, TargetValue *TVP )
: Feature( ios, TVP ) {}

void Sewers::toFile( std::iostream &ios ) {

	// Write Feature base information:
	Feature::toFile( ios );

	}

targetType Sewers::gettype( void ) const {
	return tg_sewer;
	}

bool Sewers::istype( targetType t ) const {

	if ( t == tg_sewer )
		return true;
	else return Feature::istype(t);

	}

String Sewers::name( NameOption ) {
	return String("#sewers~");
	}

long Sewers::getVal( StatType s ) {

  switch(s) {
  
    case attrib_ooo_ness:
      return 2;
    case attrib_unique:
      return true;
    default:
      return Feature::getVal(s);
    }
  }

/////////////////////////
/////////////////////////
/////////////////////////

#ifdef AT_FEATURES

int main() {

//	Random::randomize();
//	global::reset();

	Screen::init();
  Screen::cls();
	Keyboard::init();

	Screen::cls();
  Screen::update();

  SpecialRoom::loadData();
  SpecialRoom::clearData();
  
  //  Keyboard::get();

  int ret_val = printTotalDeletes();
	Screen::end();

	return ret_val;
	}


#endif // AT_FEATURES


#endif // FEATURES_CPP
