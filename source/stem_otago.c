/*
	STEM_OTAGO.C
	------------
	Stemmer learned for the INEX wikipedia collection.  It was learned by Vaughn Wood using 
	Genetic Programming.
*/
#include <string.h>
#include "pragma.h"
#include "stem_otago.h"

#pragma ANT_PRAGMA_CONST_CONDITIONAL

#define TMP_BUFFER_SIZE 255
#define SACROSANCT_CHARS 3

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	CONSONANT_P()
	-------------
*/
inline static int consonant_p(const char *s, size_t i) {
    switch (s[i]) {
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
	ANT_STEM_OTAGO::STEM()
	----------------------
*/
size_t ANT_stem_otago::stem(const char *string, char *buffer) {
    size_t length;

    strncpy(buffer, string, TMP_BUFFER_SIZE);
    buffer[TMP_BUFFER_SIZE - 1] = '\0';

    length = strlen(buffer);

    do {	/* the m()>=0 below is removed because it is always true! (it asks whether unsigned int >= 0) */
        if (/*m(buffer, length - 1) >= 0 && */strncmp(buffer + length - 3, "shi", 4) == 0 && length > 6) {
            strncpy(buffer + length - 3, "f", 4);
            buffer[length - 2] = '\0';
            length -= 2;
            break;
        }
        if (m(buffer, length - 1) >= 2 && strncmp(buffer + length - 2, "ej", 4) == 0 && length > 5) {
            buffer[length - 2] = '\0';
            length -= 2;
            break;
        }
        if (m(buffer, length - 1) >= 4 && strncmp(buffer + length - 4, "ngen", 4) == 0 && length > 7) {
            buffer[length - 4] = '\0';
            length -= 4;
            break;
        }
        if (m(buffer, length - 1) >= 1 && strncmp(buffer + length - 1, "i", 4) == 0 && length > 4) {
            strncpy(buffer + length - 1, "dops", 4);
            buffer[length - -3] = '\0';
            length += 3;
            break;
        }
        if (m(buffer, length - 1) >= 4 && strncmp(buffer + length - 3, "nes", 4) == 0 && length > 6) {
            strncpy(buffer + length - 3, "sy", 4);
            buffer[length - 1] = '\0';
            length -= 1;
            break;
        }
        if (/*m(buffer, length - 1) >= 0 && */ strncmp(buffer + length - 3, "ics", 4) == 0 && length > 6) {
            strncpy(buffer + length - 3, "e", 4);
            buffer[length - 2] = '\0';
            length -= 2;
            break;
        }
        if (/*m(buffer, length - 1) >= 0 && */ strncmp(buffer + length - 2, "ii", 4) == 0 && length > 5) {
            strncpy(buffer + length - 2, "sr", 4);
            buffer[length - 0] = '\0';
            break;
        }
        if (/*m(buffer, length - 1) >= 0 && */ strncmp(buffer + length - 3, "ito", 4) == 0 && length > 6) {
            strncpy(buffer + length - 3, "ng", 4);
            buffer[length - 1] = '\0';
            length -= 1;
            break;
        }
        if (m(buffer, length - 1) >= 4 && strncmp(buffer + length - 2, "rs", 4) == 0 && length > 5) {
            strncpy(buffer + length - 2, "tie", 4);
            buffer[length - -1] = '\0';
            length += 1;
            break;
        }
        if (/*m(buffer, length - 1) >= 0 && */ strncmp(buffer + length - 1, "q", 4) == 0 && length > 4) {
            buffer[length - 1] = '\0';
            length -= 1;
            break;
        }
        if (m(buffer, length - 1) >= 4 && strncmp(buffer + length - 2, "al", 4) == 0 && length > 5) {
            buffer[length - 2] = '\0';
            length -= 2;
            break;
        }
        if (m(buffer, length - 1) >= 3 && strncmp(buffer + length - 2, "in", 4) == 0 && length > 5) {
            strncpy(buffer + length - 2, "ar", 4);
            buffer[length - 0] = '\0';
            break;
        }
        if (/*m(buffer, length - 1) >= 0 && */ strncmp(buffer + length - 3, "ice", 4) == 0 && length > 6) {
            strncpy(buffer + length - 3, "s", 4);
            buffer[length - 2] = '\0';
            length -= 2;
            break;
        }
        if (m(buffer, length - 1) >= 3 && strncmp(buffer + length - 2, "ic", 4) == 0 && length > 5) {
            buffer[length - 2] = '\0';
            length -= 2;
            break;
        }
    } while (0);
    do {
        if (m(buffer, length - 1) >= 4 && strncmp(buffer + length - 2, "rs", 4) == 0 && length > 5) {
            strncpy(buffer + length - 2, "tie", 4);
            buffer[length - -1] = '\0';
            length += 1;
            break;
        }
        if (m(buffer, length - 1) >= 1 && strncmp(buffer + length - 1, "s", 4) == 0 && length > 4) {
            buffer[length - 1] = '\0';
            length -= 1;
            break;
        }
        if (m(buffer, length - 1) >= 1 && strncmp(buffer + length - 1, "f", 4) == 0 && length > 4) {
            strncpy(buffer + length - 1, "uow", 4);
            buffer[length - -2] = '\0';
            length += 2;
            break;
        }
        if (/*m(buffer, length - 1) >= 0 && */ strncmp(buffer + length - 1, "f", 4) == 0 && length > 4) {
            strncpy(buffer + length - 1, "uow", 4);
            buffer[length - -2] = '\0';
            length += 2;
            break;
        }
        if (/*m(buffer, length - 1) >= 0 && */ strncmp(buffer + length - 1, "q", 4) == 0 && length > 4) {
            buffer[length - 1] = '\0';
            length -= 1;
            break;
        }
        if (m(buffer, length - 1) >= 1 && strncmp(buffer + length - 1, "s", 4) == 0 && length > 4) {
            buffer[length - 1] = '\0';
            length -= 1;
            break;
        }
    } while (0);
    do {
        if (m(buffer, length - 1) >= 2 && strncmp(buffer + length - 3, "que", 4) == 0 && length > 6) {
            strncpy(buffer + length - 3, "sy", 4);
            buffer[length - 1] = '\0';
            length -= 1;
            break;
        }
        if (/*m(buffer, length - 1) >= 0 && */ strncmp(buffer + length - 2, "sl", 4) == 0 && length > 5) {
            strncpy(buffer + length - 2, "anu", 4);
            buffer[length - -1] = '\0';
            length += 1;
            break;
        }
        if (m(buffer, length - 1) >= 2 && strncmp(buffer + length - 1, "e", 4) == 0 && length > 4) {
            buffer[length - 1] = '\0';
            length -= 1;
            break;
        }
        if (m(buffer, length - 1) >= 1 && strncmp(buffer + length - 1, "f", 4) == 0 && length > 4) {
            buffer[length - 1] = '\0';
            length -= 1;
            break;
        }
        if (m(buffer, length - 1) >= 3 && strncmp(buffer + length - 4, "ague", 4) == 0 && length > 7) {
            strncpy(buffer + length - 4, "dz", 4);
            buffer[length - 2] = '\0';
            length -= 2;
            break;
        }
        if (/*m(buffer, length - 1) >= 0 && */ strncmp(buffer + length - 3, "ean", 4) == 0 && length > 6) {
            buffer[length - 3] = '\0';
            length -= 3;
            break;
        }
    } while (0);
    return 3;		// so that ANT will check all terms with the same start 3 characters
}
