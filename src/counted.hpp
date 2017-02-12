// [counted.hpp]
// (C) Copyright 2000 Michael Blackney
#if !defined ( COUNTED_HPP )
#define COUNTED_HPP

#include "enum.hpp"

class RCObject {
	public:
		virtual void addReference();
		virtual void removeReference();

		void markUnshareable();
		bool isShareable() const;

		bool isShared() const;

		int Count( void ) {return refCount;}

	protected:
		RCObject();
		RCObject(const RCObject& rhs);
		RCObject& operator=(const RCObject& rhs);
		virtual ~RCObject() = 0;

	private:
		int refCount;
		bool shareable;
	};


template<class T>
class RCPtr {
	public:
		RCPtr(T* realPtr = 0);
		RCPtr(const RCPtr& rhs);
		~RCPtr();

		RCPtr& operator=(const RCPtr& rhs);

		T* operator->() const;
		T& operator*() const;

	private:
		T *pointee;

		void init();
	};

template <class T>
RCPtr<T>::RCPtr(T *realPtr)
: pointee(realPtr) {
	init();
	}

template <class T>
RCPtr<T>::RCPtr(const RCPtr<T> &rhs)
: pointee (rhs.pointee) {
	init();
	}

template <class T>
RCPtr<T>::~RCPtr() {
	if (pointee) pointee->removeReference();
	}

template <class T>
void RCPtr<T>::init() {

	if (pointee == 0)    // if the dumb pointer is null,
		return;            // so is the smart one

	if (pointee->isShareable() == false) { // if the value
		pointee = new T(*pointee);           // isn't shareable,
		}                                    // copy it

	pointee->addReference();   // note that there is now a
														 // new reference to the value
	}

template <class T>
RCPtr<T>& RCPtr<T>::operator=(const RCPtr<T>& rhs) {

	if (pointee != rhs.pointee) {
		T *oldPointee = pointee;
		pointee = rhs.pointee;
		init();
		if (oldPointee) oldPointee->removeReference();
		}

	return *this;
	}

template <class T>
T* RCPtr<T>::operator->() const { return pointee; }

template <class T>
T& RCPtr<T>::operator*() const { return *pointee; }




#endif // COUNTED_HPP
