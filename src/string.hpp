// [string.hpp]
// (C) Copyright 2000 Michael Blackney
#if !defined ( STRING_HPP )
#define STRING_HPP

#include "compiler.hpp"

#ifdef OBSOLETE_INCLUDE
#  include <string.h>
#  include <iostream.h>
#else
#  include <string>
#  include <iostream>
#endif

#include "counted.hpp"
#include "standard.hpp"

class String {
	public:
		class CharProxy {
			public:
				CharProxy(String& str, int index);

				CharProxy& operator=(const CharProxy &rhs);
				CharProxy& operator=(char c);

				operator char() const;

			private:
				String &theString;
				int charIndex;
			};

		String(const char *const value = "");
		String(long value);
		String(char value);

		int length( void ) const;
		int formatlen( void ) const;
		String &fillnchar( char, int );
		const char *const toChar() const;

		const CharProxy operator[](int index) const;
		CharProxy operator[](int index);
		bool operator !( void ) const;
		bool operator ==( const String & ) const;
		bool operator !=( const String & ) const;

		String &trunc( int );
		String &abbrev( int );
		String &operator +=( int );
		String &operator +=( char );
		String &operator +=( long );
		String &operator +=( const char * );
		String &operator +=( const String & );

		String &operator << ( int );
		String &operator << ( char );
		String &operator << ( long );
		String &operator << ( const char * );
		String &operator << ( const String & );

		long toInt( void );

		String titlecase( void ) const;
    String sentence( void ) const;
		String upcase( void ) const;
		String locase( void ) const;

    static String digitorder( long );

		friend class String::CharProxy;
		friend STDCLASS std::ostream & operator <<(std::ostream& lhs, const String &rhs);
		friend String operator +(const String &lhs,      const String &rhs);
		friend String operator +(const char  *const lhs, const String &rhs);
		friend String operator +(const String &lhs, const char  *const rhs);
		friend String operator +(const String &lhs,             long   rhs);
		friend String operator +(const String &lhs,             char   rhs);
	private:
		// class representing string values:
		struct StringValue: public RCObject {
			char *data;

			StringValue(const char *initValue);
			StringValue(const StringValue& rhs);
			void init(const char*initValue);
			~StringValue();
			};

		RCPtr<StringValue> value;
	};


class MenuString {
	public:
		MenuString( const String &s ) : string(s) {}
		String menustring( char len = 80 ) {
			String retval = string;
			return retval.abbrev(len);
			}
		Image menuletter( void ) { return Image(0,0); }
		bool operator ==( const MenuString &rms ) const {
			return ( string == rms.string );
			}
		bool operator ==( const String &rs ) const {
			return ( string == rs );
			}
	private:
		String string;
	};

#endif // STRING_HPP

