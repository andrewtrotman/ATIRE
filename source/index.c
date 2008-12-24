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
#include "time_stats.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_time_stats stats;
ANT_disk disk;
ANT_parser parser;
ANT_string_pair *token;
unsigned char *file;
long param, done_work;
ANT_memory_index *index;
long long doc, now;
long terms_in_document;

if (argc < 2)
	exit(printf("Usage:%s <filespec> ...\n", argv[0]));
doc = 0;
terms_in_document = 0;
done_work = FALSE;
index = new ANT_memory_index;
for (param = 1; param < argc; param++)
	{
	now = stats.start_timer();
	file = (unsigned char *)disk.read_entire_file(disk.get_first_filename(argv[param]));
	stats.add_disk_input_time(stats.stop_timer(now));
	while (file != NULL)
		{
		done_work = FALSE;
		doc++;
		if (doc % 10000 == 0)
			{
			printf("Documents Indexed:%lld Memory used:%lld", doc, index->memory->used);
			stats.print_elapsed_time();
			printf("\n");
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
		now = stats.start_timer();
		file = (unsigned char *)disk.read_entire_file(disk.get_next_filename());
		stats.add_disk_input_time(stats.stop_timer(now));
		}
	}

now = stats.start_timer();
index->serialise("index.aspt");
stats.add_disk_output_time(stats.stop_timer(now));
delete index;

printf("\nTIMINGS\n-------\n");
stats.text_render();
return 0;
}

