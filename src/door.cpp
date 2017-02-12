// [door.cpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( DOOR_CPP )
#define DOOR_CPP

#include "door.hpp"
#include "message.hpp"
#include "random.hpp"
#include "grammar.hpp"
#include "player.hpp"
#include "level.hpp"

targetType Door::gettype( void ) const {
	return tg_door;
	}

bool Door::istype( targetType t ) const {

	if ( t == tg_door )
		return true;
	else return Item::istype(t);

	}

bool Door::capable( Action a, const Target &t ) {

	switch( a ) {
		case aBlock:
			// As of yet, Doors block the passage of *everything* when shut
			// Later to check for non-corporeal item types.
			return (open)?false:true;

		case aLeave:
			// As of yet, Doors are fixed to the ground:
			return false;

		case aBeOpened:
			return (!open && !hidden)?true:false;

		case aBeClosed:
			// Todo: check for items in doorway
			return (open && !hidden)?true:false;

		case aSpMessage:
			return true;

		case aFind:
			return hidden;

		default:
			return Item::capable(a, t);
		}
	}

bool Door::perform( Action a, const Target &t ) {

	switch( a ) {
    case aPolymorph: {
      gate = Random::randbool();
      int m_num = Random::randint(16);

      switch(m_num) {
        case  0: material = m_ice; break;
        case  1: material = m_brick; break;
        case  2: material = m_stone; break;
        case  3: material = m_bluestone; break;
        case  4: material = m_wood; break;
        case  5: material = m_straw; break;
        case  6: material = m_paper; break;
        case  7: material = m_rubber; break;
        case  8: material = m_bronze; break;
        case  9: material = m_tin; break;
        case 10: material = m_iron; break;
        case 11: material = m_steel; break;
        case 12: material = m_gold; break;
        case 13: material = m_emerald; break;
        case 14: material = m_ruby; break;
        case 15: material = m_glass; break;

        }
      image.val.color = Material::data[material].color;

      if (getVal( attrib_on_level ) ) {
        if (open)
          doPerform(aBeOpened);
        else
          doPerform(aBeClosed);
        }
      
      return true;
      }
  
		case aBeOpened: {
			hidden = false;
			open = true;
			image.val.appearance = '/';
			draw();
      Point3d loc = getlocation();
      //Level &lev = getlevel();
      Map::calcOpacity(loc);
      if (RawTarget::getVal(attrib_visible)) {
        Screen::hidecursor();
        //lev.calcLights();
        //Map::calcLights();
        Player::get().perform(aLOS,Target());
        Screen::showcursor();
        }
			return true;
      }

		case aBeClosed: {
			hidden = false;
			open = false;
			image.val.appearance = '+';
			draw();
      Point3d loc = getlocation();
      //Level &lev = getlevel();
      Map::calcOpacity(loc);
      if (RawTarget::getVal(attrib_visible)) {
        Screen::hidecursor();
        //lev.calcLights();
        //Map::calcLights();
        Player::get().perform(aLOS,Target());
        Screen::showcursor();
        }
			return true;
      }

		case aFind:
			hidden = false;
			image.val.appearance = (open)?'/':'+';
			draw();
			return true;

		case aSpMessage:
			// Special message:
			if (!open) Message::add( "You feel claustrophobic!" );
			return true;

		case aBlock:
      // Make sure that we don't open the door unless it's
      // possible to open the door:
      if (isCapable(aBlock))
  			return t->perform( aOpen, Target(THIS) );
      return false;

		default:
			return Item::perform(a, t);
		}

	}

void Door::takeDamage( const Attack &a ) {

  Target retain = Target(THIS);
	// Todo:  drop the door from hinges for
	//        certain attacks, destroy for others.

	if ( a.c_values.rollDamage() > ( (open)?10:8 ) ) {

		if ( Random::randint(2) ) {
      if (material == m_wood)
  			Message::add( Grammar::Subject(THIS,Grammar::det_definite)+" shatters!");
      else
  			Message::add( Grammar::Subject(THIS,Grammar::det_definite)+" breaks off its hinges!");

      Point3d loc = getlocation();
      //Level &lev = getlevel();
      detach();
      
      Map::calcOpacity(loc);

      if (RawTarget::getVal(attrib_visible)) Player::get().perform(aLOS,Target());
			}
		else {

			if (open) {
				perform( aClose, Target(a.instrument) );
				Message::add( Grammar::Subject(THIS,Grammar::det_definite)+" slams shut!");
				}
			else {
				perform( aOpen, Target(a.instrument) );
				Message::add( Grammar::Subject(THIS,Grammar::det_definite)+" breaks open!");
				}

			}

		}
	else Message::add("WHAM!");

	}

long Door::getVal( StatType s ) {
	switch(s) {
		case DC_spot :
			return 10;
		case attrib_opaque:
			return (open ? 1 : ( gate ? 5 : 100 ));

    case attrib_has_door:
      return true;
      
    case attrib_empty_nodoor:
    case attrib_non_wall:
      if (hidden) return false;
      return true;
		default:
			return Item::getVal(s);
		}
	}


Action Door::autoAction( void ) {
	if (!open && !hidden) return aBlock;

	return Item::autoAction();
	}


Door::Door( const Option *o )
: Item(o) {

	material = o->material;
	open = o->open;
  if (o->hidden) open = false;
	hidden = o->hidden;
  gate = o->gate;
	if (hidden) image.val.appearance = '#';
  image.val.color = Material::data[material].color;
	}

Door::Door( Door &rhs )
: Item(rhs) {

	material = rhs.material;
	open = rhs.open;
	hidden = rhs.hidden;
	gate = rhs.gate;

	}

Door::Door( std::iostream &inFile, TargetValue *TVP )
// Read Item base information:
: Item (inFile, TVP) {

	// Read materialClass material
	inFile.read( (char*) &material, sizeof(material) );

	// Read bool open
	inFile.read( (char*) &open, sizeof(open) );

	// Read bool hidden
	inFile.read( (char*) &hidden, sizeof(hidden) );

	// Read bool gate
	inFile.read( (char*) &gate, sizeof(gate) );

	}

void Door::toFile( std::iostream &outFile ) {

	// Write Item base information:
	Item::toFile( outFile );

	// Write materialClass material
	outFile.write( (char*) &material, sizeof(material) );

	// Write bool open
	outFile.write( (char*) &open, sizeof(open) );

	// Write bool hidden
	outFile.write( (char*) &hidden, sizeof(hidden) );

	// Write bool gate
	outFile.write( (char*) &gate, sizeof(gate) );

	}

Target Door::clone( void ) {

	Door *newcopy = new Door( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

bool Door::equals( const Target &rhs ) {

	bool retVal = Item::equals( rhs );

	if (retVal) {
		// Good so far - same inventories and both of same type.
		if ( material != ( (Door*) rhs.raw() )->material ) retVal = false;
		if ( open     != ( (Door*) rhs.raw() )->open )     retVal = false;
		if ( hidden   != ( (Door*) rhs.raw() )->hidden )   retVal = false;
		if ( gate     != ( (Door*) rhs.raw() )->gate )     retVal = false;
		}

	return retVal;

	}


String Door::name( NameOption n_opt ) {

  String material_str;
  if (Material::data[ material ].adj)
    material_str = Material::data[ material ].adj;
  else
    material_str = Material::data[ material ].name;
 
  switch (n_opt) {

    case n_short:
    	return String( "#") + ( hidden?material_str.toChar():"" ) +  ( hidden?" wall~":(gate?"gate~":"door~") )
       + Item::name(n_opt);

    default:
    	return String( "#") + material_str + " " + ( hidden?"wall~":(gate?"gate~":"door~") )
       + Item::name(n_opt);
    }
  }



#endif // DOOR_CPP
