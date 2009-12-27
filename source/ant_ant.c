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

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

const char *PROMPT = "]";

/*
	class ANT_ANT_FILE_ITERATOR
	---------------------------
*/
class ANT_ANT_file_iterator
{
private:
	FILE *fp;
	char query[1024];

public:
	ANT_ANT_file_iterator(char *filename)
		{
		if (filename == NULL)
			fp = stdin;
		else
			fp = fopen(filename, "rb");
		if (fp == NULL)
			exit(printf("Cannot open topic file:'%s'\n", filename));
		}
	~ANT_ANT_file_iterator() { if (fp != NULL) fclose(fp); }
	char *first(void) {
        fseek(fp, 0, SEEK_SET);
        return fgets(query, sizeof(query), fp);
    }
	char *next(void) { return fgets(query, sizeof(query), fp); }
} ;

/*
	PROMPT()
	--------
*/
void prompt(ANT_ANT_param_block *params)
{
if (params->queries_filename == NULL)
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
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_ANT_param_block param_block(argc, argv);
long last_param;
char *query;
char *query_start;
long line;
char topic_id[1024]; // the topic id not necessary has to be number
long topic_id_len = 0;
long long hits;

last_param = param_block.parse();

ANT_ANT_file_iterator input(param_block.queries_filename);

//ant(search_engine, ranking_function, map, &params, filename_list, document_list, answer_list);
ANT *ant = ant_easy_init();
ANT_ANT_params *params = ant_params(ant);
param_block_to_params(&param_block, params);

long have_assessments = params->assessments_filename == NULL ? FALSE : TRUE;

ant_setup(ant);

ant_post_processing_stats_init(ant);

line = 0;
prompt(&param_block);
for (query = input.first(); query != NULL; query = input.next())
    {
    line++;
    /*
	    Parsing to get the topic number
    */
    strip_space_inplace(query);
    if (strcmp(query, ".quit") == 0)
	    break;
    if (*query == '\0')
	    continue;			// ignore blank lines

    if (have_assessments || params->output_forum != ANT_ANT_param_block::NONE || params->queries_filename != NULL)
	    {
	    //topic_id = atol(query);
    	query_start = query;
	    if ((query = strchr(query, ' ')) == NULL)
		    exit(printf("Line %ld: Can't process query as badly formed:'%s'\n", line, query));
	    topic_id_len = query - query_start;
	    strncpy(topic_id, query_start, topic_id_len);
	    topic_id[topic_id_len] = '\0';
	    }
    else
	    strcpy(topic_id, "-1");

    ant_search(ant, &hits, query, topic_id, params->boolean);
    forum_output(ant, topic_id, hits);

    prompt(&param_block);
    }

ant_cal_map(ant);
ant_stat(ant);
ant_free(ant);
return 0;
}
