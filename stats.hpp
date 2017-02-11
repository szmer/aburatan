// [stats.hpp]
// (C) Copyright 2000 Michael Blackney
#if !defined ( STATS_HPP )
#define STATS_HPP

struct Stats {
	char
	//--==== Major Stats:
	//------ Description:
	 crd  /* Coordination  */,
	 per  /* Perception    */,
	 str  /* Strength      */,
	 tuf  /* Toughness     */,
	 wil  /* Willpower     */,
	 kno  /* Knowledge     */,
	 con  /* Concentration */,
	 spd  /* Speed         */;
  int
	 hp   /* Hit points    */;

  void reset(void);

  Stats &operator +=( const Stats& );
  Stats &operator -=( const Stats& );
  char  &operator []( int );
	};


enum {

  // Some common terminology for stats:

  stat_pathetic = 1,
  stat_dreadful = 2,    // ...
  stat_awful = 3,       //    Inhumanly poor
  stat_low = 4,         //   Acceptably low for near-human
  stat_normal = 5,      // Normal human
  stat_high = 6,        //   Acceptably high for near-human
  stat_amazing = 7,     //    Inhumanly great
  stat_incredible = 8   // ...

  };

#endif // STATS_HPP
