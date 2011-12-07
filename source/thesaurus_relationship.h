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
	enum {
	MEMBER_OF_TOPIC,
	MEMBER_OF_REGION,
	MEMBER_OF_USAGE,
	HYPONYM,
	INSTANCE_HYPONYM,
	ANTONYM,
	MEMBER_HOLONYM,
	PART_HOLONYM,
	SUBSTANCE_HOLONYM,
	VERB_GROUP,
	MEMBER_MERONYM,
	PART_MERONYM,
	SUBSTANCE_MERONYM,
	SIMILAR_TO,
	ENTAILMENT,
	DOMAIN_OF_TOPIC,
	DOMAIN_OF_REGION,
	DOMAIN_OF_USAGE,
	HYPERNYM,
	INSTANCE_HYPERNYM,
	FROM_ABJECTIVE,
	PERTAINYM,
	SEE_ALSO,
	DERIVED_FORM,
	PRINCIPLE_OF_VERB,
	ATTRIBUTE,
	CAUSE,
	SYNONYM,
	SENTINAL					// not a relationship, used to mark the end of lists
	};

static char *name_of_relationship[];

public:
	long relationship;
	char *term;

public:
	static char *get_relationship_name(long relationship) { return name_of_relationship[relationship]; }
	
} ;

#endif /* THESAURUS_RELATIONSHIP_H_ */
