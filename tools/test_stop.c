/*
	TEST_STOP.C
	-----------
*/
#include <stdio.h>
#include "../source/stop_word.h"

/*
	MAIN()
	------
*/
int main(void)
{
ANT_stop_word stopper;
long what;

char buffer[1024];

while (1)
	{
	gets(buffer);
	what = stopper.isstop(buffer);
	printf("%s is%s a stop word\n", buffer, what ? "" : " not");
	}

return 0;
}