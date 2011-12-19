/*
	THESAURUS_RELATIONSHIP.H
	------------------------
*/
#ifndef THESAURUS_RELATIONSHIP_H_
#define THESAURUS_RELATIONSHIP_H_

/*
	class ANT_THESAURUS_RELATIONSHIP
	--------------------------------
*/
class ANT_thesaurus_relationship
{
public:
	enum { NONE = 0, HYPONYM = 1, ANTONYM = 2, HOLONYM = 4, MERONYM = 8, HYPERNYM = 16, SYNONYM = 32, ALL = 63 };

public:
	long relationship;
	char *term;

public:
	static char *get_relationship_name(long relationship);
} ;

#endif /* THESAURUS_RELATIONSHIP_H_ */
