/*
	INEXTOPICS_TO_ANTQUERY.C
	------------------------
	Convert an INEX 2008 / 2009 topic file into an ANT query file
*/
#include "../source/str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char buffer[1024 * 1024];
char title[1024 * 1024];

/*
	CLEAN_SPACES_INLINE()
	---------------------
*/
char *clean_spaces_inline(char *title)
{
char *from, *to;

from = to = title;
while (*from != '\0')
	{
	if (*from == ' ')
		if (*(from + 1) == ' ')
			from++;
		else
			*to++ = *from++;
	else
		*to++ = *from++;
	}
*to = '\0';

return title;
}

/*
	CAS_CLEAN()
	-----------
*/
char *cas_clean(char *title)
{
char *pos;
long in_square = 0;
long in_round = 0;
long in_about = 0;
long comma = 0;

for (pos = title; *pos != '\0'; pos++)
	{
	if (*pos == '[')
		in_square++;
	else if (*pos == ']')
		in_square--;
	else if (*pos == '(')
		in_round++;
	else if (*pos == ')')
		{
		in_round--;
		if (comma != 0 && in_about != 0)
			in_about = 0;
		}
	else if (*pos == ',')
		comma++;

	if (in_square != 0 && in_about == 0)
		{
		if (strncmp(pos, "about", 5) == 0)
			{
			memset(pos, ' ', 5);
			pos += 5;
			in_about++;
			comma = 0;
			}
		else
			*pos = ' ';
		}

	if (in_square != 0 && in_about != 0 && comma == 0)
		{										// support elements
		if (isalnum(*pos) || *pos == '_')
			*pos = toupper(*pos);
		else
			*pos = ' ';
		}
	else if (in_square == 0)					// target elements
		{
		if (isalnum(*pos) || *pos == '_')
			*pos = toupper(*pos);
		else
			*pos = ' ';
		}
	else										// search terms
		{
		if (isupper(*pos))
			*pos = tolower(*pos);
		if (!isalnum(*pos))
			*pos = ' ';
		}
	}

return clean_spaces_inline(title); 
}

/*
	CO_CLEAN()
	----------
*/
char *co_clean(char *title)
{
char *pos;

for (pos = title; *pos != '\0'; pos++)
	{
	if (isupper(*pos))
		*pos = tolower(*pos);
	if (!isalnum(*pos))
		*pos = ' ';
	}

return clean_spaces_inline(title); 
}


/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
FILE *fp;
char *title_pos, *first_char, *into, *filename, *pos;
long topic, parameter;
bool clean = false, cas = false;
char *TITLE = "<title>";
char *SLASH_TITLE = "</title>";
char *answer;

if (argc <= 1 || argc > 4)
	exit(printf("Usage:%s [-clean] [-cas] <topic_file (INEX 2008/2009 format)>\n", argv[0]));

for (parameter = 1; parameter < argc; parameter++)
	{
	if (strcmp(argv[parameter], "-clean") == 0)
		clean = true;
	else if (strcmp(argv[parameter], "-cas") == 0)
		{
		TITLE = "<castitle>";
		SLASH_TITLE = "</castitle>";
		cas = true;
		}
	else if (argv[parameter][0] == '-')
		exit(printf("Unrecognised parameter:%s\n", argv[parameter]));
	else
		{
		filename = argv[parameter];
		break;
		}
	}

if ((fp = fopen(filename, "rb")) == NULL)
	exit(printf("Cannot open INEX topic file:%s\n", argv[1]));

into = title;
while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
	strip_end_punc(buffer);
	if (strstr(buffer, "<topic") != NULL)
		{
		into = title;
		//the new topic id format for 2009 efficiency track is id="2009-Eff-228"
		//topic = atol(strstr(buffer, "id=\"2009-Eff-") + 13);
		topic = atol(strstr(buffer, "id=") + 4);
		}
	else if ((title_pos = strstr(buffer, TITLE)) != NULL)
		{
		first_char = title_pos + strlen(TITLE);
		if ((title_pos = strstr(buffer, SLASH_TITLE)) != NULL)
			{
			strncpy(title, first_char, title_pos - first_char);
			title[title_pos - first_char] = '\0';
			}
		else
			{
			strcpy(title, first_char);
			into = title + strlen(first_char);
			while (fgets(buffer, sizeof(buffer), fp) != NULL)
				{
				strip_end_punc(buffer);
				if ((title_pos = strstr(buffer, "</title>")) == NULL)
					{
					strcpy(into, buffer);
					into += strlen(buffer);
					*into++ = ' ';
					*into = '\0';
					}
				else
					{
					strncpy(into, buffer, title_pos - buffer);
					break;
					}
				}
			}
		if (clean)
			answer = cas ? cas_clean(title) : co_clean(title);
		else
			answer = title;

		printf("%d %s\n", topic, title);
		}
	}
}

