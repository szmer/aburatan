// [level.hpp]
// (C) Copyright 2000 Michael Blackney
#if !defined ( LEVEL_HPP )
#define LEVEL_HPP

class Level;

#include "creature.hpp"
#include "tile.hpp"
#include "rectngle.hpp"
#include "list.hpp"
#include "point.hpp"
#include "target.hpp"
#include "map.hpp"



class Tile;

class Level {
	public:
		// Construction & Destruction
		~Level( void );

    void packUp( void ); // Prepare the level for putting away

		void redraw( void ); // Re-draw the whole level

		Target getTile( const Point3d& ); // Returns a temporary object that has
                               // access to the level private members
                               // and will be deleted immediately unless
                               // an object sets it as its parent.

    void tileCreate( Tile::Option t_opt );

		void      setBorder( Rectangle );
		Rectangle getBorder( void );

		void addFeature( const Target& );
		//void addCreature( const Target& );
		void removeFeature( const Target& );
		//void removeCreature( const Target& );
		void addCloud( const Target& );
		void removeCloud( const Target& );
		//void addLight( const Target& );
		//void removeLight( const Target& );
		//void calcLights( void );

    // featureList is required because we now no
    // longer keep track of each tile's feature...
		List<Target> &featureList( void );
    
    // but these lists are no longer required:
		// //List<Target> &creatureList( void );
		// //List<Target> &lightList( void );

		struct gridRec {
			int  opacity;
			char setvisible;
			};

    //struct orthoRec {
    //  Orthog visible;
    //  int opacity;
    //  };

    /*
    void allocOrtho( void );
    void clearOrtho( void );
    */

    void identify( IDmask );

    //void sunrise( void );
    //void sunset( void );

		Point3d loc( void );

    Image getimage( void );
    String name( void );

    void setLit( int, int, const Target& );
    void calcOpacity( Point );
		void placeDoors( void );
    void sprinkle( List<Target> & );

    void incOdor( int delta = 1 );
    Odor getOdor( void );

    // This function only required until the big move
    // to Map tile access:
    int getOpacity( int, int );
    
	protected:

    // OLD LEVEL MEMBER:
		// 2d array of pointer-to floor tiles:
		// Target*** tiles;
    // REPLACED BY MEMBERS:

		// Access to the following functions is restricted
		// to force access through the Map class
    long tileVal( const Point3d&, StatType );
    bool tileAdd( const Target&, const Target& );
    bool tileRemove( const Point3d&, const Target& );
		void tileDraw( const Point3d& );
		void tileView( const Point3d&, int, int );
		//void tileLight( const Point3d&, const Orthog& );
		void tileUnview( const Point3d& );
		//void tileUnlight( const Point3d& );
		void tileSetTHIS( const Point3d&, TargetValue * );
    bool tileCapable( const Point3d&, Action, const Target& =Target() );
    bool tilePerform( const Point3d&, Action, const Target& =Target() );
    void tileTakeDamage( const Point3d&, const Attack &a );
		List<Target> *tileItemList( const Point3d& );
    Image tileImage( const Point3d& );
		String tileName( const Point3d&, NameOption );
		Action tileAutoAction( const Point3d& );
    Target tileGetTarget( const Point3d&, targetType );
    //Target findTarget( const Point3d&, targetType, const Target& );
		// Action tileExitAction( Point ); // Not implemented - may not be required.


    // We have two structures: simple and complex.
    // Simple has only simple data types (nothing with pointers or
    // redirection) so we can dump the whole thing to disk in one go.
    // Complex has all the fiddly bits that get in the way of
    // the otherwise lightning-quick file io.
    
    struct TileSimple {
      materialClass
        f_material,  // Material of the floor
        o_material;  // Material of the object on it - wall, tree, etc.

      TileType type;

      int visible, hypotenuse;

      Image last_image;

      /*
      // Graffiti here adds too much bulk to the TileSimple struct.
      // We should implement graffiti as a type of sign, and
      // place it in the RawTarget family.
      Name graffiti; // Names can thankfully be dumped this way!

      Etching etching; // The shape of etching (words, smear, footprints)

      materialClass
        e_material; // Material used to make graffiti / smears
      */
      
      } tile_simple [lev_width][lev_height];
      
    struct TileComplex {

      Sorted<Target> *inventory;

      Target tile; // The temporary object that has access to our
                   // speedier, mem-efficient tile functionality.

      // Feature is now calculated from the feature list:
      // Target feature; // Within which feature is this tile?
      
      } tile_complex [lev_width][lev_height];

		// List of features:
		Sorted<Target> features;

    // No longer used:
		// //List of creatures:
		// //List<Target> creatures;
    //
		// //List of lightsources:
		// //List<Target> lights;

		// List of clouds:
		List<Target> clouds;

		// Coordinates of level:
		Point3d mapCoord;

    // Nature of monsters generated:
    Odor odor;
    
		// Outer limits of tiles:
		//static Rectangle borderRect;
		Rectangle borderRect;

    // buffer for LOS and lighting:
    //orthoRec *ortho;

		// Generic empty space where monsters are
		// kept when the player leaves the level.
    // Not currently used.
		Target foyer;

    // The following classes want access to the
    // short-circuit functions tileVal etc.
		friend class LevelHandle;
    friend class Map;
    friend class Tile;

		Level( const Point3d& );

    explicit Level( Level& ) {}
		void create( void );

		static Level *load( const Point3d& );
		void loadLevel( void );

    void setOpacity( int, int, int );
		void placeTerrain( void );
		void placeMons( void );

	};

#endif // LEVEL_HPP
