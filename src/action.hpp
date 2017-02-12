// [action.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( ACTION_HPP )
#define ACTION_HPP

#include "string.hpp"
#include "types.hpp"

enum ActionValue {
// Any modifications to this must be made also to
// the action data list! (When the data list exists...)

	aNone, // No action (do nothing)

	aOpen,  // Open and
	aClose, // Close, as in doors, chests, etc.
  aBeOpened,
  aBeClosed,

	aRelease, // Is the owner willing to relinquish control?
	aContain, // Is the item willing to accept control?

  aDrop,
  aPickUp,

	aLeave,
	aEnter,

	aBlock,  // Block entry of same location (ie. walls, other monsters)

	aIgnite,

  aPolymorph,
  aIdentify,

	aFire,
  aThrow,
  aThrowAt,
	aAttack,
  aBeAttacked,
	aKick,
  aBeKicked,
  aMount,
  aBeMounted,

  aPray,

  aParry,

	aScatter,

	aHeal, // Natural healing/regeneration
  aSwim, // for periodic swimming checks

	aFix,
  aBuild,
  aEtch,

	aEat,
	aBeEaten,
	aQuaff,
	aBeQuaffed,
  aMetabol,
  aRot, // For rotting food
  
  aRead,
  aBeRead,

  aChat,
  aRespond,

  aBuy,
  aSell,
  aAddBill,    // Add item to bill
  aDeductBill, // Deduct item from bill
  aSettle,     // Settle bill
  aPayFor,

  aApply,
  aBeApplied,
  
	aWear,
	aBeWorn,  // Is the item capable of being worn?

	aWield,   // Wield as primary weapon
	aBeWielded,  // Can the item be wielded?
	aSwitch,  // Switch primary & secondary weapons

	aTakeOff,

	aSearch,
	aFind,
	aDisarm,
  aBeDisarmed,

	aAscend,
	aDescend,
  aBeAscended,
  aBeDescended,

  aTellTime,

  aAssault, // Capable of unprovoked assault?

	aFly,     // For objects in flight: bullets, kicked items
	aLand,    // Items must lie still else they won't stack.

	aRestack, // Re-adds item to parent's inventory in attempt to stack
	aCombine,

	aLoad,    // Container loading,
	aUnload,  //           unloading
  aQuickReload, // reload weapon in hands from quiver/bandolier

  aEnterLev,// Remove from appropriate level list
  aLeaveLev,// Add to appropriate level list

  aAlertShareSquare,// Alerts an item of another entering the same tile
	aAlertTrespass,   // Alerts a creature of a trespasser
  aAlertPetDam,     // Alerts a creature that their pet is being attacked
  aWarn,            // Warns a creature of a hostile presence

	aWake,    // Tell a monster's brain to become active
	aSleep,   // Tell a monster's brain to become dormant

  aSee,     // Senses, used for special cases/
  aHear,    // special creatures where cannot
  aSpeak,   // see, hear, speak, taste, touch.
  aTaste,
  aTouch,

  //aSunrise, // Tell a tile that it mightn't need a lightsource
  //aSunset,  // Tell a tile that there is no more sunlight

	aView,    // Is the RT visible?
	aSeeInv,  // Does the RT hide its inventory?  NOT see invisible!

  aGainExp, // Gain an item or monster's experience (on id/death)
  aGainLev, // Automatically go to the next experience level
  
  aSetController, // Tell an item who is in control.
                  // Used to determine who gets the Exp.
  aSetOwner, // In the game, set who legally owns an item
             // Used for shops and theft

	// Player interface specific
	aDescribe,
  aDetails,
  aLookAround,
  aSniffAround,
	aSpMessage,
	aAdjust,
	aUpdateStatus,

	// LOS - player specific
	aLOS,
	aStartTurn,

  // Lighting
  //aLight,

	// Actor (AI) specific
	aAct,
  aAddQuest,

  // Feature specific
  aSetFeature, // Set a Tile's feature reference
  aPlaceMons,  // when performed, returns true if monsters were placed


  aAll

	};


struct Action {

	ActionValue value;

	Action( void ) : value(aNone){}
	Action( ActionValue av ) : value(av){}
	operator ActionValue( void ) { return value; }

	String menustring( char len = 80 );
	Image menuletter( void );

	};

#endif // ACTION_HPP

