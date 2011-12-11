/*
	THESAURUS.H
	-----------
*/
#ifndef THESAURUS_H_
#define THESAURUS_H_
	
#include "str.h"
#include "thesaurus_relationship.h"

class ANT_string_pair;

/*
	class ANT_THESAURUS
	-------------------
*/
class ANT_thesaurus
{
public:
	enum { HYPONYM = 1, ANTONYM = 2, HOLONYM = 4, MERONYM = 8, HYPERNYM = 16, SYNONYM = 32 };

private:
	char *filename;														// the name of the disk file that is this thesaurus
	unsigned char allowed[ANT_thesaurus_relationship::SENTINAL];	// what relationship types are allowed?

protected:
	/*
		returns true if the given relationship type is being included in the synsets
	*/
	virtual long allowable_relationship(long relationship);

public:
	ANT_thesaurus(char *filename) { this->filename = strnew(filename); set_allowable_relationships(SYNONYM); }
	virtual ~ANT_thesaurus() { delete [] filename; }

	/*
		returns the synset for the given term
	*/
	virtual ANT_thesaurus_relationship *get_synset(char *term, long long *terms_in_synset = NULL) = 0;
	virtual ANT_thesaurus_relationship *get_synset(ANT_string_pair *term, long long *terms_in_synset = NULL);

	/*
		Call this routine to allow other than just synonyms to be returned by get_synset()
	*/
	virtual void set_allowable_relationships(unsigned long types);
} ;

#endif /* THESAURUS_H_ */
