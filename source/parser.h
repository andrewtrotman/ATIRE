/*
	PARSER.H
	--------
*/
#ifndef PARSER_H_
#define PARSER_H_

#include "string_pair.h"
#include "parser_token.h"
#include "ctypes.h"
#include "unicode_tables.h"
#include "unicode.h"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE (!FALSE)
#endif

#ifdef _MSC_VER
	#define inline __forceinline
#endif

/*
	class ANT_PARSER
	----------------
*/
class ANT_parser
{
public:
	/*
		This might be confusing. For Chinese, no segmentation means documents are indexed by characters only;
		DOUBLE_SEGMENTATION means string is segmented into words and also single characters for indexing
		ONFLY_SEGMENTATION means segment the sentence using the Chinese segmentation module when parsing the document
		If ONFLY_SEGMENTATION is not set, but SHOULD_SEGMENT is still set that means the text is segmented.
	 */
	enum { NOSEGMENTATION = 0, SHOULD_SEGMENT = 1, ONFLY_SEGMENTATION = 2, DOUBLE_SEGMENTATION = 4, BIGRAM_SEGMENTATION = 8 };

protected:
	unsigned char *document;
	unsigned char *current;
	ANT_parser_token current_token;
	unsigned char *segmentation;
	long should_segment;

/*
protected:
	static int isheadchar(unsigned char val) { return ANT_isalnum(val) || val == '<' || val == '\0'; }
*/

public:
	ANT_parser(long should_segment = NOSEGMENTATION);
	virtual ~ANT_parser();

/*
	static int isXMLnamestartchar(unsigned char val) { return ANT_isalpha(val) || val == ':' || val == '_'; }		// see http://www.w3.org/TR/REC-xml/#NT-NameStartChar
	static int isXMLnamechar(unsigned char val) { return isXMLnamestartchar(val) || ANT_isdigit(val) || val == '.' || val == '-'; } // see http://www.w3.org/TR/REC-xml/#NT-NameChar
*/

	virtual void segment(unsigned char *start, long length);

	static int ischinese(unsigned long character);
	static int ischinese(unsigned char *here);
	static int ischinese(char *here) { return ischinese((unsigned char *)here); }

	static int iseuropean(unsigned char *here);
	static int iseuropean(char *here) { return iseuropean((unsigned char *)here); }

	void set_document(unsigned char *document);
	virtual ANT_parser_token *get_next_token(void);
} ;

/*
	ANT_PARSER::ISCHINESE()
	-----------------------
	Is the given character from the Chinese CodePoint?
*/
inline int ANT_parser::ischinese(unsigned long character)
{
return ((character >= 0x04e00 && character <= 0x09fff)		// CJK Unified Ideographs
	 || (character >= 0x03400 && character <= 0x04dbf)		// CJK Unified Ideographs Extension A
	 || (character >= 0x20000 && character <= 0x2a6df)		// CJK Unified Ideographs Extension B
	 || (character >= 0x0f900 && character <= 0x0faff)		// CJK Compatibility Ideographs
	 || (character >= 0x2f800 && character <= 0x2fa1f));	// CJK Compatibility Ideographs Supplement
}

/*
	ANT_PARSER::ISCHINESE()
	-----------------------
	Is the given character from the Chinese CodePoint?
*/
inline int ANT_parser::ischinese(unsigned char *here)
{
return ischinese(utf8_to_wide(here));
}

/*
	ANT_PARSER::ISEUROPEAN()
	-----------------------
	Is the given character from the European (German) CodePoint?
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

#endif  /* PARSER_H_ */


