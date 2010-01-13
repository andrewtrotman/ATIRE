/*
	PARSER.H
	--------
*/
#ifndef PARSER_H_
#define PARSER_H_

#include "string_pair.h"
#include "ctypes.h"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	class ANT_PARSER
*/
class ANT_parser
{
public:
	/*
	 * This might be confusing. For Chinese, no segmentation means documents are indexed by characters only;
	 * DOUBLE_SEGMENTATION means string is segmented into words and also single characters for indexing
	 * ONFLY_SEGMENTATION means segment the sentence using the Chinese segmentation module when parsing the document
	 * If ONFLY_SEGMENTATION is not set, but SHOULD_SEGMENT is still set that means the text is segmented.
	 */
	enum {NOSEGMENTATION = 0, SHOULD_SEGMENT = 1, ONFLY_SEGMENTATION = 2, DOUBLE_SEGMENTATION = 4};

protected:
	unsigned char *document;
	unsigned char *current;
	ANT_string_pair current_token;
	unsigned char *segmentation;
	long should_segment;

protected:
	static int isheadchar(unsigned char val) { return ANT_isalnum(val) || val == '<' || val == '\0'; }

public:
	ANT_parser(long should_segment = NOSEGMENTATION);
	virtual ~ANT_parser();

	static int isXMLnamestartchar(unsigned char val) { return ANT_isalpha(val) || val == ':' || val == '_'; }		// see http://www.w3.org/TR/REC-xml/#NT-NameStartChar
	static int isXMLnamechar(unsigned char val) { return isXMLnamestartchar(val) || ANT_isdigit(val) || val == '.' || val == '-'; } // see http://www.w3.org/TR/REC-xml/#NT-NameChar

	virtual void segment(unsigned char *start, long length);

	static int isutf8(unsigned char *here);
	static int isutf8(char *here) { return isutf8((unsigned char *)here); }

	static int ischinese(unsigned char *here);
	static int ischinese(char *here) { return ischinese((unsigned char *)here); }

	static int iseuropean(unsigned char *here);
	static int iseuropean(char *here) { return iseuropean((unsigned char *)here); }
	
	static unsigned long utf8_to_wide(unsigned char *here);
	static long utf8_bytes(unsigned char *here);
	static long utf8_bytes(char *here) { return utf8_bytes((unsigned char *)here); }

	static unsigned char *tolower(unsigned char *here);

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
	ANT_PARSER::ISUTF8()
	--------------------------
	if it is valid uft8 bytes
*/
inline int ANT_parser::isutf8(unsigned char *here)
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
inline int ANT_parser::ischinese(unsigned char *here)
{
unsigned long chinese;

if (!isutf8(here)/*(*here & 0x80) == 0*/)
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
	ANT_PARSER::ISEUROPEAN()
	-----------------------
	Is the given character from the European(German) CodePoint?
*/
inline int ANT_parser::iseuropean(unsigned char *here)
{
unsigned long european;

if (!isutf8(here)/*(*here & 0x80) == 0*/)
	return FALSE;
else
	{
	european = utf8_to_wide(here);

	return ((european == 0x00C4)|| // Ä U+00C4 Latin capital letter A with diaeresis
		(european == 0x00D6)|| // Ö U+00D6 Latin capital letter O with diaeresis
		(european == 0x00DC)|| // Ü U+00DC Latin capital letter U with diaeresis
		//(european == 0x1E9E)|| // ẞ U+1E9E LATIN CAPITAL LETTER SHARP S
		(european == 0x00E4)|| // ä U+00E4 Latin small letter a with diaeresis
		(european == 0x00F6)|| // ö U+00F6 Latin small letter o with diaeresis
		(european == 0x00FC)|| // ü U+00FC Latin small letter u with diaeresis
		(european == 0x00DF)   // ß U+00DF LATIN SMALL LETTER SHARP S
		);
	}
}

/*
	ANT_PARSER::TOLOWER()
	-----------------------
	to convert bothe ascii and European(German) character to lowercase
	this is a temporary solution
*/
inline unsigned char *ANT_parser::tolower(unsigned char *here)
{
	long number_of_bytes =  1;

	if ((*here & 0x80) == 0)
		*here = ANT_tolower(*here);
	else if (iseuropean(here))
		{
		number_of_bytes = utf8_bytes(here);
		if (strncmp((char *)here, "Ä", number_of_bytes) == 0)
			strncpy((char *)here, "ä", number_of_bytes);
		else if (strncmp((char *)here, "Ö", number_of_bytes) == 0)
			strncpy((char *)here, "ö", number_of_bytes);
		else if (strncmp((char *)here, "Ü", number_of_bytes) == 0)
			strncpy((char *)here, "ü", number_of_bytes);
//		else if (strncmp((char *)here, "ẞ", number_of_bytes) == 0)
//			strncpy((char *)here, "ß", number_of_bytes);
		}
	return here;
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

#endif  /* PARSER_H_ */
