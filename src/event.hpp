// [event.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( EVENT_HPP )
#define EVENT_HPP

#include "enum.hpp"
#include "actor.hpp"
#include "skill.hpp"

#define GRANULARITY 100 // Should be at least 100

class Event;

class ev_Handle {
	public:
		ev_Handle( Event * );
		ev_Handle( const ev_Handle& );
		~ev_Handle( void );

		bool operator > ( const ev_Handle& ) const;
		bool operator < ( const ev_Handle& ) const;
		bool operator == ( const ev_Handle& ) const;
		Event *operator ->();
		Event &operator  *();
		Event *operator  &();
		ev_Handle &operator = ( const ev_Handle& );

	private:
		Event *obj;
		mutable bool master;
	};


class Event {
	public:
		virtual ~Event() {}
		virtual bool occur( void ) = 0;

		bool operator > ( const Event& );
		bool operator < ( const Event& );

		virtual void save( std::iostream & );

		virtual EventType type( void ) = 0;
//		virtual bool equals( Event* ) = 0;

		long gettime( void );

	protected:
		Event( long t );
		Event( std::iostream & );
		static Sorted<ev_Handle> &getTimeline( void );

		long time;

	};

class ActionEvent : public Event {
	public:
		virtual bool occur( void );

		static void create( const Target&, Action, long );
		static long remove( const Target&, Action);
    static bool exists( const Target&, Action);
		static void load( std::iostream & );

		virtual void save( std::iostream & );

		virtual EventType type( void );

	protected:
		ActionEvent( const Target&, Action, long );
		ActionEvent( std::iostream & );

		Action action;
		Target target;
	};

class SkillLevelEvent : public Event {
	public:
		virtual bool occur( void );

		static void create( Skill, int, long );
		static void load( std::iostream & );

		virtual void save( std::iostream & );

		virtual EventType type( void );

	protected:
		SkillLevelEvent( Skill, int, long );
		SkillLevelEvent( std::iostream & );

		int bonus;
		Skill skill;
	};

#endif // EVENT_HPP
