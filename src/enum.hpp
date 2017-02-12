// [enum.hpp]
// (C) Copyright 2000 Michael Blackney
#if !defined ( ENUM_HPP )
#define ENUM_HPP

#include "types.hpp"
#include "compiler.hpp"
#include "standard.hpp"

// Boolean values:
//#define TRUEFALSE

#ifdef TRUEFALSE
enum bool {
	false = 0,
	true = 1
	};
#endif

// LOS values:
#define MULT 256
#define HALFMULT 127

// Sort values:
#define TYPE_SORT_SHIFT 8

enum StatType {

  // Stats:
	stat_crd,
	stat_per,
	stat_str,
	stat_tuf,
	stat_wil,
	stat_kno,
	stat_con,
	stat_spd,
	stat_hp,
  stat_total,

  // Difficulty classes:
  DC_open,
  DC_close,
  DC_untrap,
  DC_spot,
  DC_avoid,
  DC_hitmelee,
  DC_chat,

 // attribs bool:
  attrib_unique,  // Item is unique - always, 'the xx'
  attrib_disposable, // Single use item
  attrib_itemized,   // Item and contents are billed separately
	attrib_moved,
	attrib_empty,        // Tile has no contents
  attrib_empty_nodoor, // Tile has no contents other than (known) doors
  attrib_has_door,     // Tile has a door
  attrib_has_stairs,   // Tile has stairs
  attrib_non_wall,     // Tile is not perma-block (or pseudo, ie. hidden door)

	attrib_plural, // Replaced with "them","they"; preceeded by "these","those"
	attrib_pair,   // Pair of shoes, pair of pants, pair of scissors
  attrib_draw,   // Item is drawn, rather than wielded.  Pistols & swords.
  
  attrib_balanced,  // Item is balanced for combat in the off-hand 
  attrib_invisible,
  attrib_mask_color,
  attrib_mask_appearance,

  attrib_beam,  // beam weapon
  
	attrib_vis_mon,  // Does this tile contain a visible monster?
	attrib_vis_enemy,// Is the monster in this tile hostile to player?
  attrib_visible,  // Is this item/creature/tile visible?  Takes into
                   // account invisibility, lighting, etc.
  
	attrib_abstract, // Treated as abstract in the indefinite article (eg water)
	attrib_discrete, // Quantifiable by a number?
	attrib_two_handed,
	attrib_need_container,
  attrib_quaff_contents_only,
  attrib_pool,  // A level tile you can drown in
  attrib_sunlit,
  attrib_on_level,
  attrib_filled_quiver,
  attrib_auto_throw,

  // IMPORTANT: Don't confuse the next two attributes:
  attrib_alive,           // alive means 'animated' i.e. not killed or destroyed
  
  attrib_flesh_blood,     // this attrib is used to determine if a creature
                          // is "killed" or "destroyed" - living, breathing
                          // creatures are killed and undead & robots destroyed
                          // NOTE: also has nothing to do with 'breath' :)

  attrib_canswim,
  attrib_floats,
  attrib_flying,
  attrib_still,
  attrib_trader, // can trade
  attrib_shk,    // is shopkeeper (so will stay within the shop)
  attrib_floor,  // floor (as opposed to ground) Inside.

 // attribs long int:
  attrib_combat_style, // 1:melee, 2:ranged, 1&2:combat&ranged
  attrib_capacity,
	attrib_weight,
  attrib_sink_weight,
  attrib_volume,

  attrib_feat,
  attrib_proficiency,
  
  attrib_price,
  attrib_cash,
  attrib_denomination,

  attrib_reputation,
  attrib_fame,
  attrib_demeanor,

  attrib_boom,
  attrib_num_slugs,
  
  attrib_xp,
  attrib_gender,
  attrib_level,
	attrib_material,
	attrib_slot,
	attrib_opaque,
	attrib_ammotype,
	attrib_meleetype,
  
  attrib_melee_spd,
  attrib_ranged_spd,
  
	attrib_form,
  attrib_fuel,
	attrib_connecting,
  
  attrib_sort_category,
  
  //attrib_light_radius, // Light no longer exists
  attrib_search_radius,

  // Weapon attributes:
  attrib_beam_range,
  attrib_to_hit_bonus,
  attrib_range_unit,
  
  attrib_targeting,
  attrib_scope_flags,
  
	attrib_nutrition,     // How filling
  attrib_appetite,      // Which flavours we like
  attrib_flavour,       // Which flavours we are
  attrib_refreshment,   // How quenching
  attrib_thirst,        // Fluid flavour
  
  attrib_ooo_ness, // Out of Ordinary-ness (haha)
                   // For evaluating which feature should be shown on
                   // the map screen (i.e. the ones that make you go, "ooo!")


 // for temp objects:
  attrib_temp,
 // for grammar; second, third person, etc.
	attrib_person,

 // Number of stats:
	stat_num,
	stat_invalid = stat_num

	};

enum TileType {
  // Floor:
  tt_ground,
  tt_floor,
  tt_river,
  // Flora:
  tt_tree,
  tt_cactus,
  tt_bush,
  // Barriers:
  tt_wall,
  tt_fence,

  tt_num,
  tt_invalid

  };

enum Result {

  result_fumble,
  result_fail,
  result_pass,
  result_perfect

  };

enum NameOption {

  n_menu,         // a longrifle [0/12] (unpaid, $5.20)
  n_menu_noprice, // a longrifle [0/12]
  n_short,        // the rifle
  n_default,      // a longrifle
  n_prefix,       // long
  n_noun,         // rifle
  n_owner,        // John's ...

  n_num,
  n_invalid

  };


enum Gender {

  sex_male  = 0x01,
  sex_female= 0x02,
  
  sex_any   = sex_male&sex_female,
  sex_none  = 0x00

  };

enum Shop {

  shop_military,
  shop_hunting,
  shop_stable,
  shop_pharmacy,
  shop_hardware,
  shop_finewear,
  shop_workwear,
  shop_library,
  shop_saloon,
  shop_market,
  shop_church,

  shop_num,
  shop_random,
  shop_none
  
  };

enum FuelType {

  fuel_none,
  fuel_coal,
  fuel_cell,
  fuel_atomic,

  fuel_num,
  fuel_invalid

  };


typedef unsigned long Theme;

// Themes

#define  th_none      0x00000000

// Level types: first 32 bits
#define  th_level     0x0000FFFF
#define  th_aburatan  0x00000001
#define  th_gilead    0x00000002
#define  th_texarkana 0x00000004
#define  th_mines     0x00000008
#define  th_catacomb  0x00000010
#define  th_ruins     0x00000020
#define  th_sewers    0x00000040
#define  th_native    0x00000080

// Location themes: next 16 bits
#define  th_location  0x00FF0000
#define  th_kitchen   0x00010000 // Kitchen knife, cleaver
#define  th_workshop  0x00020000 // Hammer, bike chain
#define  th_lab       0x00040000 // Rubber gloves
#define  th_garden    0x00080000 // Shovel
#define  th_rare      0x00100000 // Katana, antique weapons


// Classification: last 16 bits
#define  th_class     0xFF000000
#define  th_steam     0x01000000
#define  th_undead    0x02000000
#define  th_mutant    0x04000000
#define  th_robot     0x08000000
#define  th_demon     0x10000000
#define  th_wild      0x20000000

// All themes:
//                    0x########
#define  th_all       0xFFFFFFFF

enum Hand {
	right, left
	};


enum Slot {

	s_head =0x001,  // Hat, helm
	s_eyes =0x002,  // Glasses, eyemask
  s_nose =0x004,  // Neckscarf, respirator
	s_neck =0x008,  // Tie, collar
  s_coat =0x010,  // Coat
	s_jack =0x020,  // Jacket
  s_shirt=0x040,  // Shirt
	s_arms =0x080,  // Bracers, wristwatch
	s_hands=0x100,  // Gloves, gauntlets
	s_legs =0x200,  // Trousers, skirts
	s_feet =0x400,  // Boots
	s_pack =0x800,  // Bandolier, belts, backpack

	s_none =0x000,
  s_suit = s_jack|s_legs, // Dress or suit - jacket and trousers slots
  s_face = s_eyes|s_nose, // Gasmask
  s_gaunt= s_hands|s_arms,// Gauntlets
	s_all  =0xFFF
	};


enum TVType {
	isRaw,
	isTarget,
	isSavedRaw
	};

enum EventType {
//	ev_actor,
	ev_action,
  ev_skill,

	ev_invalid
	};


// Opacity max for transparent tiles:
#define OPACITY_BLOCK 25

// Number of times to retry a create/place before aborting:
#define NUM_ABORT 100

// List comparison values:
enum compval {
	k_smaller,
	k_larger,
	k_same
	};

// Message constants:
#define BUFSIZE 80
#define MESSAGELINE 1
#define NUMMESSAGELINES 1 // Number of Message lines -1

// World Size
#define WORLD_WIDTH  80
#define WORLD_HEIGHT 25

enum State {
	solid,
	liquid,
	powder,
	ooze,
	cloud
	};


// Material types
enum materialClass {
// These have set values - if you edit them you have to edit the
// material data table to accommodate new/deleted entries.

  m_smoke,

  m_grass,

	m_snow,
	m_dirt,
	m_sand,
	m_ash,
  m_sulphur,

	m_water,
	m_blood,
  m_milk,
  m_acid,
  m_poison,
	m_t_oil,

	m_oil,
	m_sap,
	m_bile,
	m_ichor,
	m_vomit,

  m_flesh,
  m_putrid,
  m_bone,
  m_ice,
	m_brick,
	m_stone,
	m_bluestone,
	m_wood,
	m_straw,
	m_clay,
  m_coal,
  m_mulch,

	m_cotton,
	m_wool,
	m_leather,
  m_fur,

  m_paper,
  m_rubber,

  m_bronze,
  m_copper,
  m_tin,
  m_iron,
	m_steel,
  m_gold,
  m_silver,
  
  m_emerald,
  m_ruby,
	m_glass,

	m_total,
	m_invalid = m_total
	};

struct Material {

	const char *name;
  const char *adj;
  int weight;
	State state;
  long flavour;
	char color;
  bool floats;

  materialClass
    smash, heat, cool;

	static Material *data;
	};

enum Etching {
  // Types of writing:
  e_graffiti,
  e_engraved,

  // Other shapes marks can be in:
  e_smear,
  e_prints,
      
  e_none
  };

// Area class types
// These have set values - if you edit them you have to edit the
// areaData table to accommodate new/deleted entries.
enum areaClass {
	Sea = 0,
	Plains = 1,
	Woods = 2,
	Hills = 3,
	Mountains = 4,
	Desert = 5,
	numAreas
	};


struct area {
	// Constructor
	area( const char *const s, int i ): name(s), image(i) {}

	// Data members
	const char *name; // Area type name
	Image image; // Area image color/appearance values

	static area *Data;
	};
  


enum Volume {
  // Volumes are not meant to be accurate (cthulhu is *not*
  // the same volume as 50,000 coins!) but represent space taken
  // up when packing.
  //           // Volume  Example              Fight Bonus   OGL equiv.
	v_tiny,      //     x1  a coin, a mouse         +8         fine

	v_short,     //    x25  a rat, a short sword    +4         dimunitive
  v_long,      //    x50  a cat, a long sword     +2         tiny

	v_medium,    //   x250  a child, a dog          +1         small
	v_tall,      //   x500  an adult                 0         medium

	v_large,     //  x2500  a cow, a bear           -1         large
	v_huge,      //  x5000  an elephant             -2         huge

	v_gigantic,  // x25000  a dinosaur              -4         gargantuan
	v_cyclopean, // x50000  cthulhu                 -8         colossal

  v_num,
  v_invalid = v_num
    
	};

struct VolumeStat {

  const char *name;
  long volume;
  int  bonus;

  static VolumeStat *array;
  
  };

enum Temperature {

  // Todo: temperature (see enum.hpp)
  // Temperature events occur every few hundred turns (level
  // dependant) and affect every object on the level.

  tm_freeze,  // Colder than norm temperatures require more clothing
  tm_cool,    // and help keep food from spoiling.
  tm_norm,
  tm_heat,    // Warm temperatures melt ice and snow and require the
  tm_boil,    // player wear less clothing.

  tm_num,
  tm_invalid = tm_num

  };

enum Odor {

  od_none = 0,
  
  // The air here smells...
  od_stale,
  od_rotten,
  od_foul,
  od_fetid,
  od_noisome,
  od_mephitic,

  od_num,
  od_invalid = od_num
  };


struct OdorStr {
  const char *odor;
  const char *inc;
  const char *dec;
  static OdorStr *Type;
  };


// Why is this here?  What is the idea? :)
#define EXTENDED 128


/* Skins.
 *
 * Skins are used for menus.  Skins change the colors
 * and characters used to display the menu borders,
 * title and text.
 *
 */
enum menuSkin {
	sMain,
	sError,
	sInvent,
  sPopUp,
	sMessage,

	NumSkins,
	IllegalSkin = NumSkins
	};

struct skin {
	const char *name;
	Image
		corner_11, corner_12, corner_21, corner_22,
		north, south, east, west,
		bracket_l, bracket_r,
		cursor_l, cursor_r;

	char title, cursor, option;

	skin( const char *const nam,
				char c11c, char c11a, char c12c, char c12a, char c21c, char c21a, char c22c, char c22a,
						 char nc, char na, char sc, char sa, char ec, char ea, char wc, char wa,
						 char blc, char bla, char brc, char bra,
						 char clc, char cla, char crc, char cra,
						 char tit, char cur, char opt )
		:
		name(nam),
		corner_11(c11c, c11a), corner_12(c12c, c12a), corner_21(c21c, c21a), corner_22(c22c, c22a),
		north(nc, na), south(sc, sa), east(ec, ea), west(wc, wa),
		bracket_l(blc, bla), bracket_r(brc, bra),
		cursor_l(clc, cla), cursor_r(crc, cra),
		title(tit), cursor(cur), option(opt) {}

	static skin *Arr;

	};

// ENUM alnum
// for alpha/numeric character testing
enum alnum {
	alpha = 1,
	numeric = 2,
	alphanumeric = 3
	};


enum {
	map_width = 80,
	map_height = 20,

  lev_width = 20,
  lev_height = 14,

	map_left = 1,
	map_top = 3,

	map_right = map_left + map_width - 1,
	map_bottom = map_top + map_height - 1
	};

void initEnums( void );
#endif // ENUM_HPP
