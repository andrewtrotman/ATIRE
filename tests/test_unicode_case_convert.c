/*
	TEST_UNICODE_CASE_CONVERT.C
	---------------------------
*/
#include <stdio.h>
#include <ctype.h>
#include <string>
#include "../source/unicode_tables.h"
#include "../source/unicode.h"

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

	char buf[100];

	if (utf8_bytes(upper_ch)!=utf8_bytes(lower_ch))
		{
		buf[wide_to_utf8((unsigned char*)buf, 100, ch)]=0;

		printf("[%lx %s: size diff L:%lx U:%lx]\n", ch, buf, lower_ch, upper_ch);
		}

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
