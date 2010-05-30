/*
	PHONETIC_DOUBLE_METAPHONE.H
	---------------------------
	For the original articles on Metaphone and Double Metaphone see:
	Philips, L. (2000) The Double Metaphone Search Algorithm, C/C++ Users Journal archive, 18(6):38-43

	Interface library for Adam J. Nelson's 2003 Metaphone code.  That code has an "unrestricted license"
	for usage instructions on his code see: http://www.codeproject.com/kb/string/dmetaphone1.aspx
*/
#ifndef PHONETIC_DOUBLE_METAPHONE_H_
#define PHONETIC_DOUBLE_METAPHONE_H_

/*
	These defines are to get around a compilation problem in the original double metaphone code
*/
#ifdef _UNICODE
	#define _tcsncmp wcsncmp
	#define _tcsupr wcsupr
#else
	#define _tcsncmp strncmp
	#ifdef _MSC_VER
		#define _tcsupr _strupr
	#else
		#define _tcsupr strupper
	#endif
#endif

#include "phonetic_double_metaphone_internals.h"
#include "stem.h"
#include "str.h"

/*
	class ANT_PHONETIC_DOUBLE_METAPHONE
	-----------------------------------
*/
class ANT_phonetic_double_metaphone : public ANT_stem
{
private:
	static const int METAPHONE_KEY_LENGTH = 4;

private:
	DoubleMetaphone<METAPHONE_KEY_LENGTH> generator;

public:
	ANT_phonetic_double_metaphone() {}
	virtual ~ANT_phonetic_double_metaphone() {}
	virtual size_t stem(const char *term, char *destination) { generator.computeKeys(term); strcpy(destination, generator.getPrimaryKey()); strlower(destination); return 1; }
	virtual char *name(void) { return "Double Metaphone"; }
} ;

#endif /* PHONETIC_DOUBLE_METAPHONE_H_ */
