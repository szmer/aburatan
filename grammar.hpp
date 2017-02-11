// [grammar.hpp]
// (C) Copyright 2000-2002 Michael Blackney
#if !defined ( GRAMMAR_HPP )
#define GRAMMAR_HPP

#include "string.hpp"
#include "target.hpp"

class Grammar {

	// Articles, Determiners and Pluralisation:
	public:
		enum determiner {
		 det_definite,
		 det_indefinite,
		 det_demonst_with,
		 det_demonst_wout,
		 det_possessive,
		 det_abstract,
     det_another
		 };
    static String plural( String s, int num, bool abstract, bool pair, determiner d = det_indefinite, Target owner = Target(NULL) );
		static String plural( const Target&, determiner d = det_indefinite );

	// Pronouns:
	public:
		enum person {
		 p_second,   // Second person:          You
		 p_third_m,  // Third person masculine: He
		 p_third_f,  // Third person feminine:  She
		 p_third_n,  // Third person neuter:    It
		 p_third_pl  // Third person plural:    They
		 };

		static String Subject( const Target&, determiner d = det_indefinite );
		static String Object( const Target&, determiner d = det_indefinite );
		static const bool Pronoun( void );
		static void newsentence( void );

		static void clr( void );

	private:
		static Target *s_current, *s_last,
									*o_current, *o_last;
		static bool pronoun;

	};

#endif // GRAMMAR_HPP
