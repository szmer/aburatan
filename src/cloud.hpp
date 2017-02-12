// [cloud.hpp]
// (C) Copyright 2003 Michael Blackney
#if !defined ( CLOUD_HPP )
#define CLOUD_HPP

#include "item.hpp"

class Cloud : public Item {

	public:

    struct Option : public Item::Option {
      materialClass material;
      Option( void ) : material(m_smoke) {
        image=Image(cGrey,'*');
        }
      };
      
		static Target create( const Option &o ) {

			Cloud *cloud = new Cloud(&o);

			TargetValue *THIS = new TargetValue( cloud );

			cloud->setTHIS(THIS);

			return Target(THIS);
			}

		static Cloud *load( std::iostream &ios, TargetValue *TVP ) {
			return new Cloud(ios, TVP);
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
		virtual void takeDamage( const Attack& );

	protected:
	// Any additions to class members must be reflected
	// in changes to copy constuctor, file stream constuctor and toFile.
		materialClass material;
    int opacity;

		Cloud( const Option* );
		Cloud( Cloud& );
		Cloud( std::iostream&, TargetValue *TVP );



	};


#endif // CLOUD_HPP
