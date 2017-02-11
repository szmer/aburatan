// [target.cpp]
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
#if !defined ( TARGET_CPP )
#define TARGET_CPP

#include "target.hpp"
#include "rawtargt.hpp"
#include "catalog.hpp"
#include "enum.hpp"
#include "file.hpp"

#include "bagbox.hpp"
#include "money.hpp"
#include "food.hpp"
#include "actor.hpp"
#include "player.hpp"
#include "monster.hpp"
#include "weapon.hpp"
#include "quantity.hpp"
#include "ammo.hpp"
#include "clothing.hpp"
#include "trap.hpp"
#include "door.hpp"
#include "tile.hpp"
#include "features.hpp"
#include "stairs.hpp"
#include "fluid.hpp"
//#include "light.hpp"
#include "playerui.hpp"
#include "scope.hpp"
#include "book.hpp"
#include "cloud.hpp"
#include "terrain.hpp"
#include "widget.hpp"
#include "quest.hpp"


// We need raw() in a lot of places, all of which
// are called *very* often, so let's set up a
// macro rather than add another function call:
#define RAWDATA ((&(*value))?value->getData():NULL)

Target::Target( TargetValue *ptv )
: value(ptv) {
	}

RawTarget *Target::raw() const {
  return RAWDATA;
	}

bool Target::operator !( void ) const {
    if (&(*value))
  	  return value->isNull();
    else
      return true; // True that it is NULL
	}


String Target::menustring( char len ) {
	return RAWDATA->menustring(len);
	}
Image Target::menuletter( void ) {
	return RAWDATA->menuletter();
	}

bool Target::operator ==( const Target &t ) const {
	return ( RAWDATA == t.raw() )? true:false;
	}

bool Target::operator !=( const Target &t ) const {
	return ( *this == t )? false:true;
	}

bool Target::operator < ( const Target &t ) {
	return ( *RAWDATA < *t.raw() )? true:false;
	}

bool Target::operator > ( const Target &t ) {
	return ( *RAWDATA > *t.raw() )? true:false;
	}

int Target::numRefs( void ) {

  // Attempt to get numRefs for a NULL Target:
	assert ( & (*value) );

	return value->totalCount();
	}

void Target::save( void ) {

  // Only bother if we're in memory:
	if ( value->inMem() &&
	// Attempt to save a NULL Target is OK:
       (& (*value) )
       )
  	value->toFile();
    
	}

void Target::transform( const Target &t ) const {

	// Attempt to transform a NULL Target:
	assert ( & (*value) );
  
	value->transform(t);

	}

RawTarget* Target::operator->() const {
  assert (value->getData());
  return value->getData();
	}
RawTarget& Target::operator*() const {
  assert (value->getData());
  return *value->getData();
	}

std::iostream &operator << ( std::iostream &ios, Target &t ) {

	if ( & (*t.value) ) {
		t.value->incSaveCount();
		long write = t.value->getSerial().toLong();
		ios.write( (char*) &write, sizeof(write) );
		}
	else {
		long nullTarget = 0;
		ios.write( (char*) &nullTarget, sizeof(nullTarget) );
		}

	return ios;

	}

std::iostream &operator >> ( std::iostream &ios, Target &t ) {

	long serial = 0;
	ios.read( (char*) &serial, sizeof(serial) );

  assert(serial >= 0);

  t.value = Catalogue::get( serial );

	if ( & (*t.value) )
		t.value->decSaveCount();

	return ios;

	}


//----[ class TargetValue ]-------//


TargetValue::TargetValue( long ser )
: type(isSavedRaw), saveCount( 0 )
# ifdef SOFT_LINK
  , memSoftCount( 0 ), saveSoftCount( 0 )
# endif // SOFT_LINK
{
	init( ser );
	}

TargetValue::TargetValue(RawTarget *initValue)
: type( isRaw ), saveCount( 0 )
# ifdef SOFT_LINK
  , memSoftCount( 0 ), saveSoftCount( 0 )
# endif //SOFT_LINK
{
	init(initValue);
	}


TargetValue::TargetValue(TargetValue &)
: RCObject(){
	// TargetValue copy constructor invoked!  Illegal!
  assert(0);
	}

void TargetValue::init(RawTarget *initValue) {
	data.raw = initValue;
	type = isRaw;
	}

void TargetValue::init( Target *initValue ) {
	data.target = initValue;
	type = isTarget;
	}


TargetValue::~TargetValue() {

# ifdef SOFT_LINK
  if (memSoftCount > 0) toFile();
  assert (memSoftCount == 0);
# endif // SOFT_LINK

	if (saveCount > 0 ) {
		toFile();

		// Determine the file to access:
		long fileSerial = serial.toLong();
		long fileOffset = fileSerial % BLOCKS_PER_FILE;
		fileSerial -= fileOffset;

		String filename = SAVEDIR;
		filename = filename + Serial::toFileName( fileSerial ) + ".TV";

		assert (filename != SAVEDIR+"ERROR.TV");

		File file;
		std::fstream &myFile = file[filename];

		// Go to designated file block:
		myFile.seekp( fileOffset * BLOCK_TARGETVALUE );

		// ...save saveCount
		myFile.write( (char*) &saveCount, sizeof(saveCount) );

#   ifdef SOFT_LINK
		// ...save saveSoftCount
		myFile.write( (char*) &saveSoftCount, sizeof(saveSoftCount) );
#   endif // SOFT_LINK

		// ...save type (either isTarget or isSavedRaw)
		myFile.write( (char*) &type, sizeof(type) );

		if ( type == isTarget ) {
			myFile << *(data.target);
			delptr( data.target );
			}

    Catalogue::remove(this);

		}

# ifdef SOFT_LINK
  else if ( saveSoftCount > 0 )
    Catalogue::remove(this);
# endif // SOFT_LINK

	if (type == isRaw)
		delptr( data.raw);
	if (type == isTarget)
		delptr( data.target);
	}

void TargetValue::init( long initValue ) {
// Reinstates previous state of TV from a file.
	serial.init(initValue);

	// Determine the file to access:

	long fileSerial = serial.toLong();
	long fileOffset = fileSerial % BLOCKS_PER_FILE;
	fileSerial -= fileOffset;

	String filename = SAVEDIR;
	filename = filename + Serial::toFileName( fileSerial ) + ".TV";

	assert (filename != SAVEDIR+"ERROR.TV");

	File file;
	std::fstream &myFile = file[filename];

	// Go to designated file block:
	long seekto = fileOffset * BLOCK_TARGETVALUE;
	myFile.seekg( seekto );
//	if ( myFile.tellg() != (long)(fileOffset * BLOCK_TARGETVALUE) ) Error::fatal("Seekg failed in TV::init");

	// ...load saveCount
	myFile.read( (char*) &saveCount, sizeof(saveCount) );

# ifdef SOFT_LINK
	// ...load saveSoftCount
	myFile.read( (char*) &saveSoftCount, sizeof(saveSoftCount) );
# endif // SOFT_LINK

	if (saveCount > 0
#     ifdef SOFT_LINK
      || saveSoftCount > 0
#     endif // SOFT_LINK
      ) Catalogue::add(this);

	// ...load type (either isTarget or isSavedRaw)
	myFile.read( (char*) &type, sizeof(type) );

	if ( type != isSavedRaw ) {
		Target read;
		myFile >> read;

		data.target = new Target (read);
		}
  else data.raw = NULL;

	}


void TargetValue::toFile( void ) {

	serial.init();

	if ( (type == isRaw) && data.raw ) {

		// Determine the file to access:

		long fileSerial = serial.toLong();
		long fileOffset = fileSerial % BLOCKS_PER_FILE;
		fileSerial -= fileOffset;

		String filename = SAVEDIR;
		filename = filename + Serial::toFileName( fileSerial ) + ".TBD"; // '.TBD' == Target Block Data

		File file;
		std::fstream &myFile = file[filename];

		// Go to designated file block:
		myFile.seekp( fileOffset * BLOCK_RTDERIVED );

		// Save type of RawTarget derived object:
		targetType tgType = data.raw->gettype();
    assert (tgType < tg_invalid);

		myFile.write( (char*) &tgType, sizeof(tgType) );

		// Save object:
		data.raw->toFile( myFile );

		transform(NULL);
    type = isSavedRaw;
		}

	}

RawTarget *TargetValue::fromFile( void ) {

	assert (serial.toLong() != 0);

	long fileSerial = serial.toLong();
	long fileOffset = fileSerial % BLOCKS_PER_FILE;
	fileSerial -= fileOffset;

	String filename = SAVEDIR;
	filename = filename + Serial::toFileName( fileSerial ) + ".TBD"; // '.TBD' == Target Block Data

	File file;
	std::fstream &myFile = file[filename];

	// Go to designated file block:
	myFile.seekg( fileOffset * BLOCK_RTDERIVED );

	// Load type of RawTarget derived object:
	targetType tgType;
	myFile.read( (char*) &tgType, sizeof(tgType) );

	// Load object:
	RawTarget *rtp = NULL;

	// THE BIG SWITCH for loading RawTarget types:
	// Todo: ... *eventually* to be replaced with function pointers from an array.
  // Well, probably.  If deemed worthwhile.
	switch (tgType) {

//		case tg_floor: rtp = Floor::load(myFile, this); break;

		case tg_player: rtp = Player::load(myFile, this); break;
		case tg_monster: rtp = Monster::load(myFile, this); break;
		case tg_human: rtp = Human::load(myFile, this); break;
		case tg_mount: rtp = Mount::load(myFile, this); break;

		case tg_melee: rtp = Melee::load(myFile, this); break;
		case tg_ranged: rtp = Ranged::load(myFile, this); break;
		case tg_ammo: rtp = Ammo::load(myFile, this); break;
		case tg_clothing: rtp = Clothing::load(myFile, this); break;

		case tg_food: rtp = Food::load(myFile, this); break;

		case tg_fluid: rtp = Fluid::load(myFile, this); break;

		case tg_bag: rtp = Bag::load(myFile, this); break;
		case tg_bottle: rtp = Bottle::load(myFile, this); break;
		case tg_quiver: rtp = Quiver::load(myFile, this); break;
    //case tg_light: rtp = Light::load(myFile, this); break;
    case tg_scope: rtp = Scope::load(myFile, this); break;
    case tg_book: rtp = Book::load(myFile, this); break;

    case tg_money:  rtp = Money::load(myFile, this); break;

		case tg_trap: rtp = Trap::load(myFile, this); break;
    //case tg_widget: rtp = Widget::load(myFile, this); break;
    case tg_remains: rtp = Remains::load(myFile, this); break;
    case tg_machine_part: rtp = MachinePart::load(myFile, this); break;
//		case tg_wall: rtp = Wall::load(myFile, this); break;
//		case tg_fence: rtp = Fence::load(myFile, this); break;
//		case tg_flora: rtp = Flora::load(myFile, this); break;
		case tg_stairs: rtp = Stairs::load(myFile, this); break;
		case tg_door: rtp = Door::load(myFile, this); break;

		case tg_grave: rtp = Grave::load(myFile, this); break;
		case tg_headstone: rtp = Headstone::load(myFile, this); break;
    
		case tg_cloud: rtp = Cloud::load(myFile, this); break;

		case tg_field: rtp = Field::load(myFile, this); break;
		case tg_fourfield: rtp = FourField::load(myFile, this); break;
		case tg_solid: rtp = Solid::load(myFile, this); break;
		case tg_house: rtp = House::load(myFile, this); break;
		case tg_forest: rtp = Forest::load(myFile, this); break;
		case tg_stairway: rtp = Stairway::load(myFile, this); break;
		case tg_corridor: rtp = Corridor::load(myFile, this); break;
    case tg_town: rtp = Town::load(myFile, this); break;
    case tg_road: rtp = Road::load(myFile, this); break;
    case tg_basement: rtp = Basement::load(myFile, this); break;
    case tg_sewer: rtp = Sewers::load(myFile, this); break;
    case tg_graveyard: rtp = Graveyard::load(myFile, this); break;
    case tg_churchyard: rtp = Churchyard::load(myFile, this); break;
    case tg_perimeter: rtp = Perimeter::load(myFile, this); break;
    case tg_mech_mines: rtp = MechMines::load(myFile, this); break;

    case tg_special_room: rtp = SpecialRoom::load(myFile, this); break;
    case tg_first_level: rtp = FirstLevel::load(myFile, this); break;

		case tg_quantity: rtp = Quantity::load(myFile, this); break;

    case tg_tile: rtp = Tile::load(myFile, this); break;

		case tg_quest: rtp = Quest::load(myFile, this); break;
    
		case tg_brain: rtp = Brain::load(myFile, this); break;
		case tg_player_ui: rtp = PlayerUI::load(myFile, this); break;
    case tg_walkdir: rtp = WalkDir::load(myFile, this); break;

		default: {
      assert(0);
      }

		}

	transform( rtp );
  return rtp;

	}

void TargetValue::transform( RawTarget *rt ) {
	if (  ( type != isRaw ) || ( rt != data.raw )  ){

		switch (type) {
			case isRaw: {
       // Use this roundabout method to make sure that
       // any recursive calls don't delete the deleted:
        RawTarget *raw = data.raw;
        data.raw = NULL;
				delptr(raw);
				break;
        }
			case isTarget: {
       // Use this roundabout method to make sure that
       // any recursive calls don't delete the deleted:
        Target *target = data.target;
        data.target = NULL;
        delptr(target);
        break;
        }
                                
        default: break;
			}

		data.raw = rt;

    type = isRaw;
		}
	}

void TargetValue::transform( const Target &t ) {

	if (type == isSavedRaw) fromFile();

	switch (type) {
		case isRaw:
			delptr( data.raw);
			break;
		case isTarget:
			if (t == *data.target) return;
			delptr( data.target);
			break;

                default:  break;
		}

	init( new Target(t) );

	}

bool TargetValue::isNull( void ) const {

	switch (type) {
		case isRaw:
      return !data.raw;
      
		case isTarget:
      return !data.target;

    default:
      // Saved targets are never NULL:
      return false;
    }

  }

bool TargetValue::inMem( void ) const {

  return  type != isSavedRaw;
  
  }
  
RawTarget *TargetValue::getData( void ) {
	switch (type) {
		case isRaw:
			return data.raw;
		case isTarget:
			return data.target->raw();
		case isSavedRaw:
			return fromFile();
		default: {
      assert(0);
			return NULL;
      }
		}
	}

void TargetValue::decSaveCount( void ) {
	saveCount--;
	if (saveCount == 0
#   ifdef SOFT_LINK
    && saveSoftCount == 0
#   endif // SOFT_LINK
    )
		Catalogue::remove(this);
	}

void TargetValue::incSaveCount( void ) {
	serial.init();
	if (saveCount == 0
#   ifdef SOFT_LINK
    && saveSoftCount == 0
#   endif // SOFT_LINK
    )
		Catalogue::add(this);

	saveCount++;
	}


void TargetValue::addReference( void ) {
# ifdef SOFT_LINK
  if ( Count() == 0 && memSoftCount > 0 )
    Catalogue::unlink(this);
# endif // SOFT_LINK
  RCObject::addReference();
  }
  
void TargetValue::removeReference( void ) {
  RCObject::removeReference();
  }

#ifdef SOFT_LINK

void TargetValue::decMemSoftCount( void ) {
	memSoftCount--;
	if ( memSoftCount == 0 )
		Catalogue::unlink(this);
	}

void TargetValue::incMemSoftCount( void ) {
	serial.init();
	if ( memSoftCount == 0 && Count() == 0 )
		Catalogue::link(this);

	memSoftCount++;
	}

void TargetValue::decSaveSoftCount( void ) {
	saveSoftCount--;
	if (saveCount == 0 && saveSoftCount == 0)
		Catalogue::remove(this);
	}

void TargetValue::incSaveSoftCount( void ) {
	serial.init();
	if (saveCount == 0 && saveSoftCount == 0)
		Catalogue::add(this);

	saveSoftCount++;
	}
#endif // SOFT_LINK

Serial &TargetValue::getSerial( void ) {
	serial.init();
	return serial;
	}



#ifdef _TARGET_HARNESS

#include "item.hpp"
#include "visuals.hpp"
#include "input.hpp"
#include "point.hpp"
#include "rectngle.hpp"
#include "string.hpp"
#include "counted.hpp"
#include "target.hpp"
#include "random.hpp"
#include "rawtargt.hpp"
#include "viewable.hpp"

#define newline "\r\n"

void writeTarget( String ID, Target t ) {

	Screen::write( ID + "==" + t.menustring() + " " + t.numRefs() + newline );

	}

void mainroutine() {
	Target t1 = Melee::create();

	Target t2 = t1;

	Target t3 = Ranged::create();

	Screen::write(newline);

	writeTarget( "t1", t1 );
	writeTarget( "t2", t2 );
	writeTarget( "t3", t3 );

	Screen::write( String()+"t2=t3" + newline );
	t2 = t3;

	writeTarget( "t1", t1 );
	writeTarget( "t2", t2 );
	writeTarget( "t3", t3 );

	Screen::write( String()+"t3=t1" + newline );
	t3 = t1;

	writeTarget( "t1", t1 );
	writeTarget( "t2", t2 );
	writeTarget( "t3", t3 );

  Screen::update();

	}


int main() {

  Random::randomize();

	mainroutine();
	return 0;
  
	}

#endif // _TARGET_HARNESS

#endif // TARGET_CPP
