/*
	WORDNET_TEST.C
	--------------
*/
#include <stdio.h>
#include "../source/thesaurus_relationship.h"
#include "../source/thesaurus_wordnet.h"

/*
	MAIN()
	------
*/
int main(void)
{
ANT_thesaurus_wordnet *wordnet;
ANT_thesaurus_relationship *set;
long long set_length, current;
char buffer[1024];

wordnet = new ANT_thesaurus_wordnet("wordnet.aspt");
wordnet->set_allowable_relationships(ANT_thesaurus_relationship::ALL);

while (1)
	{
	printf(">");
	gets(buffer);
	set = wordnet->get_synset(buffer, &set_length);
	for (current = 0; current < set_length; current++)
		{
		printf("%s %s\n", ANT_thesaurus_relationship::get_relationship_name(set->relationship), set->term);
		set++;
		}
	}

delete wordnet;

return 0;
}