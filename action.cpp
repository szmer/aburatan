// [action.cpp]
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
#if !defined ( ACTION_CPP )
#define ACTION_CPP

#include "action.hpp"
#include "input.hpp"
#include "visuals.hpp"

String Action::menustring( char len ) {

	String retval;

	switch( value ) {
		case aRelease:
			retval = keyMap[ kDrop ].name;
			break;
		case aWear:
			retval = keyMap[ kWear ].name;
			break;
		case aSwitch:
			retval = keyMap[ kSwitch ].name;
			break;
		case aTakeOff:
			retval = keyMap[ kTakeOff ].name;
			break;
		case aAdjust:
			retval = keyMap[ kAdjust ].name;
			break;
		case aOpen:
			retval = keyMap[ kOpen ].name;
			break;
		case aDisarm:
			retval = keyMap[ kDisarm ].name;
			break;
		case aWield:
			retval = keyMap[ kWield ].name;
			break;
		case aBuild:
			retval = keyMap[ kBuild ].name;
			break;
		case aLoad:
			retval = keyMap[ kLoad ].name;
			break;
		case aUnload:
			retval = keyMap[ kUnload ].name;
			break;
		case aEat:
			retval = keyMap[ kEat ].name;
			break;
		case aQuaff:
			retval = keyMap[ kQuaff ].name;
			break;
		case aApply:
			retval = keyMap[ kApply ].name;
			break;
    case aRead:
			retval = keyMap[ kRead ].name;
			break;
    case aChat:
			retval = keyMap[ kChat ].name;
			break;


    case aBuy:
      retval = "Buy";
      break;
      
    case aSell:
      retval = "Sell";
      break;
      
    case aSettle:
      retval = "Settle account";
      break;

    default:
      retval = "<!Undefined!>";
      assert(0);
      break;
                
		}

	return retval.abbrev( len );
	}

Image Action::menuletter( void ) {

	char col = cDefault, app = '?';
	Command key = kInvalid;

	switch( value ) {
		case aRelease:
			key = kDrop;
			break;
		case aWear:
			key = kWear;
			break;
		case aSwitch:
			key = kSwitch;
			break;
		case aTakeOff:
			key = kTakeOff;
			break;
		case aAdjust:
			key = kAdjust;
			break;
		case aOpen:
			key = kOpen;
			break;
		case aDisarm:
			key = kDisarm;
			break;
		case aWield:
			key = kWield;
			break;
		case aBuild:
			key = kBuild;
			break;
		case aLoad:
			key = kLoad;
			break;
		case aUnload:
			key = kUnload;
			break;
		case aEat:
			key = kEat;
			break;
		case aQuaff:
			key = kQuaff;
			break;
		case aApply:
			key = kApply;
			break;
		case aRead:
			key = kRead;
			break;
		case aChat:
			key = kChat;
			break;

    case aBuy:
      app = 'b';
      break;
      
    case aSell:
      app = 's';
      break;
      
    case aSettle:
      app = 'S';
      break;

		default:
			return Image(cLRed, '?');
		}

  if (key != kInvalid) {
  	app = keyMap[key].key;
  	if (!app) app = keyMap[key].dkey;
    }

	return Image(col, app);

	}

#endif // ACTION_CPP


