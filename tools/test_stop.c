/*
	TEST_STOP.C
	-----------
*/
#include <string.h>
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

gets(buffer);
while (strcmp(buffer, ".quit") != 0)
	{
	what = stopper.isstop(buffer);
	printf("%s is%s a stop word\n", buffer, what ? "" : " not");
	gets(buffer);
	}

return 0;
}