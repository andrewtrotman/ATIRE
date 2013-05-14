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

struct ANT_UNICODE_char_chartype
{
	long character;
	unsigned char type;
};

struct ANT_UNICODE_decomposition
{
	long source;
	char const *target;
};

enum ANT_UNICODE_xml_char_class {
	XMLCC_NAME_START = 1,
	XMLCC_NAME = 2
};

unsigned long utf8_to_wide_safe(const unsigned char *here);
unsigned long utf8_to_wide_unsafe(const unsigned char *here);

inline unsigned long utf8_to_wide(const unsigned char *here) { return utf8_to_wide_safe(here); }
inline unsigned long utf8_to_wide(const char *here) { return utf8_to_wide((unsigned char *)here); }

int ischinese(unsigned long character);
inline int ischinese(const unsigned char *here) { return ischinese(utf8_to_wide(here)); }
inline int ischinese(const char *here) { return ischinese((const unsigned char *)here); }

int utf8_isupper(unsigned long character);
int utf8_isupper(long character);

int utf8_tolower(unsigned char **dest, size_t *destlen, const unsigned char **src);
unsigned char *utf8_tolower(unsigned char *here);
inline char *utf8_tolower(char *here) { return (char *)utf8_tolower((unsigned char *)here); }

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

#endif /* UNICODE_H_ */
