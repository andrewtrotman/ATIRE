/*
	THESAURUS.H
	-----------
*/
#ifndef THESAURUS_H_
#define THESAURUS_H_
	
#include "str.h"

class ANT_thesaurus_relationship;

/*
	class ANT_THESAURUS
	-------------------
*/
class ANT_thesaurus
{
private:
	char *filename;

public:
	ANT_thesaurus(char *filename) { this->filename = strnew(filename); }
	virtual ~ANT_thesaurus() { delete [] filename; }

	virtual ANT_thesaurus_relationship *get_synset(char *term, long long *terms_in_synset = NULL) = 0;
} ;

#endif /* THESAURUS_H_ */
