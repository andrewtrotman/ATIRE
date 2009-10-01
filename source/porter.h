/*
	PORTER.H
	--------
*/

#ifndef PORTER_H_
#define PORTER_H_

#include <stdio.h>
#include "ctypes.h"
#include "btree_iterator.h"		// for MAX_TERM_LENGTH only

class ANT_porter
{
private:
	char workspace[MAX_TERM_LENGTH];

private:
	long isvowely(char *c) { return *c != '\0' && (ANT_isvowel(*c) || (*c == 'y' && !ANT_isvowel(*(c + 1)))); }
	long cvc(char *what) { return ((strchr("aeiouwxy", *what) == NULL) && (isvowely(what + 1)) && (!isvowely(what + 2))) ? 1 : 0; }
	long length(char *reversed);
	long has_vowel(char *what);

public:
	ANT_porter() {}
	virtual ~ANT_porter() {}
	virtual size_t stem(char *term, char *destination);
} ;

#endif  /* PORTER_H_ */

