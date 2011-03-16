/*
	LINK_PARTS.H
	------------
	Written (w) 2008 by Andrew Trotman, University of Otago
*/

#ifndef __LINK_PARTS_H__
#define __LINK_PARTS_H__

#include <ctype.h>

#define INEX_ARCHIVE_ARTICLE_ID_SIGNITURE "<name id="
#define ARTICLE_ID_SIGNITURE "<id>"
#define ARTICLE_NAME_SIGNITURE "title"

/*
	STRING_CLEAN()
	--------------
*/
inline static char *string_clean(char *file, long lower_case_only, long keep_string_intact = 0, long trim = 1)
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
	else if (!keep_string_intact && !isalnum(*ch))	// then remove it
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
if (trim)
	{
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
	}

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

pos = strstr(file, INEX_ARCHIVE_ARTICLE_ID_SIGNITURE);
if (pos == NULL)
	pos = strstr(file, ARTICLE_ID_SIGNITURE);
if (pos == NULL)
	exit(printf("Cannot find DOC id <name id=...> or <id> in file\n"));
while (!isdigit(*pos))
	pos++;

return atol(pos);
}

/*
	 GET_DOC_NAME()
	 --------------
 */
inline char *get_doc_name(char *file, char *to)
{
	char *pos, *start, *end;
	long len = 0;

	pos = strstr(file, "<"ARTICLE_NAME_SIGNITURE);
	if (pos == NULL)
		exit(printf("Cannot find DOC title <title> in file\n"));

	start = strchr(pos, '>');
	start++;
	end = strstr(file, "</"ARTICLE_NAME_SIGNITURE);
	len = end - start;
	strncpy(to, start, len);
	to[len] = '\0';

	return to;
}
/*
	 TO_LOWER()
	 ----------
 */
inline char *to_lower(char *source)
{
size_t i = 0;
for (; i < strlen(source); i++)
	if (source[i] >= 'A' && source[i] <= 'Z')
		source[i] = source[i] - 'A' + 'a';
return source;
}

#endif __LINK_PARTS_H__

