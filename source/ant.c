#include <stdio.h>
#include <string.h>
#include "str.h"
#include "memory.h"
#include "ctypes.h"
#include "search_engine.h"
#include "search_engine_btree_leaf.h"
#include "mean_average_precision.h"
#include "disk.h"
#include "relevant_document.h"

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
double perform_query(ANT_search_engine *search_engine, char *query, long topic_id = -1, ANT_mean_average_precision *map = NULL)
{
char token[1024];
char *token_start, *token_end;
long hits;
ANT_search_engine_accumulator *ranked_list;
double average_precision = 0.0;

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

ranked_list = search_engine->generate_results_list(&hits);
if (topic_id == -1)
	printf("Query '%s' found %d documents\n", query, hits);
else
	{
	printf("Topic:%d Query '%s' found %d documents\n", topic_id, query, hits);
	average_precision = map->average_precision(topic_id, ranked_list, search_engine->document_count());
	}

return average_precision;
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
	GET_QRELS()
	-----------
	This is highly inefficient, but because it only happens once that's OK.
*/
ANT_relevant_document *get_qrels(ANT_memory *memory, char *qrel_file, long *qrel_list_length)
{
ANT_disk file_system;
ANT_relevant_document *all_assessments, *current_assessment;
FILE *qrel_fp;
char *entire_file, *ch;
long lines;
char text[80];

if ((entire_file = file_system.read_entire_file(qrel_file)) == NULL)
	exit(fprintf(stderr, "Cannot read qrel file:%s\n", qrel_file));

lines = 0;
for (ch = entire_file; *ch != '\0'; ch++)
	if (*ch == '\n')
		lines++;

current_assessment = all_assessments = (ANT_relevant_document *)memory->malloc(sizeof(*all_assessments) * lines);

if ((qrel_fp = fopen(qrel_file, "rb")) == NULL)
	exit(fprintf(stderr, "Cannot open topic file:%s\n", qrel_file));

while (fgets(text, sizeof(text), qrel_fp) != NULL)
	{
	if ((sscanf(text, "%d %d", &current_assessment->topic, &current_assessment->docid)) != 2)
		exit(printf("%s line %d:Cannot extract '<queryid> <docid>'", qrel_file, current_assessment - all_assessments));
	current_assessment++;
	}

fclose(qrel_fp);

*qrel_list_length = lines;
return all_assessments;
}

/*
	BATCH_ANT()
	-----------
*/
void batch_ant(char *topic_file, char *qrel_file)
{
ANT_relevant_document *assessments;
char query[1024];
long topic_id, line, number_of_assessments;
ANT_memory memory;
FILE *fp;
char *query_text;
double average_precision, sum_of_average_precisions, mean_average_precision;

fprintf(stderr, "Ant %s Copyright (c) 2008 Andrew Trotman, University of Otago\n", ANT_version_string);
ANT_search_engine search_engine(&memory);
fprintf(stderr, "Index contains %d documents\n", search_engine.document_count());

assessments = get_qrels(&memory, qrel_file, &number_of_assessments);
ANT_mean_average_precision map(&memory, assessments, number_of_assessments);

if ((fp = fopen(topic_file, "rb")) == NULL)
	exit(fprintf(stderr, "Cannot open topic file:%s\n", topic_file));

sum_of_average_precisions = 0.0;
line = 1;
while (fgets(query, sizeof(query), fp) != NULL)
	{
	strip_end_punc(query);
	topic_id = atol(query);
	if ((query_text = strchr(query, ' ')) == NULL)
		exit(printf("Line %d: Can't process query as badly formed:'%s'\n", line, query));

	average_precision = perform_query(&search_engine, query_text, topic_id, &map);
	sum_of_average_precisions += average_precision;
	fprintf(stderr, "Topic:%d Average Precision:%f\n", topic_id, average_precision);
	line++;
	}
fclose(fp);

mean_average_precision = sum_of_average_precisions / (double) (line - 1);
printf("Processed %d topics (MAP:%f)\n", line - 1, mean_average_precision);
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
