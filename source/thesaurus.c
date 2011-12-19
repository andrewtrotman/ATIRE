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
allowed = 0;

return answer;
}

/*
	ANT_THESAURUS::SET_ALLOWABLE_RELATIONSHIPS()
	--------------------------------------------
*/
void ANT_thesaurus::set_allowable_relationships(unsigned long types)
{
allowed = types;
}

/*
	ANT_THESAURUS::ALLOWABLE_RELATIONSHIP()
	---------------------------------------
*/
long ANT_thesaurus::allowable_relationship(long relationship)
{
return relationship & allowed;
}


