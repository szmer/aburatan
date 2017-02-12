// [standard.cpp]
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

#include "standard.hpp"
#include "input.hpp"
#include "assert.h"

int totalDeletes = 0;
int totalDeletesArr = 0;
int totalAllocs = 0;
int totalAllocsArr = 0;

#ifdef COUNT_DELETES
void incTotalDeletes( void ) {
  ++ totalDeletes;
  }
void incTotalDeletesArr( void ) {
  ++ totalDeletesArr;
  }

void incTotalAllocs( void ) {
  ++ totalAllocs;
  }
void incTotalAllocsArr( void ) {
  ++ totalAllocsArr;
  }

# include "visuals.hpp"

  void * operator new(size_t size)throw (std::bad_alloc) {
    incTotalAllocs();
    void *ret_val = malloc(size);
    // Insufficient memory:
    assert (ret_val);
    return ret_val;
    };

  void operator delete(void * mem)throw() {
    incTotalDeletes();
    free(mem);
    };

  void * operator new[](size_t size)throw (std::bad_alloc) {
    incTotalAllocsArr();
    void *ret_val = malloc(size);
    // Insufficient memory:
    assert (ret_val);
    return ret_val;
    };

  void operator delete[](void * mem)throw() {
    incTotalDeletesArr();
    free(mem);
    };

  int printTotalDeletes( void ) {

    long tA =totalAllocs,
         tAA=totalAllocsArr,
         tD =totalDeletes,
         tDA=totalDeletesArr;
#   ifdef PRINTMEM
    Screen::locate(1,23);
    Screen::write( String("Allocated items:")
                         +" normal="+tA
                         +" array=" +tAA
                         +" total=" +(tA+tAA)
                         );
    Screen::locate(1,24);
    Screen::write( String("Deleted items:")
                         +" normal="+tD
                         +" array=" +tDA
                         +" total=" +(tD+tDA)
                         );
#   endif // PRINTMEM
    Screen::locate(1,25);
    if (tA-tD != 0 || tAA-tDA != 0 ) {
      Screen::write("Memory leak detected.");
      Keyboard::get();
      return 1;
      }
    else {
      Screen::write("No errors.");
      return 0;
      }

    }

#else

  int printTotalDeletes( void ) {
    return 0;
    }

#endif


