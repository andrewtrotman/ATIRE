/*
	ANT_PARAM_BLOCK.C
	-----------------
*/
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "str.h"
#include "ant_param_block.h"
#include "version.h"
#include "stemmer_factory.h"
#include "relevance_feedback_factory.h"
#include "atire_api.h"
#include "search_engine_accumulator.h"
#include "snippet_factory.h"
#include "thesaurus.h"
#include "evaluator.h"
#include "ranking_function_factory_object.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_ANT_PARAM_BLOCK::ANT_ANT_PARAM_BLOCK()
	------------------------------------------
*/
ANT_ANT_param_block::ANT_ANT_param_block(int argc, char *argv[])
{
port = 0;
this->argc = argc;
this->argv = argv;
logo = TRUE;
sort_top_k = LLONG_MAX;
focus_top_k = 2000;
evaluator = new ANT_evaluator;
assessments_filename = NULL;
queries_filename = NULL;
query_fields = "t";
output_forum = NONE;
run_name = participant_id = "unknown";
output_filename = "ant.out";
results_list_length = -1;
stats = SHORT | PRECISION;
segmentation = TRUE;
trim_postings_k = LONG_MAX;
file_or_memory = INDEX_IN_FILE;
focussing_algorithm = NONE;
feedbacker = NONE;
query_type = ATIRE_API::QUERY_NEXI;
query_stopping = NONE;
feedback_documents = 17;
feedback_terms = 5;
feedback_lambda = 0.5;
index_filename = strnew("index.aspt");
doclist_filename = strnew("doclist.aspt");
pregen_count = 0;
pregen_ratio = 1.0;
snippet_algorithm = NONE;
snippet_tag = "title";
snippet_length = 300;		// this is the INEX 2011 maximum snippet length
snippet_stemmer = NONE;
snippet_word_cloud_terms = 40;
title_tag = "title";
title_algorithm = NONE;
title_length = 300;
expander_tf_types = ANT_thesaurus_relationship::SYNONYM;
expander_query_types = ANT_thesaurus_relationship::SYNONYM;
processing_strategy = TERM_AT_A_TIME;								// term at a time by default
quantization = false; // by default don't quantize
quantization_bits = -1; // by default use the maths to calculate the bits we need
quantum_stopping = QUANTUM_STOP_NONE;
}

/*
	ANT_ANT_PARAM_BLOCK::~ANT_ANT_PARAM_BLOCK()
	-------------------------------------------
*/
ANT_ANT_param_block::~ANT_ANT_param_block()
{
delete evaluator;
delete [] doclist_filename;
delete [] index_filename;

for (int i = 0; i < pregen_count; i++)
	delete [] pregen_names[i];
}

/*
	ANT_ANT_PARAM_BLOCK::USAGE()
	----------------------------
*/
void ANT_ANT_param_block::usage(void)
{
printf("Usage:%s [option...]\n", argv[0]);
printf("     : -? for help\n");
exit(0);
}

/*
	ANT_ANT_PARAM_BLOCK::HELP()
	---------------------------
*/
void ANT_ANT_param_block::help(void)
{
puts(ANT_version_string);
puts("");

puts("GENERAL");
puts("-------");
puts("-? -h -H        Display this help message");
puts("-nologo         Suppress banner");
puts("-people         Display credits");
puts("");

puts("FILE HANDLING");
puts("-------------");
puts("-findex <fn>    Filename of index");
puts("-fdoclist <fn>  Filename of doclist");
puts("-a<filenane>    Topic assessments are in <filename> (formats: ANT, TREC, INEX 2008)");
puts("-q<filename>    Queries are in file <filename> (format: ANT, TREC, INEX 2008)");
puts("-q:<port>       ANT SERVER:Queries from TCP/IP port <port> [default=8088]");
puts("");

ANT_indexer_param_block_stem::help(TRUE);		// stemmers

puts("QUERY TYPE");
puts("----------");
puts("-Q[s][nbt][-rmT][NI][wW][R]Query type");
puts("  n             NEXI [default]");
puts("  b             Boolean");
puts("  N:<t><n><d>   NIST (TREC) query file (from trec.nist.gov) <t>itle, <n>arrative, <d>escription [default=t]");
puts("  I:<t><c><n><d>INEX query file (from inex.otago.ac.nz) <t>itle, <c>astitle, <n>arrative, <d>escription [default=t]");
puts("  s:<n><p><0><s><a>Stopword the query: <p>uurula's 988 list, <n>cbi 313 list, <0>numbers, or <s>hort (>=2) words, <a>tire extensions [default=false]");
puts("  t:<w>:<d>:<f> TopSig index of width <w> bits, density <d>%, and globalstats <f>");
puts("  -             No relevance feedback [default]");
puts("  r:<d>:<t>     Rocchio blind relevance feedback by analysing <d> top documents and extracting <t> terms [default d=17 t=5]");
puts("  m:<d>:<l>     Relevance Model feedback (Puurula ALATA paper) using top <d> documents and lambda=l [default d=17 l=0.5]");
puts("  R<ranker>     Use <ranker> as the relevance feedback ranking function (<ranker> is a valid RANKING FUNCTION, excludes pregen)");
puts("  T:<d>         TopSig blind relevance feedback, analysing <d> top documents [default d=10]");
puts("  w:<t>         WordNet tf-merging (wordnet.aspt) <t>=[<s>ynonym <a>ntonym <h>olonym <m>eronym hyp<o>nym hyp<e>rnym][default=s]");
puts("  W:<t>         WordNet query expansion (wordnet.aspt) <t>=[<s>ynonym <a>ntonym <h>olonym <m>eronym hyp<o>nym hyp<e>rnym][default=s]");
puts("");

puts("OPTIMISATIONS");
puts("-------------");
puts("-k<n>           Top-k search. Document results list accurate to the top <n> (0=all) [default=0]");
puts("-F<n>           Focus-k. Focused results list accurate to the top <n> [default=2000]");
puts("-K<n>           Static pruning. Process no fewer than <n> postings (0=all) [default=0]");
puts("-M              Load the index into memory at startup");
puts("-Pt             Process postings lists term-at-a-time [default]");
puts("-Pq:[ndsl]      Process postings lists quantum-at-a-time");
puts("  n             no early termination for the quantum-at-a-time approach [default]");
puts("  d             early termination based on the difference between the top k and k+1");
puts("  s             early termination based on the smallest difference among the top documents");
puts("  l             early termination based on the difference between the largest and second-largest documents");
puts("");

ANT_evaluator::help("METRICS", 'm'); // metrics

puts("TREC / INEX SPECIFIC");
puts("--------------------");
puts("-e[-Iifbet]     Export a run file for use in an Evaluation Forum");
puts("  -             Don't generate a run file [default]");
puts("  I             INEX 2008 (XML) run format");
puts("  i             INEX 2009 (TREC++) run format (documents only)");
puts("  f             INEX 2009 (TREC++) run format (documents and passages)");
puts("  b             INEX 2009 (TREC++) run format (best entry point)");
puts("  e             INEX 2009 Efficiency (XML) run format");
puts("  t             TREC run format");
puts("-o<filename>    Output filename for the run [default=ant.out]");
puts("-i<id>          Forum participant id is <id> [default=unknown]");
puts("-n<name>        Run is named <name> [default=unknown]");
puts("-l<n>           Length of the results list [default=1500 for batch, default=10 for interactive)]");
puts("-QN:<t><n><d>   NIST (TREC) query file (from trec.nist.gov) <t>itle, <n>arrative, <d>escription [default=t]");
puts("-QI:<t><c><n><d>INEX query file (from inex.otago.ac.nz) <t>itle, <c>astitle, <n>arrative, <d>escription [default=t]");

puts("");

puts("SEGMENTATION");
puts("------------");
puts("-S[n]           East-Asian language word segmentation, query is segmented into characters by default");
puts("  n             No segmentation, search with the input terms separated by space");
puts("");

puts("PREGENERATED RANK ORDERS");
puts("------------------------");
puts("-pregen name    Load pregen file with given field name on startup");
puts("");

ANT_indexer_param_block_rank::help("RANKING FUNCTION", 'R', ANT_ranking_function_factory_object::INDEXABLE | ANT_ranking_function_factory_object::NONINDEXABLE);		// ranking functions
puts("-r[n]           Quantize search results in n bits [default n=maths!]");
puts("");

puts("FOCUSED AND SNIPPET RETRIEVAL");
puts("-----------------------------");
puts("-f[a][cC][s<stemmer>][-fbBtT<tag>][wnN<n>]Focus the results list");
puts("  a             Article retrieval [default]");
puts("  r             Range retrieval: Start tag before the first occurence to end tag after the last");
puts("  -             No snippets [default]");
puts("  b<tag>        Snippet is the <tag> element with the highest term count");
puts("  B<tag>        Snippet is the <tag> element with the tf.icf score");
puts("  c             Snippet is the passage with the highest term count");
puts("  C             Snippet is the passage with the tf.icf score");
puts("  t<tag>        Snippet is the contents of the first occuernce of the <tag> element [default=title]");
puts("  f<tag>        Snippet is the text immediately following the first <tag> element [default=title]");
puts("  n<n>          Snippet maximum length is <n> characters [default=300]");
puts("  w<n>          Snippet is a KL-divergence word cloud with at most <n> terms in it [default=40]");
puts("  S<stemmer>    Use stemming in snippet generation (except clouds) <stemmer> is a valid TERM EXPANSION algorithm (e.g. -fSp)");
puts("  T<tag>        Title is the contents of the first occuernce of the <tag> element [default=title]");
puts("  N<n>          Title maximum length is <n> characters [default=300]");
puts("");

puts("REPORTING");
puts("---------");
puts("-s[-aqQs]       Report statistics");
puts("  -             No statistics");
puts("  a             All statistics (same as -sqQs)");
puts("  p             Mean precision scores (if computed)");
puts("  q             Query by query statistics");
puts("  Q             Sum of query by query statistics for this run");
puts("  s             Short reporting (hits, average precision, etc) [default]");
puts("");

exit(0);
}

/*
	ANT_ANT_PARAM_BLOCK::SET_STATS()
	--------------------------------
*/
void ANT_ANT_param_block::set_stats(char *which)
{
stats = 0;
do
	{
	switch (*which)
		{
		case '-' : stats = NONE;					break;
		case 'a' : stats |= QUERY | SUM | SHORT | PRECISION;	break;
		case 'p' : stats |= PRECISION;				break;
		case 'q' : stats |= QUERY;					break;
		case 'Q' : stats |= SUM;    				break;
		case 's' : stats |= SHORT;    				break;
		default : exit(printf("Unknown stat: '%c'\n", *which)); break;
		}
	which++;
	}
while (*which != '\0');
}

/*
	ANT_ANT_PARAM_BLOCK::SET_INDEX_FILENAME()
	-----------------------------------------
*/
void ANT_ANT_param_block::set_index_filename(char *filename)
{
delete [] index_filename;
index_filename = strnew(filename);
}

/*
	ANT_ANT_PARAM_BLOCK::SET_DOCLIST_FILENAME()
	-------------------------------------------
*/
void ANT_ANT_param_block::set_doclist_filename(char *filename)
{
delete [] doclist_filename;
doclist_filename = strnew(filename);
}

/*
	ANT_ANT_PARAM_BLOCK::SWAP_INDEX_FILENAME()
	------------------------------------------

	Set the index filename to point to the given string (allocated with new char[]), and return the pointer
	to the old string (you're responsible for the memory management of the returned string, allocated
	with new char[])
*/
char *ANT_ANT_param_block::swap_index_filename(char *filename)
{
char *result = index_filename;

index_filename = filename;
return result;
}

/*
	ANT_ANT_PARAM_BLOCK::SWAP_DOCLIST_FILENAME()
	--------------------------------------------
*/
char *ANT_ANT_param_block::swap_doclist_filename(char *filename)
{
char *result = doclist_filename;

doclist_filename = filename;
return result;
}

/*
	ANT_ANT_PARAM_BLOCK::DECODE_EXPANSION_TYPES()
	---------------------------------------------
*/
unsigned long ANT_ANT_param_block::decode_expansion_types(char *which)
{
unsigned long answer = 0;

while (*which != '\0')
	{
	switch (*which)
		{
		case ':':
			break;
		case 'a':
			answer |= ANT_thesaurus_relationship::ANTONYM;
			break;
		case 'm':
			answer |= ANT_thesaurus_relationship::MERONYM;
			break;
		case 's':
			answer |= ANT_thesaurus_relationship::SYNONYM;
			break;
		case 'h':
			answer |= ANT_thesaurus_relationship::HOLONYM;
			break;
		case 'o':
			answer |= ANT_thesaurus_relationship::HYPONYM;
			break;
		case 'e':
			answer |= ANT_thesaurus_relationship::HYPERNYM;
			break;
		default:
			exit(printf("Unknown type in query expansion '%c'\n", *which));
			break;
		}
	which++;
	}

return answer;
}

/*
	ANT_ANT_PARAM_BLOCK::SET_FEEDBACKER()
	-------------------------------------
*/
void ANT_ANT_param_block::set_feedbacker(char *which)
{
char *fields, *check;
double first, second;
long done;
const long perform_query_mask = ATIRE_API::QUERY_BOOLEAN | ATIRE_API::QUERY_NEXI | ATIRE_API::QUERY_TOPSIG | ATIRE_API::QUERY_TREC_FILE | ATIRE_API::QUERY_INEX_FILE;

do
	{
	done = FALSE;
	switch (*which)
		{
		case 's':
			which++;
			if (*which != ':')
				exit(printf("':' expected in stop word selection parameter\n"));

			query_stopping = NONE;
			for (which++; *which != '\0'; which++)
				{
				switch (*which)
					{
					case '0':
						query_stopping |= STOPWORDS_NUMBERS;
						break;
					case 's':
						query_stopping |= STOPWORDS_SHORT;
						break;
					case 'n':
						query_stopping |= STOPWORDS_NCBI;
						break;
					case 'p':
						query_stopping |= STOPWORDS_PUURULA;
						break;
					case 'a':
						query_stopping |= STOPWORDS_ATIRE;
						break;
					default:
						exit(printf("Unknown stopword parameter:'%c'\n", *which));
					}
				}
			if ((query_stopping & STOPWORDS_NCBI) && (query_stopping & STOPWORDS_PUURULA))
				exit(printf("Can't use both the NCBI and Puurula stop word list, choose one or the other\n"));
			if ((query_stopping & STOPWORDS_ATIRE) != 0 && (query_stopping & (STOPWORDS_PUURULA | STOPWORDS_NCBI)) == 0)
				exit(printf("The ATIRE ammendmenst must be used with either the NCBI or Puurula stop word list\n"));
			done = true;
			break;
		case 'n':
			query_type &= ~perform_query_mask;
			query_type |= ATIRE_API::QUERY_NEXI;
			break;
		case 'b':
			query_type &= ~perform_query_mask;
			query_type |= ATIRE_API::QUERY_BOOLEAN;
			break;
		case 'N':
		case 'I':
			query_type &= ~perform_query_mask;
			if (*which == 'N')
				query_type |= ATIRE_API::QUERY_TREC_FILE | ATIRE_API::QUERY_NEXI;
			else
				query_type |= ATIRE_API::QUERY_INEX_FILE | ATIRE_API::QUERY_NEXI;

			fields = strchr(which, ':');
			if (fields == NULL)
				query_fields = "t";
			else
				{
				fields++;
				if (*fields == '\0')
					query_fields = "t";
				else
					{
					for (check = fields; *check != '\0'; check++)
						if (strchr(*which == 'N' ? "tdn" : "tcdn", *check) == NULL)
							exit(printf("Unknown field combination to extract from query file:%s\n", fields));
					query_fields = fields;
					}
				}
			done = TRUE;
			break;
		case '-':
			query_type &= ~ATIRE_API::QUERY_FEEDBACK;
			feedbacker = ANT_relevance_feedback_factory::NONE;
			break;
		case 't':
			query_type &= ~perform_query_mask;
			query_type = ATIRE_API::QUERY_TOPSIG;
			topsig(*(which + 1) == '\0' ? which + 1 : which + 2);
			done = TRUE;
			break;
		case 'r':
			if (query_type == ATIRE_API::QUERY_TOPSIG)
				exit(printf("Cannot do Rocchio with TopSig"));

			query_type |= ATIRE_API::QUERY_FEEDBACK;
			feedbacker = ANT_relevance_feedback_factory::BLIND_KL;
			first = second = -1;
			get_two_parameters(which + 1, &first, &second);
			if (first != -1)
				feedback_documents = (long)first;
			if (second != -1)
				feedback_terms = (long)second;
			done = TRUE;
			break;
		case 'm':
			if (query_type == ATIRE_API::QUERY_TOPSIG)
				exit(printf("Cannot do RM with TopSig"));

			query_type |= ATIRE_API::QUERY_FEEDBACK;
			feedbacker = ANT_relevance_feedback_factory::BLIND_RM;
			first = second = -1;
			get_two_parameters(which + 1, &first, &second);
			if (first != -1)
				feedback_documents = (long)first;
			if (second != -1)
				feedback_lambda = second;
			done = TRUE;
			break;
		case 'R':
			if (!set_ranker(which + 1, TRUE))
				exit(printf("Bad feedback ranking function or ranking parameters '%s'\n", which + 1));
			done = TRUE;
			break;
		case 'T':
			if ((query_type & ATIRE_API::QUERY_TOPSIG) == 0)
				exit(printf("Can only use TopSig feedback with TopSig"));

			query_type |= ATIRE_API::QUERY_FEEDBACK;
			feedbacker = ANT_relevance_feedback_factory::TOPSIG;
			first = -1;
			get_one_parameter(which + 1, &first);
			if (first != -1)
				feedback_documents = (long)first;
			done = TRUE;
			break;
		case 'w':
			/*
				Query expansion (inplace) with wordnet.  This is done like stemming by treating the expanded
				query terms as adding to the term_frequency of the original term rather than just adding words
				to the query.
			*/
			query_type |= ATIRE_API::QUERY_EXPANSION_INPLACE_WORDNET;
			if (*(which + 1) == ':')
				expander_tf_types = decode_expansion_types(which + 2);
			else
				expander_tf_types = ANT_thesaurus_relationship::SYNONYM;
			done = TRUE;
			break;
		case 'W':
			/*
				Query expansion with wordnet.  This is done by adding words to the query
			*/
			query_type |= ATIRE_API::QUERY_EXPANSION_WORDNET;
			if (*(which + 1) == ':')
				expander_query_types = decode_expansion_types(which + 2);
			else
				expander_query_types = ANT_thesaurus_relationship::SYNONYM;
			done = TRUE;
			break;
		default:
			exit(printf("Unknown query type modifier: '%c'\n", *which));
			break;
		}
	which++;
	}
while (*which != '\0' && done == FALSE);

/*
	If we have no other query type then use NEXI
*/
if ((query_type & perform_query_mask) == 0)
	query_type |= ATIRE_API::QUERY_NEXI;
}

/*
	ANT_ANT_PARAM_BLOCK::EXPORT_FORMAT()
	------------------------------------
*/
void ANT_ANT_param_block::export_format(char *forum)
{
do
	{
	switch (*forum)
		{
		case '-': output_forum = NONE;   break;
		case 'I': output_forum = INEX;   break;
		case 'e': output_forum = INEX_EFFICIENCY; break;
		case 'i': output_forum = TREC;   break;			// in 2009 INEX moved to the TREC format with extra stuff on the end of each line
		case 'b': output_forum = INEX_BEP;   break;		// the INEX 2009 format with best entry points
		case 't': output_forum = TREC;   break;
		case 'f': 											// the INEX 2009 format with focused results included
			output_forum = INEX_FOCUS;
			if (focussing_algorithm == NONE)
				focussing_algorithm = ARTICLE;				// if we're not focusing yet then we are now!
			break;
		default : exit(printf("Unknown export format: '%c'\n", *forum)); break;
		}
	forum++;
	}
while (*forum != '\0');
}

/*
	ANT_ANT_PARAM_BLOCK::SET_FOCUSED_RANKER()
	-----------------------------------------
*/
void ANT_ANT_param_block::set_focused_ranker(char *which)
{
long got;

switch (*which)
	{
	case '-':
		focussing_algorithm = NONE;
		snippet_algorithm = NONE;
		title_algorithm = NONE;
		break;
	case 'a':
		focussing_algorithm = ARTICLE;
		break;
	case 'r':
		focussing_algorithm = RANGE;
		break;
	case 'b':
		snippet_algorithm = ANT_snippet_factory::SNIPPET_BEST_TF_TAG;
		if (*(which + 1) != '\0')
			snippet_tag = which + 1;
		break;
	case 'B':
		snippet_algorithm = ANT_snippet_factory::SNIPPET_BEST_TFICF_TAG;
		if (*(which + 1) != '\0')
			snippet_tag = which + 1;
		break;
	case 'c':
		snippet_algorithm = ANT_snippet_factory::SNIPPET_TF;
		break;
	case 'C':
		snippet_algorithm = ANT_snippet_factory::SNIPPET_TFICF;
		break;
	case 'f':
		snippet_algorithm = ANT_snippet_factory::SNIPPET_BEGINNING;
		if (*(which + 1) != '\0')
			snippet_tag = which + 1;
		break;
	case 'n':
		if ((got = atol(which + 1)) > 0)
			snippet_length = got;
		break;
	case 'N':
		if ((got = atol(which + 1)) > 0)
			title_length = got;
		break;
	case 'S':
		{
		ANT_indexer_param_block_stem snip_stem;

		snip_stem.term_expansion(which + 1, false);
		this->snippet_stemmer = snip_stem.stemmer;
		break;
		}
	case 't':
		snippet_algorithm = ANT_snippet_factory::SNIPPET_TITLE;
		if (*(which + 1) != '\0')
			snippet_tag = which + 1;
		break;
	case 'T':
		title_algorithm = ANT_snippet_factory::SNIPPET_TITLE;
		if (*(which + 1) != '\0')
			title_tag = which + 1;
		break;
	case 'w':
		snippet_algorithm = ANT_snippet_factory::SNIPPET_WORD_CLOUD_KL;
		if ((got = atol(which + 1)) > 0)
			snippet_word_cloud_terms = got;
		break;
	default:
		exit(printf("Unknown focusing algorithm: '%c'\n", *which));
		break;
	}
}

/*
	ANT_ANT_PARAM_BLOCK::PARSE()
	----------------------------
*/
long ANT_ANT_param_block::parse(void)
{
long param;
char *command;

for (param = 1; param < argc; param++)
	{
	if (*argv[param] == '-' || *argv[param] == '/')		// command line switch
		{
		command = argv[param] + 1;
		if (strcmp(command, "?") == 0)
			help();
		else if (strcmp(command, "h") == 0)
			help();
		else if (strcmp(command, "H") == 0)
			help();
		else if (strcmp(command, "nologo") == 0)
			logo = FALSE;
		else if (*command == 't')
			term_expansion(command + 1, TRUE);
		else if (*command == 'k')
			{
			sort_top_k = atol(command + 1);
			if (sort_top_k == 0)
				sort_top_k = LLONG_MAX;
			}
		else if (*command == 'K')
			{
			trim_postings_k = atol(command + 1);
			if (trim_postings_k == 0)
				trim_postings_k = LONG_MAX;
			}
		else if (*command == 'F')
			focus_top_k = atol(command + 1);
		else if (*command == 'M')
			file_or_memory = INDEX_IN_MEMORY;
		else if (strcmp(command, "Pt") == 0)
			processing_strategy = TERM_AT_A_TIME;
		else if (strncmp(command, "Pq", 2) == 0)
			{
			processing_strategy = QUANTUM_AT_A_TIME;
			// quantum-at-a-time only works when the index is in memory
			file_or_memory = INDEX_IN_MEMORY;
			quantum_stopping = QUANTUM_STOP_NONE;
			if ((command[2] == ':') && (command[3] != '\0'))
				{
					switch(command[3])
					{
						case 'n':
							quantum_stopping = QUANTUM_STOP_NONE;
							break;
						case 'd':
							quantum_stopping = QUANTUM_STOP_DIFF;
							break;
						case 's':
							quantum_stopping = QUANTUM_STOP_DIFF | QUANTUM_STOP_DIFF_SMALLEST;
							break;
						case 'l':
							quantum_stopping = QUANTUM_STOP_DIFF | QUANTUM_STOP_DIFF_LARGEST;
							break;
						default :
							printf("the early termination operation \"%c\" is not supported\n", command[3]);
							exit(0);
					}
				}
			}
		else if (*command == 'm')
			evaluator->add_evaluation(command + 1);
		else if (*command == 'a')
			if (*(command + 1) == '\0' && param < argc - 1)
				assessments_filename = argv[++param];
			else
				assessments_filename = command + 1;
		else if (*command == 'Q')
			set_feedbacker(command + 1);
		else if (*command == 'q')
			{
			if (command[1] == ':')
				{
				port = (unsigned short)(isdigit(command[2]) ? atol(command + 2) : 8088);
				printf("Server started on port:%lld\n", (long long)port);
				}
			else
				{
				if (*(command + 1) == '\0' && param < argc - 1)
					queries_filename = argv[++param];
				else
					queries_filename = command + 1;
				}
			}
		else if (*command == 'e')
			export_format(command + 1);
		else if (*command == 'i')
			participant_id = command + 1;
		else if (*command == 'n')
			run_name = command + 1;
		else if (*command == 'o')
			{
			if (*(command + 1) != '\0')
				output_filename = command + 1;
			}
		else if (*command == 'l')
			results_list_length = atol(command + 1);
		else if (*command == 's')
			set_stats(command + 1);
		else if (*command == 'S')
			{
			++command;
			if (*command == 'n')
				segmentation = FALSE;
			else
				segmentation = TRUE;
			}
		else if (*command == 'R')
			{
			if (!set_ranker(command + 1))
				exit(printf("Bad ranking function or ranking parameters '%s'\n", command + 1));
			}
		else if (*command == 'r')
			{
			quantization = true;
			if (*(command + 1))
				quantization_bits = atol(command + 1);
			if (quantization_bits > 16)
				exit(printf("Cannot quantize using more than 16 bits"));
			}
		else if (strcmp(command, "findex") == 0)
			{
			delete [] index_filename;
			index_filename = strnew(argv[++param]);
			}
		else if (strcmp(command, "fdoclist") == 0)
			{
			delete [] doclist_filename;
			doclist_filename = strnew(argv[++param]);
			}
		else if (*command == 'f')
			set_focused_ranker(command + 1);
		else if (strcmp(command, "pregen") == 0)
			{
			if (command[6] == ':')
				{
				pregen_ratio = atof(&command[7]);
				printf("Pregen Ratio: %f\n", pregen_ratio);
				}
			pregen_names[pregen_count] = strnew(argv[++param]);
			pregen_count++;
			}
		else if (strcmp(command, "people") == 0)
			{
			ANT_credits();
			exit(0);
			}
		else
			usage();
		}
	else
		break;
	}

/*
	If we're in batch mode (a query file has been specified) then the default
	length of the list of results is 1500.  If we're in interactive mode (no
	query file specified) then the default length is 10.
*/
if (results_list_length == -1)
	if (queries_filename == NULL)
		results_list_length = 10;
	else
		results_list_length = 1500;

return param;		// first parameter that isn't a command line switch
}

