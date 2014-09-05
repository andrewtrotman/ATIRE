/*
	ATIRE.C
	-------
*/
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <new>
#include "atire_api.h"
#include "str.h"
#include "maths.h"
#include "ant_param_block.h"
#include "stats_time.h"
#include "channel_file.h"
#include "channel_socket.h"
#include "channel_trec.h"
#include "channel_inex.h"
#include "relevance_feedback_factory.h"
#include "ranking_function_pregen.h"
#include "ranking_function_factory_object.h"
#include "snippet.h"
#include "snippet_factory.h"
#include "stemmer_factory.h"
#include "stem.h"
#include "thesaurus_wordnet.h"
#include "focus.h"
#include "focus_lowest_tag.h"
#include "focus_result.h"
#include "focus_results_list.h"
#include "search_engine.h"
#include "memory_index_one_node.h"

using namespace std;

const char * const PROMPT = "]";		// tribute to Apple
const long MAX_TITLE_LENGTH = 1024;

ATIRE_API *atire = NULL;

ATIRE_API *ant_init(ANT_ANT_param_block & params);
long ant_init_ranking(ATIRE_API *atire, ANT_ANT_param_block &params);
int run_atire(int argc, char *argv[]);
int run_atire(char *files);

ANT_stop_word *stop_word_list = NULL;

static const char *new_stop_words[] =
	{
//	"alternative",				 Needed for TREC topic 84
	"arguments",
	"can",
	"current",
	"dangers",
	"data",
	"description",
	"developments",
	"document",
	"documents",
	"done",
	"discuss",
	"discusses",
	"efforts",
	"enumerate",
	"examples",
	"help",
	"ideas",
	"identify",
	"inform",
	"information",
	"instances",
	"latest",
	"method",
	"narrative",
	"occasions",
	"problems",
	"provide",
	"relevant",
	"report",
	"reports",
	"state",
	"topic",
	NULL
	} ;

/*
	STOP_QUERY()
	------------
*/
char *stop_query(char *query, long stop_type)
{
static const char *SEPERATORS = ",./;'[]!@#$%^&*()_+-=\\|<>?:{}\r\n\t \"`~";
char *ch, *copy;
ostringstream stopped_query;

copy = strnew(query);
strip_space_inplace(query);

for (ch = strtok(query, SEPERATORS); ch != NULL; ch = strtok(NULL, SEPERATORS))
	{
	if ((stop_type & ANT_ANT_param_block::STOPWORDS_NCBI | ANT_ANT_param_block::STOPWORDS_PUURULA) && stop_word_list->isstop(ch))
		continue;

	if ((stop_type & ANT_ANT_param_block::STOPWORDS_NUMBERS) && isdigit(*ch))
		continue;

	if ((stop_type & ANT_ANT_param_block::STOPWORDS_SHORT) &&strlen(ch) <= 2)
		continue;

	stopped_query << ' ' << ch;
	}

if (stopped_query.str().empty())
	strcpy(query, copy);
else
	{
	stopped_query << ends;
	strcpy(query, (char *)stopped_query.str().c_str());
	}

return query;
}

/*
	PERFORM_QUERY()
	---------------
*/
double *perform_query(long topic_id, ANT_channel *outchannel, ANT_ANT_param_block *params, char *query, long long *matching_documents)
{
ANT_stats_time stats;
long long now, search_time;
long valid_to_evaluate;
double *evaluations;

if (params->query_stopping != ANT_ANT_param_block::NONE)
	stop_query(query, params->query_stopping);

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
	*outchannel << "<query>" << query << "</query>" << "<numhits>" << *matching_documents << "</numhits>" << "<time>" << stats.time_to_milliseconds(search_time) << "</time>" << ANT_channel::endl;

if (params->stats & ANT_ANT_param_block::QUERY)
	atire->stats_text_render();

/*
	Return average precision
*/
if (params->evaluator)
	{
	evaluations = params->evaluator->perform_evaluation(atire->get_search_engine(), topic_id, &valid_to_evaluate);
	if (evaluations == NULL || !valid_to_evaluate)
		return NULL;

	return evaluations;
	}

return NULL;
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
double *ant(ANT_ANT_param_block *params)
{
char *print_buffer, *pos;
ANT_stats_time post_processing_stats;
char *command, *query, *ranker;
long topic_id = -1, number_of_queries, number_of_queries_evaluated, evaluation;
long long line;
long long hits, result, last_to_list, first_to_list;
ANT_ANT_param_block params_rank(params->argc, params->argv);
int custom_ranking;
double *average_precision, *sum_of_average_precisions, *mean_average_precision;
double relevance;
long length_of_longest_document;
unsigned long current_document_length;
long long docid;
char *document_buffer;
ANT_channel *inchannel, *outchannel;
char *snippet = NULL, *title = NULL;
ANT_snippet *snippet_generator = NULL;
ANT_snippet *title_generator = NULL;
ANT_stem *snippet_stemmer = NULL;
#ifdef FILENAME_INDEX
	char *document_name;
	document_name = new char [MAX_TITLE_LENGTH];
#else
	char **answer_list;
#endif

if (params->port != 0)
	inchannel = outchannel = new ANT_channel_socket(params->port);	// in/out to given port
else
	{
	inchannel = new ANT_channel_file(params->queries_filename);		// my stdin
	outchannel = new ANT_channel_file();							// my stdout

	if (params->queries_filename != NULL)
		{
		char first_bytes[0x200];		// enough to hold the INEX DTD and then a bit
		/*
			We might be a TREC topic file (usually a .gz) or an INEX topic file (usually a .zip) so we'll file out which.
			Its unreasonable to make the assumption based on the file type (who knows, INEX might start using .gz or
			TREC might start using .zip) so we'll open the file, read the first few bytes, and guess.  Guessing also
			allows us to handle ANT query files (one topic per line, in the form <id> <query>).
		*/
		ANT_channel_file test_channel(params->queries_filename);
		if (test_channel.read(first_bytes, sizeof(first_bytes)) == NULL)
			{
			/*
				Does the file exist?
			*/
			ANT_channel_file test_channel(params->queries_filename);
			if (test_channel.read(first_bytes, 1) == NULL)
				exit(printf("Query file '%s' not found!", params->queries_filename));

			/*
				else
					probably and ANT file as its very small.
			*/
			//exit(printf("Cannot determine the format of the queries file... is it valid?"));
			}
		else
			{
			/*
				We can use the first few bytes to guess the file type
			*/
			if (atoll(first_bytes) != 0)
				{
				/*
					no work required because we're a compressed ANT formatted queries file
				*/
				}
			else if (strstr(first_bytes, "inex-topic-file") != NULL)
				inchannel = new ANT_channel_inex(inchannel, params->query_fields);
			else if (strstr(first_bytes, "<top>") != NULL)
				inchannel = new ANT_channel_trec(inchannel, params->query_fields);
			else
				{
				/*
					We're going to guess based on file type
				*/
				if (strrcmp(params->queries_filename, "gz") == 0)			// probably a TREC file
					inchannel = new ANT_channel_trec(inchannel, params->query_fields);
				else if (strrcmp(params->queries_filename, "zip") == 0)		// probably an INEX file
					inchannel = new ANT_channel_trec(inchannel, params->query_fields);
				}
			}
		}
	}

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

sum_of_average_precisions = new double[params->evaluator->number_evaluations_used];
for (evaluation = 0; evaluation < params->evaluator->number_evaluations_used; evaluation++)
	sum_of_average_precisions[evaluation] = 0.0;
number_of_queries = number_of_queries_evaluated = 0;
line = 0;
custom_ranking = 0;

if (params->snippet_algorithm != ANT_ANT_param_block::NONE)
	{
	snippet = new (std::nothrow) char [params->snippet_length + 1];
	*snippet = '\0';
	if (params->snippet_stemmer == ANT_stemmer_factory::NONE)
		snippet_stemmer = NULL;
	else
		if ((snippet_stemmer = ANT_stemmer_factory::get_core_stemmer(params->snippet_stemmer)) == NULL)
			exit(printf("Invalid snippet stemmer requested somehow\n"));

	snippet_generator = ANT_snippet_factory::get_snippet_maker(params->snippet_algorithm, params->snippet_length, atire->get_longest_document_length(), params->snippet_tag, atire->get_search_engine(), snippet_stemmer, params->snippet_word_cloud_terms);
	}

if (params->title_algorithm != ANT_ANT_param_block::NONE)
	{
	title = new (std::nothrow) char [params->title_length + 1];
	*title = '\0';
	title_generator = ANT_snippet_factory::get_snippet_maker(params->title_algorithm, params->title_length, atire->get_longest_document_length(), params->title_tag);
	}

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
			ATIRE_API *new_api = ant_init(*params);

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
#ifdef IMPACT_HEADER
			outchannel->write("<uniquetermnum>");
			outchannel->write(atire->get_unique_term_count());
			outchannel->puts("</uniquetermnum>");
#endif
			outchannel->write("<quantized>");
			long long var = atire->get_search_engine()->get_is_quantized();
			outchannel->write(var);
			outchannel->puts("</quantized>");
			var = atire->get_search_engine()->get_variable("~quantmax");
			outchannel->write("<quantmax>");
			printf("%f", *(double *)&var);
			outchannel->puts("</quantmax>");
			var = atire->get_search_engine()->get_variable("~quantmin");
			outchannel->write("<quantmin>");
			printf("%f", *(double *)&var);
			outchannel->puts("</quantmin>");
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
		else if (strncmp(command, ".morelike ", 10) == 0)
			{
			*document_buffer = '\0';
			if ((current_document_length = length_of_longest_document) != 0)
				{
				atire->get_document(document_buffer, &current_document_length, atoll(command + 10));
				query = atire->extract_query_terms(document_buffer, 10);		// choose top 10 terms

				delete [] command;
				command = query;
				}
			else
				{
				delete [] command;
				continue;
				}
			}
		else if (strncmp(command, ".get ", 5) == 0)
			{
			*document_buffer = '\0';
			if ((current_document_length = length_of_longest_document) != 0)
				{
				atire->get_document(document_buffer, &current_document_length, atoll(command + 5));

				if (params->focussing_algorithm == ANT_ANT_param_block::RANGE)
					{
					/*
						Setup for the focussing game
					*/
					ANT_focus_results_list focused_results_list(10);		// top 10 focused results
					ANT_focus_result *document_range;
					ANT_focus *focusser = new ANT_focus_lowest_tag(&focused_results_list);
					long focus_length;
					unsigned char zero_x_ff[] = {0xff, 0x00};
					ANT_string_pair *search_terms, *current_search_term, *token;
					ANT_parser parser;

					/*
						Prime the focusser by adding the search terms to the matcher
					*/
					current_search_term = search_terms = new ANT_string_pair [strcountchr(command, ' ')];
					parser.set_document(command + 1);			// skip over the '.'
					parser.get_next_token();					// get
					parser.get_next_token();					// docid
					while ((token = parser.get_next_token()) != NULL)
						{
						*current_search_term = *token;
						focusser->add_term(current_search_term);			// search terms
						current_search_term++;
						}

					/*
						Focus the document
					*/
					document_range = focusser->focus((unsigned char *)document_buffer, &focus_length);

					/*
						Send the document length (with space added for the markers)
					*/
					*outchannel << current_document_length + 2 << ANT_channel::endl;

					/*
						Send:
							The first part of the document
							The start marker
							the middle of the document
							The end marker
							The end of the document

					*/
					outchannel->write(document_buffer, document_range->start - document_buffer);
					outchannel->write(zero_x_ff, 1);
					outchannel->write(document_range->start, document_range->finish- document_range->start);
					outchannel->write(zero_x_ff, 1);
					outchannel->write(document_range->finish, current_document_length - (document_range->finish - document_range->start));

					/*
						Clean up
					*/
					delete [] search_terms;
					}
				else
					{
					*outchannel << current_document_length << ANT_channel::endl;
					outchannel->write(document_buffer, current_document_length);
					}
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
				*outchannel << "<length>" << current_document_length << "</length>" << ANT_channel::endl;
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

		if (params->stats & ANT_ANT_param_block::SHORT)
			outchannel->puts("<ATIREsearch>");
		/*
			Do the query and compute average precision
		*/
		number_of_queries++;
		average_precision = perform_query(topic_id, outchannel, params, query, &hits);
		if (average_precision != NULL)
			{
			number_of_queries_evaluated++;
			for (evaluation = 0; evaluation < params->evaluator->number_evaluations_used; evaluation++)
				sum_of_average_precisions[evaluation] += average_precision[evaluation];		// zero if we're using a focused metric
			}
		/*
			Report the average precision for the query
		*/
		if ((params->assessments_filename != NULL) && ((params->stats & ANT_ANT_param_block::SHORT) != 0) && (average_precision != NULL))
			{
			*outchannel << "<topic>" << topic_id << "</topic>" << ANT_channel::endl;
			*outchannel << "<evaluations>";
			for (evaluation = 0; evaluation < params->evaluator->number_evaluations_used; evaluation++)
				{
				sprintf(print_buffer, "%f", average_precision[evaluation]);
				*outchannel << "<" << params->evaluator->evaluation_names[evaluation] << ">" << print_buffer << "</" << params->evaluator->evaluation_names[evaluation] << ">";
				}
			outchannel->puts("</evaluations>");
			}

		/*
			How many results to display on the screen.
		*/
		if (first_to_list > hits)
			first_to_list = last_to_list = hits;
		if (first_to_list < 0)
			first_to_list = 0;
		if (last_to_list > hits)
			last_to_list = hits;
		if (last_to_list < first_to_list)
			last_to_list = first_to_list;
		/*
			Convert from a results list into a list of documents and then display (or write to the forum file)
		*/
		if (params->output_forum != ANT_ANT_param_block::NONE)
			atire->write_to_forum_file(topic_id);
		else
			{
			#ifndef FILENAME_INDEX
				answer_list = atire->generate_results_list();
			#endif

			if (first_to_list < last_to_list)
				outchannel->puts("<hits>");

			/*
				We're going to generate snippets to parse the query for that purpose
			*/
			if (snippet_generator != NULL)
				snippet_generator->parse_query(query);
			for (result = first_to_list; result < last_to_list; result++)
				{
				docid = atire->get_relevant_document_details(result, &docid, &relevance);
				if ((current_document_length = length_of_longest_document) != 0)
					{
					/*
						Load the document if we need a snippet or a title
					*/
					if (title_generator != NULL || snippet_generator != NULL)
						atire->get_document(document_buffer, &current_document_length, docid);

					/*
						Generate the title
					*/
					if (title_generator != NULL)
						title_generator->get_snippet(title, document_buffer);

					/*
						Generate the snippet
					*/
					if (snippet_generator != NULL)
						snippet_generator->get_snippet(snippet, document_buffer);
					}
				*outchannel << "<hit>";
				*outchannel << "<rank>" << result + 1 << "</rank>";
				*outchannel << "<id>" << docid << "</id>";
				#ifdef FILENAME_INDEX
					*outchannel << "<name>" << atire->get_document_filename(document_name, docid) << "</name>";
				#else
					*outchannel << "<name>" << answer_list[result] << "</name>";
				#endif
				sprintf(print_buffer, "%0.2f", relevance);
				*outchannel << "<rsv>" << print_buffer << "</rsv>";
				if (title != NULL && *title != '\0')
					*outchannel << "<title>" << title << "</title>";
				if (snippet != NULL && *snippet != '\0')
					*outchannel << "<snippet>" << snippet << "</snippet>";
				*outchannel << "</hit>" << ANT_channel::endl;
				}
			if (first_to_list < last_to_list)
				outchannel->puts("</hits>");
			}

		if (params->stats & ANT_ANT_param_block::SHORT)
			outchannel->puts("</ATIREsearch>");
		delete [] command;
		}
	}
/*
	delete the document buffer (and the "filename" buffer).
*/
#ifdef FILENAME_INDEX
	delete [] document_name;
#endif
delete [] document_buffer;

/*
	Compute Mean Average Precision
*/
mean_average_precision = new double[params->evaluator->number_evaluations_used];
for (evaluation = 0; evaluation < params->evaluator->number_evaluations_used; evaluation++)
	mean_average_precision[evaluation] = sum_of_average_precisions[evaluation] / (double)number_of_queries_evaluated;

/*
	Report MAP
*/
if (params->assessments_filename != NULL && params->stats & ANT_ANT_param_block::PRECISION)
	{
	printf("\nProcessed %ld topics (%ld evaluated):\n", number_of_queries, number_of_queries_evaluated);
	for (evaluation = 0; evaluation < params->evaluator->number_evaluations_used; evaluation++)
		printf("%s: %f\n", params->evaluator->evaluation_names[evaluation], mean_average_precision[evaluation]);
	puts("");
	}

/*
	Report the summary of the stats
*/
if (params->stats & ANT_ANT_param_block::SUM)
	atire->stats_all_text_render();

/*
	Clean up
*/
if (outchannel != inchannel)
	delete outchannel;
delete inchannel;

delete [] print_buffer;
delete [] snippet;
delete [] title;
delete [] sum_of_average_precisions;
delete snippet_generator;
delete title_generator;
delete snippet_stemmer;

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
long ant_init_ranking(ATIRE_API *atire, ANT_ANT_param_block &params)
{
long ranker_ok;
	
if (params.ranking_function == ANT_ranking_function_factory_object::PREGEN)
	ranker_ok = atire->set_ranking_function_pregen(params.field_name, params.p1) == 0;
else
	ranker_ok = atire->set_ranking_function(params.ranking_function, params.quantization, params.quantization_bits, params.p1, params.p2, params.p3) == 0;

if (!ranker_ok)
	return ranker_ok;

return atire->set_feedback_ranking_function(params.feedback_ranking_function, params.quantization, params.quantization_bits, params.feedback_p1, params.feedback_p2, params.feedback_p3) == 0;
}

/*
	ANT_INIT()
	----------
	Create and return a new API and search engine using the given parameters.

	If the search engine fails to load the index from disk (e.g. sharing violation),
	return NULL.
*/
ATIRE_API *ant_init(ANT_ANT_param_block &params)
{
/*
	Instead of overwriting the global API, create a new one and return it.
	This way, if loading the index fails, we can still use the old one.
*/
ATIRE_API *atire = new ATIRE_API();
long fail;
ANT_thesaurus *expander;

if (params.logo)
	puts(atire->version());				// print the version string if we parsed the parameters OK

if (params.ranking_function == ANT_ranking_function_factory_object::READABLE)
	fail = atire->open(ATIRE_API::READABILITY_SEARCH_ENGINE | params.file_or_memory, params.index_filename, params.doclist_filename, params.quantization, params.quantization_bits);
else
	fail = atire->open(params.file_or_memory, params.index_filename, params.doclist_filename, params.quantization, params.quantization_bits);

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
	atire->load_assessments(params.assessments_filename, params.evaluator);

if (params.output_forum != ANT_ANT_param_block::NONE)
	atire->set_forum(params.output_forum, params.output_filename, params.participant_id, params.run_name, params.results_list_length);

atire->set_trim_postings_k(params.trim_postings_k);
atire->set_stemmer(params.stemmer, params.stemmer_similarity, params.stemmer_similarity_threshold);
atire->set_feedbacker(params.feedbacker, params.feedback_documents, params.feedback_terms);
if (params.feedbacker == ANT_relevance_feedback_factory::BLIND_RM)
	atire->set_feedback_interpolation(params.feedback_lambda);

if ((params.query_type & ATIRE_API::QUERY_EXPANSION_INPLACE_WORDNET) != 0)
	{
	atire->set_inplace_query_expansion(expander = new ANT_thesaurus_wordnet("wordnet.aspt"));
	expander->set_allowable_relationships(params.expander_tf_types);
	}
if ((params.query_type & ATIRE_API::QUERY_EXPANSION_WORDNET) != 0)
	{
	atire->set_query_expansion(expander = new ANT_thesaurus_wordnet("wordnet.aspt"));
	expander->set_allowable_relationships(params.expander_query_types);
	}

atire->set_segmentation(params.segmentation);

ant_init_ranking(atire, params); //Error value ignored...
atire->set_processing_strategy(params.processing_strategy, params.quantum_stopping);

// set the pregren to use for accumulator initialisation
if (params.ranking_function != ANT_ranking_function_factory_object::PREGEN)
	atire->get_search_engine()->results_list->set_pregen(atire->get_pregen(), params.pregen_ratio);
return atire;
}

/*
	RUN_ATIRE()
	-----------
	for the simplicity of JNI calling
	options are separated with +
*/
int run_atire(char *options)
{
static char *seperators = "+";
char **argv, **file_list;
char *token;
size_t total_length = (options ? strlen(options) : 0) + 7;
char *copy, *copy_start;

copy = copy_start = new char[total_length];
memset(copy, 0, sizeof(*copy) * total_length);

memcpy(copy, "atire+", 6);
copy += 6;
if (options)
	{
	memcpy(copy, options, strlen(options));
	copy += strlen(options);
	}
*copy = '\0';

argv = file_list = new char *[total_length];
int argc = 0;
token = strtok(copy_start, seperators);

#ifdef DEBUG
	fprintf(stderr, "Start atire with options: %s\n", options);
#endif
for (; token != NULL; token = strtok(NULL, seperators))
	{
#ifdef DEBUG
	fprintf(stderr, "%s ", token);
#endif
	*argv++ = token;
	++argc;
	}
#ifdef DEBUG
	fprintf(stderr, "\n");
#endif
*argv = NULL;
int result = run_atire(argc, file_list);
delete [] copy;
delete [] file_list;

return result;
}

/*
	RUN_ATIRE()
	-----------
*/
int run_atire(int argc, char *argv[])
{
ANT_stats stats;
ANT_ANT_param_block params(argc, argv);

params.parse();
if (params.query_stopping & ANT_ANT_param_block::STOPWORDS_NCBI)
	stop_word_list = new ANT_stop_word(ANT_stop_word::NCBI);
else if (params.query_stopping & ANT_ANT_param_block::STOPWORDS_PUURULA)
	stop_word_list = new ANT_stop_word(ANT_stop_word::PUURULA);
if (params.query_stopping & ANT_ANT_param_block::STOPWORDS_ATIRE)
	stop_word_list->addstop((const char **)new_stop_words);
	

atire = ant_init(params);

delete [] ant(&params);

delete atire;

printf("Total elapsed time including startup and shutdown ");
stats.print_elapsed_time();
ANT_stats::print_operating_system_process_time();

return 0;
}

#ifndef ATIRE_LIBRARY

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
return run_atire(argc, argv);
}

#endif
