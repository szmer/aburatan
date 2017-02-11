// [serial.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( SERIAL_HPP )
#define SERIAL_HPP

#include "string.hpp"

class Serial {
	public:
		Serial();
		void init( long l = 0 );

		String toFileName( void ) const;
		long toLong( void ) const;

		static String toFileName( long );

		bool operator ==( long ) const;
		friend STDCLASS std::iostream &operator << ( std::iostream &, Serial & );
		friend STDCLASS std::iostream &operator >> ( std::iostream &, Serial & );

		static void save( void );
		static void load( void );
		static void reset( void );

	private:
		long number;
		static long next;
	};


std::iostream &operator << ( std::iostream &, Serial & );
std::iostream &operator >> ( std::iostream &, Serial & );
#endif // SERIAL_HPP
