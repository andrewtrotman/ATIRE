/*
	ATIRE.C
	-------
*/
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "atire_api.h"
#include "str.h"
#include "maths.h"
#include "ant_param_block.h"
#include "stats_time.h"
#include "channel_file.h"
#include "channel_socket.h"
#include "relevance_feedback_factory.h"
#include "ranking_function_pregen.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

const char * const PROMPT = "]";		// tribute to Apple
const long MAX_TITLE_LENGTH = 1024;

ATIRE_API *atire = NULL;

ATIRE_API *ant_init(ANT_ANT_param_block & params);
int ant_init_ranking(ATIRE_API * atire, ANT_indexer_param_block_rank & params);

/*
	PERFORM_QUERY()
	---------------
*/
double perform_query(long topic_id, ANT_channel *outchannel, ANT_ANT_param_block *params, char *query, long long *matching_documents)
{
ANT_stats_time stats;
char message[1024];
long long now, search_time;

/*
	Search
*/
now = stats.start_timer();
*matching_documents = atire->search(query, params->sort_top_k, params->query_type);
search_time = stats.stop_timer(now);

/*
	Report
*/
if (params->stats & ANT_ANT_param_block::SHORT)
	{
	if (topic_id >= 0)
		{
		sprintf(message, "Topic:%ld ", topic_id);
		outchannel->puts(message);
		}
	sprintf(message, "<query>%s</query><numhits>%lld</numhits><time>%lld</time>", query, *matching_documents, stats.time_to_milliseconds(search_time));
	outchannel->puts(message);
	}

if (params->stats & ANT_ANT_param_block::QUERY)
	atire->stats_text_render();

/*
	Return average precision
*/
return atire->get_whole_document_precision(topic_id, params->metric, params->metric_n);
}

/*
	PROMPT()
	--------
*/
void prompt(ANT_ANT_param_block *params)
{
if (params->queries_filename == NULL && params->port == 0)		// coming from stdin
	printf(PROMPT);
}

/*
	BETWEEN()
	---------
*/
char *between(char *source, char *open_tag, char *close_tag)
{
char *start,*finish;

if ((start = strstr(source, open_tag)) == NULL)
	return NULL;

start += strlen(open_tag);

if ((finish = strstr(start, close_tag)) == NULL)
	return NULL;

return strnnew(start, finish - start);
}

/*
	ANT()
	-----
*/
double ant(ANT_ANT_param_block *params)
{
char *print_buffer, *ch, *pos;
ANT_stats_time post_processing_stats;
char *command, *query, *ranker;
long topic_id, number_of_queries;
long long line;
long long hits, result, last_to_list, first_to_list;
ANT_indexer_param_block_rank params_rank;
int custom_ranking;
double average_precision, sum_of_average_precisions, mean_average_precision, relevance;
long length_of_longest_document;
unsigned long current_document_length;
long long docid;
char *document_buffer, *title_start, *title_end;
ANT_channel *inchannel, *outchannel;
char **answer_list;

if (params->port == 0)
	{
	inchannel = new ANT_channel_file(params->queries_filename);		// stdin
	outchannel = new ANT_channel_file();							// stdout
	}
else
	inchannel = outchannel = new ANT_channel_socket(params->port);	// in/out to given port

print_buffer = new char [MAX_TITLE_LENGTH + 1024];

if (atire != NULL)
	{
	length_of_longest_document = atire->get_longest_document_length();
	document_buffer = new char [length_of_longest_document + 1];
	}
else
	{
	length_of_longest_document = 0;
	document_buffer = NULL;
	}

sum_of_average_precisions = 0.0;
number_of_queries = 0;
line = 0;
custom_ranking = 0;

prompt(params);
for (command = inchannel->gets(); command != NULL; prompt(params), command = inchannel->gets())
	{

	if (custom_ranking)
		{
		/* Revert to default ranking function for next query */
		ant_init_ranking(atire, *params); //Just assume that it worked.
		custom_ranking = 0;
		}

	first_to_list = 0;
	last_to_list = first_to_list + params->results_list_length;

	line++;
	/*
		Parsing to get the topic number
	*/
	strip_space_inplace(command);

	if (strcmp(command, ".loadindex") == 0 || strncmp(command, "<ATIREloadindex>", 16) == 0)
		{
		/*
			NOTE: Do not expose this command to untrusted users as it could almost certainly
			cause arbitrary code execution by loading specially-crafted attacker-controlled indexes.
		*/
		char *oldindexfilename, *olddoclistfilename;

		if (strcmp(command, ".loadindex") == 0)
			{
			olddoclistfilename = params->swap_doclist_filename(strip_space_inplace(inchannel->gets()));
			oldindexfilename = params->swap_index_filename(strip_space_inplace(inchannel->gets()));
			}
		else
			{
			olddoclistfilename = params->swap_doclist_filename(between(command, "<doclist>", "</doclist>"));
			oldindexfilename = params->swap_index_filename(between(command, "<index>", "</index>"));
			}

		if (strlen(params->doclist_filename) == 0 && strlen(params->index_filename) == 0)
			{
			/* This is a request to unload the index */
			delete atire;
			atire = NULL;

			delete [] olddoclistfilename;
			delete [] oldindexfilename;

			outchannel->puts("<ATIREloadindex>1</ATIREloadindex>");
			}
		else
			{
			ATIRE_API * new_api = ant_init(*params);

			if (new_api)
				{
				delete [] olddoclistfilename;
				delete [] oldindexfilename;

				delete atire;
				atire = new_api;

				length_of_longest_document = atire->get_longest_document_length();
				delete [] document_buffer;
				document_buffer = new char [length_of_longest_document + 1];

				outchannel->puts("<ATIREloadindex>1</ATIREloadindex>");
				}
			else
				{
				/* Leave global 'atire' unchanged */
				outchannel->puts("<ATIREloadindex>0</ATIREloadindex>");

				/* Restore the filenames in params for later .describeindex queries to return, and delete the filenames
				 * we tried to load */
				delete [] params->swap_doclist_filename(olddoclistfilename);
				delete [] params->swap_index_filename(oldindexfilename);
				}
			}
		delete [] command;
		continue;
		}
	else if (strcmp(command, ".quit") == 0)
		{
		delete [] command;
		break;
		}
	else if (*command == '\0')
		{
		delete [] command;
		continue;			// ignore blank lines
		}
	else
		{
		/* Commands that require a working atire instance */
		if (atire == NULL)
			{
			outchannel->puts("<ATIREerror>");
			outchannel->puts("<description>No index loaded</description>");
			outchannel->puts("</ATIREerror>");
			delete [] command;
			continue;
			}

		if (strncmp(command, "<ATIREdescribeindex>", 18) == 0)
			{
			delete [] command;

			outchannel->puts("<ATIREdescribeindex>");

			outchannel->write("<doclist filename=\"");
			outchannel->write(params->doclist_filename);
			outchannel->puts("\"/>");

			outchannel->write("<index filename=\"");
			outchannel->write(params->index_filename);
			outchannel->puts("\"/>");
			outchannel->write("<docnum>");
			outchannel->write(atire->get_document_count());
			outchannel->puts("</docnum>");
			outchannel->write("<termnum>");
			outchannel->write(atire->get_term_count());
			outchannel->puts("</termnum>");
			outchannel->write("<longestdoc>");
			outchannel->write(atire->get_longest_document_length());
			outchannel->puts("</longestdoc>");
			outchannel->puts("</ATIREdescribeindex>");

			continue;
			}
		else if (strcmp(command, ".describeindex") == 0)
			{
			delete [] command;

			outchannel->puts(params->doclist_filename);
			outchannel->puts(params->index_filename);
			outchannel->write(atire->get_document_count());
			outchannel->puts("");
			continue;
			}
		else if (strncmp(command, ".get ", 5) == 0)
			{
			*document_buffer = '\0';
			if ((current_document_length = length_of_longest_document) != 0)
				{
				atire->get_document(document_buffer, &current_document_length, atoll(command + 5));
				sprintf(print_buffer, "%lld", (long long) current_document_length);
				outchannel->puts(print_buffer);
				outchannel->write(document_buffer, current_document_length);
				}
			delete [] command;
			continue;
			}
		else if (strncmp(command, "<ATIREsearch>", 13) == 0)
			{
			topic_id = -1;
			if ((query = between(command, "<query>", "</query>")) == NULL)
				{
				delete [] command;
				continue;
				}

			if ((pos = strstr(command, "<top>")) != NULL)
				first_to_list = atol(pos + 5)  - 1;
			else
				first_to_list = 0;

			if ((pos = strstr(command, "<n>")) != NULL)
				last_to_list = first_to_list + atol(pos + 3);
			else
				last_to_list = first_to_list + params->results_list_length;

			if ((ranker = between(command, "<ranking>", "</ranking>")) != NULL)
				{
				if (params_rank.set_ranker(ranker) && ant_init_ranking(atire, params_rank))
					custom_ranking = 1;
				else
					{
					outchannel->puts("<ATIREsearch>");
					outchannel->puts("<error>Bad ranking function</error>");
					outchannel->puts("</ATIREsearch>");
					delete [] query;
					delete [] ranker;
					delete [] command;

					continue;
					}

				delete [] ranker;
				}

			delete [] command;
			command = query;
			}
		else if (strncmp(command, "<ATIREgetdoc>", 13) == 0)
			{
			*document_buffer = '\0';
			if ((current_document_length = length_of_longest_document) != 0)
				{
				atire->get_document(document_buffer, &current_document_length, atoll(strstr(command, "<docid>") + 7));
				outchannel->puts("<ATIREgetdoc>");
				sprintf(print_buffer, "<length>%lld</length>", (long long) current_document_length);
				outchannel->puts(print_buffer);
				outchannel->write(document_buffer, current_document_length);
				outchannel->puts("</ATIREgetdoc>");
				}
			else
				{
				outchannel->puts("<ATIREgetdoc>");
				outchannel->puts("<length>0</length>");
				outchannel->puts("</ATIREgetdoc>");
				}
			delete [] command;
			continue;
			}
		else if (params->assessments_filename != NULL || params->output_forum != ANT_ANT_param_block::NONE || params->queries_filename != NULL)
			{
			topic_id = atol(command);
			if ((query = strchr(command, ' ')) == NULL)
				exit(printf("Line %lld: Can't process query as badly formed:'%s'\n", line, command));
			}
		else
			{
			topic_id = -1;
			query = command;
			}

		outchannel->puts("<ATIREsearch>");
		/*
			Do the query and compute average precision
		*/
		number_of_queries++;
		average_precision = perform_query(topic_id, outchannel, params, query, &hits);
		sum_of_average_precisions += average_precision;		// zero if we're using a focused metric

		/*
			Report the average precision for the query
		*/
		if (params->assessments_filename != NULL && params->stats & ANT_ANT_param_block::SHORT)
			printf("Topic:%ld Average Precision:%f\n", topic_id , average_precision);

		/*
			How many results to display on the screen.
		*/
		if (first_to_list > hits)
			first_to_list = last_to_list = hits;
		if (first_to_list < 0)
			first_to_list = 0;
		if (last_to_list > hits)
			last_to_list = hits;
		if (last_to_list < 0)
			last_to_list = 0;
		/*
			Convert from a results list into a list of documents and then display (or write to the forum file)
		*/
		if (params->output_forum != ANT_ANT_param_block::NONE)
			atire->write_to_forum_file(topic_id);
		else
			{
			answer_list = atire->generate_results_list();

			if (first_to_list < last_to_list)
				outchannel->puts("<hits>");
			for (result = first_to_list; result < last_to_list; result++)
				{
				docid = atire->get_relevant_document_details(result, &docid, &relevance);
				if ((current_document_length = length_of_longest_document) == 0)
					title_start = "";
				else
					{
					/*
						Get the title of the document (this is a bad hack and should be removed)
					*/
					atire->get_document(document_buffer, &current_document_length, docid);
					if ((title_start = strstr(document_buffer, "<title>")) == NULL)
						if ((title_start = strstr(document_buffer, "<TITLE>")) == NULL)
							title_start = "";
					if (*title_start != '\0')
						{
						title_start += 7;
						if ((title_end = strstr(title_start, "</title>")) == NULL)
							title_end = strstr(title_start, "</TITLE>");
						if (title_end != NULL)
							{
							if (title_end - title_start > MAX_TITLE_LENGTH)
								title_end = title_start + MAX_TITLE_LENGTH;
							*title_end = '\0';
							for (ch = title_start; *ch != '\0'; ch++)
								if (!ANT_isprint(*ch))
									*ch = ' ';
							}
						}
					}
				sprintf(print_buffer, "<hit><rank>%lld</rank><id>%lld</id><name>%s</name><rsv>%0.2f</rsv><title>%s</title></hit>", result + 1, docid, answer_list[result], relevance, title_start);
				outchannel->puts(print_buffer);
				}
			if (first_to_list < last_to_list)
				outchannel->puts("</hits>");
			}
		outchannel->puts("</ATIREsearch>");
		delete [] command;
		}
	}
/*
	delete the document buffer
*/
delete [] document_buffer;

/*
	Compute Mean Average Precision
*/
mean_average_precision = sum_of_average_precisions / (double)number_of_queries;

/*
	Report MAP
*/
if (params->assessments_filename != NULL && params->stats & ANT_ANT_param_block::PRECISION)
	printf("\nProcessed %ld topics (MAP:%f)\n\n", number_of_queries, mean_average_precision);

/*
	Report the summary of the stats
*/
if (params->stats & ANT_ANT_param_block::SUM)
	atire->stats_all_text_render();

/*
	Clean up
*/
delete inchannel;
if (inchannel != outchannel)
	delete outchannel;
delete [] print_buffer;

/*
	And finally report MAP
*/
return mean_average_precision;
}

/*
	ANT_INIT_RANKING()
	------------------

	Set up the ranking portion of the API parameters from the given ANT_indexer_param_block_rank

	Return true if successful. On failure, the API is not altered.
*/
int ant_init_ranking(ATIRE_API * atire, ANT_indexer_param_block_rank & params) {
switch (params.ranking_function)
	{
	case ANT_indexer_param_block_rank::BM25:
		return atire->set_ranking_function(params.ranking_function, params.bm25_k1, params.bm25_b) == 0;
	case ANT_indexer_param_block_rank::LMD:
		return atire->set_ranking_function(params.ranking_function, params.lmd_u, 0.0) == 0;
	case ANT_indexer_param_block_rank::LMJM:
		return atire->set_ranking_function(params.ranking_function, params.lmjm_l, 0.0) == 0;
	case ANT_indexer_param_block_rank::KBTFIDF:
		return atire->set_ranking_function(params.ranking_function, params.kbtfidf_k, params.kbtfidf_b) == 0;
	case ANT_indexer_param_block_rank::DOCID:
		return atire->set_ranking_function(params.ranking_function, params.ascending, 0) == 0;
	case ANT_indexer_param_block_rank::PREGEN:
		return atire->set_ranking_function_pregen(params.field_name, params.ascending) == 0;

	default:
		return atire->set_ranking_function(params.ranking_function, 0.0, 0.0) == 0;
	}
}

/*
	ANT_INIT()
	----------
	Create and return a new API and search engine using the given parameters.

	If the search engine fails to load the index from disk (e.g. sharing violation),
	return NULL.
*/
ATIRE_API *ant_init(ANT_ANT_param_block & params)
{
/*
	Instead of overwriting the global API, create a new one and return it.
	This way, if loading the index fails, we can still use the old one.
*/
ATIRE_API *atire = new ATIRE_API();
long fail;

if (params.logo)
	puts(atire->version());				// print the version string is we parsed the parameters OK

if (params.ranking_function == ANT_ANT_param_block::READABLE)
	fail = atire->open(ANT_ANT_param_block::READABLE | params.file_or_memory, params.index_filename, params.doclist_filename);
else
	fail = atire->open(params.file_or_memory, params.index_filename, params.doclist_filename);

if (params.inversion_type == ANT_indexer_param_block_topsig::TOPSIG)
	atire->load_topsig(params.topsig_width, params.topsig_density, params.topsig_global_stats);


if (fail) 
	{
	delete atire;

	return NULL;
	}

/* Load in all the pregens */
for (int i = 0 ; i < params.pregen_count; i++)
	{
	//Derive the pregen's filename from the index filename and pregen fieldname
	std::stringstream buffer;

	buffer << params.index_filename << "." << params.pregen_names[i];

	if (!atire->load_pregen(buffer.str().c_str()))
		fprintf(stderr, "Failed to load pregen %s, ignoring...\n", params.pregen_names[i]);
	}

if (params.assessments_filename != NULL)
	atire->load_assessments(params.assessments_filename);

if (params.output_forum != ANT_ANT_param_block::NONE)
	atire->set_forum(params.output_forum, params.output_filename, params.participant_id, params.run_name, params.results_list_length);

atire->set_trim_postings_k(params.trim_postings_k);
atire->set_stemmer(params.stemmer, params.stemmer_similarity, params.stemmer_similarity_threshold);
atire->set_feedbacker(params.feedbacker, params.feedback_documents, params.feedback_terms);

atire->set_segmentation(params.segmentation);

ant_init_ranking(atire, params); //Error value ignored...

return atire;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_stats stats;
ANT_ANT_param_block params(argc, argv);

params.parse();

atire = ant_init(params);

ant(&params);

delete atire;

printf("Total elapsed time including startup and shutdown ");
stats.print_elapsed_time();
ANT_stats::print_operating_system_process_time();
return 0;
}
