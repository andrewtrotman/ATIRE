/*
	INDEX.C
	-------
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "ranking_function_factory.h"
#include "directory_iterator_tar.h"
#include "directory_recursive_iterator.h"
#include "directory_iterator_multiple.h"
#include "directory_iterator_file.h"
#include "file.h"
#include "parser.h"
#include "parser_readability.h"
#include "readability_factory.h"
#include "memory.h"
#include "memory_index.h"
#include "indexer_param_block.h"
#include "memory_index_stats.h"
#include "time_stats.h"
#include "version.h"
#include "instream_file.h"
#include "instream_deflate.h"
#include "instream_bz2.h"
#include "instream_buffer.h"

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
void report(long long doc, ANT_memory_index *index, ANT_time_stats *stats, long long bytes_indexed)
{
printf("%lld Documents (%lld bytes) in %lld bytes of memory in ", doc, bytes_indexed, index->get_memory_usage());
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
ANT_directory_iterator *source = NULL;
#ifdef PARALLEL_INDEX
ANT_directory_iterator_multiple *disk = NULL;
ANT_instream_buffer *instream_file_buffer;
#else
ANT_directory_iterator *disk = NULL;
#endif
ANT_parser *parser;
ANT_readability_factory *readability;
ANT_string_pair *token;
unsigned char *file, *new_file;
long param;
ANT_memory_index *index;
long long doc, now, last_report;
long terms_in_document, first_param;
ANT_memory file_buffer(1024 * 1024);
ANT_file id_list(&file_buffer);
char *filename;
long long files_that_match;
long long bytes_indexed, current_file_length;
ANT_instream *file_stream = NULL, *decompressor = NULL;

if (argc < 2)
	param_block.usage();

first_param = param_block.parse();

if (param_block.logo)
	puts(ANT_version_string);				// print the version string is we parsed the parameters OK

if (first_param >= argc)
	exit(0);				// no files to index so terminate

last_report = 0;
doc = 0;
terms_in_document = 0;
index = new ANT_memory_index;
id_list.open("doclist.aspt", "wb");

index->set_compression_scheme(param_block.compression_scheme);
index->set_compression_validation(param_block.compression_validation);

if (param_block.readability_measure == ANT_readability_factory::NONE)
	parser = new ANT_parser(param_block.segmentation);
else
	parser = new ANT_parser_readability();

readability = new ANT_readability_factory;
readability->set_measure(param_block.readability_measure);
readability->set_parser(parser);

/*
	The first parameter that is not a command line switch is the start of the list of files to index
*/
#ifdef PARALLEL_INDEX
	disk = new ANT_directory_iterator_multiple;
#endif
current_file_length = bytes_indexed = 0;

for (param = first_param; param < argc; param++)
	{
#ifdef PARALLEL_INDEX
#else
	delete disk;
	delete file_stream;
	delete decompressor;
#endif
	if (param_block.recursive == ANT_indexer_param_block::DIRECTORIES)
		source = new ANT_directory_recursive_iterator;						// this dir and below
	else if (param_block.recursive == ANT_indexer_param_block::TAR_BZ2)
		{
		file_stream = new ANT_instream_file(&file_buffer, argv[param]);
		decompressor = new ANT_instream_bz2(&file_buffer, file_stream);
#ifdef PARALLEL_INDEX
		instream_file_buffer = new ANT_instream_buffer(&file_buffer, decompressor);
		source = new ANT_directory_iterator_tar(instream_file_buffer);
#else
		source = new ANT_directory_iterator_tar(decompressor);
#endif
		}
	else if (param_block.recursive == ANT_indexer_param_block::TAR_GZ)
		{
		file_stream = new ANT_instream_file(&file_buffer, argv[param]);
		decompressor = new ANT_instream_deflate(&file_buffer, file_stream);
		source = new ANT_directory_iterator_tar(decompressor);
		}
	else if (param_block.trec_docnos)
		source = new ANT_directory_iterator_file(ANT_disk::read_entire_file(argv[param]));
	else
		source = new ANT_directory_iterator;									// current directory

#ifdef PARALLEL_INDEX
	disk->add_iterator(source);
	}


	{
#else
	disk = source;
#endif
	files_that_match = 0;
	now = stats.start_timer();
	current_file_length = 0;
	if ((filename = disk->first(argv[param])) == NULL)
		new_file = NULL;
	else
		new_file = (unsigned char *)disk->read_entire_file(&current_file_length);
	stats.add_disk_input_time(stats.stop_timer(now));
	file = new_file;
	bytes_indexed += current_file_length;
	while (file != NULL)
		{
		#pragma omp parallel sections
			{
			#pragma omp section
				{
				files_that_match++;
				doc++;
				if (doc % param_block.reporting_frequency == 0 && doc != last_report)
					report(last_report = doc, index, &stats, bytes_indexed);

				readability->set_document(file);
				while ((token = readability->get_next_token()) != NULL)
					{
					if (!ANT_isupper(token->start[0]))			// uppercase words are XML tags
						terms_in_document++;
					readability->handle_node(index->add_term(token, doc));
					}
				if (terms_in_document == 0)
					doc--;
				else
					{
					index->set_document_length(doc, terms_in_document);
					readability->index(index);
					id_list.puts(filename);		// save the "external" ID of the document
					}
				terms_in_document = 0;
				delete [] file;
				}

			#pragma omp section
				{
				now = stats.start_timer();
				current_file_length = 0;
				if ((filename = disk->next()) == NULL)
					new_file = NULL;
				else
					new_file = (unsigned char *)disk->read_entire_file(&current_file_length);
				stats.add_disk_input_time(stats.stop_timer(now));
				}
			}
		file = new_file;
		bytes_indexed += current_file_length;
		}
	if (files_that_match == 0)
		printf("Warning: '%s' does not match any files\n", argv[param]);
	}

id_list.close();
now = stats.start_timer();
index->serialise("index.aspt", &param_block);
stats.add_disk_output_time(stats.stop_timer(now));
index->text_render(param_block.statistics);
delete index;
delete disk;
delete parser;
delete readability;

if (param_block.statistics & ANT_indexer_param_block::STAT_TIME)
	{
	printf("\nTIMINGS\n-------\n");
	stats.text_render();
	}

return 0;
}

