/*
	TOPIC_TREE.C
	------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../source/disk.h"

#define MAX_TERMS_PER_QUERY 20
#define MAX_QUERY_LENGTH 1024

static char *SEPERATORS = " []()\"";

/*
	class ANT_QUERY
	---------------
*/
class ANT_query
{
public:
	long terms_in_query;
	char term_buffer[MAX_QUERY_LENGTH];
	char *term[MAX_TERMS_PER_QUERY];
} *term_list;

long term_list_length;

/*
	FIND_LINKS()
	------------
*/
void find_links(long from)
{
long current;
char **current_term, **term;
long *simalarity_list;

simalarity_list = new long [term_list_length];
memset(simalarity_list, 0, sizeof(*simalarity_list) * term_list_length);

for (term = term_list[from].term; *term != NULL; term++)
	for (current = 0; current < term_list_length; current++)
		if (current != from)
			for (current_term = term_list[current].term; *current_term != NULL; current_term++)
				if (strcmp(*term, *current_term) == 0)
					simalarity_list[current]++;

for (current = 0; current < term_list_length; current++)
	if (simalarity_list[current] != 0)
		printf("%d %d %d w %d\n", from + 1, current + 1, simalarity_list[current], simalarity_list[current]);

delete [] simalarity_list;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_disk disk;
char *file, **line_list;
char *term;
char **search_term;
long lines, current_term, current;

file = disk.read_entire_file(argv[1]);
line_list = disk.buffer_to_list(file, &lines);

term_list = new ANT_query[lines];
for (current = 0; current < lines; current++)
	{
	if (strlen(line_list[current]) >= MAX_QUERY_LENGTH)
		exit(printf("Line %d: Query too long (exceeds %d chars)\n", current, MAX_QUERY_LENGTH));
	strcpy(term_list[current].term_buffer, line_list[current]);
	current_term = 0;
	for (term = strtok(line_list[current], SEPERATORS); term != NULL; term = strtok(NULL, SEPERATORS))
		{
		if (current_term > MAX_TERMS_PER_QUERY)
			exit(printf("Line %d: Too many search terms (exceeds %d)\n", current, current_term));
		term_list[current].term[current_term] = term;
		current_term++;
		}
	term_list[current].terms_in_query = current_term;
	term_list[current].term[current_term] = NULL;
	}
term_list_length = current;

/*
	Node list
*/
printf("*Vertices %d\n", lines);
for (current = 0; current < lines; current++)
	printf("%d \"%d\" x_fact %d y_fact %d\n", current + 1, current + 1, term_list[current].terms_in_query, term_list[current].terms_in_query);

printf("*Arcs\n");
for (current = 0; current < lines; current++)
	find_links(current);

return 0;
}
