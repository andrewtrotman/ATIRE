/**
 * DoubleMetaphone.h, contains the definition of a C++ template class which implements
 * Lawrence Phillips' Double Metaphone phonetic matching algorithm, as published in the 
 * June 2000 issue of C/C++ Users Journal.  
 * 
 * Implementation and optimizations by
 * Adam J. Nelson (anelson@apocryph.org).  
 * 
 * To get the latest version of this library, as well as implementations for other languages,
 * go to http://www.apocryph.org/metaphone/.  The aforementioned URL also includes links 
 * to the series of articles I have written on the use of my various Double Metaphone
 * implementations.
 * 
 * This version is a completely different implementation, and while I did shamelessly 
 * copy/paste the main workhorse function (buildMetaphoneKeys), there are significant
 * differences between my implementation and Lawrence's. 
 * 
 * First, my implementation does not require MFC; in fact, it should work with any modern C++
 * compiler on any platform.  I've only tested it with Visual C++ 6 and 7 on Windows, however
 * I use no Windows or VC++ constructs.
 * 
 * Second, my implementation is not a subclass of a String class, while Lawrences MString class
 * derived from the MFC CString class.  This reflects my belief that the entity being represetned
 * by the class is not the string itself, but rather the metaphone keys for a string.  This approach
 * lends itself well to using the DoubleMetaphone class as a key in various container classes.  To that
 * end:
 * 
 * Third, my implementation implements all of the comparison operators, in terms of the double
 * metaphone keys computed for a given string.  Thus, not only can you perform comparisons yourself
 * on the metaphone representation of words, but you can use the countless container templates, such as
 * STL and the MFC generic containers, using the DoubleMetaphone class as the key.  In this way you can perform
 * phonetic lookups with nothing more than an associative container and this class.
 * 
 * Fourth, in my unscientific tests with a word list of 21k surnames, my implementation computes the keys
 * for all 21k names approx 2x faster.  This is due primarily to the improved ability of the compiler to 
 * inline when using a fully inline class definition, and a few minor optimizations over Lawrence's original
 * code.
 * 
 * Fifth, my implementation works with Unicode or ANSI builds, while Lawrence's worked only with ANSI.  That said,
 * Double Metaphone works only w/ the Roman alphabet and a few other characters, so don't run it against a 
 * Hangul string and expect it to work.
 * 
 * Current Version: 1.0.0
 * Revision History:
 * 	1.0.0 - ajn - First release
 * 
 * This implementation is Copyright (C) 2003 Adam J. Nelson, All Rights Reserved.
 * The Double Metaphone algorithm was written by Lawrence Phillips, and is 
 * Copyright (c) 1998, 1999 by Lawrence Philips.
 */
#pragma once

//Apparently Lawrence doesn't like our boolean operators...
#define AND &&
#define OR ||

#include <string.h>
#include <stdarg.h>
#include "str.h"

//Depending upon whether this build is unicode or not, use the appropriate CRT functions
#ifdef _UNICODE
typedef wchar_t tchar;
	#define _tcslen	wcslen
	#define _tcscpy	wcscpy
	#define	_tcscmp	wcscmp
	#define _tcscat wcscat
	#define _tcsupy wcsupr
	#define _tcsstr	wcsstr
#else //!_UNICODE
typedef char tchar;
	#define _tcslen	strlen
	#define _tcscpy	strcpy
	#define	_tcscmp	strcmp
	#define _tcscat strcat
	#ifdef _MSC_VER
		#define _tcsupy _strupr
	#else
		#define _tcsupy strupr
	#endif
	#define _tcsstr	strstr
#endif //!_UNICODE

//Macro to preceed literal strings with "L", which denotes a unicode string, for unicode builds, or
//Do nothing for non-unicode builds
#ifndef _TEXT
	#ifdef _UNICODE
		#define	_TEXT(x) L##x
	#else
		#define _TEXT(x) x
	#endif //_UNICODE
#endif //_TEXT


/**
 * Template class which implements the DoubleMetaphone algorithm for arbitrary key lengths.
 * To use, pass the word to be keyed with the metaphone algorithm to the ctor, and call
 * getPrimaryKey and getAlternateKey to retrive the resultant keys.  Alternatively, use
 * the static buildMetaphoneKeys to populate pre-allocated strings with keys for a given 
 * word
 * 
 * @author Algorithm by Lawrence Phillips; Template implementation and enhancements by Adam Nelson
 */
template<unsigned int _KeyLength>
class DoubleMetaphone {
public:
	/**
	 * Create an instance of the DoubleMetaphone class, computing and storing the double metaphone
	 * keys for a given word.
	 * 
	 * @param word   Word for which to compute metaphone keys
	 */
	DoubleMetaphone(const tchar* word) {
		m_word = NULL;
		m_originalWord = NULL;
		computeKeys(word);
	}

	/** Default ctor, initializes to a zero-length string.  Call computeKeys to compute keys for a word */
	DoubleMetaphone() {
		computeKeys(_TEXT(""));
	}

	/** Copy ctor, called when you do things like this:
	 * DoubleMetaphone mphone1("foo");
	 * DoubleMetaphone mphone2 = mphone1;
	 * 
	 * as well as under less explicit circumstances.
	 * This impl copies the results of the metaphone key computation
	 * 
	 * Implemented not so you can do that, but so this class will be more
	 * useful with container classes, esp the STL container templates */
	DoubleMetaphone(const DoubleMetaphone<_KeyLength>& rhs) {
		m_originalWord = m_word = NULL;

		//Re-use the assignment operator, which does effectively the same thing
		//we want to do here in the copy ctor
		*this = rhs;
	}

	/**
	 * dtor.  Called to cleanup.  Note the key strings are freed in the dtor; you must
	 * copy them before the object is destroyed if you want to use them
	 */
	virtual ~DoubleMetaphone() {
		if (m_word) {
			delete[] m_word;
		}
	}

	/** Assignment operator, called when you do this:
	 * DoubleMetaphone mphone1("foo");
	 * DoubleMetaphone mphone2("bar");
	 * 
	 * mphone2 = mphone1;
	 * 
	 * Implemented not so you can do that, but so this class will be more
	 * useful with container classes, esp the STL container templates */
	DoubleMetaphone& operator=(const DoubleMetaphone<_KeyLength>& rhs) {
		m_originalWord = rhs.m_originalWord;
		if (m_word) {
			delete[] m_word;
		}
		m_word = new tchar[::_tcslen(rhs.m_word)+1];

		::_tcscpy(m_word, rhs.m_word);

		::_tcscpy(m_primaryKey, rhs.m_primaryKey);
		::_tcscpy(m_alternateKey, rhs.m_alternateKey);

		m_hasAlternate = rhs.m_hasAlternate;

		return *this;
	}

	/** Equality operator, compares two instances of a class, like:
	 * DoubleMetaphone mphone1("Nelson");
	 * DoubleMetaphone mphone2("Neilsen");
	 * 
	 * if (mphone1 == mphone2) {
	 *	...
	 * 
	 * Equality for the doublempetahone class is defined as either of the two
	 * metaphone keys of rhs matching either of the two metaphone keys of the instance.
	 * That is:
	 * 		primary = primary
	 * 		primary = alt
	 * 		alt = primary
	 * 		alt = alt
	 * This four-way comparison is necessary because in some cases, given two words m and n,
	 * primary(m) != primary(n), but primary(m) = alternate(n) or alternate(m) = primary(n)
	 **/
	bool operator==(const DoubleMetaphone<_KeyLength>& rhs) const {
		return(
			  (::_tcscmp(m_primaryKey, rhs.m_primaryKey)==0) ||
			  (rhs.m_hasAlternate && ::_tcscmp(m_primaryKey, rhs.m_alternateKey)==0) ||
			  (m_hasAlternate && ::_tcscmp(m_alternateKey, rhs.m_primaryKey)==0) ||
			  (m_hasAlternate && rhs.m_hasAlternate && ::_tcscmp(m_alternateKey, rhs.m_alternateKey)==0)
			  );               
	} 

	/** Inequality operator, compares two instances of a class, like:
	 * DoubleMetaphone mphone1("Nelson");
	 * DoubleMetaphone mphone2("Neilsen");
	 * 
	 * if (mphone1 != mphone2) {
	 *	...
	 * 
	 * Equality for the doublempetahone class is defined as either of the two
	 * metaphone keys of rhs matching either of the two metaphone keys of the instance.
	 * That is:
	 * 		primary = primary
	 * 		primary = alt
	 * 		alt = primary
	 * 		alt = alt
	 * This four-way comparison is necessary because in some cases, given two words m and n,
	 * primary(m) != primary(n), but primary(m) = alternate(n) or alternate(m) = primary(n)
	 **/
	bool operator!=(const DoubleMetaphone<_KeyLength>& rhs) const {
		//Simply negate the operator==
		return !(*this == rhs);             
	} 

	/** Discards previous results and computes the metaphone keys for a new word. 
	 * For maximum performance, use one instance of DoubleMetaphone, and call computeKeys for each word
	 * to be keyed.  This amortizes the one-time initialization cost over time */
	void computeKeys(const tchar* word) {
		m_length = 0;
		m_last = 0;
		m_hasAlternate = false;
		m_originalWord = word;

		memset(m_primaryKey, 0, sizeof(m_primaryKey));
		memset(m_alternateKey, 0, sizeof(m_alternateKey));

		m_primaryCurrentPos = m_primaryKey;
		m_alternateCurrentPos = m_alternateKey;

		m_primaryKeyLength = m_alternateKeyLength = 0;

		//Copy the word to the internal working buffer, which is padded with four spaces at the end so 
		//the string can be slightly overindexed without danger of invalid data.  
		if (m_word) {
			delete[] m_word;
		}
		m_word = new tchar[::_tcslen(m_originalWord)+4+1];
		::_tcscpy(m_word, m_originalWord);
		::_tcscat(m_word, _TEXT("    "));

		//Convert the word to uppercase, since double metaphone is not case sensitive
		::_tcsupy(m_word);

		m_length = static_cast<int>(::_tcslen(word));

		//Compute the keys		
		buildMetaphoneKeys();
	}

	/**
	 * Static wrapper around the otherwise non-static DoubleMetaphone class.  Allows easy, one-line
	 * computation of double metaphone keys for a word.  
	 * 
	 * @param word       Word for which to compute double metaphone keys
	 * @param primaryKey Pre-allocated buffer of _KeyLength+1 tchars which will receive a null-terminated string
	 *                   containing the primary metaphone key for the word
	 * @param alternateKey
	 *                   Pre-allocated buffer of _KeyLength+1 tchars which will receive a null-terminated string
	 *                   containing the secondary metaphone key for the word, or will be filled with a zero-length
	 *                   string if the word does not have an alternate key by the double metaphone algorithm
	 */
	static void doubleMetaphone(const tchar* word, tchar*& primaryKey, tchar*& alternateKey) {
		//Simply instantiate a DoubleMetaphone class, and copy the resulting keys
		DoubleMetaphone<_KeyLength> mphone(word);

		::_tcscpy(primaryKey, mphone.getPrimaryKey());
		::_tcscpy(alternateKey, mphone.getAlternateKey());
	}

	/**
	 * Gets the primary metaphone key computed for the word passed to the ctor.  
	 * 
	 * @return A null-terminated tstring containing the primary metaphone key of the word passed
	 *         to the ctor.  Note that this string is freed by the class dtor when the class is destroyed,
	 *         therefore it must be copied to another string if you wish to use it after the class
	 *         goes out of scope
	 */
	const tchar* getPrimaryKey() {
		return m_primaryKey;
	}

	/**
	 * Gets the alternate metaphone key computed for the word passed to the ctor.
	 * 
	 * @return A null-terminated tstring containing the alternate metaphone key of the word passed
	 *         to the ctor, or NULL if the word does not have an alternate key by the double metaphone algorightm.
	 *         Note that this string is freed by the class dtor when the class is destroyed,
	 *         therefore it must be copied to another string if you wish to use it after the class
	 *         goes out of scope
	 */
	const tchar* getAlternateKey() {
		return(m_hasAlternate ? m_alternateKey : NULL);
	}

	/**
	 * Gets the word passed to the ctor
	 * 
	 * @return The word passed to the ctor for keying
	 */
	const tchar* getWord() {
		return m_originalWord;
	}

private:
	/** Internal buffers containing primary and alternate keys for the word
	 * space for the primary and alternate key, plus two characters of scratch space which allows us to be sloppy
	 * and not test the key lengths each time before an append
	 **/
	tchar m_primaryKey[_KeyLength+2+1], m_alternateKey[_KeyLength+2+1];

	/** "cursors", pointing at the element in each key buffer where the next character shoudl go */
	tchar* m_primaryCurrentPos, * m_alternateCurrentPos;

	/** Counters tracking the length of each key */
	unsigned int m_primaryKeyLength, m_alternateKeyLength;

	/** length of the word, cached for performance reasons */
	int m_length;

	/** Last valid index into the word */
	int m_last;

	/** Flag set to indicate an alternate key was computed */
	bool m_hasAlternate;

	/** The pointer passed to the ctor to the word being processed */
	const tchar* m_originalWord;

	/** Working copy of the word passed to the ctor; modified during processing */
	tchar* m_word;

	/** Internal impl of double metaphone algorithm.  Populates m_primaryKey and m_alternateKey.  Modified copy-past of
	 * Phillips' original code */
	void buildMetaphoneKeys() {
		int current = 0;
		if (m_length < 1)
			return;
		m_last = m_length - 1;//zero based index

		//skip these when at start of word
		if (areStringsAt(0, 2, _TEXT("GN"), _TEXT("KN"), _TEXT("PN"), _TEXT("WR"), _TEXT("PS"), _TEXT("")))
			current += 1;

		//Initial 'X' is pronounced 'Z' e.g. 'Xavier'
		if (m_word[0] == 'X') {
			addMetaphoneCharacter(_TEXT("S"));	//'Z' maps to 'S'
			current += 1;
		}

		///////////main loop//////////////////////////
		while ((m_primaryKeyLength < _KeyLength) OR (m_alternateKeyLength < _KeyLength)) {
			if (current >= m_length)
				break;

			switch (m_word[current]) {
			case 'A':
			case 'E':
			case 'I':
			case 'O':
			case 'U':
			case 'Y':
				if (current == 0)
					//all init vowels now map to 'A'
					addMetaphoneCharacter(_TEXT("A"));
				current +=1;
				break;

			case 'B':

				//"-mb", e.g", _TEXT("dumb"), already skipped over...
				addMetaphoneCharacter(_TEXT("P"));

				if (m_word[current + 1] == 'B')
					current +=2;
				else
					current	+=1;
				break;
/*
	ANT:This is the C cedilla French Character
*/
			case 'Ç':
				addMetaphoneCharacter(_TEXT("S"));
				current += 1;
				break;

			case 'C':
				//various germanic
				if ((current > 1)
					AND !isVowel(current - 2) 
					AND areStringsAt((current - 1), 3, _TEXT("ACH"), _TEXT("")) 
					AND ((m_word[current + 2] != 'I') AND ((m_word[current + 2] != 'E') 
														   OR areStringsAt((current - 2), 6, _TEXT("BACHER"), _TEXT("MACHER"), _TEXT(""))) )) {
					addMetaphoneCharacter(_TEXT("K"));
					current +=2;
					break;
				}

				//special case 'caesar'
				if ((current == 0) AND areStringsAt(current, 6, _TEXT("CAESAR"), _TEXT(""))) {
					addMetaphoneCharacter(_TEXT("S"));
					current +=2;
					break;
				}

				//italian 'chianti'
				if (areStringsAt(current, 4, _TEXT("CHIA"), _TEXT(""))) {
					addMetaphoneCharacter(_TEXT("K"));
					current +=2;
					break;
				}

				if (areStringsAt(current, 2, _TEXT("CH"), _TEXT(""))) {
					//find 'michael'
					if ((current > 0) AND areStringsAt(current, 4, _TEXT("CHAE"), _TEXT(""))) {
						addMetaphoneCharacter(_TEXT("K"), _TEXT("X"));
						current +=2;
						break;
					}

					//greek roots e.g. 'chemistry', 'chorus'
					if ((current == 0)
						AND (areStringsAt((current + 1), 5, _TEXT("HARAC"), _TEXT("HARIS"), _TEXT("")) 
							 OR areStringsAt((current + 1), 3, _TEXT("HOR"), _TEXT("HYM"), _TEXT("HIA"), _TEXT("HEM"), _TEXT(""))) 
						AND !areStringsAt(0, 5, _TEXT("CHORE"), _TEXT(""))) {
						addMetaphoneCharacter(_TEXT("K"));
						current +=2;
						break;
					}

					//germanic, greek, or otherwise 'ch' for 'kh' sound
					if ((areStringsAt(0, 4, "VAN ", "VON ", _TEXT("")) OR areStringsAt(0, 3, _TEXT("SCH"), _TEXT("")))
						// 'architect but not 'arch', 'orchestra', 'orchid'
						OR areStringsAt((current - 2), 6, _TEXT("ORCHES"), _TEXT("ARCHIT"), _TEXT("ORCHID"), _TEXT(""))
						OR areStringsAt((current + 2), 1, _TEXT("T"), _TEXT("S"), _TEXT(""))
						OR ((areStringsAt((current - 1), 1, _TEXT("A"), _TEXT("O"), _TEXT("U"), _TEXT("E"), _TEXT("")) OR (current == 0))
							//e.g., 'wachtler', 'wechsler', but not 'tichner'
							AND areStringsAt((current + 2), 1, _TEXT("L"), _TEXT("R"), _TEXT("N"), _TEXT("M"), _TEXT("B"), _TEXT("H"), _TEXT("F"), _TEXT("V"), _TEXT("W"), " ", _TEXT("")))) {
						addMetaphoneCharacter(_TEXT("K"));
					} else {
						if (current > 0) {
							if (areStringsAt(0, 2, _TEXT("MC"), _TEXT("")))
								//e.g., _TEXT("McHugh")
								addMetaphoneCharacter(_TEXT("K"));
							else
								addMetaphoneCharacter(_TEXT("X"), _TEXT("K"));
						} else
							addMetaphoneCharacter(_TEXT("X"));
					}
					current +=2;
					break;
				}
				//e.g, 'czerny'
				if (areStringsAt(current, 2, _TEXT("CZ"), _TEXT("")) AND !areStringsAt((current - 2), 4, _TEXT("WICZ"), _TEXT(""))) {
					addMetaphoneCharacter(_TEXT("S"), _TEXT("X"));
					current += 2;
					break;
				}

				//e.g., 'focaccia'
				if (areStringsAt((current + 1), 3, _TEXT("CIA"), _TEXT(""))) {
					addMetaphoneCharacter(_TEXT("X"));
					current += 3;
					break;
				}

				//double 'C', but not if e.g. 'McClellan'
				if (areStringsAt(current, 2, _TEXT("CC"), _TEXT("")) AND !((current == 1) AND (m_word[0] == 'M')))
					//'bellocchio' but not 'bacchus'
					if (areStringsAt((current + 2), 1, _TEXT("I"), _TEXT("E"), _TEXT("H"), _TEXT("")) AND !areStringsAt((current + 2), 2, _TEXT("HU"), _TEXT(""))) {
						//'accident', 'accede' 'succeed'
						if (((current == 1) AND (m_word[current - 1] == 'A')) 
							OR areStringsAt((current - 1), 5, _TEXT("UCCEE"), _TEXT("UCCES"), _TEXT("")))
							addMetaphoneCharacter(_TEXT("KS"));
						//'bacci', 'bertucci', other italian
						else
							addMetaphoneCharacter(_TEXT("X"));
						current += 3;
						break;
					} else {//Pierce's rule
						addMetaphoneCharacter(_TEXT("K"));
						current += 2;
						break;
					}

				if (areStringsAt(current, 2, _TEXT("CK"), _TEXT("CG"), _TEXT("CQ"), _TEXT(""))) {
					addMetaphoneCharacter(_TEXT("K"));
					current += 2;
					break;
				}

				if (areStringsAt(current, 2, _TEXT("CI"), _TEXT("CE"), _TEXT("CY"), _TEXT(""))) {
					//italian vs. english
					if (areStringsAt(current, 3, _TEXT("CIO"), _TEXT("CIE"), _TEXT("CIA"), _TEXT("")))
						addMetaphoneCharacter(_TEXT("S"), _TEXT("X"));
					else
						addMetaphoneCharacter(_TEXT("S"));
					current += 2;
					break;
				}

				//else
				addMetaphoneCharacter(_TEXT("K"));

				//name sent in 'mac caffrey', 'mac gregor
				if (areStringsAt((current + 1), 2, " C", " Q", " G", _TEXT("")))
					current += 3;
				else
					if (areStringsAt((current + 1), 1, _TEXT("C"), _TEXT("K"), _TEXT("Q"), _TEXT("")) 
						AND !areStringsAt((current + 1), 2, _TEXT("CE"), _TEXT("CI"), _TEXT("")))
					current += 2;
				else
					current	+= 1;
				break;

			case 'D':
				if (areStringsAt(current, 2, _TEXT("DG"), _TEXT("")))
					if (areStringsAt((current + 2), 1, _TEXT("I"), _TEXT("E"), _TEXT("Y"), _TEXT(""))) {
						//e.g. 'edge'
						addMetaphoneCharacter(_TEXT("J"));
						current += 3;
						break;
					} else {
						//e.g. 'edgar'
						addMetaphoneCharacter(_TEXT("TK"));
						current += 2;
						break;
					}

				if (areStringsAt(current, 2, _TEXT("DT"), _TEXT("DD"), _TEXT(""))) {
					addMetaphoneCharacter(_TEXT("T"));
					current += 2;
					break;
				}

				//else
				addMetaphoneCharacter(_TEXT("T"));
				current += 1;
				break;

			case 'F':
				if (m_word[current + 1] == 'F')
					current += 2;
				else
					current	+= 1;
				addMetaphoneCharacter(_TEXT("F"));
				break;

			case 'G':
				if (m_word[current + 1] == 'H') {
					if ((current > 0) AND !isVowel(current - 1)) {
						addMetaphoneCharacter(_TEXT("K"));
						current += 2;
						break;
					}

					if (current < 3) {
						//'ghislane', ghiradelli
						if (current == 0) {
							if (m_word[current + 2] == 'I')
								addMetaphoneCharacter(_TEXT("J"));
							else
								addMetaphoneCharacter(_TEXT("K"));
							current += 2;
							break;
						}
					}
					//Parker's rule (with some further refinements) - e.g., 'hugh'
					if (((current > 1) AND areStringsAt((current - 2), 1, _TEXT("B"), _TEXT("H"), _TEXT("D"), _TEXT("")) )
						//e.g., 'bough'
						OR ((current > 2) AND areStringsAt((current - 3), 1, _TEXT("B"), _TEXT("H"), _TEXT("D"), _TEXT("")) )
						//e.g., 'broughton'
						OR ((current > 3) AND areStringsAt((current - 4), 1, _TEXT("B"), _TEXT("H"), _TEXT("")) )) {
						current += 2;
						break;
					} else {
						//e.g., 'laugh', 'McLaughlin', 'cough', 'gough', 'rough', 'tough'
						if ((current > 2) 
							AND (m_word[current - 1] == 'U') 
							AND areStringsAt((current - 3), 1, _TEXT("C"), _TEXT("G"), _TEXT("L"), _TEXT("R"), _TEXT("T"), _TEXT(""))) {
							addMetaphoneCharacter(_TEXT("F"));
						} else
							if ((current > 0) AND m_word[current - 1] != 'I')
							addMetaphoneCharacter(_TEXT("K"));

						current += 2;
						break;
					}
				}

				if (m_word[current + 1] == 'N') {
					if ((current == 1) AND isVowel(0) AND !isWordSlavoGermanic()) {
						addMetaphoneCharacter(_TEXT("KN"), _TEXT("N"));
					} else
						//not e.g. 'cagney'
						if (!areStringsAt((current + 2), 2, _TEXT("EY"), _TEXT("")) 
							AND (m_word[current + 1] != 'Y') AND !isWordSlavoGermanic()) {
						addMetaphoneCharacter(_TEXT("N"), _TEXT("KN"));
					} else
						addMetaphoneCharacter(_TEXT("KN"));
					current += 2;
					break;
				}

				//'tagliaro'
				if (areStringsAt((current + 1), 2, _TEXT("LI"), _TEXT("")) AND !isWordSlavoGermanic()) {
					addMetaphoneCharacter(_TEXT("KL"), _TEXT("L"));
					current += 2;
					break;
				}

				//-ges-,-gep-,-gel-, -gie- at beginning
				if ((current == 0)
					AND ((m_word[current + 1] == 'Y') 
						 OR areStringsAt((current + 1), 2, _TEXT("ES"), _TEXT("EP"), _TEXT("EB"), _TEXT("EL"), _TEXT("EY"), _TEXT("IB"), _TEXT("IL"), _TEXT("IN"), _TEXT("IE"), _TEXT("EI"), _TEXT("ER"), _TEXT("")))) {
					addMetaphoneCharacter(_TEXT("K"), _TEXT("J"));
					current += 2;
					break;
				}

				// -ger-,  -gy-
				if ((areStringsAt((current + 1), 2, _TEXT("ER"), _TEXT("")) OR (m_word[current + 1] == 'Y'))
					AND !areStringsAt(0, 6, _TEXT("DANGER"), _TEXT("RANGER"), _TEXT("MANGER"), _TEXT(""))
					AND !areStringsAt((current - 1), 1, _TEXT("E"), _TEXT("I"), _TEXT("")) 
					AND !areStringsAt((current - 1), 3, _TEXT("RGY"), _TEXT("OGY"), _TEXT(""))) {
					addMetaphoneCharacter(_TEXT("K"), _TEXT("J"));
					current += 2;
					break;
				}

				// italian e.g, 'biaggi'
				if (areStringsAt((current + 1), 1, _TEXT("E"), _TEXT("I"), _TEXT("Y"), _TEXT("")) OR areStringsAt((current - 1), 4, _TEXT("AGGI"), _TEXT("OGGI"), _TEXT(""))) {
					//obvious germanic
					if ((areStringsAt(0, 4, "VAN ", "VON ", _TEXT("")) OR areStringsAt(0, 3, _TEXT("SCH"), _TEXT("")))
						OR areStringsAt((current + 1), 2, _TEXT("ET"), _TEXT("")))
						addMetaphoneCharacter(_TEXT("K"));
					else
						//always soft if french ending
						if (areStringsAt((current + 1), 4, "IER ", _TEXT("")))
						addMetaphoneCharacter(_TEXT("J"));
					else
						addMetaphoneCharacter(_TEXT("J"), _TEXT("K"));
					current += 2;
					break;
				}

				if (m_word[current + 1] == 'G')
					current += 2;
				else
					current	+= 1;
				addMetaphoneCharacter(_TEXT("K"));
				break;

			case 'H':
				//only keep if first & before vowel or btw. 2 vowels
				if (((current == 0) OR isVowel(current - 1)) 
					AND isVowel(current + 1)) {
					addMetaphoneCharacter(_TEXT("H"));
					current += 2;
				} else//also takes care of 'HH'
					current	+= 1;
				break;

			case 'J':
				//obvious spanish, 'jose', 'san jacinto'
				if (areStringsAt(current, 4, _TEXT("JOSE"), _TEXT("")) OR areStringsAt(0, 4, "SAN ", _TEXT(""))) {
					if (((current == 0) AND (m_word[current + 4] == ' ')) OR areStringsAt(0, 4, "SAN ", _TEXT("")))
						addMetaphoneCharacter(_TEXT("H"));
					else {
						addMetaphoneCharacter(_TEXT("J"), _TEXT("H"));
					}
					current +=1;
					break;
				}

				if ((current == 0) AND !areStringsAt(current, 4, _TEXT("JOSE"), _TEXT("")))
					addMetaphoneCharacter(_TEXT("J"), _TEXT("A"));//Yankelovich/Jankelowicz
				else
					//spanish pron. of e.g. 'bajador'
					if (isVowel(current - 1) 
						AND !isWordSlavoGermanic()
						AND ((m_word[current + 1] == 'A') OR (m_word[current + 1] == 'O')))
					addMetaphoneCharacter(_TEXT("J"), _TEXT("H"));
				else
					if (current == m_last)
					addMetaphoneCharacter(_TEXT("J"), _TEXT(" "));
				else
					if (!areStringsAt((current + 1), 1, _TEXT("L"), _TEXT("T"), _TEXT("K"), _TEXT("S"), _TEXT("N"), _TEXT("M"), _TEXT("B"), _TEXT("Z"), _TEXT("")) 
						AND !areStringsAt((current - 1), 1, _TEXT("S"), _TEXT("K"), _TEXT("L"), _TEXT("")))
					addMetaphoneCharacter(_TEXT("J"));

				if (m_word[current + 1] == 'J')//it could happen!
					current += 2;
				else
					current	+= 1;
				break;

			case 'K':
				if (m_word[current + 1] == 'K')
					current += 2;
				else
					current	+= 1;
				addMetaphoneCharacter(_TEXT("K"));
				break;

			case 'L':
				if (m_word[current + 1] == 'L') {
					//spanish e.g. 'cabrillo', 'gallegos'
					if (((current == (m_length - 3)) 
						 AND areStringsAt((current - 1), 4, _TEXT("ILLO"), _TEXT("ILLA"), _TEXT("ALLE"), _TEXT("")))
						OR ((areStringsAt((m_last - 1), 2, _TEXT("AS"), _TEXT("OS"), _TEXT("")) OR areStringsAt(m_last, 1, _TEXT("A"), _TEXT("O"), _TEXT(""))) 
							AND areStringsAt((current - 1), 4, _TEXT("ALLE"), _TEXT("")))) {
						addMetaphoneCharacter(_TEXT("L"), _TEXT(" "));
						current += 2;
						break;
					}
					current += 2;
				} else
					current	+= 1;
				addMetaphoneCharacter(_TEXT("L"));
				break;

			case 'M':
				if ((areStringsAt((current - 1), 3, _TEXT("UMB"), _TEXT("")) 
					 AND (((current + 1) == m_last) OR areStringsAt((current + 2), 2, _TEXT("ER"), _TEXT(""))))
					//'dumb','thumb'
					OR  (m_word[current + 1] == 'M'))
					current += 2;
				else
					current	+= 1;
				addMetaphoneCharacter(_TEXT("M"));
				break;

			case 'N':
				if (m_word[current + 1] == 'N')
					current += 2;
				else
					current	+= 1;
				addMetaphoneCharacter(_TEXT("N"));
				break;

/*
	ANT:This is the N tilde Character (Spanish ene)
*/
			case 'Ñ':
				current += 1;
				addMetaphoneCharacter(_TEXT("N"));
				break;

			case 'P':
				if (m_word[current + 1] == 'H') {
					addMetaphoneCharacter(_TEXT("F"));
					current += 2;
					break;
				}

				//also account for _TEXT("campbell"), _TEXT("raspberry")
				if (areStringsAt((current + 1), 1, _TEXT("P"), _TEXT("B"), _TEXT("")))
					current += 2;
				else
					current	+= 1;
				addMetaphoneCharacter(_TEXT("P"));
				break;

			case 'Q':
				if (m_word[current + 1] == 'Q')
					current += 2;
				else
					current	+= 1;
				addMetaphoneCharacter(_TEXT("K"));
				break;

			case 'R':
				//french e.g. 'rogier', but exclude 'hochmeier'
				if ((current == m_last)
					AND !isWordSlavoGermanic()
					AND areStringsAt((current - 2), 2, _TEXT("IE"), _TEXT("")) 
					AND !areStringsAt((current - 4), 2, _TEXT("ME"), _TEXT("MA"), _TEXT("")))
					addMetaphoneCharacter(_TEXT(""), _TEXT("R"));
				else
					addMetaphoneCharacter(_TEXT("R"));

				if (m_word[current + 1] == 'R')
					current += 2;
				else
					current	+= 1;
				break;

			case 'S':
				//special cases 'island', 'isle', 'carlisle', 'carlysle'
				if (areStringsAt((current - 1), 3, _TEXT("ISL"), _TEXT("YSL"), _TEXT(""))) {
					current += 1;
					break;
				}

				//special case 'sugar-'
				if ((current == 0) AND areStringsAt(current, 5, _TEXT("SUGAR"), _TEXT(""))) {
					addMetaphoneCharacter(_TEXT("X"), _TEXT("S"));
					current += 1;
					break;
				}

				if (areStringsAt(current, 2, _TEXT("SH"), _TEXT(""))) {
					//germanic
					if (areStringsAt((current + 1), 4, _TEXT("HEIM"), _TEXT("HOEK"), _TEXT("HOLM"), _TEXT("HOLZ"), _TEXT("")))
						addMetaphoneCharacter(_TEXT("S"));
					else
						addMetaphoneCharacter(_TEXT("X"));
					current += 2;
					break;
				}

				//italian & armenian
				if (areStringsAt(current, 3, _TEXT("SIO"), _TEXT("SIA"), _TEXT("")) OR areStringsAt(current, 4, _TEXT("SIAN"), _TEXT(""))) {
					if (!isWordSlavoGermanic())
						addMetaphoneCharacter(_TEXT("S"), _TEXT("X"));
					else
						addMetaphoneCharacter(_TEXT("S"));
					current += 3;
					break;
				}

				//german & anglicisations, e.g. 'smith' match 'schmidt', 'snider' match 'schneider'
				//also, -sz- in slavic language altho in hungarian it is pronounced 's'
				if (((current == 0) 
					 AND areStringsAt((current + 1), 1, _TEXT("M"), _TEXT("N"), _TEXT("L"), _TEXT("W"), _TEXT("")))
					OR areStringsAt((current + 1), 1, _TEXT("Z"), _TEXT(""))) {
					addMetaphoneCharacter(_TEXT("S"), _TEXT("X"));
					if (areStringsAt((current + 1), 1, _TEXT("Z"), _TEXT("")))
						current += 2;
					else
						current	+= 1;
					break;
				}

				if (areStringsAt(current, 2, _TEXT("SC"), _TEXT(""))) {
					//Schlesinger's rule
					if (m_word[current + 2] == 'H')
						//dutch origin, e.g. 'school', 'schooner'
						if (areStringsAt((current + 3), 2, _TEXT("OO"), _TEXT("ER"), _TEXT("EN"), _TEXT("UY"), _TEXT("ED"), _TEXT("EM"), _TEXT(""))) {
							//'schermerhorn', 'schenker'
							if (areStringsAt((current + 3), 2, _TEXT("ER"), _TEXT("EN"), _TEXT(""))) {
								addMetaphoneCharacter(_TEXT("X"), _TEXT("SK"));
							} else
								addMetaphoneCharacter(_TEXT("SK"));
							current += 3;
							break;
						} else {
							if ((current == 0) AND !isVowel(3) AND (m_word[3] != 'W'))
								addMetaphoneCharacter(_TEXT("X"), _TEXT("S"));
							else
								addMetaphoneCharacter(_TEXT("X"));
							current += 3;
							break;
						}

					if (areStringsAt((current + 2), 1, _TEXT("I"), _TEXT("E"), _TEXT("Y"), _TEXT(""))) {
						addMetaphoneCharacter(_TEXT("S"));
						current += 3;
						break;
					}
					//else
					addMetaphoneCharacter(_TEXT("SK"));
					current += 3;
					break;
				}

				//french e.g. 'resnais', 'artois'
				if ((current == m_last) AND areStringsAt((current - 2), 2, _TEXT("AI"), _TEXT("OI"), _TEXT("")))
					addMetaphoneCharacter(_TEXT(""), _TEXT("S"));
				else
					addMetaphoneCharacter(_TEXT("S"));

				if (areStringsAt((current + 1), 1, _TEXT("S"), _TEXT("Z"), _TEXT("")))
					current += 2;
				else
					current	+= 1;
				break;

			case 'T':
				if (areStringsAt(current, 4, _TEXT("TION"), _TEXT(""))) {
					addMetaphoneCharacter(_TEXT("X"));
					current += 3;
					break;
				}

				if (areStringsAt(current, 3, _TEXT("TIA"), _TEXT("TCH"), _TEXT(""))) {
					addMetaphoneCharacter(_TEXT("X"));
					current += 3;
					break;
				}

				if (areStringsAt(current, 2, _TEXT("TH"), _TEXT("")) 
					OR areStringsAt(current, 3, _TEXT("TTH"), _TEXT(""))) {
					//special case 'thomas', 'thames' or germanic
					if (areStringsAt((current + 2), 2, _TEXT("OM"), _TEXT("AM"), _TEXT("")) 
						OR areStringsAt(0, 4, "VAN ", "VON ", _TEXT("")) 
						OR areStringsAt(0, 3, _TEXT("SCH"), _TEXT(""))) {
						addMetaphoneCharacter(_TEXT("T"));
					} else {
						addMetaphoneCharacter(_TEXT("0"), _TEXT("T"));
					}
					current += 2;
					break;
				}

				if (areStringsAt((current + 1), 1, _TEXT("T"), _TEXT("D"), _TEXT("")))
					current += 2;
				else
					current	+= 1;
				addMetaphoneCharacter(_TEXT("T"));
				break;

			case 'V':
				if (m_word[current + 1] == 'V')
					current += 2;
				else
					current	+= 1;
				addMetaphoneCharacter(_TEXT("F"));
				break;

			case 'W':
				//can also be in middle of word
				if (areStringsAt(current, 2, _TEXT("WR"), _TEXT(""))) {
					addMetaphoneCharacter(_TEXT("R"));
					current += 2;
					break;
				}

				if ((current == 0) 
					AND (isVowel(current + 1) OR areStringsAt(current, 2, _TEXT("WH"), _TEXT("")))) {
					//Wasserman should match Vasserman
					if (isVowel(current + 1))
						addMetaphoneCharacter(_TEXT("A"), _TEXT("F"));
					else
						//need Uomo to match Womo
						addMetaphoneCharacter(_TEXT("A"));
				}

				//Arnow should match Arnoff
				if (((current == m_last) AND isVowel(current - 1)) 
					OR areStringsAt((current - 1), 5, _TEXT("EWSKI"), _TEXT("EWSKY"), _TEXT("OWSKI"), _TEXT("OWSKY"), _TEXT("")) 
					OR areStringsAt(0, 3, _TEXT("SCH"), _TEXT(""))) {
					addMetaphoneCharacter(_TEXT(""), _TEXT("F"));
					current +=1;
					break;
				}

				//polish e.g. 'filipowicz'
				if (areStringsAt(current, 4, _TEXT("WICZ"), _TEXT("WITZ"), _TEXT(""))) {
					addMetaphoneCharacter(_TEXT("TS"), _TEXT("FX"));
					current +=4;
					break;
				}

				//else skip it
				current +=1;
				break;

			case 'X':
				//french e.g. breaux
				if (!((current == m_last) 
					  AND (areStringsAt((current - 3), 3, _TEXT("IAU"), _TEXT("EAU"), _TEXT("")) 
						   OR areStringsAt((current - 2), 2, _TEXT("AU"), _TEXT("OU"), _TEXT("")))))
					addMetaphoneCharacter(_TEXT("KS"));

				if (areStringsAt((current + 1), 1, _TEXT("C"), _TEXT("X"), _TEXT("")))
					current += 2;
				else
					current	+= 1;
				break;

			case 'Z':
				//chinese pinyin e.g. 'zhao'
				if (m_word[current + 1] == 'H') {
					addMetaphoneCharacter(_TEXT("J"));
					current += 2;
					break;
				} else
					if (areStringsAt((current + 1), 2, _TEXT("ZO"), _TEXT("ZI"), _TEXT("ZA"), _TEXT("")) 
						OR (isWordSlavoGermanic() AND ((current > 0) AND m_word[current - 1] != 'T'))) {
					addMetaphoneCharacter(_TEXT("S"), _TEXT("TS"));
				} else
					addMetaphoneCharacter(_TEXT("S"));

				if (m_word[current + 1] == 'Z')
					current += 2;
				else
					current	+= 1;
				break;

			default:
				current += 1;
			}
		}

		//Finally, chop off the keys at the proscribed length
		if (m_primaryKeyLength > _KeyLength) {
			m_primaryKey[_KeyLength] = 0;
		}

		if (m_alternateKeyLength > _KeyLength) {
			m_alternateKey[_KeyLength] = 0;
		}
	}

	/** Returns true if m_word is classified as "slavo-germanic" by Phillips' algorithm */
	bool isWordSlavoGermanic() {
		if ((::_tcsstr(m_word, _TEXT("W"))) OR (::_tcsstr(m_word, _TEXT("K"))) OR (::_tcsstr(m_word, _TEXT("CZ"))) OR (::_tcsstr(m_word, _TEXT("WITZ"))))
			return true;

		return false;
	}

	/** Returns true if letter at given position in word is a Roman vowel */
	bool isVowel(int pos) {
		if ((pos < 0) OR (pos >= m_length))
			return false;

		tchar it = m_word[pos];

		if ((it == 'E') OR (it == 'A') OR (it == 'I') OR (it == 'O') OR (it == 'U') OR (it == 'Y'))
			return true;

		return false;
	}

	/** Appends a metaphone character to the primary, and possibly alternate,
	 * metaphone keys for the word.  */
	void addMetaphoneCharacter(tchar* primaryCharacter, tchar* alternateCharacter = NULL) {
		//Is the primary character valid?
		if (*primaryCharacter) {
			tchar* temp = primaryCharacter;
			while (*temp) {
				*m_primaryCurrentPos = *temp;
				temp++;
				m_primaryCurrentPos++;
				m_primaryKeyLength++;
			}
		}

		//Is the alternate character valid?
		if (alternateCharacter) {
			//Alternate character was provided.  If it is not zero-length, append it, else
			//append the primary string as long as it wasn't zero length and isn't a space character
			if (*alternateCharacter) {
				m_hasAlternate = true;
				if (alternateCharacter[0] != ' ') {
					while (*alternateCharacter) {
						*m_alternateCurrentPos = *alternateCharacter;
						alternateCharacter++;
						m_alternateCurrentPos++;
						m_alternateKeyLength++;
					}
				}
			} else {
				//No, but if the primary character is valid, add that instead
				if (*primaryCharacter AND (primaryCharacter[0] != ' ')) {
					while (*primaryCharacter) {
						*m_alternateCurrentPos = *primaryCharacter;
						primaryCharacter++;
						m_alternateCurrentPos++;
						m_alternateKeyLength++;
					}
				}
			}
		} else if (*primaryCharacter) {
			//Else, no alternate character was passed, but a primary was, so append the primary character to the alternate key
			while (*primaryCharacter) {
				*m_alternateCurrentPos = *primaryCharacter;
				primaryCharacter++;
				m_alternateCurrentPos++;
				m_alternateKeyLength++;
			}
		}
	}

	/** Tests if any of the strings passed as variable arguments are at the given start position and
	 * length within word */
	bool areStringsAt(int start, int length, ...) {
		if (start < 0) {
			//Sometimes, as a result of expressions like "current - 2" for start, 
			//start ends up negative.  Since no string can be present at a negative offset, this is always false
			return false;
		}

		tchar* test = NULL;
		tchar* target = NULL;

		target = m_word + start;

		va_list sstrings;
		va_start(sstrings, length);

		do {
			test = va_arg(sstrings, tchar*);
			if (*test)
				if (::_tcsncmp(test, target, length)==0)
					return true;


		} while (*test);

		va_end(sstrings);

		return false;
	}
};

//Provide a typedef to the double metaphone with the ideal key length for most applications
typedef DoubleMetaphone<4> DoubleMetaphone4;
