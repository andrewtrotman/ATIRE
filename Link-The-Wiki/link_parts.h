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

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

#pragma warning(disable:4996)

/*
	STRIP_SPACE_INLINE()
	--------------------
*/
char *strip_space_inline(char *source)
{
char *end, *start = source;

while (isspace(*start))
	start++;

if (start > source)
	memmove(source, start, strlen(start) + 1);		// copy the '\0'

end = source + strlen(source) - 1;
while ((end >= source) && (isspace(*end)))
	*end-- = '\0';

return source;
}

/*
	STRING_CLEAN()
	--------------
*/
inline static char *string_clean(char *file, long lower_case_only, long keep_string_intact = 0, long trim = 1)
{
char *ch;
//char *from, *to;

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
//if (trim)
//	{
//	from = to = file;
//	while (isspace(*from))
//		from++;
//	while (*from != '\0')
//		{
//		while (isalnum(*from))
//			*to++ = *from++;
//		if (isspace(*from))
//			*to++ = *from++;
//		while (isspace(*from))
//			from++;
//		}
//	if (to > file && isspace(*(to - 1)))
//		to--;
//	*to = '\0';
//	}
// use the new trimming function instead because the original one won't work for the non-alphabet characters
if (trim)
	strip_space_inline(file);

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
	{
	fprintf(stderr, "Cannot find DOC id <name id=...> or <id> in file\n");
	return -1;
	//exit(printf("Cannot find DOC id <name id=...> or <id> in file\n"));
	}
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

/*
 	STRING_REMOVE_SPACE()
	--------------------
*/
void string_remove_space(char *s)
{
char *last, *previous;
//char *temp;
long len, has_space;
has_space = FALSE;
last = s + strlen(s);
while (last != s)
	{
	previous = last - 1;
	if (*previous == ' ')
		{
		has_space = TRUE;
		while (*previous == ' ' && previous >= s)
			--previous;
		++previous;
		}
	if (has_space)
		{
		len = strlen(last);
		memmove(previous, last, len);
		previous[len] = '\0';
		has_space = FALSE;
		}

	last = previous;
	}
}

/*
	UTF8_TOKEN_COMPARE()
	--------------------
*/
int utf8_token_compare(char *s1, char *s2, long *is_substring = NULL)
{
int cmp, min_len;
char *new_s1 = strdup(s1);
char *new_s2 = strdup(s2);

if (strchr(new_s1, ' ') != NULL)
	string_remove_space(new_s1);

if (strchr(new_s2, ' ') != NULL)
	string_remove_space(new_s2);

min_len = MIN(strlen(new_s1), strlen(new_s2));

cmp = memcmp(new_s1, new_s2, min_len);

if (cmp == 0)
	{
	if (strlen(new_s1) <= strlen(new_s2))
		{
		if (is_substring != NULL)
			*is_substring = TRUE;
		if (strlen(new_s1) < strlen(new_s2))
			cmp = -1;
		}
	else
		cmp = 1;
	}

free(new_s1);
free(new_s2);
return cmp;
}

#endif // __LINK_PARTS_H__

