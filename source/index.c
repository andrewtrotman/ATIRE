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
#include "directory_iterator_object.h"
#include "directory_iterator_pkzip.h"
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
ANT_directory_iterator *disk = NULL;
ANT_parser *parser;
ANT_readability_factory *readability;
ANT_string_pair *token;
long param;
ANT_memory_index *index;
long long doc, now, last_report;
long terms_in_document, first_param;
ANT_memory file_buffer(1024 * 1024);
ANT_file id_list(&file_buffer);
long long files_that_match;
long long bytes_indexed;
ANT_instream *file_stream = NULL, *decompressor = NULL, *instream_buffer = NULL;
ANT_directory_iterator_object file_object, *current_file;
ANT_directory_iterator_multiple *parallel_disk;

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
index = new ANT_memory_index("index.aspt");
id_list.open("doclist.aspt", "wb");

index->set_document_compression_scheme(param_block.document_compression_scheme);
index->set_compression_scheme(param_block.compression_scheme);
index->set_compression_validation(param_block.compression_validation);

if (param_block.readability_measure == ANT_readability_factory::NONE)
	parser = new ANT_parser(param_block.segmentation);
else
	parser = new ANT_parser_readability();

readability = new ANT_readability_factory;
readability->set_measure(param_block.readability_measure);
readability->set_parser(parser);

#ifdef PARALLEL_INDEXING
	parallel_disk = new ANT_directory_iterator_multiple;
#endif
/*
	The first parameter that is not a command line switch is the start of the list of files to index
*/
bytes_indexed = 0;

for (param = first_param; param < argc; param++)
	{
#ifdef PARALLEL_INDEXING
#else
	delete disk;
	delete file_stream;
	delete decompressor;
	delete instream_buffer;
#endif
	if (param_block.recursive == ANT_indexer_param_block::DIRECTORIES)
		source = new ANT_directory_recursive_iterator(argv[param]);			// this dir and below
	else if (param_block.recursive == ANT_indexer_param_block::TAR_BZ2)
		{
		file_stream = new ANT_instream_file(&file_buffer, argv[param]);
		decompressor = new ANT_instream_bz2(&file_buffer, file_stream);
		instream_buffer = new ANT_instream_buffer(&file_buffer, decompressor);
		source = new ANT_directory_iterator_tar(instream_buffer);
		}
	else if (param_block.recursive == ANT_indexer_param_block::TAR_GZ)
		{
		file_stream = new ANT_instream_file(&file_buffer, argv[param]);
		decompressor = new ANT_instream_deflate(&file_buffer, file_stream);
		instream_buffer = new ANT_instream_buffer(&file_buffer, decompressor);
		source = new ANT_directory_iterator_tar(instream_buffer);
		}
	else if (param_block.recursive == ANT_indexer_param_block::TREC)
		source = new ANT_directory_iterator_file(ANT_disk::read_entire_file(argv[param]));
	else if (param_block.recursive == ANT_indexer_param_block::PKZIP)
		source = new ANT_directory_iterator_pkzip(argv[param]);
	else
		source = new ANT_directory_iterator(argv[param]);					// current directory
#ifdef PARALLEL_INDEXING
	parallel_disk->add_iterator(source);
	}
	disk = parallel_disk;
	files_that_match = 0;

	now = stats.start_timer();
	current_file = disk->first(&file_object, ANT_directory_iterator::READ_FILE);
	stats.add_disk_input_time(stats.stop_timer(now));
	{
#else
	disk = source;
	files_that_match = 0;

	now = stats.start_timer();
	current_file = disk->first(&file_object, argv[param], ANT_directory_iterator::READ_FILE);
	stats.add_disk_input_time(stats.stop_timer(now));
#endif

	while (current_file != NULL)
		{
		/*
			How much data do we have?
		*/
		files_that_match++;
		doc++;
		bytes_indexed += current_file->length;

		/*
			Report
		*/
		if (doc % param_block.reporting_frequency == 0 && doc != last_report)
			report(last_report = doc, index, &stats, bytes_indexed);

		/*
			Index the file
		*/
		readability->set_document((unsigned char *)current_file->file);
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

			/*
				Store the document in the repository
			*/
			if (param_block.document_compression_scheme != ANT_indexer_param_block::NONE)
				{
//				index->add_to_document_repository(NULL, current_file->file, current_file->length + 1);		// +1 so that we also get the '\0'
				index->add_to_document_repository(current_file->filename, current_file->file, current_file->length + 1);		// +1 so that we also get the '\0'
				}
			id_list.puts(current_file->filename);
			}
		terms_in_document = 0;
		delete [] current_file->file;

		/*
			Get the next file
		*/
		now = stats.start_timer();
		current_file = disk->next(&file_object, ANT_directory_iterator::READ_FILE);
		stats.add_disk_input_time(stats.stop_timer(now));
		}

#ifdef NEVER
	if (files_that_match == 0)
		printf("Warning: '%s' does not match any files\n", argv[param]);
#endif
	}

if (param_block.reporting_frequency != LLONG_MAX && doc != last_report)
	report(doc, index, &stats, bytes_indexed);

if (doc == 0)
	puts("No documents indexed (check your file list)");
else
	{
	id_list.close();
	now = stats.start_timer();
	index->serialise(&param_block);
	stats.add_disk_output_time(stats.stop_timer(now));
	index->text_render(param_block.statistics);
	}
delete index;
delete disk;
delete parser;
delete readability;
delete file_stream;
delete decompressor;
delete instream_buffer;

if (param_block.statistics & ANT_indexer_param_block::STAT_TIME)
	{
	printf("\nTIMINGS\n-------\n");
	stats.text_render();
	}

return 0;
}

