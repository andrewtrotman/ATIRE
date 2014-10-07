/*
 * language.h
 *
 *  Created on: Sep 28, 2009
 *      Author: Andrew Trotman, monfee
 *
 */

#ifndef LANGUAGE_H_
#define LANGUAGE_H_

#include <string>
#include <set>
#include <map>

//#ifndef FALSE
	#define FALSE 0
//#endif

//#ifndef TRUE
	#define TRUE (!FALSE)
//#endif

class language
{
public:
	typedef	std::map<std::string, int>	term_freq_map_type;

public:
	static const char *EN_STOP_WORDS[];
	static int EN_STOP_WORDS_LENGTH;
	static std::set<std::string> STOP_WORD_SET;

public:
	language();
	virtual ~language();

	static bool isstopword(const char *term);
	static void remove_en_stopword(std::string& source, const char *sep = " ");

	static int string_compare(const void *a, const void *b);

	static int isutf8(const unsigned char *here);
	static int isutf8(const char *here) { return isutf8((const unsigned char *)here); }

	static int is_cjk_language(const unsigned char *here);
	static int is_cjk_language(const char *here) { return is_cjk_language((const unsigned char *)here); }

	static unsigned long utf8_to_wide(const unsigned char *here);
	static long utf8_bytes(const unsigned char *here);
	static long utf8_bytes(const char *here) { return utf8_bytes((const unsigned char *)here); }

	static void codepoint_to_utf8(char *buf, long cp);

	static void collect_chinese_from_xml_entities(std::string& text, const char *source);
	static void collect_chinese(std::string& text, const char *source, bool segmentation = false);

	static void collect_chinese_from_xml_entities(term_freq_map_type& text, const char *source);
	static void collect_chinese(term_freq_map_type& text, const char *source);

	static void remove_xml_entities(std::string& text);

	static int count_terms(const char *src);
};

/*
	ANT_PARSER::UTF8_TO_WIDE()
	--------------------------
	Convert a UTF8 sequence into a wide character
*/
inline unsigned long language::utf8_to_wide(const unsigned char *here)
{
if (*here < 0x80)				// 1-byte (ASCII) character
	return *here;
if ((*here & 0xE0) == 0xC0)	// 2-byte sequence
	return ((*here & 0x1F) << 6) | (*(here + 1) & 0x3F);
if ((*here & 0xF0) == 0xE0)	// 3-byte sequence
	return ((*here & 0x0F) << 12) | ((*(here + 1) & 0x3F) << 6) | (*(here + 2) & 0x3F);
if ((*here & 0xF8) == 0xF0)	// 4-byte sequence
	return ((*here & 0x03) << 18) | ((*(here + 1) & 0x3F) << 12) | ((*(here + 2) & 0x3F) << 6) | (*(here + 1) & 0x3F);
return 0;
}

/*
	ANT_PARSER::ISUTF8()
	--------------------------
	if it is valid uft8 bytes
*/
inline int language::isutf8(const unsigned char *here)
{
int number_of_bytes = utf8_bytes(here);
int i = 1;

for (; i < number_of_bytes; ++i)
	{
	++here;
	char c = (*here) >> 6;
	if (c != 2)
		return FALSE;
	}
return TRUE;
}

/*
	ANT_PARSER::ISCHINESE()
	-----------------------
	Is the given character from the Chinese CodePoint?
*/
inline int language::is_cjk_language(const unsigned char *here)
{
unsigned long chinese;

if (!isutf8(here))
	return FALSE;
else
	{
	chinese = utf8_to_wide(here);

	return ((chinese >= 0x4e00 && chinese <= 0x9fff)		// CJK Unified Ideographs
		|| (chinese >= 0x3400 && chinese <= 0x4dbf)		// CJK Unified Ideographs Extension A
		|| (chinese >=0x20000 && chinese <= 0x2a6df)		// CJK Unified Ideographs Extension B
		|| (chinese >=0xf900 && chinese <= 0xfaff)		// CJK Compatibility Ideographs
		|| (chinese >=0x2f800 && chinese <= 0x2fa1f));	// CJK Compatibility Ideographs Supplement
	}
}

/*
	ANT_PARSER::UTF8_BYTES()
	------------------------
	How many bytes does the UTF8 character take?
*/
inline long language::utf8_bytes(const unsigned char *here)
{
if (*here < 0x80)				// 1-byte (ASCII) character
	return 1;
if ((*here & 0xE0) == 0xC0)		// 2-byte sequence
	return 2;
if ((*here & 0xF0) == 0xE0)		// 3-byte sequence
	return 3;
if ((*here & 0xF8) == 0xF0)		// 4-byte sequence
	return 4;
return 1;		// dunno so make it 1
}

inline void language::codepoint_to_utf8(char *buf, long cp)
{
	if (cp < 0x80) {				// 1-byte (ASCII) character
		buf[0] = cp;
		buf[1] = '\0';
	}
	else if (cp < 0x800/*(*here & 0xE0) == 0xC0*/)	{ // 2-byte sequence
		//return ((*here & 0x1F) << 6) | (*(here + 1) & 0x3F);
		buf[0] = 0xC0 | (cp >> 6);
		buf[1] = 0xC0 | (cp & 0x3F);
		buf[2] = '\0';
	}
	else if (cp < 0x10000/*(*here & 0xF0) == 0xE0*/)	 { // 3-byte sequence
		buf[0] = 0xE0 | (cp >> 12);;
		buf[1] = 0x80 | ((cp & 0xFC0) >> 6);
		buf[2] = 0x80 | (cp & 0x3F);
		//return ((*here & 0x0F) << 12) | ((*(here + 1) & 0x3F) << 6) | (*(here + 2) & 0x3F);
		buf[3] = '\0';
	}
	else if (cp < 0x11000/*(*here & 0xF8) == 0xF0*/)	{// 4-byte sequence
		buf[0] = 0xF0 | (cp >> 18);;
		buf[1] = 0x80 | ((cp & 0x3F000) >> 12);
		buf[2] = 0x80 | ((cp & 0xFC0) >> 6);
		buf[3] = 0x80 | (cp & 0x3F);
		//return ((*here & 0x03) << 18) | ((*(here + 1) & 0x3F) << 12) | ((*(here + 2) & 0x3F) << 6) | (*(here + 1) & 0x3F);
		buf[4] = '\0';
	}
	else
		buf[0] = '\0';
}

#endif /* LANGUAGE_H_ */
