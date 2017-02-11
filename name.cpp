// [name.cpp]
// (C) Copyright 2003 Michael Blackney
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
#if !defined ( NAME_CPP )
#define NAME_CPP

#include "name.hpp"
#include "file.hpp"

//-[ class IndexedString ]--

IndexedString::IndexedString( Index i, String s ) {
  index = i;
  string = s;
  }

bool IndexedString::operator > ( const IndexedString &istr ) const {
  return index > istr.index;
  }
  
bool IndexedString::operator < ( const IndexedString &istr ) const {
  return index < istr.index;
  }

Index IndexedString::getIndex( void ) const {
  return index;
  }

String IndexedString::getString( void ) const {
  return string;
  }
    
void IndexedString::set( String s ) {
  string = s;
  }

//-[ class Name ]--

Name::Name( void ) {
  index = 0;
  }

Name::~Name( void ) {
  // If loaded, remove from string_list
  if ( find(index) )
    string_list.remove();
  }

bool Name::generated( void ) const {
// Returns true if index != 0
  return (index != 0);
  }
  
String Name::toString( void ) const {
// Look up the name in the string list

  assert ( index != 0 );
  // Error::fatal("Error: Uninitialised names cannot convert to string form.");
    
  String ret_str;

  if ( !find(index) ) {
    fromFile(index);
    find(index);
    }
    
  ret_str = string_list.get().getString();
  
  return ret_str;
  }
  
Name &Name::operator =( const String &str ) {
  // If not generated add to string_list
  if ( !generated() )
  
    string_list.add( IndexedString( index = ++num, str ) );
    
  // otherwise alter string_list
  else {

    if (!find(index)) {
      fromFile(index);
      find(index);
      }
      
    string_list.get().set( str );
  
    }

  return *this;
  }

bool Name::operator ==( const String &str ) const {

  if ( find(index) ) {

    if ( string_list.get().getString() == str )
      return true;
    }
    
  return false;
  }
  
  
std::iostream &operator << ( std::iostream &ios, Name &name ) {

  // Write index to stream
  ios.write( (char*)&name.index, sizeof(name.index) );
  
  // Save entry and remove from string_list
  name.toFile();

  return ios;
  }
  
std::iostream &operator >> ( std::iostream &ios, Name &name ) {

  // Read index from stream
  ios.read( (char*)&name.index, sizeof(name.index) );

  return ios;

  }


void Name::save( void ) {
  /*
  // save index
  File myFile;
  String file_name = SAVEDIR + "name.dat";

  myFile[file_name].write( (char*) &num, sizeof(num) );
  */

	std::fstream myFile;

  String file_name = SAVEDIR + "name.dat";
	myFile.open( file_name.toChar(), std::ios::out | std::ios::binary );
  
	assert (!! myFile);
  // Error::fatal(  "Name save error: Error opening file " + file_name);

  myFile.write( (char*) &num, sizeof(num) );
  
  // if there are any strings still loaded, there must be an error:
  assert ( ! string_list.numData() );

  }
  
void Name::load( void ) {
  // load index
  /*
  File myFile;
  String file_name = SAVEDIR + "name.dat";

  myFile[file_name].read( (char*) &num, sizeof(num) );
  */

	std::fstream myFile;

  String file_name = SAVEDIR + "name.dat";
	myFile.open( file_name.toChar(), std::ios::in | std::ios::binary );
	assert (!! myFile);// Error::fatal(  "Name load error: Error opening file " + file_name);

  myFile.read( (char*) &num, sizeof(num) );

  string_list.clr();
  }

void Name::reset( void ) {
  string_list.clr();
  num = 0;
  }

bool Name::find( Index i ) {

  if ( string_list.toTail() )
  do {

    if ( string_list.get().getIndex() == i )
      return true;
  
    } while ( string_list.previous() );

  return false;
  }
  
void Name::toFile() {

  if ( find(index) ) {

    String save_str = string_list.get().getString();

    String file_name = SAVEDIR;
    file_name += (long) index;
    file_name += ".nam";

  	std::fstream *myFile;

    myFile = new std::fstream ;
  	myFile->open( file_name.toChar(), std::ios::out | std::ios::binary );

    /*
    // Relax the file system if we can't open the file
    while (! (*myFile) && File::relax()) {
      delptr(myFile);
      myFile = new std::fstream ;
    	myFile->open( file_name.toChar(), std::ios::out | std::ios::binary );
      }
    */

    assert (!! (*myFile)); // Error::fatal(  "Name toFile error: Error opening file " + file_name);

    int len = save_str.length();

    myFile->write( (char*) &len, sizeof(len) );
    myFile->write( save_str.toChar(), len+1 );

    string_list.remove();
    delptr(myFile)
    }
    
  }

void Name::fromFile( Index index ) {

  String load_str;

  String file_name = SAVEDIR;
  file_name += (long) index;
  file_name += ".nam";

	std::fstream myFile;
	myFile.open( file_name.toChar(), std::ios::in | std::ios::binary );
	assert (!! myFile); // Error::fatal(  "Name fromFile error: Error opening file " + file_name);

  int len;

  myFile.read( (char*) &len, sizeof(len) );

  char *buffer = new char[len+1];
  
  myFile.read( buffer, len+1 );

  load_str = buffer;
  delarr( buffer );

  if ( find(index) )
    string_list.get().set( load_str );
  else
    string_list.add( IndexedString( index, load_str ) );

  }


List<IndexedString> Name::string_list;
Index Name::num = 0;

Name::Name( Name& ) { } // Copying Names is illegal

#endif // NAME_CPP
