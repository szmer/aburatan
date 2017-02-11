// [rawtargt.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( RAWTARGT_HPP )
#define RAWTARGT_HPP


#include "point.hpp"
#include "viewable.hpp"
#include "point.hpp"
#include "types.hpp"
#include "enum.hpp"
#include "string.hpp"
#include "target.hpp"
#include "list.hpp"
#include "sorted.hpp"
#include "attack.hpp"
#include "action.hpp"
#include "dir.hpp"
#include "skill.hpp"
#include "rectngle.hpp"

#include "compiler.hpp"
#ifdef OBSOLETE_INCLUDE
#  include <fstream.h>
#else
#  include <fstream>
#endif

class Target;
class Level;

enum structType {

  // Melee Combat
  s_offence_cv, // Offensive
  s_defence_cv, // and defensive combat values

  // Ranged Combat
  s_shoot_cv, // Shooting
  s_dodge_cv, // and dodging combat values
  
  s_border, // Rectangle: border

  s_num,
  s_invalid = s_num
  
  };

// Target types:
enum targetType {
	tg_target,

	tg_feature,
		tg_field,
		tg_fourfield,
		tg_solid,
		tg_house,
		tg_forest,
		tg_stairway,
		tg_corridor,
    tg_town,
    tg_mech_mines,
    tg_abura_tan,
    tg_gilead,
    tg_texarkana,
    tg_churchyard,
    tg_perimeter,
    tg_graveyard,
    tg_basement,
    tg_sewer,
    tg_road,
    tg_special_room,
    tg_first_level,

	tg_actor,  // Actors
		tg_flight,
		tg_brain,
		tg_player_ui,
    tg_walkdir,

  tg_quest,

	tg_item,         // Items - anything with a 'physical' nature.
    tg_cloud,      //  Clouds at the top of stacks

		tg_creature,   //  Order in this list determines stacking order
			tg_player,   //  when items are in a pile on the floor
			tg_monster,  //  and when in inventories.
			tg_human,
      tg_mount,
      
    // All items that can be carried should follow tg_weapon
    // so that weapons are always be accessible first.
    tg_buildable,
  		tg_weapon,
  			tg_melee,
  			tg_ranged,
  		tg_clothing,
		tg_trap,
    tg_machine_part,
    tg_remains,
		tg_ammo,

		tg_food,

    tg_book,

    //tg_light,
    tg_scope,

		tg_container,
			tg_bottle,
			tg_quiver,
			tg_bag,

    tg_money,

		tg_fluid,

    tg_seat,
    tg_altar,
    tg_headstone,
    tg_grave,
    
		tg_door,
		tg_flora,
		tg_wall,
    tg_fence,
		tg_stairs,

	tg_quantity,     // A stack of items.

	tg_tile,
		tg_floor,

	tg_area,
		tg_sea,
		tg_plains,
		tg_hills,
		tg_mountains,
		tg_summit,
		tg_settlement,

	tg_task,

	tg_temp,


  // The following targetTypes are for use with
  // the functions getTarget() and findTarget()
  
  tg_owner,       // Who legally (in-game) owns the item?
  tg_parent,      // What is the item contained within?
  //tg_feature,   // Which level feature is the item within?
  tg_possessor,   // Which creature currently possesses the item?
  tg_controller,  // Which creature last had control over the item?
  tg_top_blocker, // What is the topmost blocking item in a tile?

	tg_invalid
	};

// Point value for Target types that are not on the map:
#define OFF_MAP Point3d(-1,-1,-1)

class RawTarget :	public Viewable {

	public:

    struct Option {
      Image image;
      //Point3d location;
      Option(void) : image(0)/*, location(OFF_MAP)*/ {}
      };
  
		RawTarget( const Option* );
		RawTarget( std::iostream&, TargetValue * );
		virtual ~RawTarget( void );
		virtual targetType gettype( void ) const = 0;
		virtual bool  istype( targetType ) const = 0;
		virtual String menustring( char len = 80 );
		virtual String name( NameOption=n_default ) = 0;
		virtual Image menuletter( void ) = 0;
		virtual void setletter( char ) = 0;

		virtual int quantity( void );
		virtual Target detach( int = -1 );

		virtual void draw( void );
		virtual Point3d getlocation( void );
    virtual Rectangle getborder( void );

		virtual Target clone( void ) = 0;
		virtual void toFile( std::iostream & ) = 0;

		virtual bool capable( Action, const Target &t ) = 0;
		virtual bool perform( Action, const Target &t ) = 0;
		virtual bool isCapable( Action );
		virtual bool doPerform( Action );

    virtual void gainExp( long );

    virtual void identify( IDmask );
    virtual bool identified( IDmask );

    //virtual void light( const Orthog & );
    virtual void  view( int, int );
    //virtual void unlight( void );
    virtual void  unview( void );

		virtual Action autoAction( void );
		virtual Action exitAction( void );

    virtual void setParent( TargetValue * );
    /** // These functions"
    virtual Target getowner( void ) = 0;
		virtual Target getparent( void );
		virtual Target getfeature( void );
    virtual Target getdeliverer( void );
    virtual Target getpossessor( void );
    **/ // replaced by:
    
    virtual Target getTarget( targetType );
    //virtual Target findTarget( targetType, const Target& );
    
		virtual Level &getlevel( void );

    //virtual void   setParent( const Target &t );
    //virtual void   setlocation( const Point3d& );

		virtual bool equals( const Target& ) = 0;  // are we stackable?

		virtual List<Target> *itemList( void );

		bool operator < ( RawTarget& );
		bool operator > ( RawTarget& );
		virtual compval compare( RawTarget& );

		virtual long getVal( StatType );
    virtual long getSkill( Skill, bool = true );
    virtual void modSkill( Skill, int = 0 );
    virtual Result test( Skill, int, const Target& );

		virtual Attack getAttack( void );
		virtual void takeDamage( const Attack& );

    virtual bool getStruct( structType, void*, size_t );
#   define  GET_STRUCT(a,b) getStruct(a,(void*)&b,sizeof(b))

    Target Force( String );

    // The following classes require access to parent link:
    friend class Creature;
    friend class Human;
    friend class Buildable;
    // The following classes require access to the THIS pointer:
    friend class Level;

		class Link {
			public:
				Link( TargetValue* );
				Link( long );
        ~Link();
				Target toTarget( void ) const;

				bool operator ==( TargetValue* ) const;
				bool operator !=( TargetValue* ) const;
        bool operator ! ( void ) const;

				Link &operator =( TargetValue* );
//				Link &operator =( long );
#       ifdef SOFT_LINK
        void unlink( void );
#       endif // SOFT_LINK
        bool inMem( void );
        
				RawTarget* operator->();

			private:
				mutable union LinkValue {
					TargetValue* pointer;
					long serial;
					} value;

				mutable enum LVType {
					isPointer,
					isSerial
					} type;

      friend std::iostream &operator << ( std::iostream &, RawTarget::Link & );
      friend std::iostream &operator >> ( std::iostream &, RawTarget::Link & );
			};

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.

		TargetValue *THIS;
		List<Target> *inventory;

    //Point3d location;

		Link parent;

    IDmask id_mask;

		RawTarget( RawTarget & );

		friend struct TargetValue;
		virtual void clr( void );

    virtual void allocInventory( void );

		virtual void add( const Target & );
		virtual bool remove( const Target & );

		virtual void setTHIS( TargetValue* );

	};

std::iostream &operator << ( std::iostream &, RawTarget::Link & );
std::iostream &operator >> ( std::iostream &, RawTarget::Link & );


// Macros for create functions:
#define METHOD_create_option(CLASSNAME);		static Target create( const Option &o ) {\
			CLASSNAME *instance = new CLASSNAME(&o);\
			TargetValue *THIS = new TargetValue( instance );\
			instance->setTHIS(THIS);\
			return Target(THIS);\
			}

#define METHOD_create_default(CLASSNAME);   static Target create( void ) {\
      Option opt_default;\
      CLASSNAME *instance = new CLASSNAME(&opt_default);\
			TargetValue *THIS = new TargetValue( instance );\
			instance->setTHIS(THIS);\
			return Target(THIS);\
      }

#define METHOD_create_br_opt(CLASSNAME);		static Target create( const Option &o ) {\
			CLASSNAME *instance = new CLASSNAME(&o);\
			TargetValue *THIS = new TargetValue( instance );\
			instance->setTHIS(THIS);\
    	Target tThis = Target(THIS);\
      Brain::Option brain_opt;\
      brain_opt.object = tThis;\
    	instance->brain = Brain::create(brain_opt);\
			return Target(THIS);\
			}

#define METHOD_create_br_def(CLASSNAME);   static Target create( void ) {\
      Option opt_default;\
      CLASSNAME *instance = new CLASSNAME(&opt_default);\
			TargetValue *THIS = new TargetValue( instance );\
			instance->setTHIS(THIS);\
    	Target tThis = Target(THIS);\
      Brain::Option brain_opt;\
      brain_opt.object = tThis;\
    	instance->brain = Brain::create(brain_opt);\
			return Target(THIS);\
      }



#endif // RAWTARGT_HPP
