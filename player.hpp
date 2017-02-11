// [player.hpp]
// (C) Copyright 2000 Michael Blackney
#if !defined ( PLAYER_HPP )
#define PLAYER_HPP

#include "creature.hpp"
#include "actor.hpp"
#include "event.hpp"
#include "skill.hpp"
#include "name.hpp"
#include "timeline.hpp"

class Player : public Creature {
	public:
	 // Construction / Destruction:
    struct Option : public Creature::Option {
      Option( void ) {}
      };
      
		static Target create( Option& );

		static Player *load( std::iostream &ios, TargetValue *TVP );

		virtual Target clone( void );

		virtual Target detach( int = -1 );
    //virtual void   setlocation( const Point3d& );
    virtual void setParent( TargetValue * );
		virtual void toFile( std::iostream & );

		virtual ~Player( void );

		virtual targetType gettype( void ) const;
		virtual bool istype( targetType ) const;

		virtual bool alive( void ) const;

		virtual void draw( void );

		virtual String name( NameOption=n_default );

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );
		virtual void takeDamage( const Attack & );
		virtual long getVal( StatType );
    virtual long getSkill( Skill, bool = true );
    virtual void modSkill( Skill, int = 0 );
    virtual Result test( Skill, int, const Target& );

		void updateStatus( void );
		void updateLocation( void );

    void setDayTime( TimeOfDay );

		friend class PlayerUI;

		static void loadInfo( void );
		static Point3d levLoc;
    static Player &get( void );
    static Target handle( void );
    static void save( void );
    
	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
    friend Target playercreate( void );

  
		Player( const Option* );
		Player( Player& );
  	Player( std::iostream&, TargetValue *TVP );
    bool init( void );
    void calcBurden( void );
		virtual void add( const Target & );
		virtual bool remove( const Target & );

    SkillSet skill;
    Feat        feat_normal;
    Proficiency feat_weapon;
    
    Notoriety notoriety;
    
		bool moved;

    enum Burden {
    
      bur_none,
      bur_stressed,
      bur_overload
      
      } burden;

    HungerState hunger;

    List<Target> account;

  // This member is not saved.
    static Target thePlayer;

	};

#endif // PLAYER_HPP
