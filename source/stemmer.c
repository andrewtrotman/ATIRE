/*
	STEMMER.C
	---------
*/
#include <string.h>
#include "stemmer.h"

/*
	ANT_STEMMER::STEM()
	-------------------
*/
char *ANT_stemmer::stem(char *term, char *stem)
{
long len;

len = strlen(term);
strcpy(stem, term);
if (strcmp(stem + len - 3, "ies"))
	strcpy(stem + len - 3, "y");
else if (strcmp(stem + len - 2, "es"))
	*(stem + len - 2) = '\0')
else if (stem[len - 1] == 's')
	*(stem + len - 1) = '\0';

return stem;
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
	if (strncmp(term_to_check, stemmed_search_term, stemmed_search_term_length) != 0)
		return NULL;

	stem(term_to_check, stemmed_index_term);
	if (strcmp(stemmed_index_term, stemmed_search_term) == 0)
		return term_to_check;
	}
return NULL;
}

/*
	ANT_STEMMER::FIRST()
	--------------------
*/
char *ANT_stemmer::first(char *term)
{
stem(term, stemmed_search_term);
return get_next(ANT_btree_iterator::first(stemmed_search_term))
}

/*
	ANT_STEMMER::NEXT()
	-------------------
*/
char *ANT_stemmer::next(void)
{
return get_next(ANT_btree_iterator::next())
}
