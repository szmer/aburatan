// [target.hpp]
// (C) Copyright 2000 Michael Blackney
#if !defined ( TARGET_HPP )
#define TARGET_HPP

//#define TargetHarness
//#define TRUEFALSE


#include "string.hpp"
#include "counted.hpp"
#include "serial.hpp"

#include "compiler.hpp"
#ifdef OBSOLETE_INCLUDE
#  include <fstream.h>
#else
#  include <fstream>
#endif

class RawTarget;
class Target;
class cat_Handle;
class Catalogue;

struct TargetValue: public RCObject {

	TargetValue( long );
	TargetValue( RawTarget * );
	~TargetValue();

	void init( RawTarget * );
	void init( Target * );
	void init( long );

	void       toFile( void );
	RawTarget *fromFile( void );
  
	void transform( RawTarget * );
	void transform( const Target& );

  bool isNull( void ) const;
  bool inMem( void ) const;

	virtual void addReference();
	virtual void removeReference();
  
	void decSaveCount( void );
	void incSaveCount( void );

# ifdef SOFT_LINK
	void decMemSoftCount( void );
	void incMemSoftCount( void );
	void decSaveSoftCount( void );
	void incSaveSoftCount( void );
# endif // SOFT_LINK

  int totalCount( void ) { return Count() + saveCount; }

	RawTarget *getData( void );

	Serial &getSerial( void );

	friend CLASS RCPtr<TargetValue>;

	private:
	explicit TargetValue( TargetValue& rhs );

		union {
			RawTarget *raw;
			Target *target;
			} data;

		TVType type;

		Serial serial;

		int saveCount;

# ifdef SOFT_LINK
    int memSoftCount;
    int saveSoftCount;
# endif //SOFT_LINK
	};


class Target {
	private:
		RCPtr<TargetValue> value;

	public:
		Target( TargetValue *ptv = NULL );

		INLINE RawTarget *raw() const;

		String menustring( char len = 80 );
		Image menuletter( void );

		INLINE int numRefs( void );

		void transform( const Target& ) const;

		void save( void );
		void load( void );

		bool operator ==( const Target & ) const;
		bool operator !=( const Target & ) const;
		bool operator !( void ) const;

		bool operator < ( const Target& );
		bool operator > ( const Target& );

		RawTarget* operator->() const;
		RawTarget& operator*() const;

    friend class Catalogue;
		friend STDCLASS std::iostream &operator << ( std::iostream &, Target & );
		friend STDCLASS std::iostream &operator >> ( std::iostream &, Target & );
	};

std::iostream &operator << ( std::iostream &, Target & );
std::iostream &operator >> ( std::iostream &, Target & );

#endif // TARGET_HPP

