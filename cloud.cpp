// [cloud.cpp]
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
#if !defined ( CLOUD_CPP )
#define CLOUD_CPP

#include "cloud.hpp"

#include "message.hpp"
#include "random.hpp"
#include "grammar.hpp"
#include "player.hpp"
#include "level.hpp"
#include "event.hpp"
#include "timeline.hpp"

targetType Cloud::gettype( void ) const {
	return tg_cloud;
	}

bool Cloud::istype( targetType t ) const {

	if ( t == tg_cloud )
		return true;
	else return Item::istype(t);

	}

bool Cloud::capable( Action a, const Target &t ) {

	switch( a ) {

		case aLeave:
			// Clouds refuse requests to leave the ground:
			return false;

    case aBeKicked:
    case aBeAttacked:
      return false;

		case aSpMessage:
			return true;

		default:
			return Item::capable(a, t);
		}
	}

bool Cloud::perform( Action a, const Target &t ) {

	switch( a ) {

		case aSpMessage:
			// No Special message
			return true;

    case aEnterLev: {
     	ActionEvent::create( Target(THIS), aAct, GRANULARITY / 2 );

      //Point loc = getlocation().toPoint();
      Level &lev = getlevel();
      lev.addCloud( Target(THIS) );
      //lev.calcOpacity(loc);
      Point3d location = getlocation();
      Map::calcOpacity(location);
      
      if (RawTarget::getVal(attrib_visible)) {
        Screen::hidecursor();
        //lev.calcLights();
        //Map::calcLights();
        Player::get().perform(aLOS,Target());
        Screen::showcursor();
        }
      return Item::perform(a,t);
      }

    case aLeaveLev: {
      //Point loc = getlocation().toPoint();
      Level &lev = getlevel();
      lev.removeCloud( Target(THIS) );
      //lev.calcOpacity(loc);
      Point3d location = getlocation();
      Map::calcOpacity(location);
      if (RawTarget::getVal(attrib_visible)) {
        Screen::hidecursor();
        //lev.calcLights();
        //Map::calcLights();
        Player::get().perform(aLOS,Target());
        Screen::showcursor();
        }
      return Item::perform(a,t);
      }

    case aSleep:
      detach();
      ActionEvent::remove( Target(THIS), aAll );
      return true;

    case aAct: {
      // Each 'cloud' turn there is a chance of dispersing:
      int x, y;

      Point3d my_loc = getlocation();
      Level &lev = getlevel();
      do {
        x = Random::randint(3)-1;
        y = Random::randint(3)-1;
        } while (! lev.getBorder().contains( my_loc+Point3d(x,y) ));
        
      Target dest = lev.getTile( my_loc+Point3d(x,y) );

      if ( (x == 0 && y == 0) || !dest->capable(aContain,THIS) ) {
       	ActionEvent::create( Target(THIS), aAct, GRANULARITY / 2 );
        return true;
        }

      Screen::hidecursor();
      
      Target my_clone = clone();
      
      if (opacity <= 0) {
        detach();
        Screen::showcursor();
        return true;
        }
      
      dest->perform( aContain, my_clone );
      doPerform(aEnterLev);
      Screen::showcursor();
      
      return true;
      }

		default:
			return Item::perform(a, t);
		}

	}

void Cloud::takeDamage( const Attack & ) {

  // Clouds are currently completely unaffected by all attacks.

	}

long Cloud::getVal( StatType s ) {
	switch(s) {
		case attrib_opaque:
			return opacity?(opacity/2+50):0;
      
    case attrib_mask_color:
      return true;

    case attrib_empty_nodoor:
    case attrib_non_wall:
      return true;
		default:
			return Item::getVal(s);
		}
	}


Cloud::Cloud( const Option *o )
: Item( o ) {

	material = o->material;
  image.val.color = Material::data[material].color;
  
  opacity = 100;

	}

Cloud::Cloud( Cloud &rhs )
: Item(rhs) {

	material = rhs.material;
  opacity = (rhs.opacity/=2);

	}

Cloud::Cloud( std::iostream &inFile, TargetValue *TVP )
// Read Item base information:
: Item (inFile, TVP) {

	// Read materialClass material
	inFile.read( (char*) &material, sizeof(material) );

	// Read int opacity
	inFile.read( (char*) &opacity, sizeof(opacity) );

	}

void Cloud::toFile( std::iostream &outFile ) {

	// Write Item base information:
	Item::toFile( outFile );

	// Write materialClass material
	outFile.write( (char*) &material, sizeof(material) );

	// Write int opacity
	outFile.write( (char*) &opacity, sizeof(opacity) );

	}

Target Cloud::clone( void ) {

	Cloud *newcopy = new Cloud( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

bool Cloud::equals( const Target &rhs ) {

	bool retVal = Item::equals( rhs );

	if (retVal) {
		// Good so far - same inventories and both of same type.
		if ( material != ( (Cloud*) rhs.raw() )->material ) retVal = false;
		if ( opacity  != ( (Cloud*) rhs.raw() )->opacity  ) retVal = false;
		}

    // ***** Clouds are not able to stack yet: ****************
    retVal = false;
	return retVal;

	}


String Cloud::name( NameOption n_opt ) {
  switch (n_opt) {
    default:
      const char *m_name = Material::data[ material ].adj;
      if (!m_name) m_name = Material::data[ material ].name;
    	return String( "#cloud~ of " ) + m_name + (Material::data[ material ].state==cloud?"":" gas")
              + Item::name(n_opt);
    }
    
	}




#endif // CLOUD_CPP
