/*
	PARSER.H
	--------
*/
#ifndef __PARSER_H__
#define __PARSER_H__

#include "string_pair.h"
#include "ctypes.h"

/*
	class ANT_PARSER
*/
class ANT_parser
{
protected:
	unsigned char *document;
	unsigned char *current;
	ANT_string_pair current_token;

protected:
	static int isheadchar(unsigned char val) { return ANT_isalnum(val) || val == '<' || val == '\0'; }

public:
	ANT_parser();
	virtual ~ANT_parser();

	static int isXMLnamestartchar(unsigned char val) { return ANT_isalpha(val) || val == ':' || val == '_'; }		// see http://www.w3.org/TR/REC-xml/#NT-NameStartChar
	static int isXMLnamechar(unsigned char val) { return isXMLnamestartchar(val) || ANT_isdigit(val) || val == '.' || val == '-'; } // see http://www.w3.org/TR/REC-xml/#NT-NameChar

#ifdef ONE_PARSER
	static int ischinese(unsigned char *here);
	static unsigned long utf8_to_wide(unsigned char *here);
#endif

	virtual void set_document(unsigned char *document);
	virtual ANT_string_pair *get_next_token(void);
} ;

#ifdef ONE_PARSER
	/*
		ANT_PARSER::UTF8_TO_WIDE()
		--------------------------
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
	*/
	inline int ANT_parser::ischinese(unsigned char *here)
	{
	unsigned long cp = utf8_to_wide(here);

	return ((cp >= 0x4e00 && cp <= 0x9fff)
		|| (cp >= 0x3400 && cp <= 0x4dbf)
		|| (cp >=0x20000 && cp <= 0x2a6df)
		|| (cp >=0xf900 && cp <= 0xfaff)
		|| (cp >=0x2f800 && cp <= 0x2fa1f));
	}
#endif

#endif __PARSER_H__
