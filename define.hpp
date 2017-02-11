// [define.hpp]
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
#if !defined ( DEFINE_HPP )
#define DEFINE_HPP


#define SECONDPOINT     // LOS algorithm uses twin points (slower)

#define DARKEN_NON_LOS  // Blacks out unseen areas

#define RARE_PRONOUN    // Pronouns are only used in messages
                        // from the same turn

#define TEST_BUILD      // Define for testing, player starts with
                        // helpful items inc. a kaleidoscope

#undef  NDEBUG          // Define to remove assert statements from build

#define PRINTMEM        // Prints totals for objects allocated and
                        // deleted, and the difference

#endif // DEFINE_HPP
