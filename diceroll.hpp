// [diceroll.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( DICEROLL_HPP )
#define DICEROLL_HPP

#include "string.hpp"

class DiceRoll {

	public:
		DiceRoll( void )
		 : num(1), sides(6), bonus(0) {}
     
		DiceRoll( char n, char s, char b = 0 )
		 : num(n), sides(s), bonus(b) {}

		String toString();
		int roll() const;

  	DiceRoll &operator +=( const DiceRoll& );
	private:

		char num;
		char sides;
		char bonus;


	};

#endif // DICEROLL_HPP
