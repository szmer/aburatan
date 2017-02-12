// [features.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( FEATURES_HPP )
#define FEATURES_HPP

#include "feature.hpp"
#include "dir.hpp"

class Field : public Feature {

	public:

    struct Option : public Feature::Option {
      materialClass material;
      Option( void ) : material(m_invalid) {}
      };
      
		static Target create( const Option &o ) {

			Field *field = new Field( &o );
			TargetValue *THIS = new TargetValue( field );
			field->setTHIS(THIS);

			Target target = THIS;

			field->build(o);

			//l.addFeature( target );

			return Target(target);
			}

		static Field *load( std::iostream &ios, TargetValue *TVP ) {
			return new Field(ios, TVP);
			}

		virtual void toFile( std::iostream & );


		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual String name( NameOption=n_default );

		void build( const Option& );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		Field( const Option * );
		Field( std::iostream&, TargetValue *TVP );

		materialClass type;

	};


class Solid : public Feature {

	public:

    struct Option : public Feature::Option {
      materialClass material;
      Option( void ) : material(m_invalid) {}
      };
      
		static Target create( const Option &o ) {

			Solid *solid = new Solid( &o );
			TargetValue *THIS = new TargetValue( solid );
			solid->setTHIS(THIS);

			Target target = THIS;

			solid->build(o);

			//l.addFeature( target );

			return Target(target);
			}

		static Solid *load( std::iostream &ios, TargetValue *TVP ) {
			return new Solid(ios, TVP);
			}

		virtual void toFile( std::iostream & );


		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual String name( NameOption=n_default );

    void build( const Option& );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		Solid( const Option* );
		Solid( std::iostream&, TargetValue *TVP );

		materialClass type;

	};


class House : public Feature {

	public:

    struct Option : public Feature::Option {
      Shop shop;
      int max_shops;
      bool split;
      Option( void ) : shop(shop_none), max_shops(0), split(true) {}
      };
      
		static Target create( const Option &o ) {

			House *house = new House( &o );
			TargetValue *THIS = new TargetValue( house );
			house->setTHIS(THIS);
			Target target = THIS;

			house->build(o);

      // Added to level in build()
			// l.addFeature( target );

			return target;
			}

		static House *load( std::iostream &ios, TargetValue *TVP ) {
			return new House(ios, TVP);
			}

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual String name( NameOption=n_default );
		virtual bool perform( Action, const Target &t );

    void build( const Option& );
    long getVal( StatType );

		virtual void toFile( std::iostream & );
    
	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
    Shop shop;
  
		House( const Option* );
		House( std::iostream&, TargetValue *TVP );

	};

class Forest : public Feature {

	public:

    struct Option : public Feature::Option {
      Option( void ) {}
      };

		static Target create( const Option &o ) {

			Forest *forest = new Forest( &o );
			TargetValue *THIS = new TargetValue( forest );
			forest->setTHIS(THIS);
			Target target = THIS;

			forest->build(o);

			//l.addFeature( target );

			return target;
			}

		static Forest *load( std::iostream &ios, TargetValue *TVP ) {
			return new Forest(ios, TVP);
			}

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual String name( NameOption=n_default );

    void build( const Option& );
    long getVal( StatType );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		Forest( const Option* );
		Forest( std::iostream&, TargetValue *TVP );

	};

class Stairway : public Feature {

	public:

    struct Option : public Feature::Option {
      char connecting;
      Point3d location;
      Option( void ) : connecting(0), location(OFF_MAP) {
        image=Image(cGrey,'>');
        }
      };
      
		static Target create( const Option &o ) {

			Stairway *stairway = new Stairway( &o );
			TargetValue *THIS = new TargetValue( stairway );
			stairway->setTHIS(THIS);
			Target target = THIS;

			stairway->build(o);

			//l.addFeature( target );

			return target;
			}

		static Stairway *load( std::iostream &ios, TargetValue *TVP ) {
			return new Stairway(ios, TVP);
			}

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual String name( NameOption=n_default );

    void build( const Option& );
		virtual long getVal( StatType );

		virtual void toFile( std::iostream & );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		char connecting;

		Stairway( const Option* );
		Stairway( std::iostream&, TargetValue *TVP );

	};


class Corridor : public Feature {

	public:

    struct Option : public Feature::Option {
      Point start, finish;
      bool include_ends;
      bool stop_on_floor;
      materialClass material;
      Option( void )
      : start(0,0),
        finish(0,0),
        include_ends(false),
        stop_on_floor(false),
        material(m_invalid) {}
      };
      
		static Target create( const Option &o ) {

			Corridor *corridor = new Corridor( &o );
			TargetValue *THIS = new TargetValue( corridor );
			corridor->setTHIS(THIS);
			Target target = THIS;

			corridor->build(o);

			//l.addFeature( target );

			return target;
			}

		static Corridor *load( std::iostream &ios, TargetValue *TVP ) {
			return new Corridor(ios, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual String name( NameOption=n_default );

    void build( const Option& );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		Point finish;
		materialClass type;

		Corridor( const Option* );
		Corridor( std::iostream&, TargetValue *TVP );

	};


class FourField : public Feature {

	public:

    struct Option : public Feature::Option {
      materialClass north, east, south, west;
      Option( void ):north(m_invalid), east(m_invalid),
                     south(m_invalid), west(m_invalid) {}
      };

		static Target create( const Option &o ) {

			FourField *field = new FourField( &o );
			TargetValue *THIS = new TargetValue( field );
			field->setTHIS(THIS);

			Target target = THIS;

			field->build(o);

			//l.addFeature( target );

			return Target(target);
			}

		static FourField *load( std::iostream &ios, TargetValue *TVP ) {
			return new FourField(ios, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual String name( NameOption=n_default );

    void build( const Option& );
    long getVal( StatType );


	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		FourField( const Option* );
		FourField( std::iostream&, TargetValue *TVP );

	};



class Town : public Feature {

	public:

    struct Option : public Feature::Option {
      Option( void ) {
        image=Image(cGrey,'#');
        }
      };
      
		static Target create( const Option &o ) {

			Town *town = new Town( &o );
			TargetValue *THIS = new TargetValue( town );
			town->setTHIS(THIS);

			Target target = THIS;

			town->build(o);

			//l.addFeature( target );

			return Target(target);
			}

		static Town *load( std::iostream &ios, TargetValue *TVP ) {
			return new Town(ios, TVP);
			}

		virtual void toFile( std::iostream & );


		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual String name( NameOption=n_default );

    void build( const Option& );
    long getVal( StatType );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		Town( const Option* );
		Town( std::iostream&, TargetValue *TVP );

	};

class Churchyard : public Feature {

	public:

    struct Option : public Feature::Option {
      Option( void ) {
        image=Image(cWhite,'+');
        }
      };
      
		static Target create( const Option &o ) {

			Churchyard *churchyard = new Churchyard( &o );
			TargetValue *THIS = new TargetValue( churchyard );
			churchyard->setTHIS(THIS);

			Target target = THIS;

			churchyard->build(o);

			//l.addFeature( target );

			return Target(target);
			}

		static Churchyard *load( std::iostream &ios, TargetValue *TVP ) {
			return new Churchyard(ios, TVP);
			}

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual String name( NameOption=n_default );

    void build( const Option& );
    long getVal( StatType );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		Churchyard( const Option* );
		Churchyard( std::iostream&, TargetValue *TVP );

	};


class Graveyard : public Feature {

	public:

    struct Option : public Feature::Option {
      Option( void ) {
        image=Image(cGrey,'+');
        }
      };
      
		static Target create( const Option &o ) {

			Graveyard *graveyard = new Graveyard( &o );
			TargetValue *THIS = new TargetValue( graveyard );
			graveyard->setTHIS(THIS);

			Target target = THIS;

			graveyard->build(o);

			//l.addFeature( target );

			return Target(target);
			}

		static Graveyard *load( std::iostream &ios, TargetValue *TVP ) {
			return new Graveyard(ios, TVP);
			}

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual String name( NameOption=n_default );

    void build( const Option& );
    long getVal( StatType );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		Graveyard( const Option* );
		Graveyard( std::iostream&, TargetValue *TVP );

	};


class Perimeter : public Feature {

	public:

    struct Option : public Feature::Option {
      List<Rectangle> rect_list;
      materialClass material;
      int largest_gap;
      bool is_wall;
      Option( void ) :rect_list(), material(m_iron),
        largest_gap(6), is_wall(false) {
        image=Image(cDGrey,'#');
        }
      };
      
		static Target create( const Option &o ) {

			Perimeter *perimeter = new Perimeter( &o );
			TargetValue *THIS = new TargetValue( perimeter );
			perimeter->setTHIS(THIS);

			Target target = THIS;

   		perimeter->build(o);

			//l.addFeature( target );

			return Target(target);
			}

		static Perimeter *load( std::iostream &ios, TargetValue *TVP ) {
			return new Perimeter(ios, TVP);
			}

		virtual void toFile( std::iostream & );
    
		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual String name( NameOption=n_default );

    void build( const Option& );
		void build( List<Rectangle>, Level& );
    long getVal( StatType );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		Perimeter( const Option* );
		Perimeter( std::iostream&, TargetValue *TVP );

		materialClass type;
    bool is_wall;

	};

class Basement : public Feature {

	public:

    struct Option : public Feature::Option {
      Option( void ) {
        image=Image(cGrey,'#');
        }
      };
      
		static Target create( const Option &o ) {

			Basement *basement = new Basement( &o );
			TargetValue *THIS = new TargetValue( basement );
			basement->setTHIS(THIS);

			Target target = THIS;

			basement->build(o);

			//l.addFeature( target );

			return Target(target);
			}

		static Basement *load( std::iostream &ios, TargetValue *TVP ) {
			return new Basement(ios, TVP);
			}

		virtual void toFile( std::iostream & );


		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual String name( NameOption=n_default );

    void build( const Option& );
    long getVal( StatType );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		Basement( const Option* );
		Basement( std::iostream&, TargetValue *TVP );

	};

class Road : public Feature {

	public:

    struct Option : public Feature::Option {
      Orthog orthog;
      materialClass material;
      Option( void ) : orthog(), material(m_invalid) {}
      };
      
		static Target create( const Option &o ) {

			Road *road = new Road( &o );
			TargetValue *THIS = new TargetValue( road );
			road->setTHIS(THIS);
			Target target = THIS;

			road->build(o);

			//l.addFeature( target );

			return target;
			}

		static Road *load( std::iostream &ios, TargetValue *TVP ) {
			return new Road(ios, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual String name( NameOption=n_default );

    void build( const Option& );
    long getVal( StatType );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		Orthog exits;
		materialClass type;

    void Trace( Point, Point, Level& );

		Road( const Option* );
		Road( std::iostream&, TargetValue *TVP );

	};



class SpecialRoom : public Feature {

	public:

    struct Option : public Feature::Option {
      String name;
      Option( void ) : name() {}
      };
      
		static Target create( const Option &o ) {

			SpecialRoom *special_room = new SpecialRoom( &o );
			TargetValue *THIS = new TargetValue( special_room );
			special_room->setTHIS(THIS);
			Target target = THIS;

			special_room->build(o);

      // Added to level in build()
			// l.addFeature( target );

			return target;
			}

		static SpecialRoom *load( std::iostream &ios, TargetValue *TVP ) {
			return new SpecialRoom(ios, TVP);
			}

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual String name( NameOption=n_default );
		virtual bool perform( Action, const Target &t );

    void build( const Option& );
    long getVal( StatType );

		virtual void toFile( std::iostream & );
    
    static void loadData( void );
    static void clearData( void );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
    struct Data {
      String name;
      List<String> map;

      struct Legend {
        char symbol;
        targetType type;
        String param;
        };
      List<Legend> legend;

      int width( void );
      int height( void );
      };

    static List<Data> data_list;
  
		SpecialRoom( const Option* );
		SpecialRoom( std::iostream&, TargetValue *TVP );

	};


class FirstLevel : public Feature {

	public:

    struct Option : public Feature::Option {
      Option( void ) {
        image=Image(cGrey,'#');
        }
      };
      
		static Target create( const Option &o ) {

			FirstLevel *first_level = new FirstLevel( &o );
			TargetValue *THIS = new TargetValue( first_level );
			first_level->setTHIS(THIS);

			Target target = THIS;

			first_level->build(o);

			//l.addFeature( target );

			return Target(target);
			}

		static FirstLevel *load( std::iostream &ios, TargetValue *TVP ) {
			return new FirstLevel(ios, TVP);
			}

		virtual void toFile( std::iostream & );


		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual String name( NameOption=n_default );

    void build( const Option& );
    long getVal( StatType );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		FirstLevel( const Option* );
		FirstLevel( std::iostream&, TargetValue *TVP );
	};



class MechMines : public Feature {

	public:

    struct Option : public Feature::Option {
      Point3d entrance;
      Option( void ) : entrance(OFF_MAP) {
        image=Image(cGrey,'>');
        }
      };

		static void create( const Option &o ) {

			MechMines *mines = new MechMines( &o );
			TargetValue *THIS = new TargetValue( mines );
			mines->setTHIS(THIS);

      assert(!tTHIS);
      tTHIS = THIS;

      entrance = o.entrance;
			mines->build(o);

			}
      
    static void buildLev( Level & );

		static MechMines *load( std::iostream &ios, TargetValue *TVP ) {
			return new MechMines(ios, TVP);
			}

		virtual void toFile( std::iostream & );


		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual String name( NameOption=n_default );

    void build( const Option& );
    long getVal( StatType );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		MechMines( const Option* );
		MechMines( std::iostream&, TargetValue *TVP );

    // Information about mine level creation:
    static struct MineInfo {

      char num_corridors:5,
           has_workshop:1,
           has_repairshop:1,
           has_leeching_pit:1;
           
      } mine_info [map_width][map_height];
    static int mine_depth;
    static int levels_created;
    static Point3d entrance;
    static Target tTHIS;
    static List<Point3d> connections;
    friend class Resource;

	};


class Sewers: public Feature {

	public:

    struct Option : public Feature::Option {
      Point3d entrance;
      Option( void ) : entrance(OFF_MAP) {
        image=Image(cDGrey,'=');
        }
      };

		static void create( const Option &o ) {

			Sewers *sewers = new Sewers( &o );
			TargetValue *THIS = new TargetValue( sewers );
			sewers->setTHIS(THIS);

      assert(!tTHIS);
      tTHIS = THIS;

      entrance = o.entrance;
			sewers->build(o);

			}
      
    static void buildLev( Level & );

		static Sewers *load( std::iostream &ios, TargetValue *TVP ) {
			return new Sewers(ios, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual String name( NameOption=n_default );

    void build( const Option& );
    long getVal( StatType );

	protected:
  // Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		Sewers( const Option* );
		Sewers( std::iostream&, TargetValue *TVP );

    // Information about sewers level creation:
    static List<Point3d> wet_levels; // Which levels have a water track
    static List<Point3d> dry_levels; // Levels without a water track
    static List<Point3d> stair_locs; // Locations of to-be-placed stairs
    static List<Point3d> to_connect; // Edge tiles that must connect
    static List<Point3d> water_track; // Edge tiles that are water-track
    static Point3d entrance;
    static int levels_planned;
    static int level_remainder; // Number of sections not built last dlevel
    static Target tTHIS;
    static bool finished;       // Means that the *design* is complete.
    friend class Resource;

    static void createMap( int );

	};
  
  

#endif // FEATURES_HPP
