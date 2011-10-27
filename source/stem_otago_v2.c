/*
	STEM_OTAGO_V2.C
	---------------
	Version 2 of the Otago stemmer learned by Vaughn Wood's Genetic Algorithm code.
*/
#include <string.h>
#include "pragma.h"
#include "stem_otago_v2.h"

#define TMP_BUFFER_SIZE 255
#define SACROSANCT_CHARS 2

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

#pragma ANT_PRAGMA_CONST_CONDITIONAL

/*
	CONSONANT_P()
	-------------
*/
inline static size_t consonant_p(const char *s, size_t i)
{
switch (s[i])
	{
	case 'a': case 'e': case 'i': case 'o': case 'u': return FALSE;
	case 'y': return (i == 0) ? TRUE : !consonant_p(s, i-1);
	default: return TRUE;
	}
}

/*
	M()
	---
*/
inline static size_t m(const char *s, size_t j)
{
size_t n = 0, i = 0;
while(1)
	{
	if (i > j)
		return n;
	if (!consonant_p(s, i))
		break;
	i++;
	}
i++;
while(1)
	{
	while(1)
		{
		if (i > j)
			return n;
		if (consonant_p(s, i))
			break;
		i++;
		}
	i++;
	n++;
	while(1)
		{
		if (i > j)
			return n;
		if (!consonant_p(s, i))
			break;
		i++;
		}
	i++;
	}
}

/*
	ANT_STEM_OTAGO_V2::STEM()
	-------------------------
	Note that buffer should be at least TMP_BUFFER_SIZE
*/
size_t ANT_stem_otago_v2::stem(const char *string, char *buffer)
{
size_t length;

strncpy(buffer, string, TMP_BUFFER_SIZE);
buffer[TMP_BUFFER_SIZE - 1] = '\0';

length = strlen(buffer);
size_t buf_m = m(buffer, length - 1);

if (buf_m >= 1 && strncmp(buffer + length - 1, "s", 4) == 0 && length >= 3)
	{
	buffer[length - 1] = '\0';
	length -= 1;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 0 && strncmp(buffer + length - 3, "dar", 4) == 0 && length >= 5)
	{
	strncpy(buffer + length - 3, "ater", 4);
	buffer[length - -1] = '\0';
	length += 1;
	buf_m = m(buffer, length - 1);
	}

if (buf_m >= 4 && strncmp(buffer + length - 4, "enet", 4) == 0 && length >= 6)
	{
	strncpy(buffer + length - 4, "r", 4);
	buffer[length - 3] = '\0';
	length -= 3;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 0 && strncmp(buffer + length - 2, "ya", 4) == 0 && length >= 4)
	{
	strncpy(buffer + length - 2, "ros", 4);
	buffer[length - -1] = '\0';
	length += 1;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 4 && strncmp(buffer + length - 1, "y", 4) == 0 && length >= 3)
	{
	strncpy(buffer + length - 1, "itsk", 4);
	buffer[length - -3] = '\0';
	length += 3;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 1 && strncmp(buffer + length - 2, "ce", 4) == 0 && length >= 4)
	{
	strncpy(buffer + length - 2, "ing", 4);
	buffer[length - -1] = '\0';
	length += 1;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 0 && strncmp(buffer + length - 1, "c", 4) == 0 && length >= 3)
	{
	buffer[length - 1] = '\0';
	length -= 1;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 1 && strncmp(buffer + length - 4, "apse", 4) == 0 && length >= 6)
	{
	strncpy(buffer + length - 4, "z", 4);
	buffer[length - 3] = '\0';
	length -= 3;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 3 && strncmp(buffer + length - 1, "n", 4) == 0 && length >= 3)
	{
	buffer[length - 1] = '\0';
	length -= 1;
	buf_m = m(buffer, length - 1);
	}

if (buf_m >= 0 && strncmp(buffer + length - 2, "ki", 4) == 0 && length >= 4)
	{
	strncpy(buffer + length - 2, "gby", 4);
	buffer[length - -1] = '\0';
	length += 1;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 1 && strncmp(buffer + length - 1, "s", 4) == 0 && length >= 3)
	{
	buffer[length - 1] = '\0';
	length -= 1;
	buf_m = m(buffer, length - 1);
	}

if (buf_m >= 4 && strncmp(buffer + length - 2, "na", 4) == 0 && length >= 4)
	{
	strncpy(buffer + length - 2, "koto", 4);
	buffer[length - -2] = '\0';
	length += 2;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 0 && strncmp(buffer + length - 3, "bco", 4) == 0 && length >= 5)
	{
	buffer[length - 3] = '\0';
	length -= 3;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 0 && strncmp(buffer + length - 4, "alur", 4) == 0 && length >= 6)
	{
	strncpy(buffer + length - 4, "s", 4);
	buffer[length - 3] = '\0';
	length -= 3;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 2 && strncmp(buffer + length - 4, "nonc", 4) == 0 && length >= 6)
	{
	strncpy(buffer + length - 4, "eris", 4);
	buffer[length - 0] = '\0';
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 3 && strncmp(buffer + length - 2, "rd", 4) == 0 && length >= 4)
	{
	strncpy(buffer + length - 2, "ller", 4);
	buffer[length - -2] = '\0';
	length += 2;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 4 && strncmp(buffer + length - 2, "al", 4) == 0 && length >= 4)
	{
	buffer[length - 2] = '\0';
	length -= 2;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 3 && strncmp(buffer + length - 4, "welt", 4) == 0 && length >= 6)
	{
	strncpy(buffer + length - 4, "n", 4);
	buffer[length - 3] = '\0';
	length -= 3;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 0 && strncmp(buffer + length - 1, "w", 4) == 0 && length >= 3)
	{
	strncpy(buffer + length - 1, "gia", 4);
	buffer[length - -2] = '\0';
	length += 2;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 4 && strncmp(buffer + length - 4, "ness", 4) == 0 && length >= 6)
	{
	strncpy(buffer + length - 4, "man", 4);
	buffer[length - 1] = '\0';
	length -= 1;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 3 && strncmp(buffer + length - 2, "ri", 4) == 0 && length >= 4)
	{
	strncpy(buffer + length - 2, "n", 4);
	buffer[length - 1] = '\0';
	length -= 1;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 0 && strncmp(buffer + length - 4, "ioco", 4) == 0 && length >= 6)
	{
	buffer[length - 4] = '\0';
	length -= 4;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 4 && strncmp(buffer + length - 3, "mer", 4) == 0 && length >= 5)
	{
	buffer[length - 3] = '\0';
	length -= 3;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 0 && strncmp(buffer + length - 2, "uk", 4) == 0 && length >= 4)
	{
	strncpy(buffer + length - 2, "ia", 4);
	buffer[length - 0] = '\0';
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 1 && strncmp(buffer + length - 1, "a", 4) == 0 && length >= 3)
	{
	strncpy(buffer + length - 1, "or", 4);
	buffer[length - -1] = '\0';
	length += 1;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 2 && strncmp(buffer + length - 4, "ecci", 4) == 0 && length >= 6)
	{
	strncpy(buffer + length - 4, "ks", 4);
	buffer[length - 2] = '\0';
	length -= 2;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 1 && strncmp(buffer + length - 2, "sk", 4) == 0 && length >= 4)
	{
	buffer[length - 2] = '\0';
	length -= 2;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 4 && strncmp(buffer + length - 4, "ongo", 4) == 0 && length >= 6)
	{
	buffer[length - 4] = '\0';
	length -= 4;
	buf_m = m(buffer, length - 1);
	}

if (buf_m >= 0 && strncmp(buffer + length - 2, "ya", 4) == 0 && length >= 4)
	{
	strncpy(buffer + length - 2, "kas", 4);
	buffer[length - -1] = '\0';
	length += 1;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 0 && strncmp(buffer + length - 1, "y", 4) == 0 && length >= 3)
	{
	strncpy(buffer + length - 1, "i", 4);
	buffer[length - 0] = '\0';
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 1 && strncmp(buffer + length - 2, "ce", 4) == 0 && length >= 4)
	{
	strncpy(buffer + length - 2, "yaka", 4);
	buffer[length - -2] = '\0';
	length += 2;
	buf_m = m(buffer, length - 1);
	}

if (buf_m >= 3 && strncmp(buffer + length - 1, "n", 4) == 0 && length >= 3)
	{
	buffer[length - 1] = '\0';
	length -= 1;
	buf_m = m(buffer, length - 1);
	}

if (buf_m >= 0 && strncmp(buffer + length - 2, "ki", 4) == 0 && length >= 4)
	{
	strncpy(buffer + length - 2, "gby", 4);
	buffer[length - -1] = '\0';
	length += 1;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 1 && strncmp(buffer + length - 4, "rder", 4) == 0 && length >= 6)
	{
	strncpy(buffer + length - 4, "pia", 4);
	buffer[length - 1] = '\0';
	length -= 1;
	buf_m = m(buffer, length - 1);
	}

if (buf_m >= 4 && strncmp(buffer + length - 2, "na", 4) == 0 && length >= 4)
	{
	strncpy(buffer + length - 2, "koto", 4);
	buffer[length - -2] = '\0';
	length += 2;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 0 && strncmp(buffer + length - 3, "miy", 4) == 0 && length >= 5)
	{
	buffer[length - 3] = '\0';
	length -= 3;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 0 && strncmp(buffer + length - 4, "alur", 4) == 0 && length >= 6)
	{
	strncpy(buffer + length - 4, "s", 4);
	buffer[length - 3] = '\0';
	length -= 3;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 2 && strncmp(buffer + length - 3, "ovo", 4) == 0 && length >= 4)
	{
	strncpy(buffer + length - 3, "ozor", 4);
	buffer[length - -1] = '\0';
	length += 1;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 0 && strncmp(buffer + length - 4, "wahj", 4) == 0 && length >= 6)
	{
	strncpy(buffer + length - 4, "nes", 4);
	buffer[length - 1] = '\0';
	length -= 1;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 4 && strncmp(buffer + length - 1, "i", 4) == 0 && length >= 3)
	{
	strncpy(buffer + length - 1, "ssig", 4);
	buffer[length - -3] = '\0';
	length += 3;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 3 && strncmp(buffer + length - 4, "hner", 4) == 0 && length >= 6)
	{
	buffer[length - 4] = '\0';
	length -= 4;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 3 && strncmp(buffer + length - 2, "st", 4) == 0 && length >= 4)
	{
	strncpy(buffer + length - 2, "ish", 4);
	buffer[length - -1] = '\0';
	length += 1;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 4 && strncmp(buffer + length - 1, "k", 4) == 0 && length >= 3)
	{
	strncpy(buffer + length - 1, "idae", 4);
	buffer[length - -3] = '\0';
	length += 3;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 0 && strncmp(buffer + length - 1, "s", 4) == 0 && length >= 3)
	{
	buffer[length - 1] = '\0';
	length -= 1;
	buf_m = m(buffer, length - 1);
	}

if (buf_m >= 1 && strncmp(buffer + length - 3, "uel", 4) == 0 && length >= 5)
	{
	strncpy(buffer + length - 3, "r", 4);
	buffer[length - 2] = '\0';
	length -= 2;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 2 && strncmp(buffer + length - 2, "ry", 4) == 0 && length >= 4)
	{
	strncpy(buffer + length - 2, "g", 4);
	buffer[length - 1] = '\0';
	length -= 1;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 0 && strncmp(buffer + length - 4, "rian", 4) == 0 && length >= 6)
	{
	strncpy(buffer + length - 4, "n", 4);
	buffer[length - 3] = '\0';
	length -= 3;
	buf_m = m(buffer, length - 1);
	}
else if (buf_m >= 2 && strncmp(buffer + length - 1, "e", 4) == 0 && length >= 3)
	{
	buffer[length - 1] = '\0';
	length -= 1;
	buf_m = m(buffer, length - 1);
	}

if (buf_m >= 0 && strncmp(buffer + length - 3, "lit", 4) == 0 && length >= 5)
	{
	strncpy(buffer + length - 3, "ns", 4);
	buffer[length - 1] = '\0';
	length -= 1;
	buf_m = m(buffer, length - 1);
	}
return SACROSANCT_CHARS;
}
