/*
	LINK_PARTS.H
	------------
*/

#ifndef __LINK_PARTS_H__
#define __LINK_PARTS_H__

/*
	STRING_CLEAN()
	--------------
*/
inline static char *string_clean(char *file)
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
		ch++;
	}

/*
	now remove multiple spaces.
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
inline static char *strnnew(char *source, long length)
{
char *ans;

ans = new char [length + 1];
strncpy(ans, source, length);
ans[length] = '\0';

return ans;
}

#endif __LINK_PARTS_H__
