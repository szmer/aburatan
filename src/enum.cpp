// [enum.cpp]
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
#if !defined ( ENUM_CPP )
#define ENUM_CPP

#include "visuals.hpp"
#include "food.hpp"
#include "fluid.hpp"
#include <values.h>

#if !defined (BITSPERBYTE)
#  define BITSPERBYTE 8
#endif


Material storageMaterial[m_total] = {

	{"smoke", NULL, 20, cloud, ap_none, cLGrey, true,  m_smoke, m_smoke, m_smoke },

	{"grass", NULL, 50, powder, ap_grass, cGreen, true, m_grass, m_grass, m_grass },

	{"snow", NULL,     100, powder, ap_none, cWhite, true, m_snow, m_water, m_snow },
	{"dirt", "earthen", 75, powder, ap_none, cBrown, false, m_dirt, m_dirt, m_dirt },
	{"sand", NULL,     100, powder, ap_none, cYellow, false, m_sand, m_glass, m_sand },
	{"ash", NULL,       50, powder, ap_none, cGrey, true, m_ash, m_ash, m_ash },
	{"sulphur", "sulphurous",   50, powder, ap_none, cOrange, false, m_sulphur, m_sulphur, m_sulphur },

	{"water", NULL, 100, liquid, dr_water,cWater, true, m_water, m_water, m_ice },
	{"blood", NULL, 100, liquid, dr_blood, cRed, true, m_blood, m_blood, m_blood },
	{"milk",  NULL, 100, liquid, dr_milk, cWhite, true, m_milk, m_milk, m_milk },
	{"acid",  "acidic", 100, liquid, dr_chem, cGreen, false, m_acid, m_acid, m_acid },
	{"poison", "poisonous",  100, liquid, dr_chem, cOrange, false, m_poison, m_water, m_poison },
	{"triffid oil", NULL, 100, liquid, dr_oil, cGreen, true, m_t_oil, m_t_oil, m_t_oil },

	{"oil", NULL,   90, ooze, dr_oil,  cDGrey, true, m_oil, m_oil, m_oil },
	{"sap", NULL,   85, ooze, dr_none, cBrown, true, m_sap, m_sap, m_sap },
	{"bile", NULL,  85, ooze, dr_filth,cYellow, true, m_bile, m_bile, m_bile },
	{"ichor", NULL, 85, ooze, dr_filth,cLGreen, true, m_ichor, m_ichor, m_ichor },
	{"vomit", NULL, 80, ooze, dr_filth,cBrown, true, m_vomit, m_vomit, m_vomit },

  {"flesh", NULL,   125, solid, ap_allmeat, cBrown, true, m_flesh, m_flesh, m_flesh },
  {"putrid",NULL,   125, solid, ap_carrion, cGreen, true, m_flesh, m_flesh, m_flesh },
	{"bone",  NULL,   125, solid, ap_allmeat, cLGrey, false, m_bone, m_bone, m_bone },
  {"ice",   NULL,    95, solid, ap_none, cWhite, true, m_snow, m_water, m_ice },
	{"brick", NULL,   180, solid, ap_none, cGrey, false, m_brick, m_brick, m_brick },
	{"stone", NULL,   200, solid, ap_none, cLGrey, false, m_stone, m_stone, m_stone },
	{"bluestone", NULL,200,solid, ap_none, cDGrey, false, m_bluestone, m_bluestone, m_bluestone },
	{"wood", "wooden",150, solid, ap_wood, cBrown, true, m_wood, m_wood, m_wood },
	{"straw", NULL,    40, solid, ap_grass,cYellow, true, m_straw, m_straw, m_straw },
	{"clay", NULL,    100, solid, ap_none, cBrown, false, m_clay, m_clay, m_clay },
	{"coal", NULL,    100, solid, ap_none, cDGrey, false, m_ash, m_coal, m_coal },
	{"mulch", NULL,    50, solid,  ap_grass, cGreen, true, m_mulch, m_mulch, m_mulch },

	{"cotton", NULL,  60, solid, ap_fabric, cBlue, true, m_cotton, m_cotton, m_cotton },
	{"wool", "woolen",80, solid, ap_fabric, cLGrey, true, m_wool, m_wool, m_wool },
	{"leather", NULL, 70, solid, ap_fabric, cBrown, false, m_leather, m_leather, m_leather },
  {"fur", NULL,     80, solid, ap_fabric, cOrange, true, m_fur, m_fur, m_fur },

  {"paper",  NULL,  60, solid, ap_fabric, cLGrey, true, m_paper, m_paper, m_paper },
  {"rubber", NULL, 120, solid, ap_none, cDGrey, false, m_rubber, m_rubber, m_rubber },

	{"bronze", NULL, 220, solid, ap_none, cOrange, false, m_bronze, m_bronze, m_bronze },
	{"copper", NULL, 200, solid, ap_none, cOrange, false, m_copper, m_copper, m_copper },
	{"tin", NULL,    190, solid, ap_none, cLGrey, false, m_tin, m_tin, m_tin },
	{"iron", NULL,   230, solid, ap_none, cDGrey, false, m_iron, m_iron, m_iron },
	{"steel", NULL,  200, solid, ap_none, cGrey, false, m_steel, m_steel, m_steel },
	{"gold", NULL,   250, solid, ap_none, cYellow, false, m_gold, m_gold, m_gold },
	{"silver",NULL,  220, solid, ap_none, cWhite,  false, m_silver, m_silver, m_silver },

  {"ruby", NULL,    80, solid, ap_none, cRed, false, m_ruby, m_ruby, m_ruby },
	{"emerald", NULL, 80, solid, ap_none, cGreen, false, m_emerald, m_emerald, m_emerald },
	{"glass", NULL,   70, solid, ap_none, cWhite, false, m_glass, m_glass, m_glass }

	};

Material *Material::data = storageMaterial;


area storageAD[numAreas] = {
	// Sea
	area( "Sea",    ('_' | (cBlue << BITSPERBYTE) ) ),
	// Plains
	area( "Plains",	('.' | (cGreen << BITSPERBYTE) ) ),
	// Forest
	area( "Forest",	('&' | (cGreen << BITSPERBYTE) ) ),
	// Hills
	area( "Hills",	('~' | (cBrown << BITSPERBYTE) ) ),
	// Mountains
	area( "Mountains", ('^' | (cGrey << BITSPERBYTE) ) ),
	// Desert
	area( "Desert", ('.' | (cYellow << BITSPERBYTE) ) )
	};


area *area::Data = storageAD;

skin skinStorage[NumSkins] = {
	//sMain
		skin( "Main",
			cGrey, '+', cGrey, '+', cGrey, '+', cGrey, '+',
			cDGrey, '-', cDGrey, '-', cDGrey, '|', cDGrey, '|',
			cGrey, '[', cGrey, ']',
			cGrey, '<', cGrey, '>',
			cWhite, cWhite, cGrey
			),
	//sError
		skin( "Error",
			cLRed, '.', cLRed, '.', cLRed, ':', cLRed, ':',
			cLRed, '.', cLRed, '.', cLRed, ':', cLRed, ':',
			cLRed, ':', cLRed, ':',
			cLRed, '=', cLRed, '=',
			cWhite, cYellow, cRed
			),
	//sInvent
		skin( "Inventory",
			cGrey, '-', cGrey, '-', cGrey, '-', cGrey, '-',
			cGrey, '-', cGrey, '-', cGrey, ' ', cGrey, ' ',
			cWhite, '(', cWhite, ')',
			cGrey, '<', cGrey, '>',
			cWhite, cWhite, cGrey
			),
	//sPopUp
		skin( "Pop Up",
			cGrey, '.', cGrey, '.', cGrey, '`', cGrey, '\'',
			cGrey, '-', cGrey, '-', cGrey, '|', cGrey, '|',
			cWhite, '(', cWhite, ')',
			cGrey, '<', cGrey, '>',
			cWhite, cWhite, cGrey
			),
	//sMessage
		skin( "Message",
			cDGrey, '.', cDGrey, '.', cDGrey, ':', cDGrey, ':',
			cDGrey, '.', cDGrey, '.', cDGrey, ':', cDGrey, ':',
			cDGrey, ':', cDGrey, ':',
			cGrey, '[', cGrey, ']',
			cWhite, cWhite, cGrey
			),

	};

skin *skin::Arr = skinStorage;

OdorStr OdorStrType[od_num] = {
 // Air is      Air smells
  { "clean",    NULL,     "cleaner" },
  { "stale",    NULL,     NULL },
  { "rotten",   NULL,     NULL },
  { "foul",     "fouler", NULL },
  { "fetid",    NULL,     NULL },
  { "noisome",  NULL,     NULL },
  { "mephitic", NULL,     NULL },
  };

OdorStr *OdorStr::Type = OdorStrType;

VolumeStat arr_volume [v_num] = {
  // OGL equivalent names are given FTM:
    {"fine",          1, +8},
    {"dimunitive",   25, +4},
    {"tiny",         50, +2},
    {"small",       250, +1},
    {"medium",      500,  0},
    {"large",      2500, -1},
    {"huge",       5000, -2},
    {"gigantic",  25000, -4},
    {"cyclopean", 50000, -8},
    };

VolumeStat *VolumeStat::array = arr_volume;

void initEnums( void ) {
// ToDo:
// Move the clumsy arrays out of this file...
// Initialise enums by loading data from the files...
	}

#endif // ENUM_CPP
