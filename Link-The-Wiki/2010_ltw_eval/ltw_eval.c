/*
	LTW_EVAL.C
	----------
	Evaluate the INEX 2010 Link the Wiki (Te Ara) runs
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>

#define UNASSESSED_RESULT (-1)
#define UNASSESSED_TOPIC (-2)
#define UNSCORED_RESULT (-3)
#define DUPLICATE_RESULT (-4)

#define TREC_EVAL 1
#define TOP_EVAL  2
#define BEST_EVAL 4


#define RUN_TREC  0
#define RUN_LTW   1
static char *SEPERATORS = " \t\r\n";

#define atoll _atoi64

long long minus_c = false;					// this is the TREC_EVAL -c option
long long tight = false;					// print summary all on one line?

/*
	class LTW_ASSESSMENT
	--------------------
*/
class ltw_assessment
{
public:
	long long topic;
	char *target;
	long long relevant;
} ;

/*
	class LTW_RESULT
	----------------
*/
class ltw_result : public ltw_assessment
{
public:
	long long position;
	double rsv;
} ;

/*
	class LTW_TOPIC_DETAILS
	-----------------------
*/
class ltw_topic_details
{
public:
	long long topic;
	long long relevant;
	long long non_relevant;
	long long used_in_evaluation;
} ;

/*
	class LTW_METRICS
	-----------------
*/
class ltw_metrics
{
public:
	char *run_name;
	long long assessed_topics;
	long long run_topics;
	long long assessed_run_topics;
	long long num_ret, num_rel, num_ret_rel;
	double run_sum_of_precisions;
	double precision_at_recall[11];
	double sum_recip_rank;
public:
	void text_render(void);
} metric;

/*
	LTW_METRICS::TEXT_RENDER()
	--------------------------
*/
void ltw_metrics::text_render(void)
{
long long recall_point;
long long divisor;

if (minus_c)
	divisor = assessed_topics; 
else
	divisor = assessed_run_topics;

if (tight)
	{
	printf("%s", run_name);
	printf(" %0.4f", run_sum_of_precisions / (double)divisor);
	for (recall_point = 0; recall_point <= 10; recall_point++)
		printf(" %0.4f", precision_at_recall[recall_point] / (double)divisor);
	printf("\n");
	}
else
	{
	printf("runid                           :%s\n", run_name);
	printf("num_q                           :%lld\n", divisor);
	printf("num_ret                         :%lld\n", num_ret);
	printf("num_rel                         :%lld\n", num_rel);
	printf("num_rel_ret                     :%lld\n", num_ret_rel);
	#ifdef NEVER
		printf("Topics_assessed                 :%lld\n", assessed_topics);
		printf("Topics_in_run_and_assessed      :%lld\n", assessed_run_topics);
		printf("Sum of Average Precisions       :%0.4f\n", run_sum_of_precisions);
	#endif
	printf("map                             :%0.4f\n", run_sum_of_precisions / (double)divisor);
	printf("recip_rank                      :%0.4f\n", sum_recip_rank / (double)divisor);

	for (recall_point = 0; recall_point <= 10; recall_point++)
		printf("iprec_at_recall_%0.2f            :%0.4f\n", (double)recall_point / 10.0, precision_at_recall[recall_point] / (double)divisor);
	}
}

/*
	DECODE()
	--------
*/
char *decode(long long relevant)
{
if (relevant == 1)
	return "Relevant";
else if (relevant == 0)
	return "Not";
else if (relevant == UNASSESSED_RESULT)
	return "Unassessed Result";
else if (relevant == UNASSESSED_TOPIC)
	return "Unassessed Topic";
else if (relevant == UNSCORED_RESULT)
	return "Skipping";
else if (relevant == DUPLICATE_RESULT)
	return "Duplicate";
else
	return "Unknown";
}

/*
	READ_ENTIRE_FILE()
	------------------
*/
char *read_entire_file(char *filename, long long *file_length)
{
long long unused;
char *block = NULL;
FILE *fp;
struct stat details;

if (filename == NULL)
	return NULL;

if (file_length == NULL)
	file_length = &unused;

if ((fp = fopen(filename, "rb")) == NULL)
	return NULL;

if (fstat(fileno(fp), &details) == 0)
	if ((*file_length = details.st_size) != 0)
		if ((block = new char [(size_t)(details.st_size + 1)]) != NULL)		// +1 for the '\0' on the end
			if (fread(block, (long)details.st_size, 1, fp) == 1)
				block[details.st_size] = '\0';
			else
				{
				delete [] block;
				block = NULL;
				}
fclose(fp);

return block;
}

/*
	BUFFER_TO_LIST()
	----------------
*/
char **buffer_to_list(char *buffer, long long *lines)
{
char *pos, **line_list, **current_line;
long n_frequency, r_frequency;

n_frequency = r_frequency = 0;
for (pos = buffer; *pos != '\0'; pos++)
	if (*pos == '\n')
		n_frequency++;
	else if (*pos == '\r')
		r_frequency++;

*lines = r_frequency > n_frequency ? r_frequency : n_frequency;
current_line = line_list = new char * [(size_t)(*lines + 2)]; 		// +1 in case the last line has no \n; +1 for a NULL at the end of the list

if (line_list == NULL)		// out of memory!
	return NULL;

*current_line++ = pos = buffer;
while (*pos != '\0')
	{
	if (*pos == '\n' || *pos == '\r')
		{
		*pos++ = '\0';
		while (*pos == '\n' || *pos == '\r')
			pos++;
		*current_line++ = pos;
		}
	else
		pos++;
	}
*current_line = NULL;
*lines = current_line - line_list - 1;		// the true number of lines

return line_list;
}

/*
	ASSESSMENT_TOPIC_CMP()
	----------------------
*/
int assessment_topic_cmp(const void *a, const void *b)
{
ltw_assessment *one, *two;

one = (ltw_assessment *)a;
two = (ltw_assessment *)b;

if (one->topic < two->topic)
	return -1;
else if (one->topic > two->topic)
	return 1;
else
	return strcmp(one->target, two->target);
}

/*
	QREL_CMP()
	----------
*/
int qrel_cmp(const void *a, const void *b)
{
int got;
ltw_assessment *one, *two;

one = (ltw_assessment *)a;
two = (ltw_assessment *)b;

if (one->topic < two->topic)
	return -1;
else if (one->topic > two->topic)
	return 1;
else if ((got = strcmp(one->target, two->target)) != 0)
	return got;
else if (one->relevant < two->relevant)
	return 1;
else if (one->relevant > two->relevant)
	return -1;
else
	return 0;
}

/*
	DEDUPLICATE_QRELS()
	-------------------
*/
long long deduplicate_qrels(ltw_assessment *qrels, long long num_qrels)
{
ltw_assessment *current;
long long duplicates;

duplicates = 0;

qsort(qrels, num_qrels, sizeof(*qrels), qrel_cmp);
for (current = qrels + 1; current < qrels + num_qrels; current++)
	{
	if (((current - 1)->topic == current->topic) && ((current - 1)->relevant == 1 && current->relevant == 0) && (strcmp((current - 1)->target, current->target) == 0))
		{
#ifdef NEVER
		/*
			This code writes of the list of duplicate assessments in the assessment file, that is,
			those topic / target pairs that are assessed both relevant and non-relevant at the same time!
		*/
		printf("DUP: %lld %s %lld and %lld %s %lld\n", (current - 1)->topic, (current - 1)->target, (current - 1)->relevant, current->topic, current->target, current->relevant);
#endif
		current->topic = LLONG_MAX;
		duplicates++;
		}
	}

return duplicates;
}

/*
	READ_ASSESSMENTS()
	------------------
*/
ltw_assessment *read_assessments(char *filename, long long *how_many)
{
char *assessment_file, **assessments;
long long assessment_file_length, current, num_assessments, duplicates;
ltw_assessment *qrels;
char *token;

if ((assessment_file = read_entire_file(filename, &assessment_file_length)) == NULL)
	return NULL;

assessments = buffer_to_list(assessment_file, &num_assessments);

qrels = new ltw_assessment[num_assessments + 1];
for (current = 0; current < num_assessments; current++)
	{
	/*
		Format:
			topic 0 docid relevance
	*/
	token = strtok(assessments[current], SEPERATORS);
	qrels[current].topic = atoll(token);			// topic
	
	token = strtok(NULL, SEPERATORS);
	// ignore this token							// ignore

	token = strtok(NULL, SEPERATORS);
	qrels[current].target = token;					// docid

	token = strtok(NULL, SEPERATORS);
	qrels[current].relevant = atoll(token);			// relevance
	}

qrels[current].topic = -1;			// terminate the list

duplicates = deduplicate_qrels(qrels, num_assessments);
qsort(qrels, num_assessments, sizeof(*qrels), assessment_topic_cmp); 

if (duplicates != 0)
	qrels[num_assessments - duplicates].topic = -1;			// terminate the list

*how_many = num_assessments - duplicates;
return qrels;
}

/*
	WRITE_ASSESSMENTS()
	-------------------
*/
void write_assessments(ltw_assessment *qrels, long long num_qrels)
{
ltw_assessment *current;

for (current = qrels; current->topic >= 0; current++)
	printf("%lld 1 %s %lld\n", current->topic, current->target, current->relevant);
}

/*
	RESULT_CMP()
	------------
*/
int result_cmp(const void *a, const void *b)
{
ltw_result *one, *two;

one = (ltw_result *)a;
two = (ltw_result *)b;

if (one->topic < two->topic)
	return -1;
else if (one->topic > two->topic)
	return 1;
else if (one->rsv > two->rsv)
	return -1;
else if (one->rsv < two->rsv)
	return 1;
else if (one->position < two->position)
	return -1;
else if (one->position > two->position)
	return 1;
else
	return strcmp(one->target, two->target);
}

/*
	READ_TREC_RESULTS()
	-------------------
*/
ltw_result *read_trec_results(char *filename, long long *length)
{
char *assessment_file, **results;
long long file_length, results_length, current;
ltw_result *run;
char *token;

if ((assessment_file = read_entire_file(filename, &file_length)) == NULL)
	return NULL;

results = buffer_to_list(assessment_file, &results_length);

run = new ltw_result[results_length + 1];
for (current = 0; current < results_length; current++)
	{
	/*
		Format:
			topic Q0 docid position rsv run_name
	*/
	token = strtok(results[current], SEPERATORS);
	run[current].topic = atoll(token);				// topic
	
	token = strtok(NULL, SEPERATORS);
	// ignore this token							// ignore

	token = strtok(NULL, SEPERATORS);
	run[current].target = token;						// target docid

	token = strtok(NULL, SEPERATORS);
	run[current].position = atoll(token);			// position in results list

	token = strtok(NULL, SEPERATORS);
	run[current].rsv = atof(token);					// rsv

	run[current].relevant = 0;
	if (current == 0)
		{
		token = strtok(NULL, SEPERATORS);
		metric.run_name = strdup(token);
		}
	}

run[current].topic = -1;

/*
	Rank on RSV because that is what TREC_EVAL does
*/
qsort(run, results_length, sizeof(*run), result_cmp);

*length = results_length;
return run;
}

/*
	READ_LTW_RESULTS()
	------------------
*/
ltw_result *read_ltw_results(char *filename, long long *length)
{
char *assessment_file, **results;
long long file_length, results_length, current;
ltw_result *run;
char *token;
long long line, current_result, topic, result, position, had_beps;
char *destination, *destination_end;
char *run_name, *end_run_name;
static const long long HIGHEST_RESULT (1024 * 1024);


if ((assessment_file = read_entire_file(filename, &file_length)) == NULL)
	return NULL;

results = buffer_to_list(assessment_file, &results_length);

run = new ltw_result[results_length + 1];
metric.run_name = NULL;

line = current_result = 0;
for (current = 0; current < results_length; current++)
	{
	line++;

	if (metric.run_name == NULL && (run_name = strstr(results[current], " run-id=")) != NULL)
		{
		run_name = strchr(run_name, '=') + 2;
		end_run_name = strchr(run_name, '"');
		metric.run_name = (char *)malloc(end_run_name - run_name + 1);
		strncpy(metric.run_name, run_name, end_run_name - run_name);
		metric.run_name[end_run_name - run_name] = '\0';
		}
	else if (strstr(results[current], "<topic") != NULL)
		{
		topic = atoll(strpbrk(results[current], "1234567890"));
		result = 1;
		position = 1;
		had_beps = false;
		}
	else if (strstr(results[current], "<anchor") != NULL)
		{
		if (had_beps)			// some runs (QUT's runs) have <anchor> tags without <tobep> tags!
			result++;
		}
	else if (strstr(results[current], "<tobep") != NULL)
		{
		had_beps = true;
		position++;
		destination = strchr(results[current], '>') + 1;
		destination_end = strchr(destination, '<');
		*destination_end = '\0'; 
		if (topic < 0)
			printf("Error line %lld: topic id not set\n", line);
		else
			{
			run[current_result].topic = topic;
			run[current_result].target = destination;
			run[current_result].position = result;
			run[current_result].rsv = (double)(HIGHEST_RESULT - position);
			current_result++;
			}
		}
	}

run[current_result].topic = -1;

/*
	Rank on RSV because that is what TREC_EVAL does
*/
qsort(run, current_result, sizeof(*run), result_cmp);

*length = current_result;
return run;
}

/*
	GENERATE_TOPIC_STATS()
	----------------------
*/
ltw_topic_details *generate_topic_stats(ltw_assessment *assessment, long long assessment_length, long long *number_of_topics)
{
ltw_assessment *current;
long long last, unique, grand_total_relevant;
ltw_topic_details *topic_details, *current_topic;

qsort(assessment, assessment_length, sizeof(*assessment), assessment_topic_cmp);

last = -1;
unique = grand_total_relevant = 0;
for (current = assessment; current->topic >= 0; current++)
	{
	if (current->topic != last)
		unique++;
	last = current->topic;
	}

topic_details = new ltw_topic_details[unique + 1];

last = -1;
current_topic = topic_details - 1;
for (current = assessment; current->topic >= 0; current++)
	{
	if (current->topic != last)
		{
		current_topic++;
		current_topic->topic = current->topic;
		current_topic->relevant = current_topic->non_relevant = 0;
		current_topic->used_in_evaluation = 0;
		}

	if (current->relevant)
		{
		current_topic->relevant++;
		grand_total_relevant++;
		}
	else
		current_topic->non_relevant++;

	last = current->topic;
	}

metric.assessed_topics = unique;
metric.num_rel = grand_total_relevant;

*number_of_topics = unique;
return topic_details;
}

/*
	RESULT_ASSESSMENT_CMP()
	-----------------------
*/
int result_assessment_cmp(const void *a, const void *b)
{
ltw_result *one;
ltw_assessment *two;

one = (ltw_result *)a;
two = (ltw_assessment *)b;

if (one->topic < two->topic)
	return -1;
else if (one->topic > two->topic)
	return 1;
else
	return strcmp(one->target, two->target);
}

/*
	RESULT_TOPIC_CMP()
	------------------
*/
int result_topic_cmp(const void *a, const void *b)
{
ltw_result *one;
ltw_topic_details *two;

one = (ltw_result *)a;
two = (ltw_topic_details *)b;

return one->topic < two->topic ? -1 : one->topic == two->topic ? 0 : 1;
}

/*
	EVALUATE_RUN()
	--------------
	MODE:
		TREC_EVAL = assess as if the results list is a list
		TOP_EVAL  = assess only the first result at each position in the results list
*/
void evaluate_run(ltw_topic_details *topic_stats, long long num_topics, ltw_assessment *assessments, long long num_assessments, ltw_result *run, long long run_length, long long mode)
{
ltw_result *current;
ltw_assessment *found;
long long previous_topic, previous_position, assess;

previous_topic = previous_position = -1;

for (current = run; current->topic >= 0; current++)
	{
	assess = false;
	if ((mode & TREC_EVAL) != 0)
		assess = true;
	else if ((mode & TOP_EVAL) != 0)
		{
		if (previous_topic != current->topic || previous_position != current->position)
			assess = true;
		}
	else
		printf("Unknown assessment mode\n");

	if (assess)
		{
		if ((found = (ltw_assessment *)bsearch(current, assessments, num_assessments, sizeof(*assessments), result_assessment_cmp)) == NULL)
			{
			current->relevant = UNASSESSED_RESULT;			// unassessed result for this topic
			if (bsearch(current, topic_stats, num_topics, sizeof(*topic_stats), result_topic_cmp) == NULL)
				current->relevant = UNASSESSED_TOPIC;
			}
		else
			{
			if (found->relevant > 0)
				current->relevant = 1;		// relevant
			else
				current->relevant = 0;		// not-relevant
			}
		}
	else
		current->relevant = UNSCORED_RESULT;

	previous_topic = current->topic;
	previous_position = current->position;
#ifdef NEVER
	/*
		This code dumps the line-by-line relevant / non-relevance for the given run
	*/
	printf("%lld Q0 %s %lld %s\n", current->topic, current->target, current->position, decode(current->relevant));
#endif
	}
}

/*
	TOPIC_CMP()
	-----------
*/
int topic_cmp(const void *a, const void *b)
{
long long topic;
ltw_topic_details *element;

topic = *(long long *)a;
element = (ltw_topic_details *)b;

return topic > element->topic ? 1 : topic < element->topic ? -1 : 0;
}

/*
	COMPUTE_MAP()
	-------------
*/
double compute_map(ltw_result *run, long long run_length, ltw_topic_details *topic_stats, long long num_topics, long long mode)
{
ltw_topic_details *current_topic_stat;
ltw_result *current;
long long last_topic, skip_topic;
long long topics_in_run, assessed_topics_in_run, total_relevant;
long long relevant_documents_for_current_topic;
long long grand_total_found_and_relevant;
long long point, position_in_results_list, position_to_use;
double topic_sum_of_precisions, precision_at_document, run_sum_of_precisions;
double recall_point, precision_at_recall[11];
double sum_recip_rank, recip_rank;

memset(metric.precision_at_recall, 0, sizeof(metric.precision_at_recall));
memset(precision_at_recall, 0, sizeof(precision_at_recall));
grand_total_found_and_relevant = 0;
assessed_topics_in_run = topics_in_run = 0;
last_topic = -1;
run_sum_of_precisions = topic_sum_of_precisions = 0.0;
total_relevant = 0;
relevant_documents_for_current_topic = 0;
sum_recip_rank = recip_rank = 0.0;
position_in_results_list = 0;
for (current = run; current->topic >= 0; current++)
	{
	if (current->topic != last_topic)
		{
		if (last_topic != -1)
			{
			if (relevant_documents_for_current_topic != 0)
				{
				run_sum_of_precisions += topic_sum_of_precisions / (double)relevant_documents_for_current_topic;
				for (point = 0; point <= 10; point++)
					metric.precision_at_recall[point] += precision_at_recall[point];
				}
#ifdef NEVER
			/*
				This code writes out the topic-running sum of precisions
			*/
			printf("topic sum: %0.4f relevant_docs:%lld Run Sum:%0.4f\n", topic_sum_of_precisions, relevant_documents_for_current_topic, run_sum_of_precisions);

			for (point = 0; point <= 10; point++)
				printf("P @ R=%0.2f: %0.4f\n", (double)point / 10.0, precision_at_recall[point]);
#endif
			}
		position_in_results_list = 0;
		total_relevant = 0;
		recip_rank = 0.0;
		memset(precision_at_recall, 0, sizeof(precision_at_recall));
		topic_sum_of_precisions = 0.0;
		topics_in_run++;
		if (current->relevant != UNASSESSED_TOPIC)
			assessed_topics_in_run++;

		if ((current_topic_stat = (ltw_topic_details *)bsearch(&current->topic, topic_stats, num_topics, sizeof(*topic_stats), topic_cmp)) != NULL)
			{
			current_topic_stat->used_in_evaluation++;
			relevant_documents_for_current_topic = current_topic_stat->relevant;
			}
		else
			relevant_documents_for_current_topic = 0;
		}

	position_in_results_list++;

	if (current->relevant > 0)
		{
		grand_total_found_and_relevant++;
		total_relevant++;
		/*
			Here we choose whether to use the position in the results list as found by sorting on rsv
			or alternatively on the position in the results list as stated by the run itself.
		*/
		if ((mode & TREC_EVAL) != 0)
			position_to_use = position_in_results_list;
		else if ((mode & TOP_EVAL) != 0)
			position_to_use = current->position;
		else
			printf("Unknown assessment mode\n");


		if (recip_rank == 0.0)
			{
			recip_rank = 1.0 / (double)position_to_use;
			sum_recip_rank += recip_rank;
			}
		topic_sum_of_precisions += precision_at_document = (double)total_relevant / (double)position_to_use;

		recall_point = (double)total_relevant / (double)relevant_documents_for_current_topic;
		if (recall_point > 1)
			{
			/*
				This happens when the document contains more anchors and relevant targets then there are
				relevant targets.  Remember that each target is only relevant once but can be the destination
				of more than one anchor (and to total_relevant can be greater than relevant_documents_for_current_topic)
			*/
			printf("Warning Recall > 1.0 [Topic:%lld position:%lld] rel_ret=%lld rel=%lld\n", current->topic, current->position, total_relevant, relevant_documents_for_current_topic);
			recall_point = 1;
			}

		for (point = (long long)(recall_point * 10.0); point >= 0; point--)
			if (precision_at_recall[point] < precision_at_document)
				precision_at_recall[point] = precision_at_document;
			else
				break;

#ifdef NEVER
		/*
			This code writes out the cumulateive totals of the precision per result in the results list
		*/
		printf("topic:%lld target:%s %s [precision:%lld/%lld (%0.4f) precision sum:%0.4f Recall:%0.4f, RR:%0.4f, sumRR:%0.4f]\n", 
			current->topic, 
			current->target, 
			decode(current->relevant),
			total_relevant, 
			position_to_use, 
			precision_at_document, 
			topic_sum_of_precisions,
			recall_point,
			recip_rank,
			sum_recip_rank);
#endif
		}
	last_topic = current->topic;
	}


if (relevant_documents_for_current_topic != 0)
	{
	run_sum_of_precisions += topic_sum_of_precisions / (double)relevant_documents_for_current_topic;
	for (point = 0; point <= 10; point++)
		metric.precision_at_recall[point] += precision_at_recall[point];

#ifdef NEVER
	/*
		This code writes out the topic-running sum of precisions
	*/
	printf("topic sum: %0.4f relevant_docs:%lld Run Sum:%0.4f\n", topic_sum_of_precisions, relevant_documents_for_current_topic, run_sum_of_precisions);

	for (point = 0; point <= 10; point++)
		printf("P @ R=%0.2f: %0.4f\n", (double)point / 10.0, precision_at_recall[point]);
#endif
	}

metric.run_sum_of_precisions = run_sum_of_precisions;
metric.run_topics = topics_in_run;
metric.assessed_run_topics = assessed_topics_in_run;
metric.num_ret_rel = grand_total_found_and_relevant;
metric.num_ret = run_length;
metric.sum_recip_rank = sum_recip_rank;

return 0.0;
}

/*
	COMPUTE_NUM_REL()
	-----------------
*/
void compute_num_rel(ltw_topic_details *topic_stats, long long num_topics)
{
ltw_topic_details *current;
long long topics_with_relevant;
long long query_topics_with_relevant;
long long topics_in_qrels;
long long topics_in_queries;

topics_in_qrels = topics_in_queries = query_topics_with_relevant = topics_with_relevant = 0;

metric.num_rel = 0;
for (current = topic_stats; current < topic_stats + num_topics; current++)
	{
	topics_in_qrels++;
	if (current->relevant != 0)
		topics_with_relevant++;
	if (current->used_in_evaluation)
		{
		topics_in_queries++;
		metric.num_rel += current->relevant;
		if (current->relevant != 0)
			query_topics_with_relevant++;
		}
	}

/*
*/
metric.assessed_topics = topics_with_relevant;
metric.assessed_run_topics = query_topics_with_relevant;
/*
*/
metric.assessed_topics = topics_in_qrels;
metric.assessed_run_topics = topics_in_queries;
}

/*
	PUT_RELEVANT_TOP_CMP()
	----------------------
*/
int put_relevant_top_cmp(const void *a, const void *b)
{
ltw_result *one, *two;

one = (ltw_result *)a;
two = (ltw_result *)b;

if (one->topic < two->topic)
	return -1;
else if (one->topic > two->topic)
	return 1;
else if (one->position < two->position)
	return -1;
else if (one->position > two->position)
	return 1;
else if (one->relevant > two->relevant)
	return -1;
else if (one->relevant < two->relevant)
	return 1;
else return strcmp(one->target, two->target);
}

/*
	DEDUP_CMP()
	-----------
*/
int dedup_cmp(const void *a, const void *b)
{
ltw_result *one, *two;
int got;

one = (ltw_result *)a;
two = (ltw_result *)b;

if (one->topic < two->topic)
	return -1;
else if (one->topic > two->topic)
	return 1;
else
	{
	got = strcmp(one->target, two->target);
	if (got != 0)
		return got;
	else if (one->position < two->position)
		return -1;
	else if (one->position > two->position)
		return 1;
	else
		return 0;
	}
}

/*
	WRITE_RUN()
	-----------
*/
void write_run(ltw_result *run, long long run_length)
{
ltw_result *current;
long long previous_topic, previous_position;
char *previous_target;

/*
	A document is only relevant the first time its seen in the results list
*/
qsort(run, run_length, sizeof(*run), dedup_cmp);

previous_topic = -1;
previous_target = "";
for (current = run; current->topic >= 0; current++)
	{
	if (previous_topic == current->topic && strcmp(previous_target, current->target) == 0)
		current->relevant = DUPLICATE_RESULT;
	else
		current->relevant = UNASSESSED_TOPIC;

	previous_topic = current->topic;
	previous_target = current->target;
	}

qsort(run, run_length, sizeof(*run), put_relevant_top_cmp);

for (current = run; current->topic >= 0; current++)
	if (current->relevant != DUPLICATE_RESULT)
		printf("%lld Q0 %s %lld %0.4f %s\n", current->topic, current->target, current->position, current->rsv, metric.run_name);
}

/*
	CONVERT_TO_BEST_EVAL()
	----------------------
*/
void convert_to_best_eval(ltw_result *run, long long run_length)
{
ltw_result *current;
long long previous_topic, previous_position;
char *previous_target;

/*
	A document is only relevant the first time its seen in the results list
*/
qsort(run, run_length, sizeof(*run), dedup_cmp);

previous_topic = -1;
previous_target = "";
for (current = run; current->topic >= 0; current++)
	{
	if (previous_topic == current->topic && strcmp(previous_target, current->target) == 0)
		current->relevant = DUPLICATE_RESULT;

	previous_topic = current->topic;
	previous_target = current->target;
	}

/*
	Now push all the relevant documents to the top of each rank position
	remember that duplicates are only relevant the first time so will drop to the
	bottom if a second relevant target for the same anchor is seen later in the resutls list
*/
qsort(run, run_length, sizeof(*run), put_relevant_top_cmp);			// sort the results list to put the relevant docs at the top of each position

previous_topic = previous_position = -1;
for (current = run; current->topic >= 0; current++)
	{
	if (previous_topic == current->topic && previous_position == current->position)
		if (current->relevant > 0)
			{
#ifdef NEVER
			/*
				this code write out those result that change (i.e those where there are more than one relevant result per rank position
			*/
			printf("%lld Q0 %s %lld %s -> %lld Q0 %s %lld %s\n", current->topic, current->target, current->position, decode(current->relevant), current->topic, current->target, current->position, decode(0));
#endif
			current->relevant = 0;
			}

	previous_topic = current->topic;
	previous_position = current->position;
	}
}

/*
	USAGE()
	-------
*/
long usage(char *exename)
{
printf("Usage:%s [-c | -C] [-dumpassessments -dumprun] [-topeval | -treceval | -besteval] [-trecfile | -ltwfile] <assessments> <run>\n", exename);
printf("      -c               : average over the number topics in the assessments (that have +ve assessments)\n");
printf("      -C               : average over the number assessed topics in the run (that have +ve assessments) [default]\n");
printf("      -dumpassessments : clean the assessments set, write to stdout, terminate\n");
printf("      -dumprun         : deduplicate a run, and write to stdout, for checking with trec_eval\n");
printf("      -topeval         : for multiple documents at the same rank use top rsv result only\n");
printf("      -treceval        : for multiple documsnts at the same rank use pos in results list [default]\n");
printf("      -besteval        : for multiple documents at the same rank use the best result\n");
printf("      -trecfile        : the run is in TREC format\n");
printf("      -ltwfile         : the run is in INEX 2010 Link-the-Wiki format\n");
printf("      -tight           : print run_name map and iprec_at_recall[0.00-1.00] all on the same line\n");

return 0;
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
char **next_arg;
ltw_assessment *assessments;
ltw_result *run;
ltw_topic_details *topic_details;
long long number_of_assessments, run_length, unique_assessed_topics;
long long dump_assessments = false, dump_run = false;
long long evaluation_mode, map_mode;
long long run_fileformat;

if (argc < 3)
	exit(usage(argv[0]));

evaluation_mode = map_mode = TREC_EVAL;
run_fileformat = RUN_TREC;
for (next_arg = argv + 1; next_arg < argv + argc && **next_arg == '-'; next_arg++)
	{
	if (strcmp(*next_arg, "-c") == 0)
		minus_c = true;
	else if (strcmp(*next_arg, "-C") == 0)
		minus_c = false;
	else if (strcmp(*next_arg, "-dumpassessments") == 0)
		dump_assessments = true;
	else if (strcmp(*next_arg, "-dumprun") == 0)
		dump_run = true;
	else if (strcmp(*next_arg, "-topeval") == 0)
		 evaluation_mode = map_mode = TOP_EVAL;
	else if (strcmp(*next_arg, "-treceval") == 0)
		evaluation_mode = map_mode = TREC_EVAL;
	else if (strcmp(*next_arg, "-besteval") == 0)
		{
		evaluation_mode = BEST_EVAL | TREC_EVAL;
		map_mode = BEST_EVAL | TOP_EVAL;
		}
	else if (strcmp(*next_arg, "-trecfile") == 0)
		run_fileformat = RUN_TREC;
	else if (strcmp(*next_arg, "-ltwfile") == 0)
		run_fileformat = RUN_LTW;
	else if (strcmp(*next_arg, "-tight") == 0)
		tight = true;
	else if (**next_arg == '-')
		exit(usage(argv[0]));
	}

if ((assessments = read_assessments(next_arg[0], &number_of_assessments)) == NULL)
	exit(printf("Cannot read assessment file:%s\n", next_arg[0]));


if (run_fileformat == RUN_TREC)
	run = read_trec_results(next_arg[1], &run_length);
else
	run = read_ltw_results(next_arg[1], &run_length);
if (run == NULL)
	exit(printf("Cannot read run file:%s\n", next_arg[1]));


if (dump_assessments)
	{
	write_assessments(assessments, number_of_assessments);
	exit(0);
	}

if (dump_run)
	{
	write_run(run, run_length);
	exit(0);
	}

topic_details = generate_topic_stats(assessments, number_of_assessments, &unique_assessed_topics);

#ifdef NEVER
/*
	This code dumps stats about the number of relevant and non-relevant documents per topic
*/
for (long long ch = 0; ch < unique_assessed_topics; ch++)
	printf("%lld %lld %lld\n", topic_details[ch].topic, topic_details[ch].relevant, topic_details[ch].non_relevant);
#endif

evaluate_run(topic_details, unique_assessed_topics, assessments, number_of_assessments, run, run_length, evaluation_mode);

if ((map_mode & BEST_EVAL) != 0)
	convert_to_best_eval(run, run_length);

compute_map(run, run_length, topic_details, unique_assessed_topics, map_mode);
compute_num_rel(topic_details, unique_assessed_topics);

metric.text_render();

return 0;
}
