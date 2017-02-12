// [file.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( FILE_HPP )
#define FILE_HPP
#include "compiler.hpp"

#ifdef OBSOLETE_INCLUDE
#  include <stdio.h>
#  include <fstream.h>
#else
#  include <cstdio>
#  include <fstream>
#endif

#include "enum.hpp"
#include "string.hpp"
#include "list.hpp"

class file_Handle {

	public:
		file_Handle( void );
		file_Handle( const file_Handle & );
		~file_Handle();

		bool open( const String& );

		INLINE String name( void );
		INLINE std::fstream &stream( void );

		bool operator ==( const file_Handle & );
		file_Handle &operator = ( file_Handle& );


	private:
		String theName;
		std::fstream *theStream;
		bool master;
	};



class File {
	public:
		File();
		~File();

		std::fstream &operator []( const String& );

		static bool exists( const String &fname, const String &dirname = "." );
    static bool relax( void );


	private:

		File( File & ) {};

		static int numInstances;
		static List<file_Handle> files;
	};

# define SAVEDIR String("save/")
# define SAVEINFO String("saveinfo.dat")

# define INFODIR String("info/")

# define SHOTDIR String("shots/")

// Saved object block sizes:
# define BLOCK_RTDERIVED 128
# define BLOCK_TARGETVALUE 16
# define BLOCKS_PER_FILE 4000

# define NUM_BULK_ITEMS 5
# define BLOCK_FIRST_X (NUM_BULK_ITEMS*sizeof(long)+sizeof(int))

// -- Some nice little time-saver macros:
// Load a list of items from an istream:
# define LIST_LOAD(a,type,b)\
{\
  int num_data;\
  a.read((char*)&num_data,sizeof(num_data));\
  for (int i=0; i < num_data; ++i) {\
    type temp_item;\
    a.read((char*)&temp_item,sizeof(temp_item));\
    b.add(temp_item);\
    }\
}

// Load a single item from an istream:
#define ITEM_LOAD(a,b)\
  a.read((char*)&b,sizeof(b));

// Save a list of items to an ostream:
# define LIST_SAVE(a,b)\
{\
  int num_data = b.numData();\
  a.write((char*)&num_data,sizeof(num_data));\
  if (b.reset())\
  do {\
    a.write((char*)&b.get(),sizeof(b.get()));\
    } while(b.next());\
}

// Save a single item to an ostream:
#define ITEM_SAVE(a,b)\
  a.write((char*)&b,sizeof(b));

#endif // FILE_HPP
