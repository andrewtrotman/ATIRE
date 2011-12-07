/*
	THESAURUS.H
	-----------
*/
#ifndef THESAURUS_H_
#define THESAURUS_H_
	
#include "../../source/str.h"

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
} ;

#endif /* THESAURUS_H_ */
