/*
	STEM_S.C
	--------
*/
#include <string.h>
#include "stem_s.h"

/*
	ANT_STEM_S::STEM()
	------------------
*/
size_t ANT_stem_s::stem(const char *term, char *destination)
{
size_t len;

len = strlen(term);
strcpy(destination, term);
if (strcmp(destination + len - 3, "ies") == 0)
	{
	strcpy(destination + len - 3, "y");
	len -= 3;
	}
else if (strcmp(destination + len - 2, "es") == 0)
	{
	*(destination + len - 2) = '\0';
	len -= 2;
	}
else if (destination[len - 1] == 's')
	{
	*(destination + len - 1) = '\0';
	len--;
	}

return len;
}

