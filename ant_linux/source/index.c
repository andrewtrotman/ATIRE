/*
	INDEX.C
	-------
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "disk.h"
#include "parser.h"
#include "memory.h"
#include "memory_index.h"
#include "memory_index_stats.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE 1
#endif

/*
	PRINT_ELAPSED_TIME()
	--------------------
*/
void print_elapsed_time(const char *message, long long time_taken)
{
if (time_taken > 60)
	printf("%s%lld:%lld:%lld\n", message, (time_taken / (60 * 60)), (time_taken / 60) % 60, time_taken % 60);
else if (time_taken > 60)
	printf("%s%lld:%lld\n", message, time_taken / 60, time_taken % 60);
else if (time_taken > 1)
	printf("%s%lld seconds\n", message, time_taken);
else if (time_taken == 1)
	printf("%s%lld second\n", message, time_taken);
else
	printf("%s<1 second\n", message);
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_memory_index_stats stats;
ANT_disk disk;
ANT_parser parser;
ANT_string_pair *token;
unsigned char *file;
long param, done_work;
ANT_memory_index *index;
long long doc;
long long frequency, program_start_time, program_end_time;
long long now, input_time, output_time;
long terms_in_document;

program_start_time = stats.get_clock_tick();
frequency = stats.get_clock_tick_frequency();

if (argc < 2)
	exit(printf("Usage:%s <filespec> ...\n", argv[0]));
input_time = doc = 0;
terms_in_document = 0;
done_work = FALSE;
index = new ANT_memory_index;
for (param = 1; param < argc; param++)
	{
	puts("--");
	puts(argv[param]);
	index->stats->text_render();

	now = stats.get_clock_tick();
	file = (unsigned char *)disk.read_entire_file(disk.get_first_filename(argv[param]));
	input_time += stats.get_clock_tick() - now;
	while (file != NULL)
		{
		done_work = FALSE;
		doc++;
		if (doc % 10000 == 0)
			{
			printf("Documents Indexed:%lld Memory used:%lld\n", doc, index->memory->used);
			program_end_time = stats.get_clock_tick();
			print_elapsed_time("Total Elapsed Time:", (program_end_time - program_start_time) / frequency);
			}
			

		parser.set_document(file);
		while ((token = parser.get_next_token()) != NULL)
			{
			if (ANT_isalpha(*token->start))
				{
				terms_in_document++;
				index->add_term(token, doc);
				done_work = TRUE;
				}
			else if (token->length() == 5 && strncmp(token->start, "<DOC>", 5) == 0)
				if (done_work)
					{
					index->set_document_length(doc, terms_in_document);
					doc++;
					terms_in_document = 0;
					}
			}
		index->set_document_length(doc, terms_in_document);
		delete [] file;
		now = stats.get_clock_tick();
		file = (unsigned char *)disk.read_entire_file(disk.get_next_filename());
		input_time += stats.get_clock_tick() - now;
		}
	}

output_time = 0;
now = stats.get_clock_tick();
index->serialise("index.aspt");
output_time = stats.get_clock_tick() - now;
delete index;

program_end_time = stats.get_clock_tick();
printf("\nTIMINGS\n-------\n");
print_elapsed_time("Total Elapsed Time:", (program_end_time - program_start_time) / frequency);
print_elapsed_time("Time Reading Files:", input_time / frequency);
print_elapsed_time("Time Indexing     :", ((program_end_time - program_start_time) - input_time - output_time) / frequency);
print_elapsed_time("Time Writing Files:", output_time / frequency);
}