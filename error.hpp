// [error.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( ERROR_HPP )
#define ERROR_HPP

#include "string.hpp"

class Error {
	public:
		static void fatal( const char *const );
		static void fatal( const String& );
		// Writes string to screen then exits.
	};

#endif // ERROR_HPP
