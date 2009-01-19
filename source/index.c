/*
	INDEX.C
	-------
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "disk.h"
#include "file.h"
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
long terms_in_document, first_param, trec_docnos;
ANT_memory file_buffer(1024 * 1024);
ANT_file id_list(&file_buffer);
char *filename, *uid_start, *uid_end;
char uid_buffer[1024];

if (argc < 2)
	exit(printf("Usage:%s [-docno] <filespec> ...\n-docno uses TREC <DOCNO> for document names (default: use filename and each document is in a seperate file).\n", argv[0]));
doc = 0;
terms_in_document = 0;
done_work = FALSE;
index = new ANT_memory_index;
id_list.open("doclist.aspt", "wb");

first_param = 1;
trec_docnos = FALSE;
if (strcmp(argv[1], "-docno") == 0)
	{
	first_param++;
	trec_docnos = TRUE;
	}
for (param = first_param; param < argc; param++)
	{
	now = stats.start_timer();
	file = (unsigned char *)disk.read_entire_file(filename = disk.get_first_filename(argv[param]));
	stats.add_disk_input_time(stats.stop_timer(now));
	while (file != NULL)
		{
		if (trec_docnos)
			{
			/*
				Find each and every document ID in the current file and write then to disk
			*/
			for (uid_start = strstr((char *)file, "<DOCNO>"); uid_start != NULL; uid_start = strstr(uid_end, "<DOCNO>"))
				{
				uid_start += 7;
				uid_end = strstr(uid_start, "</DOCNO>");
				if (uid_end - uid_start > sizeof(uid_buffer))
					{
					printf("UID longer than UID buffer, truncating at %d characters\n", sizeof(uid_buffer) - 1);
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
		terms_in_document = 0;
		delete [] file;
		now = stats.start_timer();
		file = (unsigned char *)disk.read_entire_file(filename = disk.get_next_filename());
		stats.add_disk_input_time(stats.stop_timer(now));
		}
	}

id_list.close();

now = stats.start_timer();
index->serialise("index.aspt");
stats.add_disk_output_time(stats.stop_timer(now));
delete index;

printf("\nTIMINGS\n-------\n");
stats.text_render();
return 0;
}

