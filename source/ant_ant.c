/*
 * ANT_ANT.C
 * ---------
 *
 */

/*
	ANT.C
	-----
*/
#include <stdio.h>
#include <string.h>
#include "str.h"
#include "memory.h"
#include "ctypes.h"
#include "search_engine.h"
#include "search_engine_readability.h"
#include "search_engine_btree_leaf.h"
#include "mean_average_precision.h"
#include "disk.h"
#include "relevant_document.h"
#include "time_stats.h"
#include "stemmer.h"
#include "stemmer_factory.h"
#include "assessment_factory.h"
#include "search_engine_forum_INEX.h"
#include "search_engine_forum_TREC.h"
#include "ant_param_block.h"
#include "encoding_utf8.h"
#include "version.h"
#include "ranking_function_impact.h"
#include "ranking_function_bm25.h"
#include "ranking_function_similarity.h"
#include "ranking_function_lmd.h"
#include "ranking_function_lmjm.h"
#include "ranking_function_bose_einstein.h"
#include "ranking_function_divergence.h"
#include "ranking_function_readability.h"

#include "ant_api.h"

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
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
ANT_ANT_param_block params(argc, argv);
long last_param;

last_param = params.parse();

/**
 * TODO implement the query loop here
 */

//ant(search_engine, ranking_function, map, &params, filename_list, document_list, answer_list);

/*
	Compute Mean Average Precision
*/
//mean_average_precision = sum_of_average_precisions / (double)number_of_queries;

/*
	Report MAP
*/
//if (map != NULL && params->stats & SHORT)
//	printf("\nProcessed %ld topics (MAP:%f)\n\n", number_of_queries, mean_average_precision);

/*
	Report the summary of the stats
*/
//if (params->stats & SUM)
//	{
//	search_engine->stats_all_text_render();
//	post_processing_stats.print_time("Post Processing I/O  :", post_processing_stats.disk_input_time);
//	post_processing_stats.print_time("Post Processing CPU  :", post_processing_stats.cpu_time);
//	}

/*
	And finally report MAP
*/
//return mean_average_precision;


return 0;
}
