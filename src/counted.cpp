// [counted.cpp]
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
#if !defined ( COUNTED_CPP )
#define COUNTED_CPP
#include "counted.hpp"
#include "standard.hpp"

RCObject::RCObject()
: refCount(0), shareable(true) {
	}

RCObject::RCObject(const RCObject&)
: refCount(0), shareable(true) {
	}

RCObject& RCObject::operator=(const RCObject&) {
	return *this;
	}

RCObject::~RCObject() {
	}

void RCObject::addReference() {
	++refCount;
	}

void RCObject::removeReference() {
	if (--refCount == 0)
		delthis( this );
	}

void RCObject::markUnshareable() {
	shareable = false;
	}

bool RCObject::isShareable() const {
	return shareable;
	}

bool RCObject::isShared() const {
	return (refCount > 1)? true: false;
	}


////////////////////////////////////////////////////



#endif // COUNTED_CPP
