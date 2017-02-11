// [playerui.hpp]
// (C) Copyright 2003 Michael Blackney
#if !defined ( PLAYERUI_HPP )
#define PLAYERUI_HPP

#include "actor.hpp"
#include "input.hpp"


class PlayerUI : public Actor {
	public:

    struct Option : public Actor::Option {
      Option( void ) {}
      };
  
		static Target create( const Option & );
    
		virtual bool perform( Action, const Target &t );

		static PlayerUI *load( std::iostream &ios, TargetValue *TVP ) {
			return new PlayerUI(ios, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType ) const;

	protected:
		PlayerUI( const Option * );
		PlayerUI( std::iostream&, TargetValue *TVP );

		void describe( const Target &, bool ignorePlayer = true );

		Target getPoint( Point loc, Command select, bool display = true, bool locate = true, bool isThrow=false );
		Target dirTarget( Command );
		Dir whereTargets( Action );
		Sorted<Target> findTargets( Action, const Target &);

		Command LoadUnload( const Target&, bool, Point );
		Command selectSlot( const Target&, Point );
		List<Target> findContainer( Action, const Target &);
		bool playerContain( const Target&, int, Point, bool disc = false, bool msg = true );

    bool autoThrow( void );

		void pickGrass( void );

		void Adjust( const Target&, Point );

    void keyHelp( void );

    unsigned long turn_time;
	};


#endif // PLAYERUI_HPP
