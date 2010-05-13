/*
	STEM_PORTER.H
	-------------
*/
#ifndef STEM_PORTER_H_
#define STEM_PORTER_H_

#include <stdio.h>
#include "ctypes.h"
#include "btree_iterator.h"		// for MAX_TERM_LENGTH only
#include "stem.h"

/*
	class ANT_STEM_PORTER
	---------------------
*/
class ANT_stem_porter : public ANT_stem
{
private:
	char workspace[MAX_TERM_LENGTH];

private:
	long isvowely(char *c) { return *c != '\0' && (ANT_isvowel(*c) || (*c == 'y' && !ANT_isvowel(*(c + 1)))); }
	long cvc(char *what) { return ((strchr("aeiouwxy", *what) == NULL) && (isvowely(what + 1)) && (!isvowely(what + 2))) ? 1 : 0; }
	long length(char *reversed);
	long has_vowel(char *what);

public:
	ANT_stem_porter() {}
	virtual ~ANT_stem_porter() {}
	virtual size_t stem(const char *term, char *destination);
	virtual char *name(void) { return "Porter"; }
} ;

#endif /* STEM_PORTER_H_ */
