// [book.hpp]
// (C) Copyright 2003 Michael Blackney
#if !defined ( BOOK_HPP )
#define BOOK_HPP


#include "item.hpp"


struct bookStats {

  const char *title;
	const char *subject;
  const char *author;

  Skill skill;
  int min_level, // Limits player skill must be
      max_level, // within to receive bonus.

      bonus,
      time;   // Time period that bonus lasts for.

	static bookStats *BookStats;
	static int numBook;

	};




class Book : public Item {

	public:

    struct Option : public Item::Option {
      Skill skill;
      Option( void ) : skill(sk_invalid) {
        image=Image(cBrown,'"');
        }
      };
      
    METHOD_create_option(Book);
    METHOD_create_default(Book);

		static Book *load( std::iostream &io, TargetValue *TVP ) {
			return new Book(io, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual Target clone( void );
		virtual bool equals( const Target& );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

		virtual String name( NameOption=n_default );
		virtual long getVal( StatType );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		char form;

		Book( const Option* );
		Book( Book & );
		Book( std::iostream&, TargetValue *TVP );



	};



#endif // BOOK_HPP
