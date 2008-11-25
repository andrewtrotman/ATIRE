#include <stdio.h>
#include <string.h>
#include "str.h"
#include "memory.h"
#include "ctypes.h"
#include "search_engine.h"
#include "search_engine_btree_leaf.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

static char *ANT_version_string = "Version 0.1 alpha";

/*
	SPECIAL_COMMAND()
	-----------------
*/
long special_command(char *command)
{
if (strcmp(command, ".quit") == 0)
	return FALSE;

return TRUE;
}

/*
	PERFORM_QUERY()
	---------------
*/
double perform_query(ANT_search_engine *search_engine, char *query, long topic_id = -1)
{
char token[1024];
char *token_start, *token_end;
long hits;

search_engine->init_accumulators();

token_end = query;

while (*token_end != '\0')
	{
	token_start = token_end;
	while (!ANT_isalpha(*token_start) && *token_start != '\0')
		token_start++;
	if (*token_start == '\0')
		break;
	token_end = token_start;
	while (ANT_isalpha(*token_end))
		token_end++;
	strncpy(token, token_start, token_end - token_start);
	token[token_end - token_start] = '\0';
	_strlwr(token);
	
	search_engine->process_one_search_term(token);
	}

search_engine->generate_results_list(&hits);
if (topic_id == -1)
	printf("Query '%s' found %d documents\n", query, hits);
else
	printf("Topic:%d Query '%s' found %d documents\n", topic_id, query, hits);

return 0.0;		// should be the Average Precision
}

/*
	COMMAND_DRIVEN_ANT()
	--------------------
*/
void command_driven_ant(void)
{
ANT_memory memory;
char query[1024];
long more;


printf("Ant %s\n", ANT_version_string);
puts("---");
puts("Copyright (c) 2008");
puts("Andrew Trotman, University of Otago");
puts("andrew@cs.otago.ac.nz");

ANT_search_engine search_engine(&memory);
printf("Index contains %d documents\n", search_engine.document_count());

puts("\nuse:\n\t.quit to quit\n\n");

more = TRUE;
while (more)
	{
	printf("]");
	if (fgets(query, sizeof(query), stdin) == NULL)
		more = FALSE;
	else
		{
		strip_end_punc(query);
		if (*query == '.')
			more = special_command(query);
		else
			perform_query(&search_engine, query);
		}
	}
puts("Bye");
}

/*
	BATCH_ANT()
	-----------
*/
void batch_ant(char *topic_file, char *qrel_file)
{
char query[1024];
long topic_id, line;
ANT_memory memory;
FILE *fp;
char *query_text;

fprintf(stderr, "Ant %s\n", ANT_version_string);
ANT_search_engine search_engine(&memory);
fprintf(stderr, "Index contains %d documents\n", search_engine.document_count());

if ((fp = fopen(topic_file, "rb")) == NULL)
	exit(fprintf(stderr, "Cannot open topic file:%s\n", topic_file));

line = 1;
while (fgets(query, sizeof(query), fp) != NULL)
	{
	strip_end_punc(query);
	topic_id = atol(query);
	if ((query_text = strchr(query, ' ')) == NULL)
		exit(printf("Line %d: Can't process query as badly formed:'%s'\n", line, query));

	perform_query(&search_engine, query_text, topic_id);
	line++;
	}
fclose(fp);

printf("Processed %d topics\n", line);
}


/*
	USAGE()
	-------
*/
void usage(char *exename)
{
printf("Usage:\n%s\nor\n", exename);
printf("%s <topic_file> <qrel_file>\n", exename);
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
if (argc == 1)
	command_driven_ant();
else if (argc == 3)
	batch_ant(argv[1], argv[2]);
else
	usage(argv[0]);

return 0;
}
