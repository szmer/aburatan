// [compiler.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( COMPILER_HPP )
#define COMPILER_HPP

#include "config.hpp"
#include "standard.hpp"

// Default defines:
#define CLASS class
#define STDCLASS
#define MUTABLE mutable
#define INLINE

// Platform specific code:
// Bits per byte of other than 8 for systems with
// non-8bit bytes may cause unusual bugs.  Requires testing.
#if !defined (BITSPERBYTE)
#  define BITSPERBYTE 8
#endif

#if !defined (BITSPERLONG)
#  define BITSPERLONG 32
#endif


//---------=[ Compilers ]=--------

#ifdef __TCPLUSPLUS__ // No longer supported as program has become too large!
#  if ( __TCPLUSPLUS__ == 0x0300 )
#    define TRUEFALSE
#    define PRINTMEM        // prints memory usage
#    define INLINE_ASSEMBLY
#    define OBSOLETE_INCLUDE // includes files as <???.h>

#    define OS_DOS
#    define DOS_CONIO
#    ifdef USE_CURSES
#      undef USE_CURSES
#    endif // USE_CURSES

#    undef CLASS
#    define CLASS

#    undef MUTABLE
#    define MUTABLE

#    undef INLINE
#    define INLINE inline

#    define using
#    define namespace
#    define std

#  endif
#endif //__TCPLUSPLUS__


#ifdef __GNUC__

#  if defined THIS  // GCC standard headers may define THIS as "void"
#    undef THIS
#  endif

#endif //__GNUC__

//---------=[ Operating Systems Per Compiler ]=--------

#ifdef __MSDOS__
#  define OS_DOS
#endif

#ifdef __WIN32__
#  define Rectangle RECTANGLE_IS_A_WIN32_FUNCTION // yes, it's a hack :-)
#  define OS_WIN
#endif

#if defined( _linux_ ) or defined( __LINUX__ )  // because I don't know
#  define OS_LINUX                              // for sure
#endif
#if defined( linux ) or defined( _linux ) or defined( __linux ) // more
#  define OS_LINUX                                              // options :-)
#endif


//---------=[ Flags Per Operating System ]=--------

#ifdef OS_DOS
#  define USE_COLOR
#endif


#endif // COMPILER_HPP

