// [string.cpp]
// (C) Copyright 2000 Michael Blackney
/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 **/
#if !defined ( STRING_CPP )
#define STRING_CPP
#include "string.hpp"

#include "compiler.hpp"
#ifdef OBSOLETE_INCLUDE
#  include <stdio.h>
#  include <ctype.h>
#else
#  include <cstdio>
#  include <cctype>
#endif

using namespace std;

String::String(const char *initValue)
: value( new StringValue(initValue) ) {
	}

String::String(long value) {
	String temp;
	temp += value;
	*this = temp;
	}

String::String(char value) {
	String temp;
	temp += value;
	*this = temp;
	}


String String::digitorder( long digit ) {
// Creates a string of the form "1st", "2nd" ... given a long int.
  String ret_str;

  ret_str += (long) digit;
  
  int last_digit = digit % 10;
  if ((digit%100) / 10 == 1) // Teens are all 'th'
    last_digit = 0;

  switch (last_digit) {
    case 1:  ret_str += "st"; break;
    case 2:  ret_str += "nd"; break;
    case 3:  ret_str += "rd"; break;
    default: ret_str += "th"; break;
    }

  return ret_str;
  }
    
int String::formatlen( void ) const {
  int len = std::strlen(value->data), i = 0;
  char *temp = value->data;

  while (temp[i] != '\0') {

    if (temp[i] == '<') {
      if ( isdigit(temp[i+1]) )
        len-=3;
      else
        len-=2;
      }
    if (temp[i] == '>') --len;

    ++i;
    }
  
	return len;
	}

int String::length( void ) const {
  return std::strlen(value->data);
	}

String &String::fillnchar( char ch, int len ) {

	if (len <= 0) {
    *this = "";
    return *this;
    }

	if (len > 255) len = 255;

	char *buffer = new char[len+1];

	std::memset( buffer, ch, len );
	buffer[len] = '\0';

	value = new StringValue(buffer);

	delarr( buffer );

  return *this;
	}

const String::CharProxy String::operator[](int index) const {
	return CharProxy( *(String *)(this), index);
	}

String::CharProxy String::operator[](int index) {
	return CharProxy(*this, index);
	}

const char* const String::toChar() const {
	return value->data;
	}

long String::toInt( void ) {
	long retval = 0;

	for (int i = 0; i < length(); i++) {
		retval *= 10;
		retval += operator[](i) - '0';
		}

	return retval;
	}

String String::titlecase( void ) const {

	String retstr = locase();

	for (int i = 0; i < length(); ++i)
		if ( (i==0) || isspace(retstr[i-1]) )
			retstr[i] = toupper( retstr[i] );

	return retstr;


	}

String String::sentence( void ) const {
// Capitalises the first letter and makes sure a period is at the end
// if there is no other punctuation there:
  String retstr = *this;

  if ( length() ) {

    retstr[0] = toupper( retstr[0] );

    if (! ispunct(retstr[length()-1]))
      retstr += ".";
    
    }

  return retstr;
  }

String String::upcase( void ) const {
	String retstr = value->data;

	for (int i = 0; i < length(); ++i)
		retstr[i] = toupper( retstr[i] );

	return retstr;
	}

String String::locase( void ) const {
	String retstr = value->data;

	for (int i = 0; i < length(); ++i)
		retstr[i] = tolower( retstr[i] );

	return retstr;
	}

bool String::operator !() const {
	return strcmp( value->data, "" ) == 0
				 ? true : false;
	}

bool String::operator ==( const String &rhs ) const {
	return strcmp( value->data, rhs.value->data ) == 0
				 ? true : false;
	}

bool String::operator !=( const String &rhs ) const {
	return ! ( operator==(rhs) ) ? true:false ;
	}


String &String::trunc( int len ) {
	if ( (formatlen() == 0) || (len >= length()) )
		return *this;

	if (len < 0) len = 0;

  int newlen = length() - (formatlen()-len);

	char *buffer = new char[ newlen + 1 ];

	strncpy( buffer, value->data, newlen );
	buffer[newlen] = '\0';

	value = new StringValue( buffer );

	delarr( buffer );

	return *this;
	}

String &String::abbrev( int len ) {
  int fl = formatlen();

	if ( (fl == 0) || (len >= fl) )
		return *this;

	if (fl > 2) {
		trunc(len - 2);
		*this += "..";
		return *this;
		}

	if (fl > 1) {
		trunc(len - 1);
		*this += '.';
		return *this;
		}

	*this = "!";
	return *this;

	}


String &String::operator +=( int i ) {
  return *this += (long) i;
  }

String &String::operator +=( char c ) {
	char buffer[2];

	buffer[0] = c;
	buffer[1] = '\0';

	(*this) += buffer;

	return *this;
	}

String &String::operator +=( long l ) {
	char buffer[20];

	sprintf(buffer, "%li", l);

	(*this) += buffer;

	return *this;
	}

String &String::operator +=( const char *cp ) {
	char *buffer = new char[ strlen( value->data )+strlen( cp ) + 1 ];

	strcpy( buffer, value->data );
	strcat( buffer, cp);

	value = new StringValue( buffer );

	delarr( buffer );

	return *this;
	}

String &String::operator +=( const String &sr ) {
	return operator+=( sr.value->data );
	}

// Concatenation operators:
String &String::operator <<( int i ) {
	return *this += i;
	}

String &String::operator <<( char c ) {
	return *this += c;
	}

String &String::operator <<( long l ) {
	return *this += l;
	}

String &String::operator <<( const char *cp ) {
	return *this += cp;
	}

String &String::operator <<( const String &sr ) {
	return *this += sr;
	}

String operator +(const String &lhs, const String &rhs) {
	String ret = lhs;
	return ret += rhs;
	}
String operator +(const char  *const lhs, const String &rhs) {
	String ret = lhs;
	return ret += rhs;
	}
String operator +(const String &lhs, const char  *const rhs) {
	String ret = lhs;
	return ret += rhs;
	}
String operator +(const String &lhs, long   rhs) {
	String ret = lhs;
	return ret += rhs;
	}
String operator +(const String &lhs, char   rhs) {
	String ret = lhs;
	return ret += rhs;
	}


////////////////////

String::CharProxy::CharProxy(String &str, int index)
: theString(str), charIndex(index) {
	}

String::CharProxy::operator char() const {
	return theString.value->data[charIndex];
	}

String::CharProxy&
String::CharProxy::operator=(const CharProxy& rhs) {
	if ( theString.value->isShared() ) {
		theString.value = new StringValue(theString.value->data);
		}

	theString.value->data[charIndex] =
		rhs.theString.value->data[rhs.charIndex];

	return *this;
	}

String::CharProxy&
String::CharProxy::operator=(char c) {
	if ( theString.value->isShared() ) {
		theString.value = new StringValue(theString.value->data);
		}

	theString.value->data[charIndex] = c;

	return *this;

	}
////////////////////

void String::StringValue::init(const char *initValue) {
  if (initValue) {
  	data = new char[ strlen(initValue)+1 ];
  	strcpy( data, initValue );
    }
  else {
    data = new char[1];
    *data = 0;
    }
	}

String::StringValue::StringValue(const char *initValue) {
	init(initValue);
	}


String::StringValue::StringValue(const StringValue &rhs)
: RCObject() {
	init( rhs.data );
	}

String::StringValue::~StringValue() {
	delarr( data );
	}


#endif // STRING_CPP
