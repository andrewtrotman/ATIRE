/*
	STEM_SNOWBALL.C
	---------------
*/
#include <string.h>

#ifdef ANT_HAS_SNOWBALL

#include "stem_snowball.h"

/*
	ANT_STEM_SNOWBALL::STEM()
	-------------------------
*/
size_t ANT_stem_snowball::stem(const char *term, char *destination)
{
const sb_symbol *answer;
int answer_length;

answer = sb_stemmer_stem(snowball, (sb_symbol *)term, (int)strlen(term));
answer_length = sb_stemmer_length(snowball);
strncpy(destination, (const char *)answer, answer_length);
destination[answer_length] = '\0';

return 2;
}

#endif
