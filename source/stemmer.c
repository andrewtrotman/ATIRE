/*
	STEMMER.C
	---------
*/
#include <string.h>
#include "pragma.h"
#include "stemmer.h"

/*
	ANT_STEMMER::STEM()
	-------------------
*/
size_t ANT_stemmer::stem(char *term, char *stem)
{
size_t len;

len = strlen(term);
strcpy(stem, term);
if (strcmp(stem + len - 3, "ies") == 0)
	{
	strcpy(stem + len - 3, "y");
	len -= 3;
	}
else if (strcmp(stem + len - 2, "es") == 0)
	{
	*(stem + len - 2) = '\0';
	len -= 2;
	}
else if (stem[len - 1] == 's')
	{
	*(stem + len - 1) = '\0';
	len--;
	}

return len;
}

/*
	ANT_STEMMER::GET_NEXT()
	-----------------------
*/
char *ANT_stemmer::get_next(char *from)
{
char *term_to_check;

for (term_to_check = from; term_to_check != NULL; term_to_check = ANT_btree_iterator::next())
	{
	stem(term_to_check, stemmed_index_term);
	if (strcmp(stemmed_index_term, stemmed_search_term) == 0)
		return term_to_check;

	if (strncmp(term_to_check, stemmed_search_term, stemmed_search_term_length) != 0)
		return NULL;
	}
return NULL;
}

/*
	ANT_STEMMER::FIRST()
	--------------------
*/
char *ANT_stemmer::first(char *term)
{
stemmed_search_term_length = stem(term, stemmed_search_term);
return get_next(ANT_btree_iterator::first(stemmed_search_term));
}

/*
	ANT_STEMMER::NEXT()
	-------------------
*/
char *ANT_stemmer::next(void)
{
return get_next(ANT_btree_iterator::next());
}

/*
	ANT_STEMMER::WEIGHT_TERMS()
	---------------------------
	Returns a boolean - should we weight tfs for stemmed terms?
*/
long ANT_stemmer::weight_terms(ANT_weighted_tf *term_weight, char *term)
{
*term_weight = 1;

return 0;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
