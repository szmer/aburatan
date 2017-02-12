// [food.cpp]
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
#if !defined ( FOOD_CPP )
#define FOOD_CPP

#include "food.hpp"
#include "random.hpp"
#include "grammar.hpp"
#include "message.hpp"

foodStats::foodStats( const char *n, Appetite fl, int nut, int w, Image i, Special sp )
: name(n), flavour(fl), nutrition(nut), weight(w), image(i), special(sp) {}


foodStats FoodStorage [] = {

	foodStats("#food ration~",   ap_omni,  10000, 10, Image(cLBlue,'%'),  foodStats::invalid ) ,
  foodStats("#pudding~",       ap_sweet,  3500,  5, Image(cBlue,'%'),   foodStats::dessert ) ,
	foodStats("#strawberry~",    ap_fruit,  1000,  1, Image(cLRed,'%'),   foodStats::berry ) ,
	foodStats("#peach~",         ap_fruit,  2500,  2, Image(cOrange,'%'), foodStats::fruit ) ,
	foodStats("#apple~",         ap_fruit,  3000,  3, Image(cGreen,'%'),  foodStats::fruit ) ,
	foodStats("#plum~",          ap_fruit,  2000,  2, Image(cIndigo,'%'), foodStats::fruit ) ,
	foodStats("#pear~",          ap_fruit,  3000,  3, Image(cLGreen,'%'), foodStats::fruit ) ,
	foodStats("#orange~",        ap_fruit,  3000,  3, Image(cOrange,'%'), foodStats::fruit ) ,
	foodStats("#banana~",        ap_fruit,  3000,  3, Image(cYellow,'%'), foodStats::banana ) ,
	foodStats("#prune~",         ap_fruit,  2000,  1, Image(cViolet,'%'), foodStats::berry ) ,
	foodStats("#turnip~",        ap_veg,    3000,  2, Image(cGrey,'%'),   foodStats::vegetable ) ,
	foodStats("#mushroom~",      ap_veg,      20,  1, Image(cWhite,'%'),  foodStats::heal ) ,
	foodStats("#toadstool~",     ap_veg,      20,  1, Image(cDGrey,'%'),  foodStats::poison ) ,
	foodStats("baker's bread",   ap_grain, 10000, 10, Image(cYellow,'%'), foodStats::loaf ) ,
	foodStats("corn pone",       ap_grain,  6000, 10, Image(cLGrey,'%'),  foodStats::loaf ) ,

};

foodStats *foodStats::FoodStats = FoodStorage;
int foodStats::numFood = 15;

targetType Food::gettype( void ) const {
	return tg_food;
	}

bool Food::istype( targetType t ) const {

	if ( t == tg_food )
		return true;
	else return Item::istype(t);

	}

bool Food::capable( Action a, const Target &t ) {

	switch( a ) {
		case aBeEaten:
			return true;

		default:
			return Item::capable(a, t);
		}
	}

bool Food::perform( Action a, const Target &t ) {

	switch( a ) {
    case aPolymorph:
      form = Random::randint( foodStats::numFood );
     	image = foodStats::FoodStats[form].image;
      return Item::perform(a,t);

		case aBeEaten: {

      // Food must be eaten by something!
      if (!t)
        return false;

      // Be eaten:
      Item::perform(a,t);

      // Hurt the eater in poisonous cases:
      if ( foodStats::FoodStats[form].special==foodStats::poison ) {
 				Attack poison( CombatValues(pen_vapor,6,dam_poison,100), getTarget(tg_controller), THIS, "poisoned" );
        if (!!t)
          t->takeDamage( poison );
        }

			return true;

			}

		default:
			return Item::perform(a, t);
		}

	}

long Food::getVal( StatType s ) {

	switch( s ) {

    case attrib_price:
      return Dollars(1);
      
    case attrib_sort_category:
      return Item::getVal(s) + form;
             
    case attrib_weight:
    case attrib_sink_weight:
      return foodStats::FoodStats[form].weight+Item::getVal(attrib_weight);
      
		case attrib_nutrition:
			return foodStats::FoodStats[form].nutrition;

		case attrib_flavour:
			return foodStats::FoodStats[form].flavour;

		default:
			return Item::getVal(s);
		}

	}

Food::Food( const Option *o )
: Item(o) {

	if ( o->special == foodStats::invalid )
    doPerform(aPolymorph);
	else for ( int i = 0; i < NUM_ABORT; ++i ) {
    doPerform(aPolymorph);
		if ( foodStats::FoodStats[form].special == o->special) break;
		}
	}

Food::Food( Food &rhs )
: Item(rhs) {

	form = rhs.form;
	}

Food::Food( std::iostream &inFile, TargetValue *TVP )
// Read Item base information:
: Item (inFile, TVP) {

	// Read char form
	inFile.read( (char*) &form, sizeof(form) );


	}

void Food::toFile( std::iostream &outFile ) {

	// Write Item base information:
	Item::toFile( outFile );

	// Write char form
	outFile.write( (char*) &form, sizeof(form) );

	}

Target Food::clone( void ) {

	Food *newcopy = new Food( *this );

	TargetValue *NEWTHIS = new TargetValue( newcopy );
	newcopy->setTHIS(NEWTHIS);

	return Target(NEWTHIS);
	}

bool Food::equals( const Target &rhs ) {

	bool retVal = Item::equals( rhs );

	if (retVal) {
		// Good so far - same inventories and both of same type.
		if ( form != ( (Food*) rhs.raw() )->form ) retVal = false;
		}

	return retVal;

	}

Attack Food::getAttack( void ) {
	return Item::getAttack();
	}

String Food::name( NameOption n_opt ) {
  switch (n_opt) {
    default: {
      String loaf;
      if (foodStats::FoodStats[form].special==foodStats::loaf)
        loaf = "#loaf~ of ";
  	  return loaf + foodStats::FoodStats[form].name + Item::name(n_opt);
      }
    }
	}


#endif // FOOD_CPP

