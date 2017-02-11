// [file.cpp]
// (C) Copyright 2000-2002 Michael Blackney
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
#if !defined ( FILE_CPP )
#define FILE_CPP

#include "file.hpp"

#include <dirent.h>

int File::numInstances = 0;
List<file_Handle> File::files;

File::File() {
	numInstances++;
	}


File::~File() {
	if (--numInstances == 0) files.clr();
	}


std::fstream &File::operator []( const String &str ) {
//iostream &File::open( String str ) {

  // Look for the file among all the open files:
	if ( files.toTail() ) {

		do {

			if ( files.get().name() == str )
				return files.get().stream();

			} while ( files.previous() );

		}

  // Not found.  If there aren't too many files
  // open, we can just create a new handle - but
  // if the create fails we have to close some.
  file_Handle file_handle;

  files.reset();
  while (! file_handle.open( str ) && files.canGet() ) {
    // This is not desired at the moment, as we
    // don't have full compliance with save routines.
    // Before we can allow this, we must make all load/save
    // calls in RawTarget family save their file read/write
    // location before calling *any* function that could
    // load or save a file.  That's a PITA so we don't allow
    // this at the moment.
    assert(0);
    files.remove();
    }

  assert( file_handle.stream() );

	files.add( file_handle );

	return operator[]( str );

	}

bool File::relax( void ) {
// Relax our stranglehold on the file system by
// closing the oldest open file:
  if (files.reset()) {
    files.remove();
    return true;
    }
  else
    return false;
  }


bool File::exists( const String &fname, const String &dirname ) {
// Returns true if the file exists,
//         false if no file is found

  DIR *dir;
  struct dirent *ent;

	if ((dir = opendir( (char*)dirname.toChar() )) == NULL)
		return false;

	while ((ent = readdir(dir)) != NULL)
		if ( fname == ent->d_name ) {
    	if (closedir(dir) != 0)
    		return false;
      return true;
      }

	closedir(dir);

	return false;
  }



//--[ file_Handle ]--------------

file_Handle::file_Handle()
: theStream(NULL),
	master(true) {
	//open( str );
	}
file_Handle::file_Handle( const file_Handle &f )
: theName( f.theName ),
	theStream( f.theStream ),
	master( true ) {
	((file_Handle&)f).master = false;
	}

file_Handle &file_Handle::operator = ( file_Handle &fh ) {

	fh.master = false;
	master = true;

	theName = fh.theName;
	theStream = fh.theStream;

	return *this;
	}

file_Handle::~file_Handle() {
	if (theStream && master) delptr( theStream);
	}


bool file_Handle::open( const String &str ) {
	theName = str;
	theStream = new std::fstream;

  theStream->open( str.toChar(), std::ios::out | std::ios::app | std::ios::binary );

  if ( ! *theStream) {
    delptr(theStream);
    return false;
    }
  
	assert(*theStream);

  theStream->close();
	theStream->open( str.toChar(), std::ios::in | std::ios::out | std::ios::binary );

  if ( ! *theStream) {
    delptr(theStream);
    return false;
    }

	assert(*theStream);
  return true;
	}

String file_Handle::name( void ) {
	return theName;
	}
std::fstream &file_Handle::stream( void ) {
	return *theStream;
	}

bool file_Handle::operator ==( const file_Handle &f ) {
	return theName == (const String) f.theName ;
	}

//#define __FILE_HARNESS

#ifdef __FILE_HARNESS


#include "string.hpp"
#include "counted.hpp"
#include "visuals.hpp"
#include "point.hpp"
#include "rectngle.hpp"
#include "error.hpp"
#include "elum.hpp"

int main( void ) {

	File f;

	int read = 0, write = 1001;

	f["SAVE\\test0.txt"].seekp(0);
	f["SAVE\\test0.txt"].write( (char*) &write, sizeof(write) );

	f["SAVE\\test0.txt"].seekg(0);
	f["SAVE\\test0.txt"].read( (char*) &read, sizeof(read) );

	return 0;
	}

#endif // __FILE_HARNESS

#endif // FILE_CPP
