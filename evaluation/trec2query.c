/*
	TREC2QUERY.C
	------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sstream>
#include "stop_word.h"

using namespace std;

char buffer[1024 * 1024];

const char *new_stop_words[] =
	{
//	"alternative",
	"arguments",
	"can",
	"current",
	"dangers",
	"data",
	"description",
	"developments",
	"document",
	"documents",
	"done",
	"discuss",
	"discusses",
	"efforts",
	"enumerate",
	"examples",
	"help",
	"ideas",
	"identify",
	"inform",
	"information",
	"instances",
	"latest",
	"method",
	"narrative",
	"occasions",
	"problems",
	"provide",
	"relevant",
	"report",
	"reports",
	"state",
	"topic",
	NULL
	} ;

#define FALSE 0
#define TRUE (!FALSE)


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

#ifndef _MSC_VER
/*
	STRLWR()
	--------
*/
void strlwr(char *source)
{
while (*source)
	{
	*source = tolower(*source);
	source++;
	}
}
void strlwr(const char *source) { strlwr((char *)source); }
#endif


/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
static const char *SEPERATORS = ",./;'[]!@#$%^&*()_+-=\\|<>?:{}\r\n\t \"`~";
ANT_stop_word stopper;
FILE *fp;
char *from, *ch, *tag;
long number, old_number, match, read;

if (argc != 3)
	exit(printf("Usage:%s <infile> <tag>\n<tag> is a combination of T,D,N (title, desc, narr)", argv[0]));

if ((fp = fopen(argv[1], "rb")) == NULL)
	exit(printf("Can't open file :%s\n", argv[1]));

tag = argv[2];
strlwr(tag);

stopper.addstop((const char **)new_stop_words);
old_number = number = -1;

read = TRUE;
while (1)
	{
	if (read)
		if (fgets(buffer, sizeof(buffer), fp) == NULL)
			break;

	read = TRUE;
	if (strncmp(buffer, "<num>", 5) == 0)
		number = atol(strchr(buffer, ':') + 1);
	else
		{
		match = FALSE;

		if (*buffer == '<')
			{
			if ((strncmp(buffer + 1, "title", 5) == 0) && (strchr(tag, 't') != NULL))
				match = TRUE;
			if ((strncmp(buffer + 1, "desc",  4) == 0) && (strchr(tag, 'd') != NULL))
				match = TRUE;
			if ((strncmp(buffer + 1, "narr",  4) == 0) && (strchr(tag, 'n') != NULL))
				match = TRUE;
			}
		if (match)
			{
			ostringstream query;

			query << strchr(buffer, '>') + 1;			// character after the "<desc>"
			while (fgets(buffer, sizeof(buffer), fp) != NULL)
				{
				if (*buffer == '<')
					{
					read = FALSE;
					break;
					}
				strip_space_inline(buffer);
				query << ' ' << buffer;		// <-
				}

			query << '\0';
			from = (char *)query.str().c_str();
			strlwr(from);
			for (ch = from; *ch != '\0'; ch++)
				if (isspace(*ch))
					*ch = ' ';

			strip_space_inline(from);

			if (number >= 0)
				{
				if (number != old_number)
					{
					printf("\n%ld", number);
					old_number = number;
					}
				for (ch = strtok(from, SEPERATORS); ch != NULL; ch = strtok(NULL, SEPERATORS))
					{
					if (!stopper.isstop(ch))
						if (strlen(ch) > 2)			// stop words 2 characters or less
							if (!isdigit(*ch))		// doesn't contain a number
								printf(" %s", ch);
					}
				}
			else
				exit(printf("\nBroken file\n"));
			}
		}
	}
printf("\n");
fclose(fp);
return 0;
}
