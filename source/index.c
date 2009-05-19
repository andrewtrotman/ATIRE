/*
	INDEX.C
	-------
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "directory_recursive_iterator.h"
#include "file.h"
#include "parser.h"
#include "memory.h"
#include "memory_index.h"
#include "indexer_param_block.h"
#include "memory_index_stats.h"
#include "time_stats.h"
#include "version.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	REPORT()
	--------
*/
void report(long long doc, ANT_memory_index *index, ANT_time_stats *stats)
{
printf("%lld Documents in %lld bytes in ", doc, index->get_memory_usage());
stats->print_elapsed_time();
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_indexer_param_block param_block(argc, argv);
ANT_time_stats stats;
ANT_disk *disk;
ANT_parser parser;
ANT_string_pair *token;
unsigned char *file;
long param, done_work;
ANT_memory_index *index;
long long doc, now;
long terms_in_document, first_param;
ANT_memory file_buffer(1024 * 1024);
ANT_file id_list(&file_buffer);
char *filename, *uid_start, *uid_end;
char uid_buffer[1024];
long long files_that_match;

if (argc < 2)
	param_block.usage();

first_param = param_block.parse();

if (param_block.logo)
	puts(ANT_version_string);				// print the version string is we parsed the parameters OK

if (first_param >= argc)
	exit(0);				// no files to index so terminate

doc = 0;
terms_in_document = 0;
done_work = FALSE;
index = new ANT_memory_index;
id_list.open("doclist.aspt", "wb");

if (param_block.recursive)
	disk = new ANT_directory_recursive_iterator;
else
	disk = new ANT_directory_iterator;

index->set_compression_scheme(param_block.compression_scheme);
index->set_compression_validation(param_block.compression_validation);

for (param = first_param; param < argc; param++)
	{
	files_that_match = 0;
	now = stats.start_timer();
	file = (unsigned char *)disk->read_entire_file(filename = disk->first(argv[param]));
	stats.add_disk_input_time(stats.stop_timer(now));
	while (file != NULL)
		{
		files_that_match++;
		if (param_block.trec_docnos)
			{
			/*
				Find each and every document ID in the current file and write then to disk
			*/
			for (uid_start = strstr((char *)file, "<DOCNO>"); uid_start != NULL; uid_start = strstr(uid_end, "<DOCNO>"))
				{
				uid_start += 7;
				uid_end = strstr(uid_start, "</DOCNO>");
				if (uid_end - uid_start > (ptrdiff_t)sizeof(uid_buffer))
					{
					printf("UID longer than UID buffer, truncating at %ld characters\n", (signed long)sizeof(uid_buffer) - 1);
					uid_end = uid_start + sizeof(uid_buffer) - 1;
					}
				strncpy(uid_buffer, uid_start, uid_end - uid_start);
				uid_buffer[uid_end - uid_start] = '\0';
				strip_space_inplace(uid_buffer);
				id_list.puts(uid_buffer);
				}
			}
		else
			id_list.puts(filename);		// each document is in a seperate file (so filenames are external document ids)
		done_work = FALSE;
		doc++;
		if (doc % param_block.reporting_frequency == 0)
			report(doc, index, &stats);

		parser.set_document(file);
		while ((token = parser.get_next_token()) != NULL)
			{
			if (param_block.trec_docnos && token->length() == 3 && strncmp(token->start, "DOC", 3) == 0)
				{
				if (done_work)
					{
					index->set_document_length(doc, terms_in_document);
					doc++;
					if (doc % param_block.reporting_frequency == 0)
						report(doc, index, &stats);
					terms_in_document = 0;
					}
				}
			else if (ANT_isalnum(*token->start))
				{
				terms_in_document++;
				index->add_term(token, doc);
				done_work = TRUE;
				}
			}
		index->set_document_length(doc, terms_in_document);
		terms_in_document = 0;
		delete [] file;
		now = stats.start_timer();
		file = (unsigned char *)disk->read_entire_file(filename = disk->next());
		stats.add_disk_input_time(stats.stop_timer(now));
		}
	if (files_that_match == 0)
		printf("Warning: '%s' does not match any files\n", argv[first_param]);
	}

id_list.close();

now = stats.start_timer();
index->serialise("index.aspt");
stats.add_disk_output_time(stats.stop_timer(now));
index->text_render(param_block.statistics);
delete index;
delete disk;

if (param_block.statistics & ANT_indexer_param_block::STAT_TIME)
	{
	printf("\nTIMINGS\n-------\n");
	stats.text_render();
	}

return 0;
}

