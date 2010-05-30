/*
	INDEX.C
	-------
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "ranking_function_factory.h"
#include "directory_iterator_tar.h"
#include "directory_iterator_warc.h"
#include "directory_iterator_recursive.h"
#include "directory_iterator_multiple.h"
#include "directory_iterator_compressor.h"
#include "directory_iterator_file.h"
#include "directory_iterator_object.h"
#include "directory_iterator_pkzip.h"
#include "directory_iterator_preindex.h"
#include "file.h"
#include "parser.h"
#include "parser_readability.h"
#include "readability_factory.h"
#include "memory.h"
#include "memory_index.h"
#include "memory_index_one.h"
#include "indexer_param_block.h"
#include "stats_memory_index.h"
#include "stats_time.h"
#include "version.h"
#include "instream_file.h"
#include "instream_deflate.h"
#include "instream_bz2.h"
#include "instream_buffer.h"
#include "stem.h"
#include "stemmer_factory.h"
#include "btree_iterator.h"

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
void report(long long doc, ANT_memory_indexer *index, ANT_stats_time *stats, long long bytes_indexed)
{
printf("%lld Documents (%lld bytes) in %lld bytes of memory in ", (long long)doc, (long long)bytes_indexed, (long long)index->get_memory_usage());
stats->print_elapsed_time();
}

/*
	INDEX_DOCUMENT()
	----------------
*/
long index_document(ANT_memory_indexer *indexer, ANT_stem *stemmer, long segmentation, ANT_readability_factory *readability, long long doc, ANT_directory_iterator_object *current_file)
{
char term[MAX_TERM_LENGTH + 1], token_stem_internals[MAX_TERM_LENGTH + 1];
ANT_string_pair *token;
long terms_in_document, length_of_token, length_of_previous_token, is_previous_token_chinese;
char *previous_token_start;

/*
	Initialise
*/
terms_in_document = 0;

/*
	Initialise the Chinese parser
*/
previous_token_start = NULL;
length_of_previous_token = 0;
is_previous_token_chinese = FALSE;

/*
	Index the file
*/
readability->set_document((unsigned char *)current_file->file);
while ((token = readability->get_next_token()) != NULL)
	{
	/*
	 * a bit redudant, the code below.
	 * I think the original code from revision 656 should be fine, except the chinese handling part
	 */
	if (ANT_islower(token->start[0]))
		{
		terms_in_document++;
		if (stemmer == NULL || token->string_length <= 3)
			readability->handle_node(indexer->add_term(token, doc));			// indexable the term
		else
			{
			token->strncpy(term, MAX_TERM_LENGTH);
			stemmer->stem(term, token_stem_internals);
			ANT_string_pair token_stem(token_stem_internals);
			readability->handle_node(indexer->add_term(&token_stem, doc));			// indexable the stem of the term
			}
		}
	else if (ANT_isdigit(token->start[0]))
		{
		terms_in_document++;
		readability->handle_node(indexer->add_term(token, doc));			// indexable term
		}
	else if (ANT_isupper(token->start[0]))
		readability->handle_node(indexer->add_term(token, doc));			// open tag
	else if ((token->start[0] & 0x80) && (ANT_parser::ischinese(token->start) || ANT_parser::iseuropean(token->start)))
		{
		terms_in_document++; // keep counting number of terms, but for dual indexing only number of the words is recorded
		readability->handle_node(indexer->add_term(token, doc));

		if (ANT_parser::ischinese(token->start))
			{
			if ((segmentation & ANT_parser::DOUBLE_SEGMENTATION) == ANT_parser::DOUBLE_SEGMENTATION && token->string_length > 4) // (> 4) means more than one character
				{
				// move a character forward, try not to re-index the last single character in previous bigram
				length_of_token = ANT_parser::utf8_bytes(token->start);
				if ((segmentation & ANT_parser::BIGRAM_SEGMENTATION) == ANT_parser::BIGRAM_SEGMENTATION && is_previous_token_chinese && previous_token_start != NULL && (token->start + length_of_token) == (previous_token_start + length_of_previous_token))
					{
					previous_token_start = token->start;
					length_of_previous_token = token->string_length;
					token->start += length_of_token;
					token->string_length -= length_of_token;
					}
				else
					{
					previous_token_start = token->start;
					length_of_previous_token = token->string_length;
					}

				while (token->string_length > 0)								// chinese
					{
					length_of_token = ANT_parser::utf8_bytes(token->start);
					ANT_string_pair next_character(token->start, length_of_token);
					readability->handle_node(indexer->add_term(&next_character, doc));
					token->start += length_of_token;
					token->string_length -= length_of_token;
					}
				}
			is_previous_token_chinese = TRUE;
			continue;
			}
		}
	is_previous_token_chinese = FALSE;
	previous_token_start = NULL;
	}

if (terms_in_document != 0)
	{
	indexer->set_document_length(doc, terms_in_document);
	readability->index(indexer);
	}

return terms_in_document;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_indexer_param_block param_block(argc, argv);
ANT_stats_time stats;
ANT_directory_iterator *source = NULL;
ANT_directory_iterator *disk = NULL;
ANT_parser *parser;
ANT_readability_factory *readability;
ANT_memory_index *index;
long long doc, now, last_report;
long param, first_param;
ANT_memory file_buffer(1024 * 1024);
ANT_file id_list;
long long files_that_match;
long long bytes_indexed;
ANT_instream *file_stream = NULL, *decompressor = NULL, *instream_buffer = NULL;
ANT_directory_iterator_object file_object, *current_file;
ANT_directory_iterator_multiple *parallel_disk;
ANT_stem *stemmer = NULL;
long terms_in_document;

if (argc < 2)
	param_block.usage();

first_param = param_block.parse();

if (param_block.logo)
	puts(ANT_version_string);				// print the version string is we parsed the parameters OK

if (first_param >= argc)
	exit(0);				// no files to index so terminate

last_report = 0;
doc = 0;
index = new ANT_memory_index("index.aspt");
id_list.open("doclist.aspt", "wb");

index->set_compression_scheme(param_block.compression_scheme);
index->set_compression_validation(param_block.compression_validation);

if (param_block.readability_measure == ANT_readability_factory::NONE)
	parser = new ANT_parser(param_block.segmentation);
else
	parser = new ANT_parser_readability();

if (param_block.stemmer != 0)
	{
	/*
		The user has asked for a stemmed index and so we create a stemmer
		and store the fact in the index (so that the search engine knows)
	*/
	ANT_string_pair squiggle_quantized("~stemmer");
	index->set_variable(&squiggle_quantized, param_block.stemmer);
	stemmer = ANT_stemmer_factory::get_core_stemmer(param_block.stemmer);
	}

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

	now = stats.start_timer();
	if (param_block.recursive == ANT_indexer_param_block::DIRECTORIES)
		source = new ANT_directory_iterator_recursive(argv[param], ANT_directory_iterator::READ_FILE);			// this dir and below
	else if (param_block.recursive == ANT_indexer_param_block::TAR_BZ2)
		{
		file_stream = new ANT_instream_file(&file_buffer, argv[param]);
		decompressor = new ANT_instream_bz2(&file_buffer, file_stream);
		instream_buffer = new ANT_instream_buffer(&file_buffer, decompressor);
		source = new ANT_directory_iterator_tar(instream_buffer, ANT_directory_iterator::READ_FILE);
		}
	else if (param_block.recursive == ANT_indexer_param_block::TAR_GZ)
		{
		file_stream = new ANT_instream_file(&file_buffer, argv[param]);
		decompressor = new ANT_instream_deflate(&file_buffer, file_stream);
		instream_buffer = new ANT_instream_buffer(&file_buffer, decompressor);
		source = new ANT_directory_iterator_tar(instream_buffer, ANT_directory_iterator::READ_FILE);
		}
	else if (param_block.recursive == ANT_indexer_param_block::WARC_GZ)
		{
		file_stream = new ANT_instream_file(&file_buffer, argv[param]);
		decompressor = new ANT_instream_deflate(&file_buffer, file_stream);
		instream_buffer = new ANT_instream_buffer(&file_buffer, decompressor);
		source = new ANT_directory_iterator_warc(instream_buffer, ANT_directory_iterator::READ_FILE);
		}
	else if (param_block.recursive == ANT_indexer_param_block::TREC)
		source = new ANT_directory_iterator_file(ANT_disk::read_entire_file(argv[param]), ANT_directory_iterator::READ_FILE);
	else if (param_block.recursive == ANT_indexer_param_block::PKZIP)
		source = new ANT_directory_iterator_pkzip(argv[param], ANT_directory_iterator::READ_FILE);
	else
		source = new ANT_directory_iterator(argv[param], ANT_directory_iterator::READ_FILE);					// current directory

	stats.add_disk_input_time(stats.stop_timer(now));

#ifdef PARALLEL_INDEXING
	parallel_disk->add_iterator(source);
	}
	disk = parallel_disk;
	if (param_block.document_compression_scheme != ANT_indexer_param_block::NONE)
		{
		ANT_compression_text_factory *factory_text = new ANT_compression_text_factory;
		factory_text->set_scheme(param_block.document_compression_scheme);
		disk = new ANT_directory_iterator_compressor(disk, 8, factory_text, ANT_directory_iterator::READ_FILE);
		}

	#ifdef PARALLEL_INDEXING_DOCUMENTS
	disk = new ANT_directory_iterator_preindex(disk, &param_block, index_document, index, 8, ANT_directory_iterator::READ_FILE);
	#endif

	files_that_match = 0;

	now = stats.start_timer();
	current_file = disk->first(&file_object);
	stats.add_disk_input_time(stats.stop_timer(now));
	{
#else
	disk = source;
	files_that_match = 0;

	now = stats.start_timer();
	current_file = disk->first(&file_object)
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
			Index, this call returns the number of terms we found in the document
		*/
		now = stats.start_timer();
#ifdef PARALLEL_INDEXING_DOCUMENTS
		index->add_indexed_document(current_file->index, doc);
		delete current_file->index;
		terms_in_document = current_file->terms;
#else
		terms_in_document = index_document(index, stemmer, param_block.segmentation, readability, doc, current_file);
#endif
		stats.add_indexing_time(stats.stop_timer(now));

		if (terms_in_document == 0)
			{
			/*
				pretend we never saw the document
			*/
			doc--;
			}
		else
			{
			/*
				Store the document in the repository.
			*/
			if (param_block.document_compression_scheme != ANT_indexer_param_block::NONE)
				{
				index->add_to_document_repository(current_file->filename, current_file->compressed, (long)current_file->compressed_length, (long)current_file->length);
				delete [] current_file->compressed;
				}
			id_list.puts(strip_space_inplace(current_file->filename));
			}
		delete [] current_file->file;

		/*
			Get the next file
		*/
		now = stats.start_timer();
		current_file = disk->next(&file_object);
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

