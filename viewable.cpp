// [viewable.cpp]
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
#if !defined ( VIEWABLE_CPP )
#define VIEWABLE_CPP
#include "viewable.hpp"

Viewable::Viewable( Image i ) : image(i) {
	}
Viewable::Viewable( Viewable &v ) : image( v.image ) {
	}
Viewable::~Viewable( void ) {
	}
Image Viewable::getimage( void ) {
	return image;
	};
void Viewable::setimage( Image i ) {
	image = i;
	};

#endif // VIEWABLE_CPP
