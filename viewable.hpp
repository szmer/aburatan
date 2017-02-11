/* [viewable.hpp]
 * (C) Copyright 2000 Michael Blackney
 *  Viewable is an interface class from which anything to be displayed
 * visually is derived.  This includes creatures, objects, areas, &c.
 */
#if !defined ( VIEWABLE_C_H )
#define VIEWABLE_C_H

#include "types.hpp"

class Viewable {
	public:
	 // Construction, Destruction:
	 Viewable( Image );
	 Viewable( Viewable & );
	 virtual ~Viewable( void );
	 // Inheritable interface:
	 virtual Image getimage( void );
	 void setimage( Image );
	protected:
	 // Protected inheritable data:
	 Image image;
	};

#endif // VIEWABLE_C_H
