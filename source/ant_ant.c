/*
     ANT_ANT.C
     ---------
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "ant_api.h"
#include "ant_param_block.h"
#include "str.h"
#include "channel.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

const char *PROMPT = "]";

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
	PARAM_BLOCK_TO_PARAMS()
	-----------------------
*/
void param_block_to_params(ANT_ANT_param_block *block, ANT_ANT_params *params)
{
params->logo = block->logo;
params->stemmer = block->stemmer;
params->stemmer_similarity = block->stemmer_similarity;
params->stemmer_similarity_threshold = block->stemmer_similarity_threshold;
params->sort_top_k = block->sort_top_k;
params->metric = block->metric;
params->metric_n = block->metric_n;
params->assessments_filename = block->assessments_filename;
params->queries_filename = block->queries_filename;
params->output_forum = block->output_forum;
params->run_name = block->run_name;
params->output_filename = block->output_filename;
params->results_list_length = block->results_list_length;
params->stats = block->stats;
params->segmentation = block->segmentation;
params->ranking_function = block->ranking_function;
params->trim_postings_k = block->trim_postings_k;
params->lmd_u = block->lmd_u;
params->lmjm_l = block->lmjm_l;
params->bm25_k1 = block->bm25_k1;
params->bm25_b = block->bm25_b;
params->file_or_memory = block->file_or_memory;
params->port = block->port;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_ANT_param_block param_block(argc, argv);
long last_param;
char *command, *query;
char *query_start;
long line;
char topic_id[1024]; // the topic id not necessary has to be number
long topic_id_len = 0;
long long hits;

last_param = param_block.parse();

//ant(search_engine, ranking_function, map, &params, filename_list, document_list, answer_list);
ANT *ant = ant_easy_init();
ANT_ANT_params *params = ant_params(ant);
param_block_to_params(&param_block, params);

long have_assessments = params->assessments_filename == NULL ? FALSE : TRUE;

ant_setup(ant);

ant_post_processing_stats_init(ant);

ANT_channel *inchannel = (ANT_channel *)params->inchannel;
line = 0;
prompt(&param_block);
for (command = inchannel->gets(); command != NULL; prompt(&param_block), command = inchannel->gets())
    {
    line++;
    /*
	    Parsing to get the topic number
    */
    strip_space_inplace(command);
    if (strcmp(command, ".quit") == 0)
	    break;
	if (strncmp(command, ".get ", 5) == 0)
		{
		get_document(ant, command);
		continue;
		}
    if (*command == '\0')
	    continue;			// ignore blank lines

    if (have_assessments || params->output_forum != ANT_ANT_param_block::NONE || params->queries_filename != NULL)
	    {
	    //topic_id = atol(query);
    	query_start = command;
	    if ((query = strchr(command, ' ')) == NULL)
		    exit(printf("Line %ld: Can't process query as badly formed:'%s'\n", line, query));
	    topic_id_len = query - query_start;
	    strncpy(topic_id, query_start, topic_id_len);
	    topic_id[topic_id_len] = '\0';
	    }
    else
		{
		strcpy(topic_id, "-1");
		query = command;
		}

    ant_search(ant, &hits, query, topic_id, params->boolean);
    forum_output(ant, topic_id, hits, query);

    prompt(&param_block);
    }

delete [] command;

ant_cal_map(ant);
ant_stat(ant);
ant_free(ant);
return 0;
}
