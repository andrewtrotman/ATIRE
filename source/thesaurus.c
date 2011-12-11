/*
	THESAURUS.C
	-----------
*/
#include "string_pair.h"
#include "thesaurus.h"

/*
	ANT_THESAURUS::GET_SYNSET()
	---------------------------
*/
ANT_thesaurus_relationship *ANT_thesaurus::get_synset(ANT_string_pair *term_pair, long long *terms_in_synset)
{
ANT_thesaurus_relationship *answer;
char *string;

string = term_pair->str();
answer = get_synset(string, terms_in_synset);
delete [] string;

return answer;
}

/*
	ANT_THESAURUS::SET_ALLOWABLE_RELATIONSHIPS()
	--------------------------------------------
*/
void ANT_thesaurus::set_allowable_relationships(unsigned long types)
{
memset(allowed, 0, sizeof(allowed));

if ((types & HYPONYM) != 0)
	{
	allowed[ANT_thesaurus_relationship::HYPONYM] = true;
	allowed[ANT_thesaurus_relationship::INSTANCE_HYPONYM] = true;
	}

if ((types & ANTONYM) != 0)
	allowed[ANT_thesaurus_relationship::ANTONYM] = true;

if ((types & HOLONYM) != 0)
	{
	allowed[ANT_thesaurus_relationship::MEMBER_HOLONYM] = true;
	allowed[ANT_thesaurus_relationship::PART_HOLONYM] = true;
	allowed[ANT_thesaurus_relationship::SUBSTANCE_HOLONYM] = true;
	}

if ((types & MERONYM) != 0)
	{
	allowed[ANT_thesaurus_relationship::MEMBER_MERONYM] = true;
	allowed[ANT_thesaurus_relationship::PART_MERONYM] = true;
	allowed[ANT_thesaurus_relationship::SUBSTANCE_MERONYM] = true;
	}

if ((types & HYPERNYM) != 0)
	{
	allowed[ANT_thesaurus_relationship::HYPERNYM] = true;
	allowed[ANT_thesaurus_relationship::INSTANCE_HYPERNYM] = true;
	}

if ((types & SYNONYM) != 0)
	allowed[ANT_thesaurus_relationship::SYNONYM] = true;
}

/*
	ANT_THESAURUS::ALLOWABLE_RELATIONSHIP()
	---------------------------------------
*/
long ANT_thesaurus::allowable_relationship(long relationship)
{
return allowed[relationship];
}


