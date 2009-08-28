/*
	PARSER.H
	--------
*/
#ifndef __PARSER_H__
#define __PARSER_H__

#include "string_pair.h"
#include "ctypes.h"

#ifndef FALSE
	#define FALSE 0
#endif

/*
	class ANT_PARSER
*/
class ANT_parser
{
protected:
	unsigned char *document;
	unsigned char *current;
	ANT_string_pair current_token;
	unsigned char *segmentation;
	long should_segment;

protected:
	static int isheadchar(unsigned char val) { return ANT_isalnum(val) || val == '<' || val == '\0'; }

public:
	ANT_parser(long should_segment = FALSE);
	virtual ~ANT_parser();

	static int isXMLnamestartchar(unsigned char val) { return ANT_isalpha(val) || val == ':' || val == '_'; }		// see http://www.w3.org/TR/REC-xml/#NT-NameStartChar
	static int isXMLnamechar(unsigned char val) { return isXMLnamestartchar(val) || ANT_isdigit(val) || val == '.' || val == '-'; } // see http://www.w3.org/TR/REC-xml/#NT-NameChar

	virtual void segment(unsigned char *start, long length);
	static int ischinese(unsigned char *here);
	static int ischinese(char *here) { return ischinese((unsigned char *)here); }
	static unsigned long utf8_to_wide(unsigned char *here);
	static long utf8_bytes(unsigned char *here);
	static long utf8_bytes(char *here) { return utf8_bytes((unsigned char *)here); }

	virtual void set_document(unsigned char *document);
	virtual ANT_string_pair *get_next_token(void);
} ;

/*
	ANT_PARSER::UTF8_TO_WIDE()
	--------------------------
	Convert a UTF8 sequence into a wide character
*/
inline unsigned long ANT_parser::utf8_to_wide(unsigned char *here)
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
	ANT_PARSER::ISCHINESE()
	-----------------------
	Is the given character from the Chinese CodePoint?
*/
inline int ANT_parser::ischinese(unsigned char *here)
{
unsigned long chinese;

if ((*here & 0x80) == 0)
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
inline long ANT_parser::utf8_bytes(unsigned char *here)
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

#endif __PARSER_H__
