// [ammo.cpp]
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
#if !defined ( AMMO_CPP )
#define AMMO_CPP

#include <cstring> // Sz. Rutkowski 11.02.2017

#include "ammo.hpp"
#include "random.hpp"
#include "grammar.hpp"
#include "actor.hpp"
#include "cloud.hpp"
#include "level.hpp"
#include "path.hpp"
#include "message.hpp"

ammoStats::ammoStats( const char *n, AmmoType at, int c, long p, int w, char ns, char acc, AmmoEffect eff )
: name(n), type(at), color(c), price(p), weight(w), numslugs(ns), accuracy(acc), effect(eff) {}

ammoStats AmmoStorage [] = {

	//__       Name                 type                         price        wgt slug acc

	ammoStats("#light pistol bullet~",   bul_lpistol,  cBlue,  CentP100( 44),  1,  1,  0, ae_none ),
	ammoStats("#heavy pistol bullet~",   bul_hpistol,  cLBlue, CentP100( 44),  1,  1,  0, ae_none ),
	ammoStats("#silver bullet~",         bul_lpistol,  cWhite, Cents( 44),     1,  1,  0, ae_none ),

	ammoStats("#rifle bullet~",    bul_rifle,   cGreen, CentP100( 70),  1,  1,  0, ae_none ),

	ammoStats("#shotgun shell~",   bul_shotgun, cBrown, CentP100(112),  1,  6, -8, ae_none ),

	ammoStats("#light harpoon~", bul_harpoon, cGrey,   Cents(20),  2,  1, 0, ae_none ),

	ammoStats("#needle slug~",   bul_needle, cRed,   CentP100(140),  1,  1, +1, ae_none ),
	ammoStats("#barb slug~",     bul_needle, cLRed,  CentP100(280),  1,  1,  0, ae_none ),

	ammoStats("#grenade~",         bul_grenade, cYellow,   Dollars(2),  4,  1, -8, ae_explode ),
	ammoStats("#smoke bomb~",      bul_grenade, cLGrey,     Cents(90),  4,  1, -8, ae_smoke ),
	ammoStats("#gas grenade~",     bul_grenade, cOrange,   Cents(120),  4,  1, -8, ae_gas ),

//	ammoStats("#heavy rivet~",     bul_rivet,   2,  1,  0, ae_none ),
//	ammoStats("#light rivet~",     bul_rivet,   1,  1,  0, ae_none ),

//	ammoStats("#steam pack~",      bul_steam,   2,  1,  0, ae_none ),

	ammoStats("#arrow~",           bul_arrow,   cIndigo,CentP100( 22),  1,  1,  0, ae_none )


};

ammoStats *ammoStats::AmmoStats = AmmoStorage;
int ammoStats::numAmmo = 12;


targetType Ammo::gettype( void ) const {
	return tg_ammo;
	}

bool Ammo::istype( targetType t ) const {

	if ( t == tg_ammo )
		return true;
	else return Item::istype(t);

	}

bool Ammo::capable( Action a, const Target &t ) {

	switch( a ) {
		default:
			return Item::capable(a, t);
		}
	}

bool Ammo::perform( Action a, const Target &t ) {

	switch( a ) {
    case aPolymorph:
      form = Random::randint( ammoStats::numAmmo );
      image.val.color = ammoStats::AmmoStats[form].color;
      return Item::perform(a,t);
		case aFire: {

      // We now just mark the bullet as fired, to ensure it disappears:
			fired = true;
      // This code should no longer be called, thus:
      /*
			Point3d me=getlocation(), target = t->getlocation();
			int acc = ammoStats::AmmoStats[form].accuracy;
			int scatter =  (acc * max( abs((me-target).getx()), abs((me-target).gety()) ) ) / 10;
			scatter = scatter / 2-scatter;

			// Multiple attacks:
			int numslugs = ammoStats::AmmoStats[form].numslugs;
			if (numslugs > 1)
			for (int slugs = 1; slugs < numslugs; slugs++) {
				Target slug = clone();
				target = t->getlocation() + Point3d( Random::randint(scatter),Random::randint(scatter), 0 );
        Target parent = getTarget(tg_parent);
        assert(!!parent);
				parent->perform(aContain,slug);
        FlightPath::Option flight_opt;
        flight_opt.object = slug;
        flight_opt.start = me;
        flight_opt.delta = Point3d( target-me ).toPoint();
  			FlightPath::create( flight_opt );
				}

      // Make sure that at least this bullet hits the target:
			target = t->getlocation();// + Point( Random::randint(scatter),Random::randint(scatter) );
			Target tMe = Target(THIS);
      FlightPath::Option flight_opt;
      flight_opt.object = tMe;
      flight_opt.start = me;
      flight_opt.delta = Point3d( target-me ).toPoint();
 			FlightPath::create( flight_opt );
      */
			return true;
		  }
		case aRestack:
			if (fired) {
        Cloud::Option cloud_opt;
        Target my_parent = getTarget(tg_parent);

        // Sometimes this can fail.  No memory leak caused,
        // though I should find the cause when I can.
        //assert(!!my_parent);
        
        if (!my_parent) {

          // May have negative effect on grenade use.

          // In any case, fail gracefully & don't tell the player:
          //Message::add("Ammo::perform(aRestack) failure.");
          
          return false;
          }
          
        switch (ammoStats::AmmoStats[form].effect) {
          case ae_smoke:
            if (my_parent != NULL) {
              cloud_opt.material = m_smoke;
              my_parent->perform( aContain, Cloud::create(cloud_opt) );
              }
            break;
          case ae_gas:
            if (my_parent != NULL) {
              cloud_opt.material = m_poison;
              my_parent->perform( aContain, Cloud::create(cloud_opt) );
              }
            break;
          case ae_explode: {
          // Grenades must soon be rewritten
          // to acommodate for the new global positioning.
          
          /*
            Point loc = getlocation().toPoint(), cursor = Screen::cursorat();
            // All grenade rounds have an explosion radius of 2
            int radius = 2, gx = loc.getx(), gy = loc.gety(),
                x, y;
            bool **los;
            los = new bool *[radius*2+1];
            for (int i = 0; i < radius*2+1; ++i)
              los[i] = new bool[radius*2+1];
//                xa = max(x-radius,0), xb = min(x+radius,(int)(map_width-1)),
//                ya = max(y-radius,0), yb = min(y+radius,(int)(map_height-1));
            Level &lev = getlevel();
            Rectangle lev_border = lev.getBorder();
            Target deliverer = getTarget(tg_controller);
       			Attack dam( 6, DiceRoll(4,3), CombatValues(dam_burn,+10), deliverer, THIS, "grenaded" );
            Screen::Buffer restore;
            restore.setSize( Rectangle(max(gx-radius+1,lev_border.getxa()+1)+Screen::mapxoff,
                                       max(gy-radius+1,lev_border.getya()+1)+Screen::mapyoff,
                                       min(gx+radius+1,lev_border.getxb()+1)+Screen::mapxoff,
                                       min(gy+radius+1,lev_border.getyb()+1)+Screen::mapyoff) );
            
            for (y = -radius; y <= radius; ++y)
              for (x = -radius; x <= radius; ++x) {
                los[x+radius][y+radius] = false;
                if ( lev_border.contains( Point(gx+x,gy+y) ) ) {
                  Path p;
          				p.set( loc, loc+Point(x,y) );
          				p.calculate();

          				if ( p.hasLOS(lev) )
                    los[x+radius][y+radius] = true;

                  }
                }

            Screen::hidecursor();
            for (y = -radius; y <= radius; ++y)
              for (x = -radius; x <= radius; ++x)
                if (los[x+radius][y+radius] && lev.getTile( Point(gx+x,gy+y) )->getVal(attrib_visible))
                  LEVDRAW(gx+x,gy+y, Image(cRed,'*'));
            Screen::locate(cursor);
            Screen::showcursor();

            Screen::nextFrame();
            Screen::hidecursor();
            for (y = -radius; y <= radius; ++y)
              for (x = -radius; x <= radius; ++x)
                if (los[x+radius][y+radius] && lev.getTile( Point(gx+x,gy+y) )->getVal(attrib_visible))
                  LEVDRAW(gx+x,gy+y, Image(cOrange,'*'));
            Screen::locate(cursor);
            Screen::showcursor();

            Screen::nextFrame();
            Screen::hidecursor();
            for (y = -radius; y <= radius; ++y)
              for (x = -radius; x <= radius; ++x)
                if (los[x+radius][y+radius] && lev.getTile( Point(gx+x,gy+y) )->getVal(attrib_visible))
                  LEVDRAW(gx+x,gy+y, Image(cYellow,'*'));
            Screen::locate(cursor);
            Screen::showcursor();

            Screen::nextFrame();
            Screen::hidecursor();
            restore.moveToScreen();
            for (y = -radius; y <= radius; ++y)
              for (x = -radius; x <= radius; ++x)
                if (los[x+radius][y+radius])
                  lev.getTile( Point(gx+x,gy+y) )->takeDamage( dam );
            Screen::locate(cursor);
            Screen::showcursor();

            for (int i = 0; i < radius*2+1; ++i)
              delarr(los[i]);
            delarr(los);
            */
            break;
            }

          default:
            break;
          }
				detach();
        }
			else
				return Item::perform(a,t);
			return true;

    case aLand:
      if ( ammoStats::AmmoStats[form].type == bul_grenade )
        fired = true;
      return Item::perform(a,t);

		case aAttack: {
    
      // If this slug was fired, make sure to give 'shooty' messages:
      String subject, object,
             hit_str = "hit~";
      Target shooter;
      // If the bullet was fired you want the, "You shoot the frog."
      // message; otherwise you want a "The bullet hits the frog." message.
      if (fired)
        shooter = getTarget(tg_controller);
      else
        shooter = THIS;
        
   		subject = Grammar::Subject(shooter,Grammar::det_definite);
  		object = Grammar::Object(t,Grammar::det_definite);
      
      if (fired)
        hit_str = "shoot~";


      bool t_was_alive = t->getVal(attrib_alive);


			t->takeDamage( getAttack() );

      // If we have killed the target:
      if ( t->getVal(attrib_alive) != t_was_alive )
        if (t->getVal(attrib_flesh_blood))
          hit_str = "kill~";
        else
          hit_str = "destroy~";

      if ( RawTarget::getVal(attrib_visible) || t->getVal(attrib_visible) )
  			Message::add( subject + " "+Grammar::plural(hit_str,2-shooter->getVal(attrib_plural),true,false)+" "+ object + "." );
      
			return true;
      }

		default:
			return Item::perform(a, t);
		}

	}

long Ammo::getVal( StatType s ) {

	switch( s ) {
    case attrib_price:
      return ammoStats::AmmoStats[form].price;
      
    case attrib_sort_category:
      return Item::getVal(s) + form;
      
    case attrib_weight:
    case attrib_sink_weight:
      return ammoStats::AmmoStats[form].weight+Item::getVal(attrib_weight);
      
		case attrib_ammotype:
			return ammoStats::AmmoStats[form].type;

		case attrib_form:
			return form;

    case attrib_num_slugs:
			return ammoStats::AmmoStats[form].numslugs;
    

		default:
			return Item::getVal(s);
		}

	}

Ammo::Ammo( const Option *o )
: Item( o ), fired(false) {

	if ( o->type == bul_none )
    doPerform(aPolymorph);
	else for ( int i = 0; i < NUM_ABORT; ++i ) {
    doPerform(aPolymorph);
		if ( ammoStats::AmmoStats[form].type == o->type) break;
		}
	}

Ammo::Ammo( Ammo &rhs )
: Item(rhs) {

	form = rhs.form;
	fired = rhs.fired;
	}

Ammo::Ammo( std::iostream &inFile, TargetValue *TVP )
// Read Item base information:
: Item (inFile, TVP) {

	// Read char form
	inFile.read( (char*) &form, sizeof(form) );

	// Read bool fired
	inFile.read( (char*) &fired, sizeof(fired) );


	}

void Ammo::toFile( std::iostream &outFile ) {

	// Write Item base information:
	Item::toFile( outFile );

	// Write char form
	outFile.write( (char*) &form, sizeof(form) );

	// Write bool fired
	outFile.write( (char*) &fired, sizeof(fired) );

	}

Target Ammo::clone( void ) {

	Ammo *newcopy = new Ammo( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

bool Ammo::equals( const Target &rhs ) {

	bool retVal = Item::equals( rhs );

	if (retVal) {
		// Good so far - same inventories and both of same type.
		if ( form != ( (Ammo*) rhs.raw() )->form ) retVal = false;
		if ( fired != ( (Ammo*) rhs.raw() )->fired ) retVal = false;
		}

	return retVal;

	}

Attack Ammo::getAttack( void ) {

  if (fired) {
    CombatValues c_val;
    GET_STRUCT(s_offence_cv, c_val);
		return Attack( c_val, getTarget(tg_controller), THIS, "shot" );
    }
	else
		return Item::getAttack();
	}


String Ammo::name( NameOption n_opt ) {
  switch (n_opt) {
    default:
    	if (!fired)
    		return String( ammoStats::AmmoStats[form].name ) + Item::name(n_opt);
    	else switch( ammoStats::AmmoStats[form].type ) {
    		case bul_shotgun:
    			return "#shot~" + Item::name(n_opt);
    		case bul_arrow:
    			return "#arrow~" + Item::name(n_opt);
    		case bul_harpoon:
    			return "#harpoon~" + Item::name(n_opt);
    		default:
    			return "#bullet~" + Item::name(n_opt);
    		}
    }
  }
  
bool Ammo::getStruct( structType type, void *buffer, size_t size ) {

  switch (type) {
    case s_shoot_cv: {
      // Get the ammo's offensive values:
    
      // Make sure that there's the exact amount of space:
      assert( size == sizeof(CombatValues) );
      CombatValues cv;

      // Fill cv with combat values

      // Make sure that the + penetration is
      // over-compensated with - to hit:
      cv.to_hit -= 8;

      // Penetration:
      // Todo: levels of penetration
      cv.penetration[pen_pierce] = 8;

      // Damage:
      // Todo: Damage types:
      // Apply purity damage for silver/wooden weapons:
      cv.damage[dam_physical] = 100;

      // Copy our CombatValues to the buffer:
      memcpy( buffer, (const void*) &cv, size );
      
      return true;
      }

    default:
      return false;
    }

  }


#endif // AMMO_CPP

