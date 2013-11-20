/*
	INEXIEEE_ASSESSMENTS_TO_ANT.C
	-----------------------------
	run through the INEX IEEE assessment files and generate a
	list of the relevant documents in a format suitable for 
	feeding into mkqrel

	Also now works on INEX 2006 Wikipedia assessments.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../source/disk.h"

#define FALSE 0
#define TRUE (!FALSE)

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
char *file_start, *file_end, **lines, **current, *file, *pos;
long long length;
long topic, done;

if ((file = ANT_disk::read_entire_file(argv[1])) == NULL)
	exit(printf("Cannot read INEX assessment file:%s\n", argv[1]));

lines = ANT_disk::buffer_to_list(file, &length);

for (current = lines; *current != NULL; current++)
	{
	if (strstr(*current, "<assessments") != NULL)
		{
		if ((pos = strstr(*current, "topic")) != NULL)
			topic = atol(strpbrk(pos, "1234567890"));				// WIKIPEDIA 2006
		else if (strchr(*current, '\"') != NULL)
			topic = atol(strpbrk(*current, "1234567890"));			// INEX 2002 and INEX 2003
		else
			{
			/*
				INEX 2004
			*/
			pos = strchr(*current, '\'');
			pos = strchr(pos + 1, '\'');
			topic = atol(strpbrk(pos, "1234567890"));
			}
		}
	else if (strstr(*current, "<file") != NULL)
		{
		if ((pos = strstr(*current, "name")) != NULL)		// INEX 2006
			{
			done = FALSE;
			file_start = strchr(pos, '\"');
			if (file_start == NULL)
				file_start = strchr(pos, '\'');

			file_start++;
			file_end = strchr(file_start, '\"');
			if (file_end == NULL)
				file_end = strchr(file_start, '\'');
			}
		else
			{							// INEX 2002, INEX 2003, INEX 2004
			done = FALSE;
			file_start = strchr(*current, '\"');
			if (file_start == NULL)
				file_start = strchr(*current, '\'');

			file_start++;
			file_end = strchr(file_start, '\"');
			if (file_end == NULL)
				file_end = strchr(file_start, '\'');
			}
		}
	else if ((strstr(*current, "relevance=\"1") != NULL) || (strstr(*current, "relevance=\"2") != NULL) || (strstr(*current, "relevance=\"3") != NULL))
		{
		/*
			INEX 2002
		*/
		if (!done)
			printf("%ld %*.*s\n", topic, (int)(file_end - file_start), (int)(file_end - file_start), file_start);
		done = TRUE;
		}
	else if ((strstr(*current, "exhaustiveness=\"1") != NULL) || (strstr(*current, "exhaustiveness=\"2") != NULL) || (strstr(*current, "exhaustiveness=\"3") != NULL))
		{
		/*
			INEX 2003
		*/
		if (!done)
			printf("%ld %*.*s\n", topic, (int)(file_end - file_start), (int)(file_end - file_start), file_start);
		done = TRUE;
		}
	else if ((strstr(*current, "exhaustiveness=\'1") != NULL) || (strstr(*current, "exhaustiveness=\'2") != NULL) || (strstr(*current, "exhaustiveness=\'3") != NULL))
		{
		/*
			INEX 2004
		*/
		if (!done)
			printf("%ld %*.*s\n", topic, (int)(file_end - file_start), (int)(file_end - file_start), file_start);
		done = TRUE;
		}

	else if ((strstr(*current, "exhaustivity=\"1") != NULL) || (strstr(*current, "exhaustivity=\"2") != NULL) || (strstr(*current, "exhaustivity=\"3") != NULL))
		{
		/*
			WIKIPEDIA 2006
		*/
		if (!done)
			printf("%ld %*.*s\n", topic, (int)(file_end - file_start), (int)(file_end - file_start), file_start);
		done = TRUE;
		}
	}
}
