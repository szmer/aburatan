// [random.cpp]
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
#if !defined ( RANDOM_CPP )
#define RANDOM_CPP

#include "random.hpp"
#include "input.hpp"

//- For random shop items:
#include "monster.hpp"
#include "weapon.hpp"
#include "scope.hpp"
#include "food.hpp"
#include "bagbox.hpp"
#include "fluid.hpp"
#include "clothing.hpp"
#include "ammo.hpp"
#include "trap.hpp"
#include "book.hpp"
#include "quantity.hpp"
#include "widget.hpp"
//-

#include "mtint.hpp"

#include "compiler.hpp"
#ifdef OBSOLETE_INCLUDE
#  include <time.h>
#  include <stdlib.h>
#else
#  include <ctime>
#  include <cstdlib>
#endif

void Random::randomize( void ) {
// Sets a random seed based on the time of day
	std::time_t t;

	sgenrand((unsigned) std::time(&t));
	}

int Random::randint( int n ) {
// returns a random number 0 < r < n
	if (n <= 0) return 0;

  // Todo: scale random numbers, rather than mod.
	return genrand() % n;
	}


bool Random::randbool( void ) {
// returns a true/false
	return randint(2)?true:false;
	}

long Random::randbit( long mask ) {
// Given a bitmask, returns a single random bit

  // Can only choose a bit when at least one bit is on:
  if (!mask) return 0;
  
  int num_bits = 0;
  int index;

  // Count the bits:
  for ( index = 0; index < BITSPERLONG; ++index )
    if ( (mask >> index) & 1 )
      ++num_bits;

  // Now iterate through the bits a random number of times
  // to find the one we want to return:
  num_bits = Random::randint(num_bits);

  for ( index = 0; index < BITSPERLONG; ++index )
    if ( (mask >> index) & 1 && num_bits-- == 0 )
      return (1 << index);

  return 0;
  
  }

Point Random::randpoint( Rectangle r ) {
// returns a random point from the rect
  Point ret;
  ret.setx( randint(r.getwidth()) + r.getxa() );
  ret.sety( randint(r.getheight()) + r.getya() );

  return ret;
  }

Point Random::borderpoint( Rectangle r ) {
// a random point from the edge

  // Grab a random point:
  Point ret = randpoint(r);

  // And make sure that one of the edges
  // is aligned with the edge of the rect:

  if (randbool()) {
    //North/south:
    if (randbool())
      // N
      ret.sety(r.getya());
    else
      // S
      ret.sety(r.getyb());
    }
  else {
    //West/east:
    if (randbool())
      // W
      ret.setx(r.getxa());
    else
      // E
      ret.setx(r.getxb());
    }
    
  return ret;
  }


// Gorey inspired names:
// Male
const char * GoreyMale[] = {
  "Angus",  "Augustus",  "Bill", "Billy", "Clavius",  "Desmond",  "Dick",
  "Edward",  "Edmund",  "Eli", "Emblus",  "Ernest",
  "Franz",  "Frederick",  "George",  "Hector",
  "Horace",  "Ike",  "James",  "Jesse", "Leo",  "Luke",  "Maximilian",
  "Marsh",  "Neville",  "Quentin",  "Robert",  "Ronald",
  "Sam",  "Serge",  "Titus",  "Tobias", "Victor",  "Wyatt", "Xerxes",

  NULL };

// Female
const char * GoreyFemale[] = {
  "Alexa",  "Alice",  "Amy",  "Celia",  "Clara",  "Charlotte",  "Drusilla",
  "Edna",  "Eliza",  "Elsie",  "Emily",  "Fanny",  "Flora",  "Grace",
  "Ida",  "Isabelle",  "Isobel",  "Kate",  "Lise",  "Louise",  "Lucy",
  "Mary",  "Maud",  "Millicent",  "Mirella",  "Nelly",  "Olive",  "Prue",
  "Rose",  "Sophia",  "Susan",  "Victoria",

  NULL };


// Native Names:
// Male
const char * NativeMale[] = {
  "Ahiga",  "Ashkii",  "Ata'halne",  "Bidziil",  "Gaagii",  "Gad",  "Hastiin",
  "Hok'ee",  "Naalnish",  "Nastas",  "Niichaad",  "Niyol",  "Sani",  "Shilah",
  "Shiye",  "Shizhe'e",  "Sicheii",  "Sik'is",  "Sike",  "T'iis",  "Tse",
  "Yanisin",  "Yas",  "Yiska",

  NULL };


// Female
const char * NativeFemale[] = {
  "Altsoba",  "Anaba",  "Asdza",  "At'eed",  "Dezba",  "Dibe",
  "Doba",  "Doli",  "Haloke",  "Hai",  "Manaba",  "Mosi",  "Nascha",
  "Ooljee",  "Sahkyo",  "Shadi",  "Shideezhi",  "Shima",  "Shimasani",
  "Sitsi",  "Yazhi",

  NULL };


// Spanish names:
// Male
const char * SpanishMale[] = {
  "Amador",  "Amato",  "Amistad",  "Castel",  "Caton",  "Chale",  "Ciro",
  "Consuelo",  "Devante",  "Diego",  "Emilio",  "Esteban",
  "Fidel",  "Galeno",  "Gaspar",  "Iago",  "Inigo", "Isidro",  "Manuel",
  "Neron",  "Oleos",  "Paco",  "Rico",  "Salvador",
  "Santiago",  "Santos",  "Senon",  "Tajo",  "Vidal",

  NULL };


// Female
const char * SpanishFemale[] = {
  "Alita",  "Amata",  "Arcadia",  "Belicia",  "Cochiti",
  "Damita",  "Dominga",  "Dulcinea",  "Eldora",  "Elena",  "Elisa",
  "Emerald",  "Esperanza",  "Gitana",
  "Herminia",  "Ines",  "Isabel",  "Isleta",  "Jaimica",  "Juanita",
  "Liani",  "Lola",  "Lujuana",  "Madeira",  "Madra",
  "Melisenda",  "Melosa",  "Mireya",  "Mora",
  "Neva",  "Nina",  "Patia",  "Querida",
  "Ria",  "Rosalind",  "Sandia",  "Savanna",  "Sierra",  "Solana",
  "Tia",  "Tierra",

  NULL };


String Random::name( bool isMale, Theme theme ) {
// returns a random, thematic name

  const char **name_list = NULL;
  
  if (isMale) {
    name_list = GoreyMale;

    if (theme & th_native)
      name_list = NativeMale;
//    else if (theme & th_spaniard)
//      name_list = SpanishMale;
    }
  else {
    name_list = GoreyFemale;

    if (theme & th_native)
      name_list = NativeFemale;
//    else if (theme & th_spaniard)
//      name_list = SpanishFemale;
    }

  return pickfrom(name_list);
  }


String Random::pickfrom( const char **name_list ) {
// returns a random string from a list
// Todo: rewrite pickfrom so that names will only be picked once
  String ret_str;
  const char **iterator = name_list;
  int list_size = 0;

  while (*iterator != NULL) {
    ++list_size;
    ++iterator;
    }

  assert(list_size);
  // Error::fatal("Invalid list used to generate a random name!");
  
  ret_str = name_list[ randint(list_size) ];
  
  return ret_str;
  }

String Random::surname( Theme ) {
// returns a random, thematic surname
  String ret_str;

  // Gorey inspired names:
  /*

  // From "The Nursery Frieze"
   // Potential items:
  Tacks  Bandages  Wax  Vanilla  Paste
  Geranium  Thunder  Whistle  Hymn
  Glue  Aspic  Dust  Tombola

   // Potential traits:
    Distaste       ( +Per      ?? )
    Hubris         ( +Wil -Kno ?? )
    Dismemberment  ( +Str -Crd ?? )
    Remorse        ( +Wil -Tuf ?? )
    Divorce        ( -Rep +Per ?? )
    Whim           ( -Wil +Spd ?? )
    Betrayal       ( -Rep +Kno ?? )
    Mistrust       ( +Per -Wil ?? )
    Velleity       ( -Wil      ?? )

   // Curious words:
  Archipelago  Cardamon  Obloquy  Ignavia
  Samisen  Gavelkind  Turmeric  Imbat
  Cedilla  Cassation  Hendiadys  Quincunx
  Corposant  Madrepore  Ophicleide  Jequirity
  Sphagnum  Aceldema  Lunistice  Cranium
  Febrifuge  Ampersand  Opopanax  Baize
  Hellebore  Obelus  Cartilage  Maze
  Antigropelos  Piacle  Occamy  Maremma
  Accismus  Badigeon  Epistle  Quodlibet
  Catafalque  Hiccup  Idioticon  Gibus
  Bortargo  Phylactery  Gegenschein  Clavicle
  Sago  Bellonion  Thurible  Aphthong
  Plumbago  Amaranth  Rhoncus  Pantechnicon
  Diaeresis  Purlicue  Saradrap  Ciatrix
  Salsify  Palindrome  Bosphorus  Narthex
  Chalcedony  Phosphorus  Ligament  Exequies
  Spandrel  Chandoo  Etui  Anamorphosis
  Wapentake  Orrery  Ganosis
  
  // From "The Pious Infant"
  Clump
  
  // From "The Evil Garden"
  Floggle

  // From "The Gilded Bat"
  Splaytoes  Marshgrass  Hochepot
  Golopine  de Zabrus  Zug

  // From "The Osbick Bird"
  Osbick  Fingby  Periboo  Oad

  // From "The Unstrung Harp"
  Earbrass  Ladderback  Disshiver  Lirp
  Hangdog  Glassglue  Truffle  Prawne
  Scuffle  Dustcough  Lawk  Avuncular
  Legbail  Frowst  Knout  Claws

  // From "The Willowdale Handcar"
  Sprocket  Queevil  Flim  Underfoot
  Hammerclaw  Chutney  Blotter  Dogear
  Claggs  Stovepipe  Umlaut
  
  // From "The Insect God"
  Frastley

  // From "The Remembered Visit"
  Crague
  Skrimpshaw

  // From "The Headless Bust"
  Gravel

  // From "The Other Statue"
  Underfold  Thump  Lisping  Lozenge
  Gollop  Stringless  MacAbloo  Belgravius
  Touchpaper  Scone
  
  // Pseudonyms
  Dowdy  Blutig  Pig  Wryde
  
  */
  
  return ret_str;
  }


const char * WestTown[] = {

  // Western themed town names:
  "Desperation",
  "Gravesend",
  "Tombstone",

  // Edward Gorey
  "Gehenna",
  "Mortshire",
  "Crampton",
  "Saint Clot",
  "Nether Millstone",
  "Yarborough",
  
  // H. P. Lovecraft
  "Arkham",
  // "Innsmouth", // Reserved for a unique location
  
  // A Canticle for Leibowitz
  "Sanly Bowitts",
  "Rio Grande",
  // "Texarkana", // Reserved for a unique location

  // The Midwich Cuckoos
  "Midwich",

  // The Dark Tower
  // "Gilead", // Reserved for a unique location
  "Lud",

  NULL };

String Random::townname( Theme ) {
// returns a random, thematic town name
  return pickfrom(WestTown);
  }

const char * Fortunes[] = {

  "a kaleidoscope could change your life",
  "sometimes trees bear more than fruit",
  "in time, your every wish will come true",
  
  NULL };

String Random::fortune() {
// returns a random, thematic town name
  return pickfrom(Fortunes);
  }

const char * Graffiti[] = {
// These are used as random graffiti scrawled
// in the town or dungeons.

 // Abura Tan website
  "DEAD END",
  
 // Misc.
  "No ammunition",
  "No food",
  
 // Dawn of the Dead (remake)
  "Alive inside",
  
 // The Dark Tower
  "Bango Skank",
  "See the Turtle",
  "F-ck the Imperium!",

 // The Fellowship of the Ring
  "They are coming...",

  NULL
  };

String Random::graffiti() {
// returns a random, thematic town name
  return pickfrom(Graffiti);
  }


const char * Epitaphs[] = {
// These are used on headstones as random engravings.

  // Expected
  "R.I.P.",
  "Rest in peace.",
  
  // Nethack - DYWYPI
  "Die? Why? Pie?",

  // Tribute
  "Here lies Raisse.  Killed by a newt, while helpless.",

  // Dark Tower (Jake)
  "Go then.  There are other worlds than these."

//# define NO_AUTHENTIC
  // Here follows a list of epitaphs and famous last
  // words.  If you find the authentic ones
  // particularly gruesome, uncomment the above #define

# ifndef NO_AUTHENTIC
  // Warner Baxter
  "Did you hear about my operation?",
  // Antoine Barnave
  "This, then, was my reward.",
  // Constance Bennett
  "Do not disturb.",
  // Humphrey Bogart
  "I should never have switched from Scotch to Martinis.",
  // Charles Burney
  "All this will soon pass away as a dream.",
  // Alexandre Dumas
  "I shall never know how it all comes out.",
  // Epicurus
  "Farewell, and remember my teachings.",
  // Adam Ferguson
  "There is another world.",
  // Cary Grant
  "He was lucky, and he knew it.",
  // Heinrich Heine
  "God will pardon me... it is His profession.",
  // George S. Kaufman
  "I'm not afraid anymore.",
  // John Keats
  "Here lies one whose name was writ in water.",
  // Andre Maginot
  "For me, this is the end, but you continue!",
  // Karl Marx
  "Last words are for fools who haven't said enough.",
  // Meher Baba
  "Don't worry, be happy.",
  // Charlotte Mew
  "Don't keep me, let me go.",
  // Henry Murger
  "No more music, no more commotion, no more Bohemia.",
  // Ramon Maria Narvez
  "I do not have to forgive my enemies, because I killed them all.",
  // Phocion
  "No resentment.",
  // Pablo Picasso
  "Drink to me.",
  // Robbie Ross
  "Here lies one whose name was written in hot water.",
  // William Shakespeare
  "Good friend, for Jesus' sake forbear; To dig the dust enclosed here.",
  // Sixtus
  "Cease weeping, you will soon follow me!",
  // Lewis Stone
  "A gentleman farmer goes back to the soil.",
  // Ben Travers
  "This is where the real fun starts.",
  
# endif // NO_AUTHENTIC

  NULL };

String Random::epitaph() {
// returns a random, thematic town name
  return pickfrom(Epitaphs);
  }

Target Random::shopitem( Shop shop ) {

    shop = shop_hardware;

    if (shop == shop_random)
      shop = (Shop) randint(shop_num);
  
    switch (shop) {
      case shop_military:
        if ( randbool() )
          return Ranged::create();

        //##FALL_THROUGH##
      case shop_hunting: {
        Target bullet;
        do
          bullet = Ammo::create();
          while (bullet->getVal(attrib_ammotype)==bul_grenade);

        Quantity::Option q_opt;
        q_opt.target = bullet;
        q_opt.quantity = DiceRoll(3,4).roll();
        return Quantity::create(q_opt);
        }

      case shop_hardware: {

        switch( randint(3) ) {
          case 0:  return Melee::create();
          //case 1:  return Mount::create();
          default: return MachinePart::create();
          }
        
        }
        
      case shop_finewear: return Clothing::create();

      case shop_workwear: return Clothing::create();

      case shop_pharmacy: {
        Target ret_item = Bottle::create();
        Fluid::Option fl_opt;
        
        if (ret_item->getVal(attrib_material) == m_leather)
          fl_opt.material = m_water;

        ret_item->perform( aContain, Fluid::create(fl_opt) );
        return ret_item;
        }

      case shop_library:  return Book::create();


      case shop_stable: /*
      {
        Monster::Option m_opt;
        m_opt.form = crHorse;
        return Monster::create(m_opt);
        }  */
      
      case shop_saloon:
      case shop_church:
      default: return Melee::create();
      }
  }

Target Random::questitem( void ) {
  return shopitem( shop_hardware );
  }

Target Random::themeitem( Theme theme ) {

  Target item;
  // Create an item of the appropriate theme:
  switch(Random::randint(10)) {
    case 0: item = Clothing::create(); break;
    case 1: item = Ranged::create(); break;
    case 2: {
            Quantity::Option q_opt;
            q_opt.quantity = DiceRoll(2,12).roll();
            q_opt.target = Ammo::create();
            item = Quantity::create( q_opt ); break;
            }
    case 3: item = Food::create(); break;
    case 4: item = Bottle::create(); break;
    case 5: item = Bag::create(); break;
    case 6: item = Fluid::create(); break;
    case 7: item = Scope::create(); break;
    case 8: item = Book::create(); break;
    
    default:
      Melee::Option mel_opt;
      mel_opt.theme = theme;
      item = Melee::create(mel_opt); break;
    }

  return item;
  }

#endif // RANDOM_CPP

