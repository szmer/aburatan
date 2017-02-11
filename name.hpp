// [name.hpp]
// (C) Copyright 2003 Michael Blackney
#if !defined ( NAME_HPP )
#define NAME_HPP


#include "string.hpp"
#include "list.hpp"

typedef unsigned int Index;

class IndexedString {

  public:
    IndexedString( Index, String );
    bool operator > ( const IndexedString& ) const;
    bool operator < ( const IndexedString& ) const;

    Index  getIndex( void ) const;
    String getString( void ) const;
    
    void set( String );

  private:
    Index index;
    String string;
  };


class Name {

  public:

    Name( void );
    ~Name( void );

    bool generated( void ) const;  // Returns true if index != 0
    String toString( void ) const; // Looks up the name in the string list
    Name &operator =( const String & );
    bool operator ==( const String & ) const;
  
    friend std::iostream &operator << ( std::iostream&, Name& );
    friend std::iostream &operator >> ( std::iostream&, Name& );


    static void save( void );
    static void load( void );
    static void reset( void );

  private:
    Index index;

    Name( Name& ); // Copying Names is illegal

    static List<IndexedString> string_list;
    static Index num;

    
    static bool find( Index );
           void toFile( );
    static void fromFile( Index );

    friend class Level;
  };

std::iostream &operator << ( std::iostream&, Name& );
std::iostream &operator >> ( std::iostream&, Name& );

#endif // NAME_HPP
