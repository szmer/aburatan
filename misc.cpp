// [misc.cpp]
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
#if !defined ( MISC_CPP )
#define MISC_CPP

#include "misc.hpp"
#include "rawtargt.hpp"

String describeList( List<Target> inventory ) {
// Gives a short(er) description of a list of items
// by categorising them.

	String retstr;
	int i;

#     define numTypes 17
			targetType types[numTypes] = { tg_trap, tg_weapon, tg_clothing, tg_ammo,
                                     tg_bag, tg_bottle, tg_creature, tg_food,
                                     tg_fluid, tg_money, tg_book,
                                     tg_remains, tg_machine_part, tg_scope };

			int quantity[numTypes +1];
			for (i = 0; i <= numTypes; ++i) quantity[i] = 0;

			String name[numTypes +1];
			String defaults[numTypes +1] = { "traps", "weapons", "clothing", "ammunition",
                                       "bags", "bottles", "corpses", "food",
                                       "fluids", "currency", "books",
                                       "remains", "machine parts",
                                       "optical devices",
                                       "misc. items" };

			inventory.reset();
			do {

				for (i = 0; i < numTypes; ++i)
					if ( inventory.get()->istype( types[i] ) ) {

						if (++quantity[i] == 1)
							name[i] = inventory.get()->menustring();
						else
							name[i] = defaults[i];

						break;
						}
					else if (i == numTypes - 1)
						if (++quantity[numTypes] == 1)
							name[numTypes] = inventory.get()->menustring();
						else
							name[numTypes] = defaults[numTypes];

				} while (inventory.next());

			for (i = 0; i <= numTypes; ++i)
				if (quantity[i] > 0) {

					retstr += name[i];

					int otherStuff = 0;

					for (int search = i + 1; search <= numTypes; ++search)
						if ( quantity[search] > 0 ) ++otherStuff;

					if (otherStuff == 1)
						retstr += " and ";
					else if (otherStuff > 1)
						retstr += ", ";

					}

	return retstr;
	}

#endif // MISC_CPP
