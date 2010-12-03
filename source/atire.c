/*
	ATIRE.C
	-------
*/
#include <stdio.h>
#include <string.h>
#include "atire_api.h"
#include "str.h"
#include "maths.h"
#include "ant_param_block.h"
#include "stats_time.h"
#include "channel_file.h"
#include "channel_socket.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

const char *PROMPT = "]";
const long MAX_TITLE_LENGTH = 1024;

ATIRE_API atire;

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
*matching_documents = atire.search(query, params->sort_top_k, ATIRE_API::QUERY_BOOLEAN);
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
	sprintf(message, "Query '%s' found %lld documents in %lld ms", query, *matching_documents, stats.time_to_milliseconds(search_time));
	outchannel->puts(message);
	}

if (params->stats & ANT_ANT_param_block::QUERY)
	atire.stats_text_render();

/*
	Return average precision
*/
return atire.get_whole_document_precision(topic_id, params->metric, params->metric_n);
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
	ANT()
	-----
*/
double ant(ANT_ANT_param_block *params)
{
char *print_buffer, *ch;
ANT_stats_time post_processing_stats;
char *command, *query;
long topic_id, line, number_of_queries;
long long hits, result, last_to_list;
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

length_of_longest_document = atire.get_longest_document_length();

document_buffer = new char [length_of_longest_document + 1];

sum_of_average_precisions = 0.0;
number_of_queries = line = 0;

prompt(params);
for (command = inchannel->gets(); command != NULL; prompt(params), command = inchannel->gets())
	{
	line++;
	/*
		Parsing to get the topic number
	*/
	strip_space_inplace(command);
	if (strcmp(command, ".quit") == 0)
		{
		delete [] command;
		break;
		}
	if (strncmp(command, ".get ", 5) == 0)
		{
		*document_buffer = '\0';
		if ((current_document_length = length_of_longest_document) != 0)
			{
			atire.get_document(document_buffer, &current_document_length, atoll(command + 5));
			sprintf(print_buffer, "%lld", current_document_length);
			outchannel->puts(print_buffer);
			outchannel->write(document_buffer, current_document_length);
			}
		continue;
		}
	if (*command == '\0')
		continue;			// ignore blank lines

	if (params->assessments_filename != NULL || params->output_forum != ANT_ANT_param_block::NONE || params->queries_filename != NULL)
		{
		topic_id = atol(command);
		if ((query = strchr(command, ' ')) == NULL)
			exit(printf("Line %ld: Can't process query as badly formed:'%s'\n", line, command));
		}
	else
		{
		topic_id = -1;
		query = command;
		}

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
	last_to_list = hits > params->results_list_length ? params->results_list_length : hits;

	/*
		Convert from a results list into a list of documents and then display (or write to the forum file)
	*/
	if (params->output_forum != ANT_ANT_param_block::NONE)
		atire.write_to_forum_file(topic_id);
	else
		{
		answer_list = atire.generate_results_list();
		for (result = 0; result < last_to_list; result++)
			{
			docid = atire.get_relevant_document_details(result, &docid, &relevance);
			if ((current_document_length = length_of_longest_document) == 0)
				title_start = "";
			else
				{
				atire.get_document(document_buffer, &current_document_length, docid);
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
			sprintf(print_buffer, "%lld:%lld:%s %f %s", result + 1, docid, answer_list[result], relevance, title_start);
			outchannel->puts(print_buffer);
			}
		}

	delete [] command;
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
	atire.stats_all_text_render();

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
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_stats stats;
ANT_ANT_param_block params(argc, argv);

params.parse();

if (params.logo)
	puts(atire.version());				// print the version string is we parsed the parameters OK

if (params.ranking_function == ANT_ANT_param_block::READABLE)
	atire.open(ANT_ANT_param_block::READABLE | params.file_or_memory);
else
	atire.open(params.file_or_memory);

if (params.assessments_filename != NULL)
	atire.load_assessments(params.assessments_filename);

if (params.output_forum != ANT_ANT_param_block::NONE)
	atire.set_forum(params.output_forum, params.output_filename, params.participant_id, params.run_name, params.results_list_length);

atire.set_trim_postings_k(params.trim_postings_k);
atire.set_stemmer(params.stemmer, params.stemmer_similarity, params.stemmer_similarity_threshold);

atire.set_segmentation(params.segmentation);
ant(&params);

printf("Total elapsed time including startup and shutdown ");
stats.print_elapsed_time();
ANT_stats::print_operating_system_process_time();
return 0;
}
