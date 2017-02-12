// [resource.hpp]
// (C) Copyright 2004 Michael Blackney
#if !defined ( RESOURCE_HPP )
#define RESOURCE_HPP

#include "file.hpp"
#include "target.hpp"

struct Resource {

  // Member functions:
	static void reset( void );
  static void init( std::fstream & );
  static void exit( std::fstream & );
  
  static Target wish( String );

  // Resource has no data members as of yet.  In the future,
  // I advocate a move of all data to the resource scope, for
  // ease of maintainability, and to possibly reuse some code.

	};

#endif // RESOURCE_HPP
