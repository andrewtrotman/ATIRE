/*
	PHONE.C
	-------
	Test the metaphone routines
*/

#include <stdio.h>
#include "../source/phonetic_double_metaphone.h"

char buffer[1024], metaphone[1024];
/*
	MAIN()
	------
*/
int main(void)
{
ANT_phonetic_double_metaphone meta;

while (1)
	{
	printf("]");
	gets(buffer);
	meta.stem(buffer, metaphone);
	printf("[%s->%s]\n", buffer, metaphone);
	}

return 0;
}
