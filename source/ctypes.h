/*
	CTYPES.H
	--------
	ANT ctype routines - Used to relace the C run time library versions that are not inlined.
*/
#ifndef CTYPES_H_
#define CTYPES_H_

enum { 
	ANT_CTYPE_UPPER = 1, 
	ANT_CTYPE_LOWER = 2, 
	ANT_CTYPE_DIGIT = 4, 
	ANT_CTYPE_CONTROL = 8, 
	ANT_CTYPE_PUNC = 16, 
	ANT_CTYPE_SPACE = 32, 
	ANT_CTYPE_HEX = 64, 
	ANT_CTYPE_HARD_SPACE = 128, 		// character 0x20
	ANT_CTYPE_VOWEL = 256, 
	ANT_CTYPE_Y = 512,
	ANT_CTYPE_HEADCHAR = 1024,			// ANT parser start of token
	ANT_CTYPE_XML_NAME_START = 2048,
	ANT_CTYPE_XML_NAME = 4096,

	ANT_CTYPE_ISALPHA = ANT_CTYPE_UPPER | ANT_CTYPE_LOWER,
	ANT_CTYPE_ISALNUM = ANT_CTYPE_ISALPHA | ANT_CTYPE_DIGIT,
	ANT_CTYPE_ISVOWELY = ANT_CTYPE_VOWEL | ANT_CTYPE_Y,
	ANT_CTYPE_ISGRAPH  = ANT_CTYPE_PUNC | ANT_CTYPE_UPPER | ANT_CTYPE_LOWER | ANT_CTYPE_DIGIT,
	ANT_CTYPE_ISPRINT = ANT_CTYPE_PUNC | ANT_CTYPE_ISALNUM | ANT_CTYPE_HARD_SPACE,

	ANT_CTYPE_ISLOWERNUM = ANT_CTYPE_LOWER | ANT_CTYPE_DIGIT						// special for indexing purposes
} ;

extern unsigned short ANT_ctype[];
extern unsigned char ANT_toupper_list[], ANT_tolower_list[];

/*
	ANT_TO_CTYPE()
	--------------
	do the cast and cause a lookup to the lookup table
*/
inline int ANT_to_ctype(int x) { return ANT_ctype[(unsigned char)(x)]; }

/*
	IS() routines
*/
inline int ANT_islower(int c) { return (ANT_to_ctype(c) & ANT_CTYPE_LOWER) != 0; }
inline int ANT_isupper(int c) { return (ANT_to_ctype(c) & ANT_CTYPE_UPPER) != 0; }
inline int ANT_isalpha(int c) { return (ANT_to_ctype(c) & ANT_CTYPE_ISALPHA) != 0; }
inline int ANT_isvowel(int c) { return (ANT_to_ctype(c) & ANT_CTYPE_VOWEL) != 0; }
inline int ANT_isvowely(int c) { return (ANT_to_ctype(c) & ANT_CTYPE_ISVOWELY) != 0; }
inline int ANT_isdigit(int c) { return (ANT_to_ctype(c) & ANT_CTYPE_DIGIT) != 0; }
inline int ANT_isxdigit(int c) { return (ANT_to_ctype(c) & ANT_CTYPE_HEX) != 0; }
inline int ANT_isalnum(int c) { return (ANT_to_ctype(c) & ANT_CTYPE_ISALNUM) != 0; }
inline int ANT_isspace(int c) { return (ANT_to_ctype(c) & ANT_CTYPE_SPACE) != 0; }
inline int ANT_ispunct(int c) { return (ANT_to_ctype(c) & ANT_CTYPE_PUNC) != 0; }
inline int ANT_isgraph(int c) { return (ANT_to_ctype(c) & ANT_CTYPE_ISGRAPH) != 0; }
inline int ANT_isprint(int c) { return (ANT_to_ctype(c) & ANT_CTYPE_ISPRINT) != 0; }
inline int ANT_iscntrl(int c) { return (ANT_to_ctype(c) & ANT_CTYPE_CONTROL) != 0; }
inline int ANT_isascii(int c) { return (unsigned char)(c) <= 0x7f; }

/*
	Special IS() routines for ANT indexing
*/
inline int ANT_islowernum(int c) { return (ANT_to_ctype(c) & ANT_CTYPE_ISLOWERNUM) != 0; }
inline int ANT_isheadchar(int c) { return (ANT_to_ctype(c) & ANT_CTYPE_HEADCHAR) != 0; }
inline int ANT_ispuncheadchar(int c) { return (ANT_to_ctype(c) & (ANT_CTYPE_HEADCHAR | ANT_CTYPE_PUNC)) != 0; }
inline int ANT_isXMLnamestartchar(int c) { return (ANT_to_ctype(c) & ANT_CTYPE_XML_NAME_START) != 0; }
inline int ANT_isXMLnamechar(int c) { return (ANT_to_ctype(c) & ANT_CTYPE_XML_NAME) != 0; }

/*
	TO() routines
*/
inline unsigned char ANT_toascii(int c) { return (((unsigned char)(c))&0x7f); }
inline unsigned char ANT_tolower(unsigned char c) { return ANT_tolower_list[c]; }
inline unsigned char ANT_toupper(unsigned char c) { return ANT_toupper_list[c]; }

#endif  /* ANT_CTYPES_H_ */
