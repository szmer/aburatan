// [message.hpp]
// (C) Copyright 2000 Michael Blackney
#if !defined ( MESSAGE_HPP )
#define MESSAGE_HPP

#include "list.hpp"
#include "string.hpp"

class Message {
	public:
		static void add( const String &, bool = false ); // Adds one new message.
		static void add( const char *, bool = false ); // Adds one new message.
		static void print( void ); // Prints new message list.
		static void flush( void ); // Moves new messages to old list &
															 // deletes oldest messages.
		static void clr( void ); // Erases _all_ messages.
		static void clrscr( void ); // Clears the message pane.
		static void recent( void ); // View recent messages.
		static void more( void ); // Display --more-- and wait for space.
	private:
		static char buffersize; // Number of old messages.
//		static List<String> messages; // Messages that haven't been printed yet.
//		static List<String> buffer; // Old messages.
	};

#endif // MESSAGE_HPP
