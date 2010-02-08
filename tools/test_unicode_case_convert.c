/*
	TEST_UNICODE_CASE_CONVERT.C
	---------------------------
*/
#include <stdio.h>
#include <ctype.h>
#include "../source/unicode_case.h"

/*
	MAIN()
	------
*/
int main(void)
{
long ch, upper_ch, lower_ch, lower_upper_ch, upper_lower_ch;

for (ch = 0; ch < 66666; ch++)
	{
	upper_ch = ANT_UNICODE_toupper(ch);
	lower_ch = ANT_UNICODE_tolower(ch);

	lower_upper_ch = ANT_UNICODE_tolower(upper_ch);
	upper_lower_ch = ANT_UNICODE_toupper(lower_ch);

	if (upper_ch != upper_lower_ch)
		printf("[%lx: ul != u L:%lx U:%lx]\n", ch, lower_ch, upper_ch);
	if (lower_ch != lower_upper_ch)
		printf("[%lx: lu != l L:%lx U:%lx]\n", ch, lower_ch, upper_ch);

	if (ch < 0x80 && isalpha(ch))
		printf("[%c L:%c U:%c]\n", ch, lower_ch, upper_ch);
	if (ch == 0x80)
		printf("\n");
	}

return 0;
}