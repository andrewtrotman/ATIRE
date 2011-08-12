/*
	INDEX.C
	-------
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "index_document.h"
#include "index_document_topsig.h"
#include "ranking_function_factory.h"
#include "directory_iterator_tar.h"
#include "directory_iterator_warc.h"
#include "directory_iterator_warc_gz_recursive.h"
#include "directory_iterator_recursive.h"
#include "directory_iterator_multiple.h"
#include "directory_iterator_compressor.h"
#include "directory_iterator_deflate.h"
#include "directory_iterator_file.h"
#include "directory_iterator_csv.h"
#include "directory_iterator_object.h"
#include "directory_iterator_pkzip.h"
#include "directory_iterator_preindex.h"
#include "directory_iterator_mysql.h"
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
#include "instream_lzo.h"
#include "stem.h"
#include "stemmer_factory.h"
#include "btree_iterator.h"
#include "unicode.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

#ifdef _MSC_VER
	#include <windows.h>
	#define PATH_MAX MAX_PATH
#else
	#include <limits.h>
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
ANT_pregens_writer *pregen = NULL;
char pregen_filename[PATH_MAX + 1];
long terms_in_document;
ANT_index_document *document_indexer;

if (argc < 2)
	param_block.usage();

first_param = param_block.parse();

if (param_block.logo)
	puts(ANT_version_string);				// print the version string is we parsed the parameters OK

if (first_param >= argc)
	exit(0);				// no files to index so terminate

last_report = 0;
doc = 0;
index = new ANT_memory_index(param_block.index_filename);
id_list.open(param_block.doclist_filename, "wbx");

index->set_compression_scheme(param_block.compression_scheme);
index->set_compression_validation(param_block.compression_validation);

if (param_block.readability_measure == ANT_readability_factory::NONE)
	parser = new ANT_parser(param_block.segmentation);
else
	parser = new ANT_parser_readability();

if (param_block.inversion_type == ANT_indexer_param_block::TOPSIG)
	document_indexer = new ANT_index_document_topsig(param_block.topsig_width, param_block.topsig_density, param_block.topsig_global_stats);
else
	document_indexer = new ANT_index_document();

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

if (param_block.num_pregen_fields)
	{
	size_t pregen_prefix_len = strlen(param_block.index_filename) + 1;
	pregen = new ANT_pregens_writer();

	if (pregen_prefix_len < PATH_MAX)
		{
		strcpy(pregen_filename, param_block.index_filename);

		pregen_filename[pregen_prefix_len - 1] = '.';

		for (int i = 0; i < param_block.num_pregen_fields; i++)
			{
			ANT_indexer_param_block::pregen_field_spec & spec = param_block.pregens[i];

			if (pregen_prefix_len + strlen(spec.field_name) > PATH_MAX)
				fprintf(stderr, "Pathname too long to for pregen field '%s'\n", spec.field_name);
			else
				{
				strcpy(pregen_filename + pregen_prefix_len, spec.field_name);
				pregen->add_field(pregen_filename, spec.field_name, spec.type);
				}
			}
		}
	else
		{
		fprintf(stderr, "Index pathname too long to derive pregen filenames\n");
		//And we'll just keep going...
		}
	}

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
		{
		source = new ANT_directory_iterator_recursive(argv[param], ANT_directory_iterator::READ_FILE);			// this dir and below
		if (strcmp(argv[param] + strlen(argv[param]) - 3, ".gz") == 0)
			source = new ANT_directory_iterator_deflate(source, ANT_directory_iterator_deflate::TEXT);			// recursive .gz files
		}
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
	else if (param_block.recursive == ANT_indexer_param_block::TAR_LZO)
		{
		file_stream = new ANT_instream_file(&file_buffer, argv[param]);
		decompressor = new ANT_instream_lzo(&file_buffer, file_stream);
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
	else if (param_block.recursive == ANT_indexer_param_block::RECURSIVE_WARC_GZ)
		source = new ANT_directory_iterator_warc_gz_recursive(argv[param], ANT_directory_iterator::READ_FILE);

#ifdef ANT_HAS_MYSQL
	else if (param_block.recursive == ANT_indexer_param_block::VBULLETIN)
		{
		source = new ANT_directory_iterator_mysql(argv[param + 2], argv[param], argv[param + 1], argv[param + 3], "select postid, username, title, pagetext from post", ANT_directory_iterator::READ_FILE);
		param += 3;
		}
	else if (param_block.recursive == ANT_indexer_param_block::PHPBB)
		{
		/* We replace special term markers in user text (Unicode character 80, which encodes to C2 80 in UTF-8)
		 * with the euro symbol (encoded as E2 82 AC in UTF-8)
		 */
		assert(SPECIAL_TERM_CHAR == 0x80);

		if (strcmp(argv[param+4], "topics")==0)
			source = new ANT_directory_iterator_mysql(argv[param + 2], argv[param], argv[param + 1], argv[param + 3],
					"SELECT topic_id, CONCAT('<id>', t.topic_id, '</id><last_post_time>', topic_last_post_time, '</last_post_time><title>', "
							"REPLACE(topic_title, '\xC2\x80', '\xE2\x82\xAC'), '</title><author>', t.topic_first_poster_name, '</author><forum>',"
							"forum_name ,'</forum>'), "
						"CONCAT('\xC2\x80""forum-', t.forum_id), IF(t.topic_approved=0, '\xC2\x80""u', '\xC2\x80""a'), "
						"GROUP_CONCAT("
							"REPLACE(CONCAT_WS(' ', p.post_subject, p.post_text), '\xC2\x80', '\xE2\x82\xAC'), "
							"CONCAT('\xC2\x80poster-', p.poster_id) "
							"SEPARATOR ' '"
						") "
					"FROM phpbb_topics t "
					"LEFT JOIN phpbb_posts p ON t.topic_id = p.topic_id "
					"LEFT JOIN phpbb_forums f ON t.forum_id = f.forum_id "
					"WHERE p.post_approved = 1 AND t.topic_id > $start "
					"GROUP BY t.topic_id "
					"ORDER BY t.topic_id ASC ",
					ANT_directory_iterator::READ_FILE);
		else
			source = new ANT_directory_iterator_mysql(argv[param + 2], argv[param], argv[param + 1], argv[param + 3],
					"SELECT post_id, CONCAT('<id>', post_id, '</id><time>', post_time, '</time><subject>', REPLACE(post_subject, '\xC2\x80', '\xE2\x82\xAC'), "
							"'</subject><title>', REPLACE(topic_title, '\xC2\x80', '\xE2\x82\xAC'), '</title><author>', u.username, '</author><forum>',"
							"forum_name ,'</forum>'), "
						"REPLACE(post_subject, '\xC2\x80', '\xE2\x82\xAC'), "
						"REPLACE(post_text, '\xC2\x80', '\xE2\x82\xAC'), "						
						"IF(post_approved=0, '\xC2\x80""u', '\xC2\x80""a'), CONCAT('\xC2\x80poster-', poster_id), "
						"CONCAT('\xC2\x80""forum-', p.forum_id), CONCAT('\xC2\x80""topic-', topic_id) "
					"FROM phpbb_posts p "
					"LEFT JOIN phpbb_forums f USING (forum_id) "
					"LEFT JOIN phpbb_topics t USING (topic_id) "
					"LEFT JOIN phpbb_users u ON p.poster_id = u.user_id "
					"WHERE p.post_id > $start "
					"ORDER BY post_id ASC ",
					ANT_directory_iterator::READ_FILE);

		static_cast<ANT_directory_iterator_mysql*>(source)->set_paging_mode(FETCH_RANGE_START_LIMIT);

		param += 4;
		}
	else if (param_block.recursive == ANT_indexer_param_block::MYSQL)
		{
		source = new ANT_directory_iterator_mysql(argv[param + 2], argv[param], argv[param + 1], argv[param + 3], argv[param + 4], ANT_directory_iterator::READ_FILE);
		param += 4;
		}
#else
	else if (param_block.recursive == ANT_indexer_param_block::VBULLETIN
			|| param_block.recursive == ANT_indexer_param_block::PHPBB
			|| param_block.recursive == ANT_indexer_param_block::MYSQL)
		{
		fprintf(stderr, "You tried to index documents from MySQL but this indexer was not built with MySQL support\n");
		exit(-1);
		}
#endif

	else if (param_block.recursive == ANT_indexer_param_block::TRECWEB)
		{
		source = new ANT_directory_iterator_recursive(argv[param], ANT_directory_iterator::READ_FILE);
		if (strcmp(argv[param] + strlen(argv[param]) - 3, ".gz") == 0)
			source = new ANT_directory_iterator_deflate(source, ANT_directory_iterator_deflate::TEXT);		// recursive .gz files
		source = new ANT_directory_iterator_file(source, ANT_directory_iterator::READ_FILE);
		}
	else if (param_block.recursive == ANT_indexer_param_block::TREC)
		source = new ANT_directory_iterator_file(ANT_disk::read_entire_file(argv[param]), ANT_directory_iterator::READ_FILE);
	else if (param_block.recursive == ANT_indexer_param_block::CSV)
		source = new ANT_directory_iterator_csv(ANT_disk::read_entire_file(argv[param]), ANT_directory_iterator::READ_FILE);
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

		disk = new ANT_directory_iterator_preindex(disk, &param_block, document_indexer, index, 8, ANT_directory_iterator::READ_FILE);

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
	current_file = disk->first(&file_object);
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
		terms_in_document = document_indexer->index_document(index, stemmer, param_block.segmentation, readability, doc, current_file);
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
			if (pregen)
				pregen->process_document(doc - 1, current_file->filename);

			/*
				Store the document in the repository.
			*/
			if (param_block.document_compression_scheme != ANT_indexer_param_block::NONE)
				{
				index->add_to_document_repository(current_file->filename, current_file->compressed, (long)current_file->compressed_length, (long)current_file->length);
				delete [] current_file->compressed;
				}
//			puts(current_file->filename);
			id_list.puts(strip_space_inplace(current_file->filename));
			}
		delete [] current_file->file;
		delete [] current_file->filename;

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

	if (pregen)
		{
		for (int i = 0; i < pregen->field_count; i++)
			if (pregen->fields[i]->doc_count == 0)
				fprintf(stderr, "Warning: Pregen field '%s' not found in any documents\n", pregen->fields[i]->field_name);
		pregen->close();
		}

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
delete pregen;

if (param_block.statistics & ANT_indexer_param_block::STAT_TIME)
	{
	printf("\nTIMINGS\n-------\n");
	stats.text_render();
	}

return 0;
}

