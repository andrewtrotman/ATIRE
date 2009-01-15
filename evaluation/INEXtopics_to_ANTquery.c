/*
	INEXTOPICS_TO_ANTQUERY.C
	------------------------
*/
#include "/ant/source/str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buffer[1024 * 1024];
char title[1024 * 1024];

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
FILE *fp;
char *title_pos, *first_char, *into;
long topic;

if (argc != 2)
	exit(printf("Usage:%s\n", argv[0]));

if ((fp = fopen(argv[1], "rb")) == NULL)
	exit(printf("Cannot open INEX topic file:%s\n", argv[1]));

into = title;
while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
	strip_end_punc(buffer);
	if (strstr(buffer, "<topic") != NULL)
		{
		into = title;
		topic = atol(strstr(buffer, "id=") + 4);
		}
	else if ((title_pos = strstr(buffer, "<title>")) != NULL)
		{
		first_char = title_pos + 7;
		if ((title_pos = strstr(buffer, "</title>")) != NULL)
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
		printf("%d %s\n", topic, title);
		}
	}
}

