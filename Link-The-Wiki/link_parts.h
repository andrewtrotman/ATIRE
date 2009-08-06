/*
	LINK_PARTS.H
	------------
	Written (w) 2008 by Andrew Trotman, University of Otago
*/

#ifndef __LINK_PARTS_H__
#define __LINK_PARTS_H__

#include <ctype.h>

#define INEX_ARCHIVE_ARTICLE_ID_SIGNITURE "name id="
#define ARTICLE_ID_SIGNITURE "id"

/*
	STRING_CLEAN()
	--------------
*/
inline static char *string_clean(char *file, long lower_case_only)
{
char *ch, *from, *to;

/*
	remove XML tags and remove all non-alnum (but keep case)
*/
ch = file;
while (*ch != '\0')
	{
	if (*ch == '<')			// then remove the XML tags
		{
		while (*ch != '>')
			*ch++ = ' ';
		*ch++ = ' ';
		}
	else if (!isalnum(*ch))	// then remove it
		*ch++ = ' ';
	else
		{
		if (lower_case_only)
			{
			*ch = (char)tolower(*ch);
			ch++;
			}
		else
			ch++;
		}
	}

/*
	now remove multiple, head, and tail spaces.
*/
from = to = file;
while (isspace(*from))
	from++;
while (*from != '\0')
	{
	while (isalnum(*from))
		*to++ = *from++;
	if (isspace(*from))
		*to++ = *from++;
	while (isspace(*from))
		from++;
	}
if (to > file && isspace(*(to - 1)))
	to--;
*to = '\0';

return file;
}


/*
	STRNNEW()
	---------
*/
inline static char *strnnew(char *source, long long length)
{
char *ans;

ans = new char [(size_t)(length + 1)];
strncpy(ans, source, (size_t)length);
ans[length] = '\0';

return ans;
}

/*
	GET_DOC_ID()
	------------
*/
inline static long get_doc_id(char *file)
{
char *pos;

pos = strstr(file, "<"INEX_ARCHIVE_ARTICLE_ID_SIGNITURE);
if (pos == NULL)
	pos = strstr(file, "<"ARTICLE_ID_SIGNITURE);
if (pos == NULL)
	exit(printf("Cannot find DOC id <name id=...> or <id> in file\n"));
while (!isdigit(*pos))
	pos++;

return atol(pos);
}

#endif __LINK_PARTS_H__

