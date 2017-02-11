// [config.hpp]
// (C) Copyright 2000,2003 Michael Blackney
// Configuration definitions file.
#if !defined ( CONFIG_HPP )
#define CONFIG_HPP

#include "define.hpp"
// Do you wish to use color output?
#define USE_CURSES
#define USE_COLOR
// Do you wish to redefine the screen colors?
#define COLOR_REDEF

// Defines for unfinished/debugging code
//#define TRUEFALSE // defines bool, true and false (for old compilers)

//#define SOFT_LINK // child objects correctly link to saved parents

// Operating system defines.
//#define OS_DOS // OLD DOS
//#define OS_WIN
//#define OS_UNIX
// ...
//#define OS_WHATEVER

#endif // CONFIG_HPP
