// [rawtargt.cpp]
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
#if !defined ( RAWTARGT_CPP )
#define RAWTARGT_CPP

#include "rawtargt.hpp"
#include "enum.hpp"
#include "visuals.hpp"
#include "sorted.hpp"
#include "attack.hpp"
#include "catalog.hpp"
#include "file.hpp"
#include "grammar.hpp"
#include "map.hpp"

RawTarget::RawTarget( const Option *o )
: Viewable( o->image ),
  THIS(NULL),
  inventory(NULL),
	parent((TargetValue*)NULL),
  //location(o->location),
  id_mask(id_none){
  }

RawTarget::RawTarget( RawTarget &t )
: Viewable( t ),
  THIS(NULL),
  inventory( NULL ),
	parent((TargetValue*)NULL),
  //location(t.location),
  id_mask(t.id_mask) {

  if ( (t.inventory) && (t.inventory->reset() )  )
  do {
      Target target = t.inventory->get()->clone();
    add( target );
    } while ( t.inventory->next() );

  }

RawTarget::~RawTarget( void ) {

  if (inventory) {

    if (inventory->reset())
    do {
      inventory->get().save();
      } while ( inventory->next() );

    delptr( inventory );
    }

  // Make sure limbo knows we're completely destroyed:
  //Map::limbo.remove(THIS);

  }

void RawTarget::clr( void ) {

  if (inventory) {

    while (inventory&&inventory->reset()) {
      inventory->get()->clr();
      inventory->remove()->detach();
      }

    delptr( inventory );
    }
  }

int RawTarget::quantity( void ) {
  return 1;
  }


Action RawTarget::autoAction( void ) {
  return aNone;
  }
Action RawTarget::exitAction( void ) {
  return aNone;
  }

targetType RawTarget::gettype( void ) const {
  return tg_target;
  }

bool RawTarget::istype( targetType t ) const {
  if (t == tg_target)
    return true;
  else
    return false;
  }


void RawTarget::setTHIS( TargetValue *TVP ) {

  THIS = TVP;

  if ( (inventory) && (inventory->reset() )  )
  do {
    inventory->get()->setParent(THIS);
    } while ( inventory->next() );

  }

/*
void RawTarget::setParent( const Target &t ) {
  // Make sure detach call doesn't delete us:
  Target tTHIS = THIS;
  
  // If we're moving from off the map,
  if (location == OFF_MAP)
    // Tell the limbo list we've left:
    Map::limbo.remove(THIS);
  else {
    // Otherwise, detach from parent:
    Target tTHIS = THIS;
    Target parent = getTarget(tg_parent);
  
    if (!!parent)
      parent->remove( tTHIS );
    }
    
  // Now set our new location.
  // If we're moving off the map,
  if (!t) {
    // Add to limbo:
    setlocation(OFF_MAP);
    Map::limbo.add(THIS);
    }
  else
    // Otherwise, set the new location:
    setlocation( t->getlocation() );

  }
*/

/*
void RawTarget::setlocation( const Point3d &p3d ) {

  // If we haven't moved, don't bother:
  if (location == p3d)
    return;

  location = p3d;

  if (location != OFF_MAP)
    Map::limbo.remove(THIS);

  if ( (inventory) && (inventory->reset() )  )
  do {
    inventory->get()->setlocation(p3d);
    } while ( inventory->next() );
    
  }
*/

/*
#include "message.hpp"
Target RawTarget::findTarget( targetType type, const Target& t ) {

  switch( type ) {
    case tg_possessor:
    case tg_parent: {
    //  assert( location == t->getlocation() );
      if (location != t->getlocation()) {
        String msg;
        msg << Grammar::plural(THIS);
        msg << "(" << location.getx() << "," << location.gety() << ") != ";
        msg << Grammar::plural(t);
        msg << "(" << t->getlocation().getx() << "," << t->getlocation().gety() << ")";

        Message::add(msg);
        //Message::add(String("Invalid group! p(")+(long)location.getx()+","+(long)location.gety()+")");
        return Target();
        }

      Target found;

      // Look through inventory
      if ( (inventory) && (inventory->reset() )  ) {
        long key = inventory->lock();

        do {
          // if an item in the inventory is the item we look for,
          if (inventory->get() == t) {

            inventory->load(key);
            // we're the parent
            return THIS;
            }

          // otherwise, ask the item if it knows who the parent/possessor is:
          found = inventory->get()->findTarget(type,t);
          if (!!found) {
            inventory->load(key);
            return found;
            }

          } while ( inventory->next() );

        inventory->load(key);
        }

      // We don't know.  Return NULL:
      return Target(NULL);
      }
      
    default:
      return Target(NULL);
    }
    
  }
*/

/*
Target RawTarget::getparent( void ) {

  if ( location == OFF_MAP )
    return Map::limbo.findParent(THIS);
  
  return Map::getTile(location)->findparent(THIS);
  }
*/

void RawTarget::add( const Target &i ) {

  Target object = i; // Make sure there is a solid reference to 'i'!

  if (object.raw()) {
    // Because it's probably owned by another object:
    object->detach();
    // Tell the object it's now ours:
    //object->setParent( THIS );
	  //if (object.raw()) {
		object->setParent( THIS );

    if (!inventory)
      allocInventory();

    // Check to see if items can stack:
    bool combined = false;

    if ( inventory->reset() )
    do {
      if (  inventory->get()->capable( aCombine, object )  ) {
        inventory->get()->perform( aCombine, object );
        combined = true;
        }
      } while ( !combined && inventory->next() );

    if (!combined) inventory->add(object);

    // *now* Tell the object it's now ours:
    //object->setParent( THIS );
    }
  }


RawTarget::RawTarget( std::iostream &inFile, TargetValue *TVP )
: Viewable(0),
  THIS(TVP),
  inventory(NULL),
	parent((TargetValue*)NULL),
  //location(OFF_MAP),
  id_mask(id_none) {

  inFile.read( (char*) &image, sizeof(image) );
  inFile.read( (char*) &id_mask, sizeof(id_mask) );

  inFile >> parent;
  //inFile.read( (char*) &location, sizeof(location) );

  bool hasinv;

  inFile.read( (char*) &hasinv, sizeof(hasinv) );

  long  saveLoc = inFile.tellg();
  String filename;

  if (hasinv) {
    using namespace std;

    // Determine the file to access:
    long fileSerial = THIS->getSerial().toLong();
    long fileOffset = fileSerial % BLOCKS_PER_FILE;
    fileSerial -= fileOffset;

    filename = SAVEDIR;
    filename = filename + Serial::toFileName( fileSerial ) + ".FX"; // '.FX' == First X

    assert (filename != SAVEDIR+"ERROR.FX");
    // Error::fatal("Error found in RawTarget::toFile!");

    File file;
    std::fstream &groupFile = file[filename];

    // Go to designated file block:
    groupFile.seekg( fileOffset * BLOCK_FIRST_X );

    // Load Number of inventory items:
    int numitems;
    groupFile.read( (char*) &numitems, sizeof(numitems) );

    int i;

    if (!inventory)
      allocInventory();

    for ( i = 0; i < (numitems > NUM_BULK_ITEMS?NUM_BULK_ITEMS:numitems); ++i ) {
      Target t;
      groupFile >> t;
      
      long groupLoc = groupFile.tellg();
      
      inventory->add(t);
 			t->parent = THIS;
      
      groupFile.seekg(groupLoc);
      }

    if (numitems > NUM_BULK_ITEMS) {
      fstream myFile;

      filename = SAVEDIR;
      filename = filename + THIS->getSerial().toFileName() + ".RTD"; // '.RTD' == Raw Target Data

      myFile.open( filename.toChar(), ios::in | ios::binary );

      assert (!! myFile);
      // Error::fatal(  "RawTarget load error: Error opening file " + filename);

      for ( int count = 0; count < numitems-NUM_BULK_ITEMS; count++ ) {
        Target t;
        myFile >> t;
        inventory->add(t);
			t->parent = THIS;
        }

      myFile.close();
      }
    }

  inFile.seekg(saveLoc);
  }

void RawTarget::toFile( std::iostream &outFile ) {

  outFile.write( (char*) &image, sizeof(image) );
  outFile.write( (char*) &id_mask, sizeof(id_mask) );
  //outFile.write( (char*) &location, sizeof(location) );

  outFile << parent;
  
  bool hasinv = (inventory && inventory->reset() )?true:false;

  outFile.write( (char*) &hasinv, sizeof(hasinv) );

# ifndef NDEBUG
  long  saveLoc = outFile.tellp();
# endif // NDEBUG

  // If this object has an inventory, save it to a separate file:

  String filename;

  if (hasinv) {
  // First x objects are saved to a group file
    using namespace std;

    // Determine the file to access:
    long fileSerial = THIS->getSerial().toLong();
    long fileOffset = fileSerial % BLOCKS_PER_FILE;
    fileSerial -= fileOffset;

    filename = SAVEDIR;
    filename = filename + Serial::toFileName( fileSerial ) + ".FX"; // '.FX' == First X

    assert (filename != SAVEDIR+"ERROR.FX");
    //Error::fatal("Error found in RawTarget::toFile!");

    File file;
    std::fstream &groupFile = file[filename];

    // Go to designated file block:
    groupFile.seekp( fileOffset * BLOCK_FIRST_X );

    // Save Number of inventory items:
    int numitems = inventory->numData();
    groupFile.write( (char*) &numitems, sizeof(numitems) );

    int i;
    for ( i = 0; i < (numitems > NUM_BULK_ITEMS?NUM_BULK_ITEMS:numitems); ++i ) {

      groupFile << inventory->get();
      inventory->next();
      }

  // Remaining objects are saved to a unique file
    if (numitems > NUM_BULK_ITEMS) {
      fstream myFile;

      filename = SAVEDIR;
      filename = filename + THIS->getSerial().toFileName() + ".RTD"; // '.RTD' == Raw Target Data

      myFile.open( filename.toChar(), ios::out | ios::binary );

      assert (!! myFile);
      // Error::fatal(  "RawTarget save error: Error opening file " + filename);

      do {
        myFile << inventory->get();
        } while ( inventory->next() );

      myFile.close();
      }
    }

  assert (saveLoc == outFile.tellp());
  
  }

List<Target> *RawTarget::itemList( void ) {
  return inventory;
  }


void RawTarget::identify( IDmask mask ) {
  id_mask = (IDmask)(id_mask | mask);
  }
  
bool RawTarget::identified( IDmask mask ) {

  if (mask == id_none)
    return id_mask == mask;

  return ((IDmask)(id_mask & mask) == mask);
  }

bool RawTarget::remove( const Target &t ) {

  long i_key;
  if (inventory) i_key = inventory->lock();
    else return false;
  if ( inventory->reset() ) {
    if ( inventory->find(t) ) {
      inventory->remove();

      if (inventory->numData() == 0)
        delptr(inventory);

      if (inventory) inventory->load(i_key);
      return true;
      }

    inventory->load(i_key);
    }
  return false;
  }


Target RawTarget::detach( int ) {

  Target tThis = Target(THIS);

	if (! (parent == NULL )) {
    Target p = parent.toTarget();
  	parent = (TargetValue*)NULL;
		p->remove( tThis );
		}
/*  Target parent = getTarget(tg_parent);
  
  if (!!parent)
    parent->remove( tThis );

  // We're now off the map and in limbo:
  //location = OFF_MAP;
  // And we must make sure all our children know it!:
  setlocation(OFF_MAP);
  
  // Addition to limbo done here so only truly
  // parentless items are in the limbo list:
  Map::limbo.add(THIS); */

  return tThis;

  }


void RawTarget::setParent( TargetValue *t ) {

  detach();

  parent = t;

  }

/*
Target RawTarget::getparent( void ) {
  return parent.toTarget();
  }

*/
Target RawTarget::getTarget( targetType type ) {

  switch( type ) {
    // If we're trying to find the parent or the possessor,
    case tg_parent:
      return parent.toTarget();

    
    case tg_possessor:
    
      if (parent != NULL)
        return parent->getTarget(tg_possessor);
        
      return Target();

    
      /*
      // if we're off-map, look in limbo:
      if ( location == OFF_MAP )
        return Map::limbo.findTarget(type,THIS);
        
      // otherwise ask the map tile:
      return Map::getTile(location)->findTarget(type,THIS);
      */

    case tg_feature:
      //assert( location != OFF_MAP );
      //return Map::getTile(location)->getTarget(tg_feature);
      assert( !!parent );
      return parent->getTarget(tg_feature);
    
  
    default:
      return Target(NULL);
    }
    
  }
/*
Target RawTarget::getowner( void ) {
  return Target(NULL);
  }
*/
/*
Target RawTarget::getdeliverer( void ) {
  return Target(THIS);
  }
*/
/*
Target RawTarget::getpossessor( void ) {

  // Must rewrite getpossessor.
  // Should be like findparent. calls findpossessor().

  // Items in limbo are not possessed at all:
  if ( location == OFF_MAP )
    return Target();
  
  return Map::getTile(location)->findpossessor(THIS);

  //
  //Target parent = getparent();
  //if (parent != NULL) {
  //  return parent->getpossessor();
  //  }
  //
  //// Items in limbo are not possessed at all:
  //return Target(NULL);
  //
  }
*/

/*
Target RawTarget::getfeature( void ) {

  assert( location != OFF_MAP );
  return Map::getTile(location)->getfeature();

  }
*/

Level &RawTarget::getlevel( void ) {

	assert (parent != NULL);
  
  Target p = parent.toTarget();
	return parent->getlevel();
	}
  //assert( location != OFF_MAP );
  //return Map::getTile(location)->getlevel();
  //}


void RawTarget::draw( void ) {
  Point3d location = getlocation();
  if ( location != OFF_MAP )
    Map::getTile(getlocation())->draw();
  }

Point3d RawTarget::getlocation( void ) {
	if ( parent != NULL ) {
    Target p = parent.toTarget();
		return parent->getlocation();
    }

	return OFF_MAP;
	}
  //return location;
  //}

Rectangle RawTarget::getborder( void ) {
  return Rectangle();
  }

bool RawTarget::equals( const Target &rhs ) {

  // Assume equal until proven dissimilar:
  bool retVal = true;

  List<Target> *rinv = rhs->itemList();

  // Are we talking apples and oranges?
  if ( gettype() != rhs->gettype() )
    return false;

  // Are either inventory lists null?
  if ( ! inventory || !inventory->reset() || ! rinv || !rinv->reset() ) {
    // Yes, but are both null?
    if ( inventory != rinv )
      // No: sorry not equal!
      return false;
    else
      // Yes: both null, we're okay!
      return true;
    }

  // Okay, both inventories exist.


  else {

    // Check inventories for identical-ness.
    if ( ( inventory->reset() == false )&&( rinv->reset() == false ) )
      return true;
    if ( ( inventory->reset() == false )||( rinv->reset() == false ) )
      return false;

    bool cont;

    do {

      if ( ! inventory->get()->equals( rinv->get() ) ) {
        retVal = false;
        }

      cont = inventory->next();
      if ( cont != rinv->next() ) {
        retVal = false;
        }

      } while ( cont && retVal );
    }

  // See if we know the same info about each:
  if ( id_mask != rhs->id_mask ) retVal = false;

  return retVal;



  }


bool RawTarget::operator < ( RawTarget &rt ) {

  return (compare(rt)==k_smaller)? true: false;

  }

bool RawTarget::operator > ( RawTarget &rt ) {

  return (compare(rt)==k_larger)? true: false;

  }

compval RawTarget::compare( RawTarget &rt ) {

  long me =  getVal(attrib_sort_category),
       you = rt.getVal(attrib_sort_category);

  if ( me < you ) return k_smaller;
  if ( me > you ) return k_larger;

  return k_same;


  }

bool RawTarget::isCapable( Action a ) {
  return capable(a,Target());
  }

bool RawTarget::doPerform( Action a ) {
  return perform(a,Target());
  }

bool RawTarget::capable( Action a, const Target &t ) {

  switch( a ) {
    case aContain: {
      assert ( t != Target(THIS) && !!t);
      return true;
      }

    case aRelease:
      return true;
    default:
      return false;
    }
  }

bool RawTarget::perform( Action a, const Target &t ) {

  switch( a ) {
    case aContain:
      if ( t == Target(THIS) ) return false;
      add(t);
      return true;
      
    case aRestack: {

      //assert(0);
      Target p = getTarget(tg_parent);
      assert (!!parent);

      Target tTHIS = THIS;
      detach();
      p->perform(aContain,tTHIS);
      return true;
      }

    case aEnterLev:
    case aLeaveLev:
      // Notify children:
      if ( inventory && inventory->reset() )
      do {
        long i_key = inventory->lock();
        inventory->get()->perform(a,t);
        inventory->load(i_key);
        } while (inventory->next());
        
      return true;


    case aRelease:
      return remove(t);
    default:
      return false;
    }

  }


//void RawTarget::light( const Orthog & ) {
//  }
void RawTarget::view( int, int ) {
  }

//void RawTarget::unlight( void ) {
//  }
void RawTarget::unview( void ) {
  }
  

long RawTarget::getVal( StatType s ) {

  switch (s) {
    case attrib_sink_weight:
      // If no sink weight has been defined, assume that
      // the item sinks:
      return getVal(attrib_weight);

    case attrib_on_level :{
      if (!parent.inMem() || parent == NULL)
        return false;

      Target p = parent.toTarget();
      return parent->getVal(s);
      }
      //return location != OFF_MAP;

    case attrib_empty :
      if (inventory && inventory->reset()) return false;
      return true;
      
    case attrib_empty_nodoor:
    case attrib_non_wall:
      if (inventory && inventory->reset())
      do {

        if (!inventory->get()->getVal(s)) return false;
      
        } while(inventory->next());

      return true;

    case attrib_has_door:
      if (inventory && inventory->reset())
      do {

        if (inventory->get()->getVal(s)) return true;
      
        } while(inventory->next());

      return false;

    case attrib_vis_mon :

      if (!getVal(attrib_visible)) return false;

      if ( isCapable(aSeeInv) ) {

        if (!inventory || !inventory->reset())
          return false;

        do {

          if ( inventory->get()->getVal(attrib_vis_mon) )
            return true;

          } while ( inventory->next() );

        }
      return false;

    case attrib_visible :
      return isCapable(aView);

    default :
      return 0;
    }

  }

long RawTarget::getSkill( Skill, bool ) {
  assert(0);
  return 0;
  }

void RawTarget::modSkill( Skill, int ) {
  assert(0);
  }


Result RawTarget::test( Skill, int, const Target& ) {
  return result_fail;
  }

void RawTarget::takeDamage( const Attack & ) {
  }

Attack RawTarget::getAttack( void ) {
  return Attack( CombatValues(),THIS,THIS );
  }

RawTarget::Link::Link( TargetValue *tvp )
  :type(isPointer) {

  value.pointer = NULL;
  *this = tvp;
  }

RawTarget::Link::Link( long l )
  :type(isPointer) {

  value.pointer = NULL;
  *this = l;
  }

RawTarget::Link::~Link() {

# ifdef SOFT_LINK
  if ( type == isPointer && value.pointer != NULL )
    value.pointer->decMemSoftCount();
# endif // SOFT_LINK
    
  }
  
Target RawTarget::Link::toTarget( void ) const {

  if ( type == isSerial ) {
    *((RawTarget::Link*)this) = Catalogue::get( value.serial );

    return Target( value.pointer );
    }
  else {
    return Target( value.pointer );
    }
  }

bool RawTarget::Link::operator ==( TargetValue*tvp ) const {

  if ( type == isPointer )
    return (value.pointer == tvp);

  else
    return toTarget() == Target(tvp);

  }

bool RawTarget::Link::operator !=( TargetValue*tvp ) const {
  return (! (*this == tvp )  );
  }

bool RawTarget::Link::operator !( void ) const {
  return *this == (TargetValue*)NULL;
  }

# ifdef SOFT_LINK
  void RawTarget::Link::unlink( void ) {

    if ( type == isPointer && value.pointer != NULL )
      value.pointer->decMemSoftCount();
    else if ( type == isSerial ) {
      Target t = Catalogue::get( value.serial );
      Catalogue::get( value.serial )->decSaveSoftCount();
      }

    }
# endif // SOFT_LINK

RawTarget::Link &RawTarget::Link::operator =( TargetValue *tvp ) {

# ifdef SOFT_LINK
  if (tvp) tvp->incMemSoftCount();
  unlink();
# endif // SOFT_LINK
  
  value.pointer = tvp;
  type = isPointer;
  return *this;
  }


#ifdef NOT_DEFINED_EVER_EVER_EVER
RawTarget::Link &RawTarget::Link::operator =( long l ) {

  if (l == 0)
    return ( *this = (TargetValue *)l );
  else {

#   ifdef SOFT_LINK
    Target newLink = Catalogue::get(l);
    newLink.value->incSaveSoftCount();
    unlink();
#   endif // SOFT_LINK
    
    value.serial = l;
    type = isSerial;
    }

  return *this;
  }
#endif

bool RawTarget::Link::inMem( void ) {
  return (type == isPointer);
  }

RawTarget* RawTarget::Link::operator->() {

  return toTarget().raw();

  }


std::iostream &operator << ( std::iostream &outFile, RawTarget::Link &link ) {

  long s;

  if ( link.type == RawTarget::Link::isPointer && link.value.pointer == NULL )
    s = 0;
  else if ( link.type == RawTarget::Link::isPointer ) {
    s = link.value.pointer->getSerial().toLong();
#   ifdef SOFT_LINK
    link.value.pointer->decMemSoftCount();
    link.value.pointer->incSaveSoftCount();
#   endif // SOFT_LINK
    }
  else
    s = link.value.serial;

  outFile.write( (char*) &s, sizeof(s) );

  long pos = outFile.tellp();

  if (s != 0) {
    link.type = RawTarget::Link::isSerial;
    link.value.serial = s;
    }
  else
    link = (TargetValue*)s;

  outFile.seekp(pos);

  return outFile;
  }

std::iostream &operator >> ( std::iostream &inFile, RawTarget::Link &link ) {

  long s;
  inFile.read( (char*) &s, sizeof(s) );

  long pos = inFile.tellg();

  if (s != 0) {
    link.type = RawTarget::Link::isSerial;
    link.value.serial = s;
    }
  else
    link = (TargetValue*)s;

  inFile.seekg(pos);

  return inFile;

  }


void RawTarget::allocInventory( void ) {
  if (!inventory)
    inventory = new Sorted<Target>;
  }

Target RawTarget::Force( String force_str ) {

  int index = 0;
  do {
    doPerform(aPolymorph);
    } while ( Grammar::plural(THIS).upcase() != force_str.upcase()
           && ++index < NUM_ABORT*10 );

  assert (index < NUM_ABORT*10);

  return Target(THIS);

  }

String RawTarget::menustring( char len ) {

  return Grammar::plural( name(n_menu),
                          quantity(),
                          getVal(attrib_abstract),
                          getVal(attrib_pair) ).abbrev(len);

  }


bool RawTarget::getStruct( structType type, void*, size_t ) {

  switch (type) {

    default:
      return false;
    }

  }

void RawTarget::gainExp( long ) {
// Gain experience points:
  // Do nothing!
  
  }


#endif // RAWTARGT_CPP

