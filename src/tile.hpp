// [levelel.hpp]
// (C) Copyright 2000 Michael Blackney
#if !defined ( LEVELEL_HPP )
#define LEVELEL_HPP

#include "point.hpp"
#include "target.hpp"
#include "types.hpp"
#include "creature.hpp"
#include "enum.hpp"
#include "list.hpp"
#include "item.hpp"
#include "string.hpp"

class Tile : public RawTarget{
	public:

    struct Option : public RawTarget::Option {
      materialClass floor;
      materialClass object;
      //materialClass etch_material;
      //String graffiti;
      //Etching etching;
      Point3d location;
      Target feature;
      TileType type;
      Option( void )
      : floor(m_stone),
        object(m_invalid),
        //etch_material(m_invalid),
        //graffiti(),
        //etching(e_none),
        location(OFF_MAP),
        type(tt_ground) {
        image.val.appearance='.';
        }
      };

		static Tile *load( std::iostream &ios, TargetValue *TVP ) {
			return new Tile(ios, TVP);
			}
      
		virtual ~Tile( void );

		virtual Target clone( void );
		virtual bool equals( const Target& );

		virtual void toFile( std::iostream & );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType ) const;

		virtual Image getimage( void );
		virtual void draw( void );

		virtual String name( NameOption=n_default );
		virtual Image menuletter();
		virtual void setletter( char ) { }

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

    //virtual void light( const Orthog & );
    virtual void  view( int, int );
    //virtual void unlight( void );
    virtual void  unview( void );

    virtual Action autoAction( void );

		virtual void takeDamage( const Attack& );
    
		virtual long getVal( StatType );

		virtual Point3d getlocation( void );
		//virtual void setlocation( Point3d );
    //virtual Target findTarget( targetType, const Target& );
		virtual Target getTarget( targetType );

		virtual void setTHIS( TargetValue* );
    /*
    virtual Target findparent( const Target& );
    virtual Target getowner( void );
		virtual Target getparent( void );
		virtual Target getfeature( void );
    virtual Target getpossessor( void );
    */
    
		virtual Level &getlevel( void );

		virtual List<Target> *itemList( void );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
    struct TempOption : public RawTarget::Option {
      Level *level;
      Point3d location;
      TempOption( void ) : level(NULL), location(OFF_MAP) {}
      };
      
    friend class Level;

    Level *level;
    Point3d location;

    static Target create( const TempOption &o ) {
			Tile *instance = new Tile(&o);
			TargetValue *THIS = new TargetValue( instance );
			instance->setTHIS(THIS);
			return Target(THIS);
			}

		Tile( const TempOption* );
		Tile( std::iostream&, TargetValue *TVP );
		Tile( Tile & );

		virtual void add( const Target & );
		virtual bool remove( const Target & );
	};

#endif // LEVELEL_HPP
