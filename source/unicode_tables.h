/*
	UNICODE_TABLES.H
	--------------
	Routines to do case conversion in uncode
*/
#ifndef UNICODE_TABLES_H_
#define UNICODE_TABLES_H_

extern const char * ANT_UNICODE_chartype_string[7];

enum ANT_UNICODE_chartype {
	CT_LETTER=0, CT_NUMBER=1, CT_PUNCTUATION=2, CT_SEPARATOR=3, CT_OTHER=4, CT_MARK=5,
	CT_CHINESE=128
};

/* A UTF codepoint could decompose into this many Unicode characters through Unicode
 * decomposition, at most.
 */
#define UTF8_LONGEST_DECOMPOSITION_LEN 30

/*
	Convert to upper case or lower case.  Note that the case conversion is not one-to-one.  For example,
	character 212B -> 00E5 -> 00C5.  This is because :
		212B is "ANGSTROM SIGN"
		00E5 is "LATIN SMALL LETTER A WITH RING ABOVE"
		00C5 is "LATIN CAPITAL LETTER A WITH RING ABOVE"
	and so the conversion is no incorrect and also not symetric
*/
long ANT_UNICODE_tolower(long character);
long ANT_UNICODE_toupper(long character);

const char* ANT_UNICODE_search_decomposition(long character);
unsigned char ANT_UNICODE_search_chartype(long character);

#endif /* UNICODE_TABLES_H_ */
