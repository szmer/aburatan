// [function.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( FUNCTION_HPP )
#define FUNCTION_HPP

#include "string.hpp"
#include "types.hpp"

class FunctionItem {
	public:
		FunctionItem( VoidFuncPtr p, const String &s, char l )
		: vfptr(p), mstring(s), letter(l) {}
		void call( void ) { vfptr(); }
		String menustring( char len = 80  );
		Image menuletter( void );
		bool operator ==( const FunctionItem & );
		bool operator <( const FunctionItem & );
		bool operator >( const FunctionItem & );
	private:
		VoidFuncPtr vfptr;
		String mstring;
		char letter;
	};

#endif // FUNCTION_HPP
