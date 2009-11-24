/*
	LINK_INDEX_MERGE.C
	------------------
	Written (w) 2008 by Andrew Trotman, University of Otago
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../source/str.h"

char text_buffer[1024 * 1024];
char index_buffer[1024 * 1024];
char head_word[1024];

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
long lines, cmp, cf, df;
char *postings, *colon;
FILE *anchor_index, *anchor_text;

if (argc != 3)
	exit(printf("Usage:%s <anchor_index.idx> <anchor_text.idx>\n", argv[0]));

if ((anchor_index = fopen(argv[1], "rb")) == NULL)
	exit(printf("Cannot open anchor_index file:%s\n", argv[1]));

if ((anchor_text = fopen(argv[2], "rb")) == NULL)
	exit(printf("Cannot open anchor_index file:%s\n", argv[1]));

fgets(text_buffer, sizeof(text_buffer), anchor_text);
printf(text_buffer);
lines = atol(text_buffer);
while (fgets(text_buffer, sizeof(text_buffer), anchor_text) != NULL)
	{
	postings = strchr(text_buffer, ':');
	strncpy(head_word,text_buffer, postings - text_buffer);
	head_word[postings - text_buffer] = '\0';

	if ((fgets(index_buffer, sizeof(index_buffer), anchor_index)) == NULL)
		{
		fprintf(stderr, "missing line:%s", text_buffer);
		break;
		}
	colon = strrchr(index_buffer, ':') + 1;

//printf("Compare (%d bytes) '%s' to '%s'\n", strlen(head_word), colon, head_word);
	if ((cmp = strncmp(colon, head_word, strlen(head_word))) == 0)		// this includes the colon
		{
		sscanf(index_buffer, "%d:%d", &df, &cf);
		printf("%s:[%d,%d]%s", head_word, df, cf, postings);
		}
	else
		fprintf(stderr, "missing line:%s", text_buffer);
	}

fclose(anchor_index);
fclose(anchor_text);

fprintf(stderr, "%s Completed\n", argv[0]);
return 0;
}
