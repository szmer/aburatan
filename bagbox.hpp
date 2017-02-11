// [bagbox.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( BAGBOX_HPP )
#define BAGBOX_HPP

#include "item.hpp"
// Abstract class Container:

class Container : public Item {

	public:

    struct Option : public Item::Option {
      Option( void ) {
        image=Image(cBrown,'=');
        }
      };
		virtual void toFile( std::iostream & );

		virtual bool equals( const Target& );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

		virtual String name( NameOption=n_default );

		virtual long getVal( StatType );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.

		Container( const Option* );
		Container( Container & );
		Container( std::iostream&, TargetValue *TVP );

	};


// class Bag:
class Bag : public Container {

	public:

    struct Option : public Container::Option {
      };
      
    METHOD_create_option(Bag);
    METHOD_create_default(Bag);

		static Bag *load( std::iostream &ios, TargetValue *TVP ) {
			return new Bag(ios, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual Target clone( void );
		virtual bool equals( const Target& );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

		virtual String name( NameOption=n_default );
		virtual long getVal( StatType );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.

		Bag( const Option* );
		Bag( Bag & );
		Bag( std::iostream&, TargetValue *TVP );

	};

// class Bottle:
class Bottle : public Container {
	public:

    struct Option : public Container::Option {
      materialClass material;
      Option( void ) : material(m_invalid) {}
      };
      
    METHOD_create_option(Bottle);
    METHOD_create_default(Bottle);

		static Bottle *load( std::iostream &ios, TargetValue *TVP ) {
			return new Bottle(ios, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual Target clone( void );
		virtual bool equals( const Target& );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

    virtual String name( NameOption=n_default );
    virtual long getVal( StatType );
    virtual Image getimage( void );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		materialClass b_material;

		Bottle( const Option* );
		Bottle( Bottle & );
		Bottle( std::iostream&, TargetValue *TVP );

	};

// class Quiver:
//   also covers Bandoliers, Gunbelts, etc.
class Quiver : public Container {
	public:

    struct Option : public Container::Option {
      Target contents;
      int form;
      Option( void ) : contents(), form(0) {}
      };
      
    METHOD_create_option(Quiver);
    METHOD_create_default(Quiver);

		static Quiver *load( std::iostream &ios, TargetValue *TVP ) {
			return new Quiver(ios, TVP);
			}

		virtual void toFile( std::iostream & );

		virtual Target clone( void );
		virtual bool equals( const Target& );

		virtual targetType gettype( void ) const;
		virtual bool  istype( targetType t ) const;

		virtual bool capable( Action, const Target &t );
		virtual bool perform( Action, const Target &t );

		virtual String name( NameOption=n_default );
		virtual long getVal( StatType );

    virtual Image getimage( void );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.

    int form;

    static bool canCarry( int, Target );

		Quiver( const Option* );
		Quiver( Quiver & );
		Quiver( std::iostream&, TargetValue *TVP );

	};

#endif // BAGBOX_HPP
