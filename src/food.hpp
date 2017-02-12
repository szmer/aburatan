// [food.hpp]
// (C) Copyright 2003 Michael Blackney
#if !defined ( FOOD_HPP )
#define FOOD_HPP

#include "item.hpp"

struct foodStats {

	const char *name;

  Appetite flavour;
	int nutrition;
  int weight;

	Image image;

	enum Special {

		heal,
		poison,
    fruit,
    vegetable,
    berry,
    banana,
    dessert,
    loaf,

		num,
		invalid = num

		} special;

	foodStats( const char*, Appetite, int, int, Image, Special );

	static foodStats *FoodStats;
	static int numFood;

	};



class Food : public Item {

	public:

    struct Option : public Item::Option {
      foodStats::Special special;
      Option( void ) : special(foodStats::invalid) {}
      };

    METHOD_create_option(Food);
    METHOD_create_default(Food);

		static Food *load( std::iostream &ios, TargetValue *TVP ) {
			return new Food(ios, TVP);
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

		virtual Attack getAttack(void);

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		char form;

		Food( const Option * );
		Food( Food & );
		Food( std::iostream&, TargetValue *TVP );



	};



#endif // FOOD_HPP
