/*
	STEM_KROVETZ.H
	--------------
	This code came from the Lemur toolkit and has been reformatted and modified
	for inclusion in ATIRE.  the original copyright notice is below.  The LICENSE
	is similar to the BSD license and allows commercial and academic use.
	Its no longer thread safe because that was Lemur specific stuff
*/
/*==========================================================================
 * Copyright (c) 2005 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.lemurproject.org/license.html
 *
 *==========================================================================
 */
/*
	dmf 07/29/2005
	C++ thread safe implementation of the Krovetz stemmer.
	requires no external data files.
*/
#ifndef STEM_KROVETZ_H_
#define STEM_KROVETZ_H_

#include <iostream>
#include <string.h>

using namespace std;

#if (defined(ANDROID) || defined(__ANDROID__))
	#include <hash_map>
	#define ATIRE_KROVETZ_HAS_HASH_MAP
#elif defined(__APPLE__)
	#define ATIRE_KROVETZ_HAS_UNORDERED_MAP
	#include <AvailabilityMacros.h>
	#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ == MAC_OS_X_VERSION_10_8
		/*
			It isn't at all obvious why or how, but the install of Xcode in the Information Retrieval Lab at
			the University of Otago has two configurations.  One has unordered_map in the tr1 directory and
			the other does not.  At present its possible to distinguish between the two by examining the OSX
			version of the machine Xcode is installed on.  The versions of Xcode are identical, but it might
			be that one has the command line tools and the other does not.
		*/
		#include <tr1/unordered_map>
		using namespace std::tr1;
	#else
		#include <unordered_map>
	#endif
#elif defined(__GNUC__)
	#include <tr1/unordered_map>
	#define ATIRE_KROVETZ_HAS_UNORDERED_MAP
	using namespace std::tr1;
//	using namespace __gnu_cxx;
#elif defined (_MSC_VER)
	#include <hash_map>
//	#include <unordered_map>
//	#define ATIRE_KROVETZ_HAS_UNORDERED_MAP
//	using namespace std::tr1;
#else
	#include <hash_map>
	#define ATIRE_KROVETZ_HAS_HASH_MAP
#endif

#include "stem.h"

/*
	class ANT_STEM_KROVETZ
	----------------------
*/
class ANT_stem_krovetz : public ANT_stem
{
public:
	static const int MAX_WORD_LENGTH = 25;	/// maximum number of characters in a word to be stemmed.

private:
	/// Dictionary table entry
	typedef struct dictEntry
		{
		bool exception;      		/// is the word an exception to stemming rules?
		const char *root;			/// stem to use for this entry.
		} dictEntry;

	/// Two term hashtable entry for caching across calls
	typedef struct cacheEntry
		{
		char flag; 						/// flag for first or second entry most recently used.
		char word1[MAX_WORD_LENGTH];	/// first entry variant
		char stem1[MAX_WORD_LENGTH];	/// first entry stem
		char word2[MAX_WORD_LENGTH];	/// second entry variant
		char stem2[MAX_WORD_LENGTH];	/// second entry stem
		} cacheEntry;

	#if defined(ATIRE_KROVETZ_HAS_UNORDERED_MAP)
		struct eqstr {bool operator()(const char* s1, const char* s2) const { return strcmp(s1, s2) == 0; }};
		typedef unordered_map<const char *, dictEntry, hash<string>, eqstr> dictTable;
	#elif defined (ATIRE_KROVETZ_HAS_HASH_MAP)
		struct eqstr {bool operator()(const char* s1, const char* s2) const { return strcmp(s1, s2) == 0; }};
		typedef hash_map<const char *, dictEntry, hash<const char *>, eqstr> dictTable;
	#else
		#if defined(_WIN32)
			//	studio 7 hash_map provides hash_compare, rather than hash
			//	needing an < predicate, rather than an == predicate.
			struct ltstr {bool operator()(const char* s1, const char* s2) const { return strcmp(s1, s2) < 0; }};
			typedef stdext::hash_map<const char *, dictEntry, stdext::hash_compare<const char *, ltstr> > dictTable;
		#else
			struct eqstr {bool operator()(const char* s1, const char* s2) const { return strcmp(s1, s2) == 0; }};
			typedef hash_map<const char *, dictEntry, hash<const char *>, eqstr> dictTable;
		#endif
	#endif

private:
	dictTable dictEntries;
	// this needs to be a bounded size cache.
	// kstem.cpp uses size 30013 entries.
	cacheEntry *stemCache;

	int stemhtsize;	// size
	int k;			// k = wordlength - 1
	int j;			// j is stemlength - 1
	char *word;		// pointer to the output buffer

private:
	bool ends(const char *s, int sufflen);
	void setsuff(const char *str, int length);
	dictEntry *getdep(char *word);
	bool lookup(char *word);
	bool cons(int i);
	bool vowelinstem(void);
	bool vowel(int i);
	bool doublec(int i);
	void plural(void);
	void past_tense(void);
	void aspect(void);
	void ion_endings(void);
	void er_and_or_endings(void);
	void ly_endings (void);
	void al_endings(void) ;
	void ive_endings(void) ;
	void ize_endings(void) ;
	void ment_endings(void) ;
	void ity_endings(void) ;
	void ble_endings(void) ;
	void ness_endings(void) ;
	void ism_endings(void);
	void ic_endings(void);
	void ncy_endings(void);
	void nce_endings(void);

	void loadTables(void);

public:
	ANT_stem_krovetz();
	~ANT_stem_krovetz();

	/*
		stem a term using the Krovetz algorithm into the specified
		buffer. The stem returned may be longer than the input term.
		Performs case normalization on its input argument. 
		returns the number of characters written to the buffer, including
		the terminating '\\0'. If 0, the caller should use the value in term.
	*/
	int kstem_stem_tobuffer(char *term, char *buffer);

	/*
		Add an entry to the stemmer's dictionary table.
	*/
	void kstem_add_table_entry(const char *variant, const char *word, bool exc = false);

	/*
		ATIRE specific stuff
	*/
	virtual size_t stem(const char *term, char *destination)
		{
		size_t length;
		if ((length = kstem_stem_tobuffer((char *)term, destination)) == 0)
			{
			strcpy(destination, term);
			return strlen(destination);
			}
		else
			return length;
		
		}
	virtual char *name(void) { return "Krovetz"; }
};

#endif /* STEM_KROVETZ_H_ */

