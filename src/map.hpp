// [map.hpp]
// (C) Copyright 2003 Michael Blackney
#if !defined ( MAP_HPP )
#define MAP_HPP

#include "point.hpp"
#include "rectngle.hpp"
#include "sorted.hpp"
#include "target.hpp"
#include "dir.hpp"
#include "compiler.hpp"
#include "rawtargt.hpp"
#ifdef OBSOLETE_INCLUDE
#  include <fstream.h>
#else
#  include <fstream>
#endif


// Forward declarations:
class Level;
class LevelHandle;
class Req;
struct Action;

class Map {

	public:

		enum levType {
			// Any modifications to this enum must
			// be reflected in appropriate changes
			// to LevTypeDetails levTypes (in map.cpp)
      
			mountains,
			hills,
      
			forest,
			swamp,
			plains,
      
			desert,
      waste,

			river,
      
      ocean,
      shore,

			num_types,
      any = num_types,
			invalid = num_types
			};


    static Target getTile( const Point3d& );

    static Point3d LevScale( const Point3d& );
    static Point3d TileScale( const Point3d& );

    /*
    // Limbo not being used ATM but code could be usedul in
    // the future:
    static struct Limbo {

      void add( TargetValue* );
      void remove( TargetValue* );
      
      //Target findTarget( targetType, const Target& );

      int numData( void ) { return items.numData(); }

      List<TargetValue*> &itemList( void ) { return items; }

      private:
        List<TargetValue*> items;

      } limbo;
    */

    struct Bin {

      public:

        Bin( Point3d );
        Bin( std::iostream& );
        void addTile( Point3d, Target );
        void addFeature( Target );

        struct TilePoint3d {
          TilePoint3d( std::iostream& );
          TilePoint3d( Point3d, Target );
          Target target;
          Point3d p3d;
          };

        List<Target> &featureList( void );
        List<TilePoint3d> &tileList( void );
        
        Point3d getLocation( void ) { return location; }

        friend std::iostream &operator << ( std::iostream &, Map::Bin & );

      private:

        Point3d location;
        
        List<Target> features;
        List<TilePoint3d> tiles;

      };

    static void binAdd( Bin * );
    static Bin *binRemove( Point3d );

		static LevelHandle &get( const Point3d& );
    static LevelHandle &focus( const Point3d& );
    static void streamData( void );
    static int  openLevel( const Point3d& = OFF_MAP );
    static void keepOpen( const Rectangle&, int );
    
		static bool created( const Point3d& );
		static levType type(Point);

		static void create( void );
		static void load( void );
		static void save( void );

		static void display( void );

    static Point3d startPoint( void ) { return start_point; }
    static Point3d focusPoint( void ) { return focus_pt; }
    static Rectangle limitsRect( void ) { return limits; }

		static void redraw( void ); // Re-draw the whole map
		static void LOS( const Target& ); // Calc Line of Sight
    static void sunrise( void );
    static void sunset( void );
    
    static void setOpacity( const Point3d&, int );
    static void calcOpacity( const Point3d& );

    static void addCreature( const Target& );
    static void removeCreature( const Target& );
    static List<Target> &creatureList( void );

		//static void addLight( const Target& );
		//static void removeLight( const Target& );
		//static void calcLights( void );

    static const Point3d &visOffset( void ) { return vis_offset; }
    static Rectangle getVisible( void );

	private:
    static Rectangle limits;
    static Point3d start_point;

    // focus_pt keeps track of the centered level:
    static Point3d focus_pt;
    // player_lev_pt keeps track of the player's level coordinates:
    static Point3d player_lev_pt;
    // lev_offset is added to all map locations to calculate the
    // screen location:
    static Point3d vis_offset;
    
    // open_levels records the levels that are currently loaded in memory:
    static List<Point3d> open_levels;
    // open_limit is the visible level space:
    static Rectangle open_limit;
    static int open_z;

		//List of creatures:
		static List<Target> creatures;

		//List of lightsources:
		//static List<Target> lights;

    // quick_level is a 2d array of short-circuit pointers to
    // handles for levels that will be accessed the most:
    static LevelHandle ***quick_level;
    
    // bin_list lists all the level features,
    // creatures, etc. for closed levels:
    static List<Bin*> bin_list;

    // Structures for LOS:
		struct gridRec {
			int opacity;
			int setvisible;
			};

    struct orthoRec {
      //Orthog /*lit,*/ visible;
      int visible;
      int opacity;
      };

    // private functions because they are more for Map internal
    // function speed, and are a little clumsier and less OO than
    // the getTile() function:
    static long tileVal( const Point3d&, StatType );
		static void tileDraw( const Point3d& );
		static void tileView( const Point3d&, int, int );
		static void tileUnview( const Point3d& );
    static bool tileCapable( const Point3d&, Action, const Target& );
    static bool tilePerform( const Point3d&, Action, const Target& );
		static Action tileAutoAction( const Point3d& );
		// static Action tileExitAction( Point ); // Not implemented - may not be required.

    // buffer for LOS and lighting:
    static orthoRec *ortho;
    static void allocOrtho( void );
    static void clearOrtho( void );
    static void shiftMap( Point );

		static Sorted<LevelHandle> ***levels;

		static void allocate( void );


    friend class Player;
    friend class Level;
    

	};

std::iostream &operator << ( std::iostream &, Map::Bin & );


class Req {

	public:
		enum Type {
      // Placement in this list defines order of creation.

      // Types that must be placed *first*:
			stairs_up,
			stairs_down,

      // All level types:

      mech_mine,   // The Mechanical Mines
      abura_tan,   // The citadel, and the ruins beneath
      gilead,      // The castle-city Gilead
      texarkana,   // Texarkana
      catacombs,   // The catacombs
      sewer,       // The ancient sewers
      
      town,        // Temporary town.  Limited to a single level
      /* // -- These requirements may come in later, but
         //    for now they're not used and only blur the theme:
			clear_area,

      lost_city,   // Native lost city, inhabited by spaniards
      camp,        // Navajo camp
      
      ghost_town,  // Colonial ghost town
      
      basement,
      sewer,

      fort,        // Colonial fort
      manor,       // Goreyesque, crumbling hall
      oasis,       // Mid-desert oasis
      ruins,
      asylum,      // Goreyesque asylum
      workshop,    // Mech parts and Mechanical men
      cemetery,    // Graves, church
      evil_garden, // Gorey's Evil Garden - Eintritt Frei!

      
      first_lev,   // The level you begin on
      ---*/
      
      // Smaller types:

      // Individual objects placed last:
      prefab_object, // Place a predefined object

			num,
			invalid = num
			};

		Req();
		explicit Req( Target );
    Req( Type, Target );
		Req( Type, materialClass m = m_stone );
		Req( Type, Point, materialClass m = m_stone );
		Req( Type, Rectangle, materialClass m = m_stone );
		Req( Type, Rectangle, Point, materialClass m = m_stone );
		const Rectangle &getarea( void );
		const Point &getlocation( void );

		bool operator == ( const Req& ) const;
    Map::levType terrain( void ) const;
    
		void create( Level & );
    Type getType( void );

		bool operator < ( const Req& );
		bool operator > ( const Req& );

		friend STDCLASS std::iostream &operator << ( std::iostream &, Req & );
		friend STDCLASS std::iostream &operator >> ( std::iostream &, Req & );

	private:
		Type type;
		Point location;
		Rectangle area;
		materialClass material;

		enum Place_Type {
			none, point, rect, both
			} place_by;

    Target object;

	};


class LevelHandle {
	public:
		LevelHandle( const Point3d& );
		LevelHandle( const LevelHandle& );
		~LevelHandle( void );



		void load( void );
		void save( void );
    bool isCreated( void );
    bool inMemory( void );

		Level* operator->();
		Level& operator*();

		Point3d loc( void );

		void addRequirement( const Req& );
		Sorted<Req> &Requirements( void );

		bool operator > ( const LevelHandle& ) const;
		bool operator < ( const LevelHandle& ) const;
		bool operator == ( const LevelHandle& ) const;

		LevelHandle &operator = ( const LevelHandle& );

//    void setImage( Image );
    Image getImage( void ) const;

	private:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		Level *lev;
		Point3d mapLoc;

		mutable bool master;
    bool created;

    Image image;

		Sorted<Req> requirements;

		void checklev( void );

		friend class Map;
		LevelHandle( std::iostream& );
		void toFile( std::iostream& );


	};


#define EW_LEV_BORDER 2
#define NS_LEV_BORDER 1
#define ORTHO_WIDTH  (lev_width  * ( EW_LEV_BORDER * 2 + 1))
#define ORTHO_HEIGHT (lev_height * ( NS_LEV_BORDER * 2 + 1))

#endif // MAP_HPP
