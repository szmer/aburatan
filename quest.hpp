// [quest.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( QUEST_HPP )
#define QUEST_HPP

#include "rawtargt.hpp"
#include "dir.hpp"
#include "path.hpp"

enum QuestType {

  // By default, quest will be of type qt_fetch:
  
  qt_fetch,     // Please help me find <ITEM>  I lost it in <FEATURE>
  qt_caravan,   // Please see that <ITEM> safely reaches <HUMAN>
  qt_eliminate, // I need you to eliminate <CREATURE>

  qt_num,
  qt_invalid
  };



enum QuestStage {

  // Quests are state-machines.  They begin in the state 'qs_briefing'
  // and as they develop they progress through the stages.  Each
  // stage may have simple, single, complex or multiple means of
  // progressing to the subsequent stage.

  qs_briefing, // Has yet to be briefed.
  qs_briefed,  // Has been briefed, now awaiting success.
  qs_reward,   // Has succeeded.  Now awaiting reward.
  qs_complete, // All done!

  qs_num,
  qs_invalid


  };

class Quest : public RawTarget {
	public:

    struct Option : public RawTarget::Option {
      Target object;
      long time;
      Option( void ) : object(NULL), time(0) {}
      };
      
		static Quest *load( std::iostream &ios, TargetValue *TVP ) {
			return new Quest(ios, TVP);
			}

    METHOD_create_option(Quest);
    METHOD_create_default(Quest);
    
		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType ) const;

		virtual void toFile( std::iostream &);

		virtual String name( NameOption=n_default );

		virtual Image menuletter( void );
		virtual void setletter( char );

		virtual Target clone( void ) { assert(0); /* Attempt to clone a Quest! */ return Target(); };
		virtual bool equals( const Target& ) { return false; } // Never combine quests!
		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

    //virtual void setlocation( const Point3d &p3d );

	protected:
		Quest( const Option* );
		Quest( std::iostream&, TargetValue *TVP );
		virtual ~Quest(void);

		Target object;
    QuestStage stage;
    char letter;

	};


#endif // QUEST_HPP

