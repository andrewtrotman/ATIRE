/*
	UNICODE.H
	---------
	Routines for working with UTF-8 text
*/

#ifndef UNICODE_H_
#define UNICODE_H_

#include <string.h>
#include <cassert>
#include "ctypes.h"
#include "fundamental_types.h"
#include "unicode_tables.h"

#ifdef _MSC_VER
	#define inline __forceinline
#endif

#define LAST_ASCII_CHAR 0x7F
#define SPECIAL_TERM_CHAR 0x80

/*
	ANT_UNICODE_char_chartype
	-------------------------
*/
struct ANT_UNICODE_char_chartype
{
long character;
unsigned char type;
};

/*
	ANT_UNICODE_decomposition
	-------------------------
*/
struct ANT_UNICODE_decomposition
{
long source;
char const *target;
};

/*
	ANT_UNICODE_xml_char_class
	--------------------------
*/
enum ANT_UNICODE_xml_char_class
{
XMLCC_NAME_START = 1,
XMLCC_NAME = 2
};

extern ANT_UNICODE_chartype unicode_chartype_ASCII_table[];

unsigned long utf8_to_wide_safe(const unsigned char *here);
unsigned long utf8_to_wide_unsafe(const unsigned char *here);

inline unsigned long utf8_to_wide(const unsigned char *here) { return utf8_to_wide_safe(here); }
inline unsigned long utf8_to_wide(const char *here) { return utf8_to_wide((unsigned char *)here); }

int ischinese(unsigned long character);
inline int ischinese(const unsigned char *here) { return ischinese(utf8_to_wide(here)); }
inline int ischinese(const char *here) { return ischinese((const unsigned char *)here); }

int iskorean(unsigned long character);
inline int iskorean(const unsigned char *here) { return iskorean(utf8_to_wide(here)); }
inline int iskorean(const char *here) { return iskorean((const unsigned char *)here); }

int isjapanese(unsigned long character);
inline int isjapanese(const unsigned char *here) { return isjapanese(utf8_to_wide(here)); }
inline int isjapanese(const char *here) { return isjapanese((const unsigned char *)here); }

int is_cjk_language(unsigned long character);
inline int is_cjk_language(const unsigned char *here) { return is_cjk_language(utf8_to_wide(here)); }
inline int is_cjk_language(const char *here) { return is_cjk_language((const unsigned char *)here); }

int utf8_ispuntuation(unsigned long character);
inline int utf8_ispuntuation(const unsigned char *here) { return utf8_ispuntuation(utf8_to_wide(here)); }
inline int utf8_ispuntuation(const char *here) { return utf8_ispuntuation((const unsigned char *)here); }

int utf8_isupper(unsigned long character);
int utf8_isupper(long character);

int utf8_tolower(unsigned char **dest, size_t *destlen, const unsigned char **src);
unsigned char *utf8_tolower(unsigned char *here);
inline char *utf8_tolower(char *here) { return (char *)utf8_tolower((unsigned char *)here); }

ANT_UNICODE_chartype unicode_chartype_utf8_multibye(unsigned char *current, unsigned long *character, long *bytes);
ANT_UNICODE_chartype unicode_chartype(unsigned long character);
unsigned char unicode_chartype_set(unsigned long character);
int unicode_xml_class(unsigned long character);

int ANT_UNICODE_normalize_lowercase_toutf8(unsigned char **buf, size_t *buflen, uint32_t character);
inline int ANT_UNICODE_normalize_lowercase_toutf8(char **buf, size_t *buflen, uint32_t character) { return ANT_UNICODE_normalize_lowercase_toutf8((unsigned char **)buf, buflen, character); }

unsigned long utf8_bytes(unsigned long c);
unsigned long utf8_bytes(const unsigned char *here);
inline unsigned long utf8_bytes(const char *here) { return utf8_bytes((unsigned char *)here); }

unsigned long wide_to_utf8(unsigned char *buf, size_t buflen, unsigned long c);
inline unsigned long wide_to_utf8(char *buf, size_t buflen, unsigned long c) { return wide_to_utf8((unsigned char *)buf, buflen, c); }

int isutf8(const unsigned char *here);

inline int isutf8(const char *here) { return isutf8((unsigned char *)here); }


/*
	UNICODE_CHARTYPE_ASCII()
	------------------------
*/
static inline ANT_UNICODE_chartype unicode_chartype_ASCII(unsigned char character)
{
return unicode_chartype_ASCII_table[character];
}

/*
	UNICODE_CHARTYPE_UTF8()
	-----------------------
	Given a pointer to a UTF-8 string, convert into a wide and return: it, its length (in bytes), and its chartype
*/
static inline ANT_UNICODE_chartype unicode_chartype_utf8(unsigned char *current, unsigned long *character, long *bytes)
{
if (*current < LAST_ASCII_CHAR)
	{
	*bytes = 1;
	*character = *current;
	return unicode_chartype_ASCII((unsigned char)*character);
	}
else
	return unicode_chartype_utf8_multibye(current, character, bytes);
}








#endif /* UNICODE_H_ */
