/*
	WORDNET_TEST.C
	--------------
*/

#include "wordnet.h"

/*
	MAIN()
	------
*/
int main(void)
{
ANT_wordnet *wordnet;

wordnet = new ANT_wordnet("wordnet.aspt");
delete wordnet;

return 0;
}