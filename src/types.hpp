// [types.hpp]
// (C) Copyright 2000 Michael Blackney
#if !defined ( TYPES_HPP )
#define TYPES_HPP


// Bitfield defined type
#define bitfield(identifier,size) char identifier:size

// Pointer to a void function(void)
typedef void (*VoidFuncPtr)();


// Image - union
// An Image is an int-size object which can be treated as
// an int or as a struct of chars, color and appearance.


union Image {
	// Data:
	struct {
		char appearance;
		char color;
		} val;
	int i;

	// Interface:
	Image (void) : i(0) { }
	Image (int temp) : i(temp) { }
	Image (char c, char a) : i(0) { val.color = c; val.appearance = a; }
	int &operator *() {
		return i;
		}
	int &operator=( const int &rhs ) {
		return (i = rhs);
		}

	char operator ==( const Image &rhs ) {
		return ( i == rhs.i );
		}
	};


enum IDmask {
  id_none =     0x00,

  id_name =     0x01, // Have we been introduced?
  id_owner =    0x02, // Do you know an item/house's owner/resident?
  id_quality =  0x04, // Do we know how awesome the item is?

  // The next two probably shouldn't be here, since when you
  // can identify an item's form/recipe you usually can id all
  // items of that type.
  id_form =     0x08, // Can we id the item type by color/markings?
  id_recipe =   0x10, // Do we know how to make the item from parts?
  
  id_abandoned =0x20, // Do we know that this item/house has been abandoned?

  id_all =      0xFF
  
  };


#endif // TYPES_HPP
