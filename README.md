Aburatan for Unix
=================

[Aburatan](http://aburatan.sourceforge.net/) is an old, unfinished roguelike
for DOS by Michael Blackney, with latest release from 2004. I found it when
skimming alphabetically through the roguelike list in RogueBasin.com (so, great
choice of name on the author's part!). I found the source code to be extensive,
reasonably clean and object-oriented: this enticed me to do some minor tweaking
to compile it under Linux.

The game has very pleasant interface, "NetHacky" but more user-friendly. It
also features interactions between mobs (at least they attack each other),
minimap, shooting from firearms and possibly more. It surely crashes when you
try to move upstairs.

Installation
------------
Provided that you have `make`, `g++` and `ncurses-devel`, installed, run `make`
in the source directory. This should produce `aburatan` executable.

Licensing
---------
See the original `main.cpp`. The author lets you choose GPL v. 2 or later, and
so do I, if you want to use my minor modifications.

Reusability?
------------
I presently have no intentions to mantain or extend the original game myself.
However, I think that the code is potentially reusable and extendable, and might
save you countless hours if you plan to make your own "major" in scale
roguelike. (Of course it would require some elementary C++ maintaining skills.)

Do consider that it was early 2000s and the author rolls out his own memory
management, string class and other things that the STL is for. But his solutions
seem to be similar to the STL's, so refactoring may be possible if you find the
source otherwise suitable for your needs.
