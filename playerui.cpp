// [playerui.cpp]
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
#if !defined ( PLAYERUI_CPP )
#define PLAYERUI_CPP

#include "playerui.hpp"
#include "event.hpp"
#include "enum.hpp"
#include "message.hpp"
#include "target.hpp"
#include "level.hpp"
#include "dialog.hpp"
#include "global.hpp"
#include "player.hpp"
#include "random.hpp"
#include "grammar.hpp"
#include "path.hpp"
#include "temp.hpp"
#include "misc.hpp"
#include "fluid.hpp"
#include "catalog.hpp"
#include "skill.hpp"
#include "timeline.hpp"
#include "input.hpp"

#include "menu.hpp"
#include "visuals.hpp"
#include "input.hpp"

//-[ class PlayerUI ]----------

// Todo: appropriate messages for other OSes
#define STR_OS_KILL "You wait for the blue screen of death."

PlayerUI::PlayerUI( const Option *o )
: Actor( o ), turn_time(0) {

	assert (object->istype(tg_player));
  // Error::fatal("PlayerUI can be the brain of Player objects only!");
	}

void PlayerUI::describe( const Target &target, bool ignorePlayer ) {

	bool specialMessage = false;
	bool pickup = false;

	if ( target->isCapable(aSpMessage)  ) {
		specialMessage = true;
		target->doPerform(aSpMessage);
		}

	List<Target> *items = target->itemList();

	Sorted<Target> interesting;

	if ( items && items->reset() )

	do {
		Target get = items->get();

		if ( !get->istype(tg_player) && get->isCapable(aLeave) )
			pickup = true;


		if ( get->isCapable(aSpMessage)  ) {
			specialMessage = true;
			get->doPerform(aSpMessage);
			}

		else if  ( ! get->istype( tg_player ) || !ignorePlayer  )
			interesting.add(get);

		} while ( items->next() );

	int num = interesting.numData();
	interesting.reset();

	Grammar::clr();
  if (num > 0)
    Message::add( String("You")+(specialMessage?" also":"")+" see here " +describeList(interesting)+".");

	if ( pickup && global::isNew.pickup ) {
		Message::add("<?Press '\\" + Keyboard::keytostr(keyMap[kPickUp].dkey) + "' to pick up items.>");
		global::isNew.pickup = false;
		}

	}


Dir PlayerUI::whereTargets( Action a ) {
	Dir retVal;

	int x, y;
	Point3d playerLoc = object->getlocation();

	for (y = -1; y <= 1; ++y)
		for (x = -1; x <= 1; ++x) {
			Point3d dirLoc = Point3d(x,y,0);
			if ( object->getlevel().getBorder().contains( Point3d(playerLoc + dirLoc).toPoint() ) ) {
				if (  findTargets( a, object->getlevel().getTile( Point3d(dirLoc+playerLoc) ) ).numData() > 0 )
					retVal[ dirLoc.toPoint() ] = true;
				}
			}

	if ( findTargets( a, object ).numData() > 0 )
		retVal.self = true;

	return retVal;

	}


Sorted<Target> PlayerUI::findTargets( Action a, const Target &baseTarget ) {

	Sorted<Target> retList;

//	if ( baseTarget->capable(a,object) ) retList.add( baseTarget );
	if ( object->capable(a,baseTarget) ) retList.add( baseTarget );

	List<Target> *baseInv = baseTarget->itemList();

	if ( baseInv && baseInv->reset() )
	do {

		if ( ( baseTarget->capable(aSeeInv,object) )&&( baseInv->get() != object ) ) {
                        Sorted<Target> find = findTargets(a, baseInv->get() );
			retList.add( find );
                        }

		} while ( baseInv->next() );

	return retList;
	}

PlayerUI::PlayerUI( std::iostream &inFile, TargetValue *TVP )
: Actor( inFile, TVP ) {

  inFile.read( (char*)&turn_time, sizeof(turn_time) );
	}

void PlayerUI::toFile( std::iostream &outFile ) {
	Actor::toFile( outFile );

  outFile.write( (char*)&turn_time, sizeof(turn_time) );
	}


targetType PlayerUI::gettype( void ) const {
	return tg_player_ui;
	}

bool  PlayerUI::istype( targetType t ) const {
	if ( t == tg_player_ui ) return true;
	else return Actor::istype(t);
	}

Target PlayerUI::dirTarget( Command key ) {
	Point3d loc;

  Dir dir = Keyboard::toDir(key);

  if (dir.self) return object;
  if (dir.ground) return object->getTarget(tg_parent);

  loc =	Point3d(dir.toPoint()) + object->getlocation();
	//if ( object->getlevel().getBorder().contains(loc) )
	if ( Map::limitsRect().contains(loc) )
		return Map::getTile( loc );

	return Target();

	}

Target PlayerUI::create( const Option &o ) {

	PlayerUI *pui = new PlayerUI( &o );

	TargetValue *THIS = new TargetValue( pui );

	pui->setTHIS(THIS);

	ActionEvent::create( Target(THIS), aAct, o.time );

	return Target( THIS );

	}

Target PlayerUI::getPoint( Point l, Command select, bool display, bool locate, bool ) {

  int z = object->getlocation().getz();
  Point3d loc = Point3d( l.getx(), l.gety(), z );
	Point3d fin = loc;

	//Level &lev = object->getlevel();
	Path path;

	bool visibleTarget = false;

  // This section should be rewritten to
  // check the Map::creatureList!!!

  Rectangle visible = Map::getVisible();
  
	// If we want to locate the nearest enemy:
	if (locate) {

		int closestSquared = -1;

		//Level &current = object->getlevel();
		//Rectangle level = current.getBorder();
		//for (int y = level.getya(); y <= level.getyb(); ++y )
			//for (int x = level.getxa(); x <= level.getxb(); ++x )
    //int vis_offset_x = Map::visOffset().getx(),
    //    vis_offset_y = Map::visOffset().gety();

		for (int y = visible.getya(); y <= visible.getyb(); ++y )
			for (int x = visible.getxa(); x <= visible.getxb(); ++x )
				if ( Map::limitsRect().contains( Point3d(x,y,z) )
            && Map::getTile( Point3d(x,y,z) )->getVal( attrib_vis_enemy ) ) {

					path.set(loc,Point3d(x,y,z));
					path.calculate();

					if (path.hasLOS() || !visibleTarget) {
						Point3d delta = Point3d(x,y,z) - loc;
						int dist = delta.getx()*delta.getx()+delta.gety()*delta.gety();
						if ( closestSquared < 0 || closestSquared > dist || !visibleTarget && path.hasLOS() ) {
							closestSquared = dist;
							fin = Point3d(x,y,z);
							visibleTarget = true;
							}
						}

					}

		}

	path.set(loc,fin);
	LEV_LOCATE(fin.getx(),fin.gety());

	path.calculate();
	if (display) path.display();
	LEV_LOCATE(fin.getx(),fin.gety());

	Command key = kInvalid;
	while ( (key != kEscape)&&(key!=select)&&(key!=kEnter)&&(key!=kSpace) ) {

		if (key != kTab)
    Map::getTile( path.getFinish() )->doPerform(aDescribe);
		Message::print();
		LEV_LOCATE(fin.getx(),fin.gety());
    
		Point3d delta(0,0,0);
		key = Keyboard::getCommand();

		switch(key) {

			case kNorthWest:
			case kHome:       delta = Point3d(-1,-1, 0); break;

			case kNorth:
			case kCursorUp:   delta = Point3d( 0,-1, 0); break;

			case kNorthEast:
			case kPageUp:     delta = Point3d( 1,-1, 0); break;

			case kWest:
			case kCursorLeft: delta = Point3d(-1, 0, 0); break;

			case kEast:
			case kCursorRight:delta = Point3d( 1, 0, 0); break;

			case kSouthWest:
			case kEnd:        delta = Point3d(-1, 1, 0); break;

			case kSouth:
			case kCursorDown: delta = Point3d( 0, 1, 0); break;

			case kSouthEast:
			case kPageDown:   delta = Point3d( 1, 1, 0); break;

      case kTab: {
        autoThrow();
        Target thrown = ( (Player*)object.raw() )->thrown;
        if (!!thrown)
          Message::add("Throwing "+Grammar::plural(thrown)+".",true);
        break;
        }

      default: break;

			}

			//if ( object->getlevel().getBorder().contains( Point3d(fin + delta).toPoint() ) ) {
      
      // Make sure the new point is on the screen:
      //if ( POINT_VISIBLE(fin.getx()+delta.getx(),fin.gety()+delta.gety()) ) {
      if ( visible.contains(fin+delta) ) {
				fin += delta;
				path.setFinish(fin);
				}

		path.calculate();
		if (display) path.display();

		}

	if (key == kEscape) return Target();

  Target t = Map::getTile( fin );
  // If we're aiming for the ground, just make sure
  // there's not a monster there:
  if (!!t->getTarget(tg_top_blocker))
    t = t->getTarget(tg_top_blocker);

  return t;
	}

void PlayerUI::Adjust( const Target &t, Point ) {
    using namespace std;
	Dialog dialog("Adjust", "New letter?", global::skin.quantity );
  //dialog.setOrigin(p);
	dialog.display();

	int key;
	do {

		key = dialog.getKey();
		if (Keyboard::toCommand(key) == kEscape) return;

		} while ( !isprint( key ) );

  if (Keyboard::toCommand(key) == kSpace) key = 0;
	t->setletter(key);

	}

Command  PlayerUI::LoadUnload( const Target &cont, bool unload, Point location ) {

	Point cursor = Screen::cursorat(),
        focus = Screen::getFocusPt();

	List<Target> *items = object->itemList();
	Sorted<Target> *equipped = &( (Player *)object.raw() )->equipped;
	Target primary =  ( (Player *)object.raw() )->primary,
				 secondary =  ( (Player *)object.raw() )->secondary;

  Target container = cont;
	Action a = (unload)?aUnload:aLoad;

	bool reattach = false;
	if (container->quantity() > 1) {
		container = container->detach(1);
		reattach = true;
		container->doPerform(aFly);
		object->perform( aContain, container );
		}

	List<Target> contents;
	List<Target> *cptr;

	if (unload) {
		cptr = container->itemList();
		}
	else {

    Target slot1, slot2;
    // If wielded weapons can be put in, add them:
		if (!!primary && container->capable(aLoad,primary)) {
      Temp::Option temp_opt;
      temp_opt.image = primary->getimage();
      temp_opt.target = primary;
      temp_opt.str2 = " (primary weapon)";
      slot1 = Temp::create(temp_opt);
      contents.add( slot1 );
      }
  	if (!!secondary && container->capable(aLoad,secondary)) {
      Temp::Option temp_opt;
      temp_opt.image = secondary->getimage();
      temp_opt.target = secondary;
      temp_opt.str2 = " (secondary weapon)";
      slot2 = Temp::create(temp_opt);
      contents.add( slot2 );
      }
    // If wearing a quiver, add the contents:
    if (equipped) {
      long e_key = equipped->lock();

      if ( equipped->reset() )
      do {

        if ( equipped->get()->istype(tg_quiver) && equipped->get() != container ) {

          List<Target> *eglist = equipped->get()->itemList();
          if ( eglist && eglist->reset() )
          do {

            if ( container->capable(aLoad,eglist->get()) ) {
              Temp::Option temp_opt;
              temp_opt.image = eglist->get()->getimage();
              temp_opt.target = eglist->get();
              temp_opt.str2 = " (ready)";
              contents.add( Temp::create( temp_opt ) );
              }
          
            } while (eglist->next());
        
          }
      
        } while ( equipped->next() );
      
      equipped->load(e_key);
      }

		if ( items && items->reset() )
		do {
			if (container->capable(aLoad,items->get()) )
				contents.add(items->get());
			} while (items->next());

			cptr = &contents;
		}

	Command retkey = kEscape;
	if (cptr->numData() == 0) {
		Point cursor = Screen::cursorat(),
          focus = Screen::getFocusPt();
		Dialog error("Error","You have nothing appropriate.", global::skin.error);
    //error.setOrigin(location);
		if (container->istype(tg_ranged))
			error.setContent("Out of ammunition.");
		error.display();
		error.getKey();
		error.hide();
		Screen::locate(cursor);
    Screen::setFocusPt( focus );

		}
	else {

		Menu<Target> contMenu( *cptr, global::skin.inventory );
		if (unload)
			contMenu.setTitle("Contents");
		else
			contMenu.setTitle("Load with");
		//contMenu.setOrigin( location );
		contMenu.display();

		Command key = kInvalid;
		while ( (key != kEscape) && (object->itemList()) && (contMenu.numitems() > 0) ) {
			key = contMenu.getCommand();

			switch (key) {
				case kEnter:
					contMenu.display();

					// Add the item:
					if (unload)
						playerContain(contMenu.get(),contMenu.get()->quantity(),location,true,false);
					else
						container->perform(aLoad,contMenu.remove());

					// If the container is full/empty, exit:
					if ( !container->isCapable(a) )
						key = kEscape;
					// Otherwise, redraw the list:
					else {
            // If loading, recheck loadable items list:
            if (!unload) {
              long key = cptr->lock();

              int items_checked = 0;

              cptr->reset();
              while (cptr->numData() > items_checked)
                if ( container->capable(aLoad,cptr->get()) ) {
                  cptr->next();
                  ++items_checked;
                  }
                else
                  cptr->remove();
              
              cptr->load(key);
              }
                
						if (contMenu.numitems())
              contMenu.redraw();
            }

					retkey = kEnter;

					break;
				default:
					if (contMenu.numitems()) contMenu.display();
					break;
				}
			}
		contMenu.hide();
		}

	// If container was in a stack, add it back to the inv:
	if (reattach) {
		container->doPerform(aLand);
		container->doPerform(aRestack);
		}
	// Otherwise, if it was in inv, check to see if it will stack:
	else if (items->find(container))
		container->doPerform(aRestack);

	Screen::locate(cursor);
  Screen::setFocusPt( focus );

	return retkey;
	}


Command PlayerUI::selectSlot( const Target &wield, Point ) {

  // If item takes two hands, just wield it:
  if ( wield->getVal(attrib_two_handed) ) {

    object->perform(aWield, wield);
    return kEnter;
    }

	// Otherwise: Select primary or secondary weapon slot or both:
  String both_str;
  if (wield->quantity() == 2) both_str = "Wield Both";
  else both_str = "Both Hands";
  Temp::Option temp_opt;
  temp_opt.image = Image(cGrey,'0');  temp_opt.str1 = both_str;
	Target both = Temp::create( temp_opt );
  
  temp_opt.image = Image(cGrey,'1');  temp_opt.str1 = "Primary";
  Target pri  = Temp::create( temp_opt );
  
  temp_opt.image = Image(cGrey,'2');  temp_opt.str1 = "Secondary";
	Target sec  = Temp::create( temp_opt );

  Command key;

	List<Target> slotList;
  if (wield->quantity() > 1)
  	slotList.add(both);
	slotList.add(pri);
	slotList.add(sec);
	Menu<Target> slotMenu( slotList, global::skin.inventory );
	slotMenu.setTitle("Which slot?");
	//slotMenu.setOrigin( p );
	slotMenu.display();

	key = kInvalid;
	while ( (key != kEscape) && (key!=kEnter) ) {
		key = slotMenu.getCommand();

		switch (key) {
			case kEnter:
				// Selected slot, now wield the item:
				if (slotMenu.get() == pri)
					object->perform( aWield, wield );
        else if (slotMenu.get() == both) {
					object->perform( aWield, wield );
					object->perform( aSwitch, wield );
					object->perform( aWield, wield );
          }
				else {
					object->perform( aSwitch, wield );
					object->perform( aWield, wield );
					object->perform( aSwitch, wield );
					}

        // Now give a message based on the wielded combination:
        if (slotMenu.get() != pri && wield->getVal(attrib_balanced))
          Message::add( Grammar::Subject(wield,Grammar::det_definite)
                        + " feels comfortable in your off hand." );


				break;

			default:
				if (slotMenu.numitems()) slotMenu.display();
				break;
			}

		}

	slotMenu.hide();

	return key;
	}


List<Target> PlayerUI::findContainer( Action a, const Target &t ) {

	List<Target> *items = object->itemList();
	// The following casts should always be legal because
	// object is assumed to only be a Player.
	Sorted<Target> *equipped = &( (Player *)object.raw() )->equipped;
	Target primary =  ( (Player *)object.raw() )->primary,
				 secondary =  ( (Player *)object.raw() )->secondary;


	if (equipped) equipped->reset();
	if (items) items->reset();

	List<Target> loadList;

	if ( !!primary && primary->capable(a,t) ) {
    Temp::Option temp_opt;
    temp_opt.image = primary->getimage();
    temp_opt.target = primary;
    temp_opt.str2 = " (primary weapon)";
    Target slot = Temp::create(temp_opt);
    loadList.add( slot );
//    loadList.add( primary );
    }
	if ( !!secondary &&secondary->capable(a,t) ) {
    Temp::Option temp_opt;
    temp_opt.image = secondary->getimage();
    temp_opt.target = secondary;
    temp_opt.str2 = " (secondary weapon)";
    Target slot = Temp::create(temp_opt);
    loadList.add( slot );
//    loadList.add( secondary );
    }

	if (equipped && equipped->reset())
	do {
		if ( equipped->get()->capable( a,t )  )
			loadList.add( equipped->get() );
		} while( equipped->next() );

	if (items && items->reset())
	do {
		if ( items->get()->capable( a,t )  )
			loadList.add( items->get() );
		} while( items->next() );

	return loadList;
	}


bool PlayerUI::playerContain( const Target &stack, int quantity, Point, bool disc, bool msg ) {

	// If we are picking up fluid, pick a container for it:
	if (stack->getVal(attrib_need_container)) {

		List<Target> loadList = findContainer(aLoad,stack);

    Temp::Option temp_opt(Image(cGrey,'.'), "discard");
		Target discard = Temp::create( temp_opt );
		if (disc) loadList.add( discard );

		if (!loadList.reset() ) {

      if (!msg) {
    		Point cursor = Screen::cursorat(),
              focus = Screen::getFocusPt();
    		Dialog error("Error","You have nothing to put it in.", global::skin.error);
    		error.display();
    		error.getKey();
    		error.hide();
    		Screen::locate(cursor);
        Screen::setFocusPt( focus );
        }
      else
        Message::add("You have nothing to put "+Grammar::Object(stack)+" within.");

			if ( global::isNew.bottle ) {
				Message::add("<?"+Grammar::plural(stack)+" and other fluids must be kept within bottles.>");
				}

			global::isNew.bottle = false;
			return false;
			}

		global::isNew.bottle = false;

		Menu<Target> loadMenu(loadList, global::skin.inventory);

		loadMenu.setTitle( "Put where?" );
		loadMenu.display();

		Command key = kInvalid;
		while ( (key != kEscape) && (key!=kEnter) ) {
			key = loadMenu.getCommand();

                        Target container;
                        bool putBack;

			switch (key) {
				case kEnter:
					loadMenu.display();
					// Chosen the item to load, now choose contents:
					container = loadMenu.get();
					if (container == discard) {
						object->getTarget(tg_parent)->perform( aContain, stack->detach(quantity) );
						break;
						}

					putBack = false;

					if (container->quantity() > quantity) {
						container = container->detach(quantity);
						putBack = true;
						}
					container->perform( aContain, stack->detach( container->quantity() ) );

					if (putBack)
						object->perform(aContain,container);
					else
						container->doPerform(aRestack);
					break;

				default:
					if (loadMenu.numitems()) loadMenu.display();
					break;
				}

			}

		}
	else {
    Screen::hidecursor();
		Target pickup = stack->detach( quantity );
    Screen::showcursor();
    // Todo: option to always display this message ...
    // ... for auto-item lettering
		if (quantity != pickup->quantity() || !!pickup->getTarget(tg_owner) || msg ) {
      char chbuf[5];
  		sprintf( chbuf, "<%02i", pickup->menuletter().val.color );
      String image_str = chbuf + String("\\")+String((char)pickup->menuletter().val.appearance)+">";

			Message::add( image_str + String(" - ") + Grammar::plural(pickup,Grammar::det_possessive) + "." );
      }
		object->perform( aContain, pickup );
		}

	return true;

	}

void PlayerUI::pickGrass( void ) {

	Point3d location = object->getlocation();

	materialClass material = (materialClass) object->getlevel().getTile(location)->getVal(attrib_material);

	if (   Material::data[ material ].state == liquid
			|| Material::data[ material ].state == powder
			|| Material::data[ material ].state == ooze	) {

		Dialog dialog("Collect", String("Collect some ")+Material::data[ material ].name+"? [Y/n]", global::skin.quantity );
		dialog.display();

		int key;
		do {

			key = dialog.getKey();

			} while (  Keyboard::toCommand(key) != kEscape
							&& Keyboard::toCommand(key) != kEnter
							&& Keyboard::toCommand(key) != kSpace
							&& key != 'y' && key != 'Y' && key != 'n' && key != 'N' );

		if ( key == 'y' || key == 'Y'||
         Keyboard::toCommand(key) == kEnter || Keyboard::toCommand(key) == kSpace ) {

      Fluid::Option fluid_opt;
      fluid_opt.material = material;
			Target fluid = Fluid::create(fluid_opt);

			playerContain(fluid,1,location.toPoint(),false);

			return;

			}
		}

	Message::add("There is nothing to pick up here.");

	}

void PlayerUI::keyHelp( void ) {

  List<Target> key_list;

  int i;

  for (i = 0; i < kNum; ++i)
    if ( keyMap[i].help ) {
      String str = keyMap[i].name;
      str = str.titlecase();
      str += String(" - ") + keyMap[i].desc;
      Temp::Option temp_opt(Image(cGrey,keyMap[i].dkey), str);
      key_list.add( Temp::create(temp_opt) );
      }

  Menu<Target> key_menu( key_list, global::skin.main );
  key_menu.setTitle("Key Help");
  key_menu.setLoop(false);
  
	Command key;
	do {

    key_menu.display();
		key = key_menu.getCommand();

		} while ( key != kEscape );


  }


bool PlayerUI::autoThrow( void ) {
  // Create a list of items that may be thrown.

	List<Target> throwables;
	List<Target> *items = object->itemList();
	Sorted<Target> *equipped = &( (Player *)object.raw() )->equipped;

  // If wearing a quiver, add the contents:
  if (equipped) {
    long e_key = equipped->lock();

    if ( equipped->reset() )
    do {

      if ( equipped->get()->istype(tg_quiver) ) {

        List<Target> *eglist = equipped->get()->itemList();
        if ( eglist && eglist->reset() )
        do {

          if ( object->capable(aThrow,eglist->get()) ) {
            Temp::Option temp_opt;
            temp_opt.image = eglist->get()->getimage();
            temp_opt.target = eglist->get();
            temp_opt.str2 = " (ready)";
            throwables.add( /*Temp::create( temp_opt )*/eglist->get() );
            }
          
          } while (eglist->next());
        
        }
      
      } while ( equipped->next() );
      
    equipped->load(e_key);
    }

  if ( items && items->reset() )
  do {
  	if (object->capable(aThrow,items->get()) )
			throwables.add(items->get());
		} while (items->next());


  if ( !throwables.numData() ) {
    Message::add("You have nothing to throw.");
    //Message::print();
    return false;
    }
  
	Menu<Target> throwMenu(throwables, global::skin.inventory);

	throwMenu.setTitle("Throw");
	throwMenu.display();

	Command key = kInvalid;
	while ( (key != kEscape) && (key!=kEnter) && (object->itemList()) && (throwMenu.numitems() > 0) ) {
		key = throwMenu.getCommand();

		switch (key) {
			case kEnter: {
				Target to_throw = throwMenu.get();

        object->perform(aThrow,to_throw);

				throwMenu.hide();

				break;
        }

			default:
				if (throwMenu.numitems()) throwMenu.display();
				break;
			}

		}


  return (key == kEnter);
  }


bool PlayerUI::perform( Action a, const Target &t ) {

  // First thing: check to see if there are 'too many'
  // levels in memory.
# define TOO_MANY_LEVELS 32
  if (Map::openLevel() >= TOO_MANY_LEVELS) {
    // If so, give the player a "PLEASE WAIT:CACHING"
    // message,
		Dialog caching_msg("Caching","Please wait...", global::skin.message);
		caching_msg.display();
    // cache,
    Map::streamData();
    // then hide the message:
		caching_msg.hide();
    }

//	if ( !capable(a, t) ) return false;

	if ( a != aAct && a != aWarn ) return Actor::perform( a, t );

	Point3d location = object->getlocation();

  if ( inventory && inventory->reset() )
    return inventory->get()->perform(a,t);

	if ( a == aWarn ) {
		ActionEvent::remove( Target(THIS), aAct );
		ActionEvent::create( Target(THIS), aAct, 0 );
		return true;
		}

  // From here on,
  // a == aAct
  
	bool move = false;

	Target par = object->getTarget(tg_parent);
  assert (!!par); // Error::fatal("PlayerUI needs player to have parent!");
  
  if (!!par && par->istype( tg_trap )) par = par->getTarget(tg_parent);

	List<Target> *floorItems = par->itemList();

	List<Target> *items = object->itemList();
	// The following casts should always be legal because
	// object is assumed to only be a Player.
	Sorted<Target> *equipped = &( (Player *)object.raw() )->equipped;
	Target primary =  ( (Player *)object.raw() )->primary,
				 secondary =  ( (Player *)object.raw() )->secondary;

	if (global::quit == global::quit_false && object->getVal(attrib_moved)) describe( par );

	object->doPerform(aStartTurn);

  // Auto-search:
  // Test only when time has passed since the last turn:
  if (  turn_time != Timeline::now() ) {
  	switch ( object->test(sk_awar, 20-object->getVal(stat_con), THIS) ) {
      
      case result_perfect:
      case result_pass: {
    		char x, y;
    		Sorted<Target> hidden;
    		Point3d playerLoc = object->getlocation();
    		Rectangle lBorder = object->getlevel().getBorder();
        int radius = object->getVal(attrib_search_radius);

    		for (y = -radius; y <= radius; ++y)
    			for (x = -radius; x <= radius; ++x)
    				if ( lBorder.contains( Point(x,y)+playerLoc.toPoint() ) )
    					hidden.add(  findTargets( aSearch, object->getlevel().getTile( Point3d(x,y)+playerLoc ) )  );


    		if ( hidden.reset() )
    		do {
    			object->perform(aSearch, hidden.get());
    			} while( hidden.next() );
        break;
        }

      default:
        break;

  		}
      
    turn_time = Timeline::now();
    }


/*
			String print;
#     ifdef PRINTMEM
      print = "Memory free=";
			print += (long)coreleft();
			Message::add(print);
#     endif //PRINTMEM
			print = "Catalogue items=";
			print += (long)Catalogue::numitems();
#     ifdef SOFT_LINK
			print += " links=";
			print += (long)Catalogue::numlinks();
#     endif // SOFT_LINK
			Message::add(print);
*/

	Message::print();

	if (global::quit != global::quit_false) return false;

	LEV_LOCATE( location.getx(),location.gety());
  Screen::focusCursor();
	bool dropMany = false;
	bool unload = false;
	Action act;
	String actStr;

  int kRaw = Keyboard::get();
	Command key = Keyboard::toCommand(kRaw);

	int turnTime = 0;
        
	// Variables used in switch statement:
	Point3d playerLoc;
	Rectangle lBorder;
	Sorted<Target> hidden;
	Target stack;

	switch( key ) {
    case kChat:
		case kOpen:
		case kClose:
		case kDisarm:
		case kKick:
    case kAttack:

			if (global::isNew.getDir) {
				Message::add("<?Use cursor keys to select a direction, '\\" + Keyboard::keytostr(keyMap[kSelf].dkey) + "' for yourself or '\\" + Keyboard::keytostr(keyMap[kDescend].dkey) + "' for the ground.>");
				global::isNew.getDir = false;
				}

		case kAscend:
		case kDescend: {
			Target openTarget;
			List<Target> openItems;

			if (key == kAscend) {
			 act = aAscend;
			 actStr = "ascend";
			 }
			else if (key == kDescend) {
			 act = aDescend;
			 actStr = "descend";
			 }
			else if (key == kOpen) {
			 act = aOpen;
			 actStr = "open";
			 }
			else if (key == kDisarm) {
			 act = aDisarm;
			 actStr = "disarm";
			 global::isNew.untrap = false;
			 }
			else if (key == kKick) {
			 act = aKick;
			 actStr = "kick";
			 }
			else if (key == kAttack) {
			 act = aAttack;
			 actStr = "fight";
			 }
      else if (key == kChat) {
       act = aChat;
       actStr = "chat";
       }
			else {
			 act = aClose;
			 actStr = "close";
			 }

			// Choose a direction:
			Command dirKey;
			if (act != aAscend && act != aDescend) {
				// Give message:
				Message::add(actStr+": Which direction?",true);
				Message::print();
				dirKey = Keyboard::getDirection();
				}
			else
				dirKey = kDescend;

			if (dirKey == kEscape) break;

			openTarget = dirTarget( dirKey );
			if ( !openTarget ) {
				Message::add(STR_OS_KILL);
				break;
				}

			openItems = findTargets( act, openTarget );

      // If the action is descend, add the player's
      // mount to the list of descendables:
      if (act == aDescend) {
        Target p_mount = object->getTarget(tg_mount);
        
        if (!!p_mount)
          openItems.add( p_mount );
        }

			openItems.reset();
			if ( openItems.numData() == 0 ) {
        if ( act == aChat )
          actStr += " to";
				if (openTarget == object)
					Message::add("You have nothing to "+actStr+".");
				else
					Message::add("You see nothing to "+actStr+" "+(openTarget->getlocation()==object->getlocation()?"":"t")+"here.");
				break;
				}

			if ( (openItems.numData() == 1) && ( openTarget != object ) ) {

          Target chatee = openItems.get();
          if (act == aChat && chatee->getVal(attrib_trader) ) {

            List<Action> action_list;

            action_list.add(aChat);
            // Todo: Trade through menus.
            //action_list.add(aBuy);
            //action_list.add(aSell);
            action_list.add(aSettle);

            Menu<Action> action_menu( action_list, global::skin.inventory );
            action_menu.setTitle("Trade");
            Command key = kInvalid;
            bool done = false;

            while (!done) {
              action_menu.display();
              key = action_menu.getCommand();

              switch(key) {
                case kEnter:
                  done = true;
                  object->perform( action_menu.get(), chatee );
                  break;
                  
                case kEscape:
                  done = true;
                  break;
                  
                default:
                  break;
                }
              }
          
            }

          else {
            object->perform( act, openItems.get() );

  					turnTime = GRANULARITY / object->getVal(stat_spd);
            }

					}

			else if (openItems.numData() >= 1) {

				Menu<Target> openMenu( openItems, global::skin.inventory );
				openMenu.setTitle(actStr.titlecase());
				openMenu.display();

				Command key = kInvalid;
				bool done = false;

				while ( !done ) {
					key = openMenu.getCommand();

					switch (key) {
						case kEnter:
							openMenu.hide();

								object->perform( act, openMenu.get() );
								turnTime = GRANULARITY / object->getVal(stat_spd);
                
							openMenu.redraw();
							done = true;
							break;
						case kEscape:
							done = true;
							break;
                        default: break;
						}

					openMenu.display();

					}

				}

			break;
      }
		case kSearch: {
			global::isNew.search = false;

			char x, y;
      playerLoc = object->getlocation();
      lBorder = object->getlevel().getBorder();
      int radius = object->getVal(attrib_search_radius);

  		for (y = -radius; y <= radius; ++y)
  			for (x = -radius; x <= radius; ++x)
					if ( lBorder.contains( Point(x,y)+playerLoc.toPoint() ) )
  					hidden.add(  findTargets( aSearch, object->getlevel().getTile( Point3d(x,y)+playerLoc ) )  );

			if ( hidden.reset() )
			do {
				object->perform(aSearch, hidden.get());
				} while( hidden.next() );

			turnTime = GRANULARITY / object->getVal(stat_spd);
			break;
      }

		case kWear:
		case kQuaff:
		case kEat:
    case kRead:
    case kApply: {

      if (key == kEat) {
			 act = aEat;
			 actStr = "eat";
			 }
			else if (key == kQuaff) {
			 act = aQuaff;
			 actStr = "quaff";
			 }
			else if (key == kWear) {
			 act = aWear;
			 actStr = "wear";
			 }
      else if (key == kRead) {
			 act = aRead;
			 actStr = "read";
       }
			else if (key == kApply) {
			 act = aApply;
			 actStr = "apply";
			 }
      else {
        assert(0);
        // Error::fatal("Unexpected key type in PlayerUI::perform()");
        }


			if ( (!items || !items->reset() ) && (!floorItems || !floorItems->reset() ) ) {
				Message::add("You are not carrying anything.");
				break;
				}

			List<Target> wearList;

      // List items on floor first:
			if ( floorItems && floorItems->reset())
			do {
				if ( object->capable( act, floorItems->get() )  ) {
          Image i = floorItems->get()->getimage();
          Target g = floorItems->get();

          String ground = "ground";
          if (Map::getTile(location)->getVal(attrib_floor)) ground = "floor";
          Temp::Option temp_opt;
          temp_opt.image = i;
          temp_opt.target = g;
          temp_opt.str2 = " (on "+ground+")";
          Target tmp = Temp::create( temp_opt );
					wearList.add(tmp);
          }
				} while( floorItems->next() );

      // Then wielded/worn items:
      Target slot1, slot2;
      if (!!primary && object->capable(act,primary) ) {
        Temp::Option temp_opt;
        temp_opt.image = primary->getimage();
        temp_opt.target = primary;
        temp_opt.str2 = " (primary weapon)";
        slot1 = Temp::create( temp_opt );
        wearList.add(slot1);
        }
      if (!!secondary && object->capable(act,secondary) ) {
        Temp::Option temp_opt;
        temp_opt.image = secondary->getimage();
        temp_opt.target = secondary;
        temp_opt.str2 = " (secondary weapon)";
        slot2 = Temp::create(temp_opt);
        wearList.add(slot2);
        }
        
      if (act != aWear && equipped && equipped->reset())
      do {
				if ( object->capable( act, equipped->get() )  )
					wearList.add( equipped->get() );
        } while (equipped->next() );

      // Then items in inventory:
			if (items && items->reset())
			do {
				if ( object->capable( act, items->get() )  )
					wearList.add( items->get() );
				} while( items->next() );

			if (!wearList.reset() ) {
        if ( act == aEat && !object->isCapable(aEat) )
          Message::add("You feel no inclination to eat.");
        else
  				Message::add("You have nothing to " + actStr + ".");
				break;
				}

			Menu<Target> wearMenu( wearList, global::skin.inventory );

			wearMenu.setTitle(actStr.titlecase());
			wearMenu.display();

      // The menu is now on screen...
      // Take this opportunity
      // to cache some map:
      Map::streamData();

			key = kInvalid;
			while ( (key != kEscape) && (key!=kEnter/*||act == aWear*/) && (wearMenu.numitems() > 0) ) {

				wearMenu.redraw();
				key = wearMenu.getCommand();

				switch (key) {
					case kEnter: {
            stack = wearMenu.remove();

						wearMenu.hide();

            //Targeting targeting = (Targeting)stack->getVal( attrib_targeting );
            Target target = object;

            /*
            if (targeting & t_corner) {
        			if (global::isNew.getDir) {
        				Message::add("<?Use cursor keys to select a direction, '\\" + Keyboard::keytostr(keyMap[kSelf].dkey) + "' for yourself or '\\" + Keyboard::keytostr(keyMap[kDescend].dkey) + "' for the ground.>");
        				global::isNew.getDir = false;
        				}
            
        			Command dirKey;
      				// Give message:
       				Message::add(actStr+": Which direction?",true);
       				Message::print();
       				dirKey = Keyboard::getDirection();

       				if (dirKey == kEscape) break;

       				target = dirTarget( dirKey );
       				if ( !target ) {
       					Message::add(STR_OS_KILL);
       					break;
       					}

              if (target->getVal(attrib_opaque) >= 100) {
                Message::add("You would need X-ray vision to see through that.");
                break;
                }
              else {
                //object->getlevel().LOS( target );
                }
              
              }
            */
            //if (targeting & t_aim) {
            if (act == aApply && stack->istype(tg_scope)) {
              Message::add("Look at what?",true);
              Message::print();
              target = getPoint( target->getlocation().toPoint(), kSelf, true, false );
              if (!target) break;
              
              stack->perform( aBeApplied, target );
              }
            /*
            else if (targeting & t_inventory) {

              Message::add("Examine what?",true);
              Message::print();

              List<Target> exList;
              
              // List items on floor first:
        			if ( floorItems && floorItems->reset())
        			do {
        				if ( stack->capable( aBeApplied, floorItems->get() )  ) {
                  Image i = floorItems->get()->getimage();
                  Target g = floorItems->get();
                
                  String ground = "ground";
                  if (object->getTarget(tg_parent)->getVal(attrib_floor)) ground = "floor";
                  Temp::Option temp_opt;
                  temp_opt.image = i;
                  temp_opt.target = g;
                  temp_opt.str2 = " (on "+ground+")";
                  Target tmp = Temp::create(temp_opt);
        					exList.add(tmp);
                  }
        				} while( floorItems->next() );
        
              // Then wielded/worn items:
              Target slot1, slot2;
              if (!!primary && stack->capable(aBeApplied,primary) ) {
                Temp::Option temp_opt;
                temp_opt.image = primary->getimage();
                temp_opt.target = primary;
                temp_opt.str2 = " (primary weapon)";
                slot1 = Temp::create(temp_opt);
                exList.add(slot1);
                }
              if (!!secondary && stack->capable(aBeApplied,secondary) ) {
                Temp::Option temp_opt;
                temp_opt.image = secondary->getimage();
                temp_opt.target = secondary;
                temp_opt.str2 = " (secondary weapon)";
                slot2 = Temp::create(temp_opt);
                exList.add(slot2);
                }
                
              Target equip;
              if (equipped && equipped->reset())
              do {
        				if ( stack->capable( aBeApplied, equipped->get() )  ) {
                  Temp::Option temp_opt;
                  temp_opt.image = equipped->get()->getimage();
                  temp_opt.target = equipped->get();
                  temp_opt.str2 = " (worn)";
                  equip = Temp::create(temp_opt);
        					exList.add( equip );
                  }
                } while (equipped->next() );

              // Then items in inventory:
        			if (items && items->reset())
        			do {
        				if ( stack->capable( aBeApplied, items->get() )  )
        					exList.add( items->get() );
        				} while( items->next() );

              String actStr = "examine";
        			if (!exList.reset() ) {
        				Message::add("You have nothing to " + actStr + ".");
        				break;
        				}

        			Menu<Target> exMenu( exList, global::skin.inventory );

        			exMenu.setTitle(actStr.titlecase());
        			exMenu.display();

        			key = kInvalid;
        			while ( (key != kEscape) && (key!=kEnter) ) {

        				exMenu.display();
        				key = exMenu.getCommand();

        				switch (key) {
        					case kEnter:
                    target = exMenu.get();
                  default:
                    break;
                  }
                }

              stack->perform( aBeApplied, target );
            
              }
            */
            else
   						object->perform( act, stack );


						turnTime += GRANULARITY / object->getVal(stat_spd);

						break;
            }

					default:
						if (wearMenu.numitems()) wearMenu.display();
						break;
					}

				}

			key = kEnter;
			break;
       }


    case kMove: {

			Message::add("Move: Use cursor keys to select a direction.",true);
      Message::print();
      
      Command runKey = Keyboard::getDirection();
      if (runKey == kEscape) {
        Message::add("Move cancelled.",true);
        break;
        }
      Dir runDir = Keyboard::toDir(runKey);
      WalkDir::Option walk_opt;
      walk_opt.dir = runDir;
      walk_opt.object = object;
      Target moveActor = WalkDir::create(walk_opt);
      
      add(moveActor);
      
      break;

      }
      
		case kRest:

			turnTime = GRANULARITY*10 / object->getVal(stat_spd);
			global::isNew.rest = false;
			break;

		case kSwitch:


      if ( !object->isCapable( aSwitch ) ) {
				if (!primary && !secondary) {
					Message::add("You are not wielding anything.");

					if ( global::isNew.wield ) {
						Message::add("<?Press '\\" + Keyboard::keytostr(keyMap[kWield].dkey) + "' to wield items as weapons.>");
						global::isNew.wield = false;
						}
					}
				else
					Message::add("You cannot switch weapons now.");
				break;
				}

			object->doPerform( aSwitch );

      // Now give a message based on the wielded combination:
      if (!!primary && primary->getVal(attrib_balanced))
        Message::add( Grammar::Subject(primary,Grammar::det_definite)
                      + " feels comfortable in your off hand." );

      // We've taken time if the player doesn't
      // have the quick draw feat:
      if ( getVal(attrib_feat)&f_draw ==0 )
  			turnTime = GRANULARITY / object->getVal(stat_spd);
        
			break;

		case kWield: {

      global::isNew.wield = false;

			if ( (!items || !items->reset() ) && !primary && !secondary ) {
				Message::add("You are not carrying anything.");
				break;
				}


			List<Target> wieldList;
      
      Temp::Option temp_opt(Image(cGrey,'-'), "bare hands");
			Target hands = Temp::create( temp_opt );

			if ( !!primary || !!secondary )
				wieldList.add(hands);

			if ( items && items->reset() )
			do {
				if ( object->capable( aWield, items->get() )  )
					wieldList.add( items->get() );
				} while( items->next() );

			Menu<Target> wieldMenu(wieldList, global::skin.inventory);

			wieldMenu.setTitle("Wield");
			wieldMenu.display();

      // The menu is now on screen...
      // Take this opportunity
      // to cache some map:
      Map::streamData();
      
			key = kInvalid;
			while ( (key != kEscape) && (key!=kEnter) && (object->itemList()) && (wieldMenu.numitems() > 0) ) {
				key = wieldMenu.getCommand();

				switch (key) {
					case kEnter: {
						Target wield = wieldMenu.remove();

            // If we wielded an item,
						if (selectSlot(wield, location.toPoint() + Point(map_left+2, map_top+2)) == kEnter)
              // and we can't quickdraw,
              if ( getVal(attrib_feat)&f_draw ==0 )
               // take time:
				  			turnTime += GRANULARITY / object->getVal(stat_spd);

						wieldMenu.hide();

						break;
             }

					default:
						if (wieldMenu.numitems()) wieldMenu.display();
						break;
					}

				}

			hands->detach();

			key = kEnter;
			break;
       }
    case kBuild: {

			if ( (!equipped || !equipped->reset() )
				&& (!primary) && (!secondary)
				&& (!items || !items->reset() )  ) {
				Message::add("You are not carrying anything.");
				break;
				}

			List<Target> buildList = findContainer(aBuild,Target());
			if (!buildList.reset() ) {
				Message::add("You have nothing to build with.");
				break;
				}

			Menu<Target> buildMenu(buildList, global::skin.inventory);

			buildMenu.setTitle("Build");
			buildMenu.display();

      // The menu is now on screen...
      // Take this opportunity
      // to cache some map:
      Map::streamData();

			key = kInvalid;
			while ( (key != kEscape) && (object->itemList()) && (buildMenu.numitems() > 0) ) {
				key = buildMenu.getCommand();

				switch (key) {
					case kEnter: {
            Point cursor = Screen::cursorat(),
                  focus = focus = Screen::getFocusPt();
            
						buildMenu.display();
            // If the item is in a stack, detach it:
            Target build_item = buildMenu.get();
            bool stack = build_item->quantity() > 1;
            if (stack)
              build_item = build_item->detach(1);

						// Now build the item:
						bool success = build_item->perform(aBuild,object);

            if (success) {
              key = kEscape;
              turnTime = GRANULARITY / object->getVal(stat_spd);
              }
            //else buildMenu.redraw();

            // If we detached the item, put it back:
            if (stack) {
              object->perform(aContain, build_item);
              if (!success) buildMenu.redraw();
              }
            Screen::locate(cursor);
            Screen::setFocusPt( focus );
						break;
            }

					default:
						if (buildMenu.numitems()) buildMenu.display();
						break;
					}

				}

			key = kEnter;
      break;
      }

		case kUnload:
			unload = true;
		case kLoad: {
			global::isNew.reload = false;

			if ( (!equipped || !equipped->reset() )
				&& (!primary) && (!secondary)
				&& (!items || !items->reset() )  ) {
				Message::add("You are not carrying anything.");
				break;
				}

			Action a;
			String word;
			if (unload) {
				a = aUnload;
				word = "unload";
				}
			else {
				a = aLoad;
				word = "load";
				}

			List<Target> loadList = findContainer(a,Target());

			if (!loadList.reset() ) {
				Message::add("You have nothing to "+word+".");
				break;
				}

			Menu<Target> loadMenu(loadList, global::skin.inventory);

			loadMenu.setTitle(word.titlecase());
			loadMenu.display();

      // The menu is now on screen...
      // Take this opportunity
      // to cache some map:
      Map::streamData();

			key = kInvalid;
			while ( (key != kEscape) && (object->itemList()) && (loadMenu.numitems() > 0) ) {
				key = loadMenu.getCommand();

				switch (key) {
					case kEnter: {
						loadMenu.display();
						// Chosen the item to load, now choose contents:
						Target container = loadMenu.get();
						if ( LoadUnload( container, unload, location.toPoint() + Point(map_left, map_top)+Point(2,2) ) == kEnter ) {
							key = kEscape;
							turnTime = GRANULARITY / object->getVal(stat_spd);
							loadMenu.redraw();
							}

						break;
             }

					default:
						if (loadMenu.numitems()) loadMenu.display();
						break;
					}

				}

			key = kEnter;
			break;
       }
		case kTakeOff: {
			if ( (!equipped || !equipped->reset() ) && (!primary) && (!secondary) )  {
				Message::add("You have nothing to take off.");
				break;
				}

			List<Target> takeoffList;

      Target slot1, slot2;
      
			if (!!primary) {
        Temp::Option temp_opt;
        temp_opt.image = primary->getimage();
        temp_opt.target = primary;
        temp_opt.str2 = " (primary weapon)";
        slot1 = Temp::create(temp_opt);
        takeoffList.add( slot1 );
        }
			if (!!secondary) {
        Temp::Option temp_opt;
        temp_opt.image = secondary->getimage();
        temp_opt.target = secondary;
        temp_opt.str2 = " (secondary weapon)";
        slot2 = Temp::create(temp_opt);
        takeoffList.add( slot2 );
        }

			if ( equipped && equipped->reset() )
			do {
				takeoffList.add( equipped->get() );
				} while( equipped->next() );

			Menu<Target> takeoffMenu(takeoffList, global::skin.inventory);

			takeoffMenu.setTitle("Take Off");
			takeoffMenu.display();

      // The menu is now on screen...
      // Take this opportunity
      // to cache some map:
      Map::streamData();

			key = kInvalid;
			while ( (key != kEscape) && (takeoffMenu.numitems() > 0) ) {
				takeoffMenu.display();
				key = takeoffMenu.getCommand();

				switch (key) {
					case kEnter: {
						Target takeoff = takeoffMenu.remove();

            if (takeoff == slot1) takeoff = primary;
            if (takeoff == slot2) takeoff = secondary;

						takeoffMenu.hide();

						object->perform( aTakeOff, takeoff );
						turnTime += GRANULARITY / object->getVal(stat_spd);

						break;
             }

					default:
						if (takeoffMenu.numitems()) takeoffMenu.display();
						break;
					}

				}


			key = kEnter;
			break;
       }

    case kVolley :
		case kTarget : {
      Point cursor = Screen::cursorat();

      // Name the projectile:
      if ( key == kTarget ) {
  			// Does player carry a weapon:
  			if (
  				  	// check primary weapon slot:
  				! (!!primary && primary->isCapable(aFire) )

  				&&  // check secondary weapon slot:
  				! (!!secondary && secondary->isCapable(aFire) ) ) {

  				//No firable weapon, give relevant message:

  				// Not wielding anything:
  				if (!primary && !secondary) {
  					Message::add("You are not wielding anything!");

  					if ( global::isNew.wield ) {
  						Message::add("<?Press '\\" + Keyboard::keytostr(keyMap[kWield].dkey) + "' to wield a firearm.>");
  						global::isNew.wield = false;
  						}

  					break;
  					}
  				// Wielding unloaded weapons:
  				else if ( (!!primary && primary->istype(tg_ranged))
  					 || (!!secondary && secondary->istype(tg_ranged)) ) {

             if (object->doPerform(aQuickReload) ) {
          		 turnTime = 20 / object->getVal(stat_spd);
               }
             else {
    					 Message::add("You need to reload.");

    					 if ( global::isNew.reload ) {
    							Message::add("<?Press '\\" + Keyboard::keytostr(keyMap[kLoad].dkey) + "' to load your weapons.>");
    							global::isNew.reload = false;
    							}
  	  				 break;
               }
  					 }
  				// Wielding items that cannot fire:
  				else {
            Message::add(String("Your weapon")+((!!primary&&!!secondary)?"s":"")+" cannot fire.");
    				if ( global::isNew.wield ) {
    					Message::add("<?Press '\\" + Keyboard::keytostr(keyMap[kWield].dkey) + "' to wield a firearm.>");
    					global::isNew.wield = false;
    					}
    				break;
            }

      		}
        }
      else {
        // Volley interface:
        //  - user presses 'v'
        //  - presented with a menu: Throw what?
        //    (If wearing a bandolier with grenades, auto select
        //     the grenades if no item has recently been thrown.)
        //  - allowed to choose a target.
        //
        // *next time*
        //
        //  - user presses 'v'
        //  - message at top reads eg:
        //    "Throwing a knife.  Press 'Tab' to change."
        //  - if ammo is depleted or Tab is pressed,
        //    presented with a menu: Throw what?
        //  - allowed to choose a target.

        // If Player has no auto-throw item:
        if ( ! object->getVal(attrib_auto_throw) )
          if (! autoThrow()) break;

        }

      if (key == kVolley) {
        Target thrown = ( (Player*)object.raw() )->thrown;
        if (!!thrown)
          Message::add("Throwing "+Grammar::plural(thrown)+".",true);
        else break;
        
    		Message::add("Press '\\" + Keyboard::keytostr(keyMap[kTab].dkey) + "' to change.");
        Message::print();
        }

			Target p = getPoint(object->getlocation().toPoint(),key,true,true,(key==kVolley));
			// If no target selected:
			if (!p) break;

			// If player selected:
			if (p->getlocation() == object->getlocation()) {
				Dialog dialog("Suicide", String("Are you sure? [y/N]"), global::skin.error );
				dialog.display();

				int key;
				do {

					key = dialog.getKey();

					} while (  Keyboard::toCommand(key) != kEscape
									&& Keyboard::toCommand(key) != kEnter
									&& Keyboard::toCommand(key) != kSpace
									&& key != 'y' && key != 'Y' && key != 'n' && key != 'N' );

				if ( key != 'y' && key != 'Y')
					break;

				}

      bool turn_spent;

      if ( key == kTarget )
        turn_spent = object->perform( aFire, p );
      else
        turn_spent = object->perform( aThrowAt, p );

			if (turn_spent)
        turnTime += GRANULARITY / object->getVal(stat_spd);
      Screen::locate(cursor);

			break;
      }

		case kLook: {

      Point cursor = Screen::cursorat();
      
      Message::add("Look mode.",true);
      Message::add("<?Use cursor keys to select a target.>",true);
      //Message::print();
    
			getPoint(object->getlocation().toPoint(),kSelf,false,false);

			Message::add("Look complete.",true);

      Screen::locate(cursor);

			break;
      }

		case kNorthWest:
		case kHome:
			location.decy();
			location.decx();
      if ( Map::limitsRect().contains(location) )
  			move = true;
      else Message::add("Nothing of interest lies in that direction.");
			break;

		case kNorth:
		case kCursorUp:
			location.decy();
      if ( Map::limitsRect().contains(location) )
  			move = true;
      else Message::add("Nothing of interest lies in that direction.");
			break;

		case kNorthEast:
		case kPageUp:
			location.decy();
			location.incx();
      if ( Map::limitsRect().contains(location) )
  			move = true;
      else Message::add("Nothing of interest lies in that direction.");
			break;

		case kWest:
		case kCursorLeft:
			location.decx();
      if ( Map::limitsRect().contains(location) )
  			move = true;
      else Message::add("Nothing of interest lies in that direction.");
			break;

		case kEast:
		case kCursorRight:
			location.incx();
      if ( Map::limitsRect().contains(location) )
  			move = true;
      else Message::add("Nothing of interest lies in that direction.");
			break;

		case kSouthWest:
		case kEnd:
			location.incy();
			location.decx();
      if ( Map::limitsRect().contains(location) )
  			move = true;
      else Message::add("Nothing of interest lies in that direction.");
			break;

		case kSouth:
		case kCursorDown:
			location.incy();
      if ( Map::limitsRect().contains(location) )
  			move = true;
      else Message::add("Nothing of interest lies in that direction.");
			break;

		case kSouthEast:
		case kPageDown:
			location.incy();
			location.incx();
      if ( Map::limitsRect().contains(location) )
  			move = true;
      else Message::add("Nothing of interest lies in that direction.");
			break;


		case kWait:
		case kSelf:
			turnTime = GRANULARITY / object->getVal(stat_spd);
			break;

		case kDropMany:
			dropMany = true;
			// Flow on to...
		case kDrop: {
			bool hasDropped = false;
			if (!items || (items->numData() == 0) ) {
				Message::add("You are not carrying anything.");
				break;
				}

			items->reset();

			Menu<Target> dropMenu(*items, global::skin.inventory);

			if (dropMany)
				dropMenu.setTitle("Drop Multiple");
			else
				dropMenu.setTitle("Drop");
			dropMenu.display();

      // The menu is now on screen...
      // Take this opportunity
      // to cache some map:
      Map::streamData();


			key = kInvalid;
			while ( (key != kEscape) && (object->itemList()) && (dropMenu.numitems() > 0) && (!hasDropped || dropMany) ) {
				key = dropMenu.getCommand();

				switch (key) {
					case kEnter: {

            long i_key = items->lock();
            
						dropMenu.display();
            //Screen::showcursor();
						Target stack = dropMenu.get();
						int quantity = stack->quantity();

						if (quantity > 1) {

							String quantstr = String("Quantity? [0..") + (long)quantity + "]";

							StringDialog numericdialog( "Drop Items", quantstr, 3, numeric,global::skin.quantity );

							numericdialog.setAlign( right );
							numericdialog.setInput( (long) quantity );

							String number;
							do {
								number = numericdialog.readString(false);
								} while (  !ascending( -1, number.toInt(), quantity + 1 )  );

							quantity = number.toInt();

							}

						if (quantity > 0) {
              //Screen::hidecursor();
							dropMenu.hide();
							Target drop = stack->detach( quantity );

              long i_key = items->lock();
							par->perform( aContain, drop );
              items->load(i_key);

							turnTime += GRANULARITY / object->getVal(stat_spd);

							if (drop != stack)
								Message::add( "You now have " + stack->menustring() + "." );
							hasDropped = true;
							}


            items->load(i_key);

						if ( (dropMany) && (object->itemList() == items) && dropMenu.numitems() )  {
							dropMenu.redraw();
							dropMenu.display();
							}

						break;
             }

					default:
						if (object->itemList() == items) dropMenu.display();

					}


				}
      //Screen::showcursor();



			key = kEnter;
			break;
       }

		case kPickUp: {


			if (!floorItems ) {
				pickGrass();
				break;
				}

			global::isNew.pickup = false;

			Sorted<Target> getList;

			// Make a list of all the valid items in the current tile.
			floorItems->reset();
			do {
				Target obj = floorItems->get();
				if (  obj != object
          //&& !obj->findTarget(tg_parent,object)
          &&  obj->isCapable( aLeave ) )
					getList.add( obj );
				} while ( floorItems->next() );

			if (! getList.reset() ) {
				pickGrass();
				break;
				}

			//Give the player a menu of the valid items.
			Menu<Target> getMenu(getList, global::skin.inventory);

			getMenu.setTitle("Pick Up");

      bool oneItem = false;
			if (getMenu.numitems() > 1 ) {
        getMenu.display();
        // The menu is now on screen...
        // Take this opportunity
        // to cache some map:
        Map::streamData();
        
				key = getMenu.getCommand();
        }
      else {
        key = kEnter;
        oneItem = true;
        }
        
			while ( (key != kEscape) && (getMenu.numitems() > 0) ) {

				switch (key) {
					case kEnter: {
						if (!oneItem) getMenu.display();
						Target stack = getList.remove();

						int quantity = stack->quantity();

            // Todo: Select pickup quantity if pickup_quantity option is set
						//if (quantity > 1 && stack->getVal(attrib_discrete) ) {
            if (false) {

							String quantstr = String("Quantity? [0..") + (long)quantity + "]";

							StringDialog numericdialog( "Pick Up Items", quantstr, 3, numeric,global::skin.quantity );

							numericdialog.setAlign( right );
							numericdialog.setInput( (long) quantity );

							String number;
							do {
								number = numericdialog.readString(false);
								} while (  !ascending( -1, number.toInt(), quantity + 1 )  );

							quantity = number.toInt();

							}


						playerContain( stack, quantity, location.toPoint(), false, oneItem );

						if (global::isNew.inventory) {
							Message::add("<?Press '\\" + Keyboard::keytostr(keyMap[kInventory].dkey) + "' to view your inventory.>");
							global::isNew.inventory = false;
							}

						turnTime += GRANULARITY / object->getVal(stat_spd);

						if (getMenu.numitems()) getMenu.redraw();
						break;
             }

					default:
						if (getMenu.numitems()) getMenu.display();
					}

				if (getMenu.numitems()) key = getMenu.getCommand();

				}



			key = kEnter;
			break;
       }

		case kInventory: {

			global::isNew.inventory = false;

      Temp::Option
        noth_opt(Image(cGrey,'-'), "  Your pack is empty."),
        naked_opt(Image(cGrey,'-'), "  You are naked." ),
        bp_opt(Image(cGrey,'1'), "  none" ),
        bs_opt(Image(cGrey,'2'), "  none" );
			Target
        nothing = Temp::create( noth_opt ),
				naked = Temp::create( naked_opt ),
				bareprim = Temp::create( bp_opt ),
				baresecond = Temp::create( bs_opt );

			if (!items || (items->numData() == 0) ) {
				object->perform(aContain,nothing);
				items = object->itemList();
				}

			if (!equipped || (equipped->numData() == 0) ) {
				object->perform(aWear,naked);
				equipped = &( (Player *)object.raw() )->equipped;
				}

			items->reset();
			equipped->reset();

			// Wielded Menu
			List<Target> wielded;
			if (!!primary) wielded.add(primary);
			else wielded.add(bareprim);
			if (!!secondary) wielded.add(secondary);
			else wielded.add(baresecond);

			Menu<Target> wielded_menu( wielded, global::skin.inventory);
			wielded_menu.focus(false);
			wielded_menu.setTitle("Wielded");
			Rectangle wieldBorder(1, 3, 35, 6);
			wielded_menu.setBorder( wieldBorder );
      wielded_menu.setLoop(false);
      
			wielded_menu.display();

			// Equipped Menu
			Menu<Target> equipped_menu( *equipped, global::skin.inventory);
			equipped_menu.focus(false);
			equipped_menu.setTitle("Equipped");
			Rectangle equipBorder(1, 7, 35, 22);
			equipped_menu.setBorder( equipBorder );
      equipped_menu.setLoop(false);
			equipped_menu.display();

			// Inventory Menu
			Menu<Target> inventory_menu(*items, global::skin.inventory);
			inventory_menu.setTitle("Inventory");
			Rectangle invBorder(36, 3, 80, 22);
			inventory_menu.setBorder( invBorder );
      inventory_menu.setLoop(false);
			inventory_menu.display();

      Point cursor;
      String tab_help = "[\\" + Keyboard::keytostr(keyMap[kTab].dkey) + "\\:Next Section]",
             esc_help = "[\\" + Keyboard::keytostr(keyMap[kEscape].dkey) + "\\:Close]";


             
#define WRITE_INVKEY_HELP  cursor = Screen::cursorat();\
      Screen::locate(35/2-esc_help.formatlen()/2,22);\
      Screen::writef(esc_help);\
      Screen::locate((80-35)/2-tab_help.formatlen()/2+35,22);\
      Screen::writef(tab_help);\
      Screen::locate(cursor);

      WRITE_INVKEY_HELP

			enum { m_wield, m_equip, m_inv } menu = m_inv;

			key = kInvalid;

      // The menu is now on screen...
      // Take this opportunity
      // to cache some map:
      Map::streamData();
      
      // Until the player presses escape to exit the menu:
			while ( key != kEscape
      // And in case the player performs a deadly action,
      // e.g. drinking poison:
        && object->getVal(attrib_alive) ) {
				switch(menu) {
					case m_inv:   key = inventory_menu.getCommand(); break;
					case m_equip: key = equipped_menu.getCommand(); break;
					case m_wield: key = wielded_menu.getCommand(); break;
					}


				switch (key) {
          case kCursorUp:   case kCursorDown:
          case kCursorLeft: case kCursorRight:
          case kHome: case kPageUp:
          case kEnd:  case kPageDown:
					case kTab:
            // This section deals with changes to other
            // menus.
            // The current menu loses focus:
            Screen::hidecursor();
            
            switch (menu) {
              case m_inv:
  							inventory_menu.focus(false);
  							inventory_menu.display(); break;
              case m_wield:
  							wielded_menu.focus(false);
  							wielded_menu.display(); break;
              case m_equip:
  							equipped_menu.focus(false);
  							equipped_menu.display(); break;
              }

            // Select the new menu:
            switch (key) {
              case kCursorLeft:
                // Left key pressed.  If we were focussed on
                // the inventory menu,
                if (menu == m_inv) {
                  // find the current cursor height:
                  int cursorRow = inventory_menu.getCursorRow();

                  // and determine if at this height we should jump to
                  // the equipped menu or the inventory menu:
                  if (cursorRow + invBorder.getya() < wieldBorder.getyb() ) {

                    wielded_menu.reset();
                    while (wielded_menu.getCursorRow() + wieldBorder.getya()
                          < cursorRow + invBorder.getya()
                        && wielded_menu.next() ) {}
                    menu = m_wield;
                    }
                    
                  else {
                    equipped_menu.reset();
                    while (equipped_menu.getCursorRow() + equipBorder.getya()
                          < cursorRow + invBorder.getya()
                        && equipped_menu.next() ) {}
                    menu = m_equip;
                    }
                      
                  }
                break;
                
              case kCursorRight:
                // No effect if we're in the inventory menu:
                if (menu == m_inv)
                  break;
                // Otherwise, calculate the cursorRow we wish to achieve:
                int cursorRow;
                if (menu == m_wield)
                  cursorRow = wielded_menu.getCursorRow() + wieldBorder.getya();
                else
                  cursorRow = equipped_menu.getCursorRow() + equipBorder.getya();

                // then locate the closest inv line:
                inventory_menu.reset();
                while (inventory_menu.getCursorRow() + invBorder.getya()
                      < cursorRow
                    && inventory_menu.next() ) {}
                menu = m_inv;

                break;

              case kHome:
              case kPageUp:
              case kCursorUp:
                // Up only ever has an effect if we're at the top
                // of the equipped list, where it moves focus to the
                // bottom item in the inventory list:
                if ( menu == m_equip && equipped_menu.atHead() ) {
                  menu = m_wield;
                  if (key == kCursorUp)
                    wielded_menu.toTail();
                  else
                    wielded_menu.reset();
                  }
                break;
                
              case kEnd:
              case kPageDown:
              case kCursorDown:
                // And down only ever has an effect if we're at the bottom
                // of the wielded list, where it moves focus to the
                // top item in the wielded list:
                if ( menu == m_wield && wielded_menu.atTail() ) {
                  menu = m_equip;
                  if (key == kCursorDown)
                    equipped_menu.reset();
                  else
                    equipped_menu.toTail();
                  }
                break;

              // Tab just cycles clockwise:
    					case kTab:
                if (menu == m_wield)
                  menu = m_inv;
                else if (menu == m_equip)
                  menu = m_wield;
                else
                  menu = m_equip;
                break;
              
              default: break;

              }

            // Now focus the new menu:
	          switch (menu) {
              case m_inv:
  							inventory_menu.focus();
  							inventory_menu.display();
                break;
              case m_wield:
  							wielded_menu.focus();
  							wielded_menu.display();
                break;
              case m_equip:
  							equipped_menu.focus();
  							equipped_menu.display();
                break;
              }
            WRITE_INVKEY_HELP

            Screen::showcursor();
						break;

					case kEnter: {

						switch(menu) {
							case m_inv:   inventory_menu.display(); break;
							case m_equip: equipped_menu.display(); break;
							case m_wield: wielded_menu.display(); break;
							}
            WRITE_INVKEY_HELP  

						cursor = Screen::cursorat();
            Point focus = Screen::getFocusPt();

						List<Action> actionList;
						Target selected;
						switch(menu) {
							case m_inv:   selected = inventory_menu.get(); break;
							case m_equip: selected = equipped_menu.get(); break;
							case m_wield: selected = wielded_menu.get(); break;
							}

						// Don't bother if the item is just a list-filler:
						if (selected->istype(tg_temp)) break;

						// Create a list of available actions:
						// (When editing this list, update the 'redraw?' sections below)
						// Wield (weapons):
						if ( menu == m_inv && selected->istype( tg_weapon ) )
							actionList.add( aWield );
						// Eat:
						if ( object->capable( aEat, selected ) )
							actionList.add( aEat );
						// Quaff:
						if ( object->capable( aQuaff, selected ) )
							actionList.add( aQuaff );
						// Read:
						if ( object->capable( aRead, selected ) )
							actionList.add( aRead );
						// Apply:
						if ( object->capable( aApply, selected ) && !selected->getVal(attrib_targeting) )
							actionList.add( aApply );
            // Reload
						//if ( menu == m_wield && selected->capable(aLoad) && object->capable( aQuickReload ) )
						//	actionList.add( aQuickReload );
						// Load:
						if ( selected->isCapable(aLoad) )
							actionList.add( aLoad );
						// Unload:
						if ( selected->isCapable(aUnload) )
							actionList.add( aUnload );
						// Wear:
						if ( menu != m_equip && object->capable( aWear, selected ) )
							actionList.add( aWear );
						// Take off:
						if ( menu == m_equip || menu == m_wield )
							actionList.add( aTakeOff );
						// Build (weapons):
						if ( selected->isCapable( aBuild ) )
							actionList.add( aBuild );
						// Switch:
						if ( menu == m_wield && object->isCapable(aSwitch) )
							actionList.add( aSwitch );
						// Drop:
						if ( object->capable( aRelease, selected ) )
							actionList.add( aRelease );
						// Wield (non weapons):
						if ( menu == m_inv && object->capable( aWield, selected )
                 && ! selected->istype( tg_weapon ) )
							actionList.add( aWield );
						// Open:
						if ( object->capable( aOpen, selected ) )
							actionList.add( aOpen );
						// Disarm:
						if ( object->capable( aDisarm, selected ) )
							actionList.add( aDisarm );
						// Adjust:
						actionList.add( aAdjust );

						Menu<Action> actionMenu(actionList, global::skin.action);
						Point actionOrigin;
						switch(menu) {
							case m_inv:   actionOrigin=invBorder.getMid(); break;
							case m_equip: actionOrigin=equipBorder.getMid(); break;
							case m_wield: actionOrigin=wieldBorder.getMid(); break;
							}
						//actionMenu.setOrigin(actionOrigin);
						actionMenu.setTitle(selected->menustring());
						actionMenu.display();

						key = kInvalid;
						while ( (key != kEscape) && (key != kEnter) ) {
							key = actionMenu.getCommand();
              Screen::hidecursor();
							switch(key) {
								case kEnter: {

                  long i_key = items->lock();
                  long e_key = equipped->lock();

									Action act = actionMenu.get();
                  if (act==aApply) {
  									actionMenu.hide();
                    inventory_menu.hide();
                    equipped_menu.hide();
                    wielded_menu.hide();
                    }

									if (act == aLoad || act == aUnload)
										key = LoadUnload(selected,(act==aUnload)?true:false,actionMenu.getOrigin() + Point(2,2));
									else if (act == aWield)
										key = selectSlot(selected,actionMenu.getOrigin() + Point(2,2));
									else if (act == aAdjust)
										Adjust(selected,actionMenu.getOrigin() + Point(2,2));
									else if (act == aQuickReload)
										object->doPerform(aQuickReload);
                  else if (act == aBuild)
                    selected->perform(aBuild,object);
									else
										object->perform( act, selected);

                  // if the action wasn't switch or wield,
                  if (act != aSwitch || act != aWield
                    // or the player doesn't have the quickdraw feat,
                    || getVal(attrib_feat)&f_draw ==0 )
                    // take time:
                    turnTime = GRANULARITY / object->getVal(stat_spd);

                  items->load(i_key);
                  equipped->load(e_key);

									break;
                 }

								default:
									actionMenu.display();
									break;
								}

							}
						if (key == kEscape) key = kInvalid;

            
						actionMenu.hide();
						Action act;
						if (actionMenu.numitems()) act = actionMenu.get();

						if (key==kEnter) {
              // Have suppressed the "Have to redraw xxx?" because
              // of so many special case situations (like wielding lit
              // lightsources... bah)  Will fix if speed difference
              // becomes obvious.
              
							// Need to redraw wield?
  						//  if ( act==aWield || act==aRelease || act==aLoad || act==aSwitch
              //     || act==aUnload || act==aApply || act == aQuickReload
  						//	  || ( (act==aTakeOff||act==aWear||act==aEat || act==aAdjust) && menu==m_wield) ){

								if (menu!=m_wield) wielded.reset();

								wielded.clr();

								primary =  ( (Player *)object.raw() )->primary;
								secondary =  ( (Player *)object.raw() )->secondary;

								if (!!primary)
									wielded.add(primary);
								else
									wielded.add(bareprim);
								if (!!secondary)
									wielded.add(secondary);
          			else
                  wielded.add(baresecond);

								wielded_menu.redraw();
								if (menu==m_wield) cursor = Screen::cursorat();
  						//		}
							// Need to redraw equip?
  						//	if (act == aWear || act==aRelease || act==aUnload || act==aApply || act == aQuickReload || act==aLoad
  						//		|| ( (act==aTakeOff|| act == aWield||act==aEat || act==aLoad || act==aAdjust) && menu==m_equip) ) {

								if (menu!=m_equip) equipped->reset();

								if (!equipped || equipped->numData()==0) object->perform(aWear,naked);
								else if (equipped->numData() > 1 && act==aWear) naked->detach();

								equipped_menu.redraw();
								if (menu==m_equip) cursor = Screen::cursorat();
  						//		}
							// Need to redraw invent?
  						//	if (act == aTakeOff || act == aOpen || act == aDisarm || act==aRelease || act==aApply
  						//		|| act==aWear || act == aWield ||act==aEat||act==aQuaff || act==aLoad || act==aUnload || act==aAdjust) {

								items = object->itemList();

								if (menu!=m_inv) items->reset();

								if (!items || items->numData()==0) object->perform(aContain,nothing);
								else if (items->numData() > 1) nothing->detach();

								if (items && items->numData() > 0)
									inventory_menu.redraw();
								if (menu==m_inv) cursor = Screen::cursorat();

  						//		}
							}

            Screen::showcursor();
						Message::print();

						Screen::locate( cursor );
            Screen::setFocusPt( focus );

						switch(menu) {
							case m_inv:   inventory_menu.display(); break;
							case m_equip: equipped_menu.display(); break;
							case m_wield: wielded_menu.display(); break;
							}
            WRITE_INVKEY_HELP  
						break;
           }

					default:
						switch(menu) {
							case m_inv:   inventory_menu.display(); break;
							case m_equip: equipped_menu.display(); break;
							case m_wield: wielded_menu.display(); break;
							}
            WRITE_INVKEY_HELP  
						break;
					}


				}

			nothing->detach();
			naked->detach();
			bareprim->detach();
			baresecond->detach();
			// Supress messages because the gui indicates all:
			Message::flush();
			key = kEnter;

			break;
      }

    case kPray:
      object->doPerform(aPray);
      break;

    case kLookAround:
      // Looking around costs no time:
      object->doPerform(aLookAround);
      object->doPerform(aSniffAround);
      break;

    case kSkills: {

      List<SkillVal> skList;

      int i;
      for (i = 0; i < sk_num; ++i)
        skList.add( ( (Player *) object.raw() )->skill[i] );

      Menu<SkillVal> skMenu(skList, global::skin.inventory);
      skMenu.setTitle("Skills");

      Command key = kInvalid;
      bool done = false;

      while ( (key != kEscape) && !done ) {
        skMenu.display();
        key = skMenu.getCommand();

        switch (key) {
          case kEscape:
            done = true;
            break;

          case kCursorLeft:
          case kWest:
            if ( Skills[skMenu.get().skill].type == tInvest ) {
              skMenu.get().invest( max(skMenu.get().getInvestment()-1,0) );
              ((Player*)object.raw())->skill[skMenu.get().skill] = skMenu.get();
              }
            skMenu.redraw();
            break;

          case kEnter:
          case kCursorRight:
          case kEast:
            if ( Skills[skMenu.get().skill].type == tInvest ) {
              if ( key == kEnter && skMenu.get().investment == skMenu.get() )
                done = true;
              skMenu.get().invest( min(skMenu.get().getInvestment()+1,(int)skMenu.get()) );
              ((Player*)object.raw())->skill[skMenu.get().skill] = skMenu.get();
              }
            else
              done = true;
              
            skMenu.redraw();
            break;
          
          default:
            break;
          }

        }
      
    
      break;
      }

		case kMap:

	    Map::display();
			break;

		case kMessages:

      Message::recent();
			turnTime = 0;
			break;

    case kCharInfo: {

      object->doPerform(aDetails);
      turnTime = 0;
      break;
      }

    case kMemory: {
			String print;
			print = "Catalogue items=";
			print += (long)Catalogue::numitems();
      Point3d Loc3d = object->getlocation();
      print += String(" Loc3d=(") + (long) Loc3d.getx() + ","
                                  + (long) Loc3d.gety() + ","
                                  + (long) Loc3d.getz() + ");";
      Point3d Focus = Map::focusPoint();
      print += String(" Focus=(") + (long) Focus.getx() + ","
                                  + (long) Focus.gety() + ","
                                  + (long) Focus.getz() + ");";

      print += String(" Levels=") + (long) Map::openLevel() + "; ";
      
#     ifdef SOFT_LINK
			print += " links=";
			print += (long)Catalogue::numlinks();
#     endif // SOFT_LINK
			Message::add(print,true);

#     ifdef MAP_LIMBO
      // If there's a list of items in limbo,
      if (Map::limbo.itemList().reset()) {

        Message::print();
        // Display them in a nice little menu:
        List<Target> limboList;
        
        do {
          limboList.add( Map::limbo.itemList().get() );
          } while (Map::limbo.itemList().next());
          
        Menu<Target> limboMenu( limboList, global::skin.inventory );
        limboMenu.setTitle( (long) Map::limbo.numData() + String(" items in Limbo") );
        limboMenu.display();
        limboMenu.getCommand();
        }
#     endif // MAP_LIMBO
      
			break;
      }

    case kQuitGame:
      break;

    case kHelp:

      keyHelp();
      
      break;
      
    default:

      if (key != kInvalid)
        Message::add("Key '\\"+Keyboard::keytostr(kRaw)+"' (\""+Keyboard::keyname(kRaw)+"\") not yet implemented or means nothing in this context.",true);
      else
        Message::add(String("Illegal command: ") << (int)key,true);

			if (global::isNew.help) {
    		Message::add("<?Press '\\" + Keyboard::keytostr(keyMap[kHelp].dkey) + "' for help.>");
				global::isNew.help = false;
				}
    
      break;

		}


  // If the player is riding,
  Target player_mount = object->getTarget(tg_mount);
  if (!!player_mount) {
    // They have to pass a piloting check every turn
  	switch ( object->test(sk_pilo,
                        // Difficulty is difficulty of mount:
                          player_mount->getSkill(sk_pilo),
                          THIS) ) {
      
      case result_fail:
      case result_fumble: {
      // Todo: rewrite this code so that:
      // 1. can't stagger nowhere (dx=0,dy=0)
      // 2. staggering like this makes movement
      //    mandatory - wall bump, attack, etc.
      
      // or risk staggering randomly:
  			location += Point3d( Random::randint(3)-1,
                             Random::randint(3)-1,
                             0 );
        if ( Map::limitsRect().contains(location) )
    			move = true;

        // And give a message:
        Message::add( String("You lose control of ")
                      << Grammar::Object( player_mount, Grammar::det_possessive )  );
        }
          
      default: {}
        // DO NOTHING
      }

    }

  // If the player has to move,
	// try to move the player or (failing that)
	// attempt to auto-act on the contents of the
	// tile:

	if (move) {
		//bool changedLev = false;

		// Will parent let us leave?
		if (! object->getTarget(tg_parent)->capable(aRelease, object) ) {
			// No.  See if there is an auto-action:

			Action action = object->getTarget(tg_parent)->exitAction();
			if ( action != aNone ) {

				object->perform( action, object->getTarget(tg_parent) );
				turnTime = GRANULARITY / object->getVal(stat_spd);
				}

			// No auto action. Tell the player off.
			else
  			Message::add("You cannot move!");
			}

		else {
    // Now that we've moved to a Map oriented tile management system
    // we don't have to worry about the "are we travelling inter-level"
    // question.  Just move:
    
			Target newPos = Map::getTile( location );


			// Will new location accept us?

			if (! newPos->capable(aContain, object) /*&& !changedLev*/ ) {

				// No.  See if there is an auto-action:

				// Todo: is the player blind, confused, etc?
				// If so, certain auto-actions should be supressed or prompted.

				Action action = newPos->autoAction();
				if ( action != aNone ) {

  				if (newPos->perform( action, Target( object ) ))
  					turnTime = GRANULARITY / object->getVal(stat_spd);
					}

				// No auto action. Tell the player off.
				else
					Message::add("You cannot move there!",true);
				}

			// All set to go; do the move:
			else {
        Screen::hidecursor();
				newPos->perform( aContain, object );
        Screen::showcursor();

				turnTime = GRANULARITY / object->getVal(stat_spd);

        /*
        Point3d levCoord( location.getx() / lev_width,
                        location.gety() / lev_height,
                        location.getz() );
        */
        
        //Map::focus(levCoord);
        //Map::focus(location);
				}

			}
		}

	// If the player hasn't acted make sure that
	// nobody else gets the next turn:
	if (turnTime == 0) turnTime = -1;
  
  // If no event yet exists for the player's next turn:
  if ( ! ActionEvent::exists( Target(THIS), aAct ) )
    // Create another event for the next movement turn:
  	ActionEvent::create( Target(THIS), aAct, turnTime );

  // Todo: perform any actions that the player can
  // concentrate on.  Quick Reload.

  // Update the player status bar:
  object->doPerform(aUpdateStatus);
  
	// If the player has quit,
  if (key == kQuitGame)
  	// indicate to the Timeline that the game is over:
    global::quit = global::quit_save;
    
	return true;
	}



#endif // PLAYERUI_CPP
