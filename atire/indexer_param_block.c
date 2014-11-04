/*
	INDEXER_PARAM_BLOCK.C
	---------------------
*/
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "memory_index.h"
#include "maths.h"
#include "indexer_param_block.h"
#include "compression_text_factory.h"
#include "compression_factory.h"
#include "readability_factory.h"
#include "version.h"
#include "directory_iterator_filter.h"
#include "directory_iterator_scrub.h"
#include "ranking_function_puurula.h"
#include "ranking_function_factory_object.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	ANT_INDEXER_PARAM_BLOCK::ANT_INDEXER_PARAM_BLOCK()
	--------------------------------------------------
*/
ANT_indexer_param_block::ANT_indexer_param_block(int argc, char *argv[])
{
this->argc = argc;
this->argv = argv;

segmentation = ANT_parser::NOSEGMENTATION;
recursive = NONE;
compression_validation = FALSE;
compression_scheme = ANT_compression_factory::VARIABLE_BYTE;
readability_measure = ANT_readability_factory::NONE;

statistics = 0;
logo = TRUE;
reporting_frequency = LLONG_MAX;
ranking_function = ANT_ranking_function_factory_object::NONE;
document_compression_scheme = NONE;
index_filename = "index.aspt";
doclist_filename = "doclist.aspt";
spam_filename = NULL;
spam_threshold = 70; // as suggested at http://durum0.uwaterloo.ca/clueweb09spam/
mime_filter = false;
static_prune_point = LLONG_MAX;
stop_word_removal = ANT_memory_index::NONE;
stop_word_df_frequencies = 1;
stop_word_df_threshold = 1.1;		// anything greater than 1.0 will do.
scrubbing = ANT_directory_iterator_scrub::NONE;
filter_filename = NULL;
quantization = FALSE;
quantization_bits = -1; // -1 indicates run-time calculation, wil be overwritten if necessary by the user
quantization_automatic = FALSE;
puurula_length_g = ANT_RANKING_FUNCTION_PUURULA_G;
inversion_extras = ANT_memory_index::NONE;
}

/*
	ANT_INDEXER_PARAM_BLOCK::USAGE()
	--------------------------------
*/
void ANT_indexer_param_block::usage(void)
{
printf("Usage:%s [option...] filename <... filename>\n", argv[0]);
printf("     : -? for help\n");
exit(0);
}

/*
	ANT_INDEXER_PARAM_BLOCK::HELP()
	-------------------------------
*/
void ANT_indexer_param_block::help(void)
{
puts(ANT_version_string);
printf("Usage:%s [option...] filename <... filename>\n", argv[0]);
puts("");

puts("GENERAL");
puts("-------");
puts("-? -h -H        Display this help message");
puts("-nologo         Suppress banner");
puts("-people         Display credits");
puts("");

puts("INPUT FILE HANDLING");
puts("-------------------");
puts("-r              Recursive search for files in this and directories below this");
puts("-rcsv           Each document is a single line of the given file");
puts("-rmysql <username> <password> <hostname> <database> <query> MySQL query returning (docid, ...)");
puts("-rphpbb <username> <password> <hostname> <database> <type> MySQL phpBB instance");
puts("-rt             Search in tar files for indexable files");
puts("-rtgz           Search in tar.gz files for indexable files");
puts("-rtbz2          Search in tar.bz2 files for indexable files");
puts("-rtlzo          Search in tar.lzo files for indexable files");
puts("-rtrec          Single file, multiple <DOC>...</DOC> identified <DOCNO>docid</DOCNO>,");
puts("-rrtrec         Recursive search for TREC formatted <DOC>...</DOC> formatted files,");
puts("-rtrecbig       Equivalent to -rtrec -iscrub:an");
puts("-rvbulletin <username> <password> <database> <instance> MySQL vBulletin instance");
puts("-rwarcgz        Search in warc.gz files for indexable files");
puts("-rrwarcgz       Search in subdirectories for warc.gz files and index them");
puts("-rzip           Search in .zip files for indexable files (PKZIP format files)");
puts("");
puts("-iscrub:[anu]   Change the following characters to spaces:");
puts("         a      Non-ascii (high bit set)");
puts("         n      NUL (\\0)");
puts("         u      Invalid UTF-8 characters");
puts("-ispam[:n] <fn> Load percentile scores from <fn> and treat those < n as spam not to be indexed [default n=70]");
puts("-imime          Filter out mime types that do not begin with text");
puts("-ifilter[ie] <fn> Either [i]nclude or [e]xclude docids that are in file <fn>");
puts("");

puts("OUPUT FILE HANDLING");
puts("--------------------");
puts("-findex <fn>    Output filename for index");
puts("-fdoclist <fn>  Output filename for doclist");
puts("");
ANT_indexer_param_block_pregen::help();

puts("COMPRESSION");
puts("-----------");
puts("-c[abBceEgnrstv] Compress postings using any of:");
puts("   a            try all schemes and choose the best  (same as -cceEfgnrstTpqQv)");
puts("   b            try all bitwise schemes and choose the best  (same as -ceEg)");
puts("   B            try all Bytewise schemes and choose the best (same as -ccfrsStTpqQv)");
puts("   c            Carryover-12  (bytewise)");
puts("   e            Elias Delta   (bitwise)");
puts("   E            Elias Gamma   (bitwise)");
puts("   f            Four Integer Variable Byte    (bytewise)");
puts("   g            Golomb        (bitwise)");
puts("   n            None          (-)");
puts("   r            Relative-10   (bytewise)");
puts("   s            Simple-9      (bytewise)");
puts("   S            Sigma         (bytewise)");
puts("   t            Simple-16     (bytewise)");
puts("   T            Simple-8b     (64-bit bytewise)");
puts("   p            Simple-9-Packed      (bytewise)");
puts("   q            Simple-16-Packed      (bytewise)");
puts("   Q            Simple-8b-Packed     (64-bit bytewise)");
puts("   v            Variable Byte (bytewise) [default]");
puts("-vc             Validate posting compression (and report decompression rates)");
puts("");
puts("-C[-bnz]        Store documents in the repository compressed with one of:");
puts("   -            don't create the repositorty [default]");
puts("   b            bz2");
puts("   n            not-compressed");
puts("   s            Snappy");
puts("   z            zip (deflate)");
puts("");

puts("SEGMENTATION");
puts("------------");
puts("-S[odb]         East-Asian word segmentation, Chinese segment is the text separated by non-Chinese");
puts("   o            Use a segmentation module to segment string on fly");
puts("   d            Double(dual) segmentation, with words and single characters");
puts("   b            Segmentation using bigram");
puts("");

puts("READABILITY");
puts("-----------");
puts("-R[-dt]        Calculate readability using one of:");
puts("   -            none [default]");
puts("   d            Dale-Chall");
puts("   t            Tag up-weighting for TITLE and CATEGORY elements");
puts("");

ANT_indexer_param_block_rank::help("QUANTIZATION", 'Q', ANT_ranking_function_factory_object::INDEXABLE);
puts("-q[-<n>]        Quantization"); 
puts("   -            Don't push the quantization to the index, only store max and min for search time quantization.");
puts("   <n>          Quantize into <n>-bits [default <n>=5.4 + 5.4e-4 * sqrt(number documents)]");
puts("");

ANT_indexer_param_block_stem::help(FALSE);
ANT_indexer_param_block_topsig::help();

puts("REPORTING");
puts("---------");
puts("-N<n>           Report time and memory every <n> documents [default -N0]");
puts("-s[-acmst]      Report statistics");
puts("   -            No statistics [default]");
puts("   a            All statistics");
puts("   c            Compression ratios");
puts("   m            Memory usage");
puts("   s            Summary statistics");
puts("   t            Timings");
puts("");

puts("OPTIMISATIONS");
puts("-------------");
puts("-K<n>           Static pruning. Write no more than <n> postings per list (0=all) [default=0]");
puts("-k[-l0t][npNP][L<n>][s<n>] Term culling");
puts("   -            All terms remain in the index [default]");
puts("   0            Do not index numbers");
puts("   l            Remove (stop) low frequency terms (where collection frequency == 1)");
puts("   L<n>         Remove (stop) low frequency terms (where document frequency <= <n>)");
puts("   s<n>         Remove (stop) words that occur in more than <n>% of documents");
puts("   n            Remove (stop) words that are on the NCBI PubMed MBR 313 word stopword list: wrd_stop");
puts("   N            see -n, but before indexing (i.e. don't add to term counts)");
puts("   p            Remove (stop) words that are on Puurula's 988 stopword list use at ADCS/ALTA 2013");
puts("   P            see -p, but before indexing (i.e. don't add to term counts)");
puts("   t            Do not index XML tag names");
puts("");

exit(0);
}

/*
	ANT_INDEXER_PARAM_BLOCK::DOCUMENT_COMPRESSION()
	-----------------------------------------------
*/
void ANT_indexer_param_block::document_compression(char *scheme)
{
switch (*scheme)
	{
	case '-': document_compression_scheme = NONE; break;
	case 'b': document_compression_scheme = ANT_compression_text_factory::BZ2; break;
	case 'n': document_compression_scheme = ANT_compression_text_factory::RAW; break;
	case 's': document_compression_scheme = ANT_compression_text_factory::SNAPPY; break;
	case 'z': document_compression_scheme = ANT_compression_text_factory::DEFLATE; break;
	default : exit(printf("Unknown compression scheme: '%c'\n", *scheme)); break;
	}

if (*(scheme + 1) != '\0')
	exit(printf("Only one document compresson scheme may be used at a time\n"));
}

/*
	ANT_INDEXER_PARAM_BLOCK::COMPRESSION()
	--------------------------------------
*/
void ANT_indexer_param_block::compression(char *scheme_list)
{
char *scheme;

for (scheme = scheme_list; *scheme != '\0'; scheme++)
	switch (*scheme)
		{
		case 'a': compression("ceEfgnrstTpqQv"); break;
		case 'b': compression("eEg"); break;
		case 'B': compression("cfrsStTpqQv"); break;
		case 'c': compression_scheme |= ANT_compression_factory::CARRYOVER_12; break;
		case 'e': compression_scheme |= ANT_compression_factory::ELIAS_DELTA; break;
		case 'E': compression_scheme |= ANT_compression_factory::ELIAS_GAMMA; break;
		case 'f': compression_scheme |= ANT_compression_factory::FOUR_INTEGER_VARIABLE_BYTE; break;
		case 'g': compression_scheme |= ANT_compression_factory::GOLOMB; break;
		case 'n': compression_scheme |= ANT_compression_factory::NONE; break;
		case 'r': compression_scheme |= ANT_compression_factory::RELATIVE_10; break;
		case 's': compression_scheme |= ANT_compression_factory::SIMPLE_9; break;
		case 'S': compression_scheme |= ANT_compression_factory::SIGMA; break;
		case 't': compression_scheme |= ANT_compression_factory::SIMPLE_16; break;
		case 'T': compression_scheme |= ANT_compression_factory::SIMPLE_8B; break;
		case 'p': compression_scheme |= ANT_compression_factory::SIMPLE_9_PACKED; break;
		case 'q': compression_scheme |= ANT_compression_factory::SIMPLE_16_PACKED; break;
		case 'Q': compression_scheme |= ANT_compression_factory::SIMPLE_8B_PACKED; break;
		case 'v': compression_scheme |= ANT_compression_factory::VARIABLE_BYTE; break;
		default : exit(printf("Unknown compression scheme: '%c'\n", *scheme)); break;
		}
}

/*
	ANT_INDEXER_PARAM_BLOCK::READABILITY()
	--------------------------------------
*/
void ANT_indexer_param_block::readability(char *measures)
{
char *measure;

for (measure = measures; *measure != '\0'; measure++)
	switch (*measure)
		{
		case '-': readability_measure = ANT_readability_factory::NONE; break;
		case 'd': readability_measure = ANT_readability_factory::DALE_CHALL; break;
		case 't': readability_measure = ANT_readability_factory::TAG_WEIGHTING; break;
		default : exit(printf("Unknown readability measure: '%c'\n", *measure)); break;
		}
}

/*
	ANT_INDEXER_PARAM_BLOCK::SEGMENTATION()
	--------------------------------------
*/
void ANT_indexer_param_block::segment(char *segment_flags)
{
char *segment_flag;

segmentation |= ANT_parser::SHOULD_SEGMENT;

for (segment_flag = segment_flags; *segment_flag != '\0'; segment_flag++)
	switch (*segment_flag)
		{
		//case 's': segmentation -= segmentation & ANT_parser::DOUBLE_SEGMENTATION; break; // only index what we have
		case 'o': segmentation |= ANT_parser::ONFLY_SEGMENTATION; break;
		case 'd': segmentation |= ANT_parser::DOUBLE_SEGMENTATION; break;
		case 'b': segmentation |= ANT_parser::BIGRAM_SEGMENTATION; break;
		default : exit(printf("Unknown segmentation flag: '%c'\n", *segment_flag)); break;
		}
}

/*
	ANT_INDEXER_PARAM_BLOCK::STATS()
	--------------------------------
*/
void ANT_indexer_param_block::stats(char *stat_list)
{
char *stat;

for (stat = stat_list; *stat != '\0'; stat++)
	switch (*stat)
		{
		case '-': statistics = 0; break;
		case 'a': stats("cmst"); break;
		case 'c': statistics |= STAT_COMPRESSION; break;
		case 'm': statistics |= STAT_MEMORY; break;
		case 's': statistics |= STAT_SUMMARY; break;
		case 't': statistics |= STAT_TIME; break;
		default : exit(printf("Unknown statistic: '%c'\n", *stat)); break;
		}
}

/*
	ANT_INDEXER_PARAM_BLOCK::TERM_REMOVAL()
	---------------------------------------
*/
void ANT_indexer_param_block::term_removal(char *mode_list)
{
char *which;

for (which = mode_list; *which != '\0'; which++)
	switch (*which)
		{
		case '-': stop_word_removal = ANT_memory_index::NONE; break;
		case '0': stop_word_removal |= ANT_memory_index::PRUNE_NUMBERS; break;
		case 'L':
			stop_word_removal |= ANT_memory_index::PRUNE_DF_SINGLETONS;
			stop_word_df_frequencies = atol(which +1);
			while (isdigit(*(which+1)))
				which++;
			if (stop_word_df_frequencies == 0)
				stop_word_df_frequencies = 1;
			break;
		case 'l': stop_word_removal |= ANT_memory_index::PRUNE_CF_SINGLETONS; break;
		case 't': stop_word_removal |= ANT_memory_index::PRUNE_TAGS; break;
		case 'n': stop_word_removal |= ANT_memory_index::PRUNE_NCBI_STOPLIST; break;
		case 'N': stop_word_removal |= ANT_memory_index::PRUNE_NCBI_STOPLIST | ANT_memory_index::PRUNE_STOPWORDS_BEFORE_INDEXING; break;
		case 'p': stop_word_removal |= ANT_memory_index::PRUNE_PUURULA_STOPLIST; break;
		case 'P': stop_word_removal |= ANT_memory_index::PRUNE_PUURULA_STOPLIST | ANT_memory_index::PRUNE_STOPWORDS_BEFORE_INDEXING; break;
		case 's':
			stop_word_removal |= ANT_memory_index::PRUNE_DF_FREQUENTS;
			stop_word_df_threshold = atof(which + 1);
			if (stop_word_df_threshold >= 100 || stop_word_df_threshold <= 0)
				exit(printf("stopiing parameter must be 0 < n%% < 100 (%f was given)", stop_word_df_threshold));
			stop_word_df_threshold /= 100.0;

			while (*(which + 1) == '.' || isdigit(*(which + 1)))
				which++;
			break;
		default : exit(printf("Unknown term cull parameter: '%c'\n", *which)); break;
		}
}

/*
	ANT_INDEXER_PARAM_BLOCK::SCRUB()
	--------------------------------
*/
void ANT_indexer_param_block::scrub(char *scrub_what)
{
char *which;

for (which = scrub_what; *which != '\0'; which++)
	switch (*which)
		{
		case 'a': scrubbing |= ANT_directory_iterator_scrub::NON_ASCII; break;
		case 'n': scrubbing |= ANT_directory_iterator_scrub::NUL; break;
		case 'u': scrubbing |= ANT_directory_iterator_scrub::UTF8; break;
		default : exit(printf("Unknown scrub flag: '%c'\n", *which)); break;
		}
}

/*
	ANT_INDEXER_PARAM_BLOCK::PARSE()
	--------------------------------
*/
long ANT_indexer_param_block::parse(void)
{
long param;
char *command;
char *start;

for (param = 1; param < argc; param++)
	{
	if (*argv[param] == '-')		// command line switch
		{
		command = argv[param] + 1;
		if (strcmp(command, "r") == 0)
			recursive = DIRECTORIES;
		else if (strcmp(command, "rt") == 0)
			recursive = TAR;
		else if (strcmp(command, "rtgz") == 0)
			recursive = TAR_GZ;
		else if (strcmp(command, "rtbz2") == 0)
			recursive = TAR_BZ2;
		else if (strcmp(command, "rzip") == 0)
			recursive = PKZIP;
		else if (strcmp(command, "rtlzo") == 0)
			recursive = TAR_LZO;
		else if (strncmp(command, "rtrec", 5) == 0)
			{
			recursive = TREC;
			doc_tag = NULL;
			docno_tag = NULL;
			if (strncmp(command + 5, ":clean", 6) == 0)
				this->scrub("an");
			else if (strncmp(command + 5, ":tag", 4) == 0)
				{
				doc_tag = command + 10;
				if ((start = strchr(doc_tag, ':')) != NULL)
					{
					*start = '\0';
					docno_tag = ++start;
					}
				}
			}
		else if (strncmp(command, "rrtrec", 6) == 0)
			{
			recursive = RECURSIVE_TREC;
			if (strncmp(command + 6, ":clean", 6) == 0)
				this->scrub("an");
			}
		else if (strcmp(command, "rtrecbig") == 0)
			{
			recursive = TREC;
			this->scrub("an");
			}
		else if (strcmp(command, "rcsv") == 0)
			recursive = CSV;
		else if (strcmp(command, "rtsv") == 0)
			recursive = TSV;
		else if (strcmp(command, "rwarcgz") == 0)
			recursive = WARC_GZ;
		else if (strcmp(command, "rrwarcgz") == 0)
			recursive = RECURSIVE_WARC_GZ;
		else if (strcmp(command, "rphpbb") == 0)
			recursive = PHPBB;
		else if (strcmp(command, "rmysql") == 0)
			recursive = MYSQL;
		else if (strcmp(command, "rvbulletin") == 0)
			recursive = VBULLETIN;
		else if (strncmp(command, "ispam", 5) == 0)
			{
			if (*(command + 5) == ':')
				{
				spam_threshold = atol(command + 6);
				if (spam_threshold < 0 || spam_threshold > 99)
					exit(printf("Spam threshold must be in range 0-99, given %lld\n", spam_threshold));
				}
			spam_filename = argv[++param];
			}
		else if (strncmp(command, "imime", 5) == 0)
			mime_filter = true;
		else if (strncmp(command, "ifilter", 7) == 0)
			{
			switch (*(command + 7))
				{
				case 'i': filter_method = ANT_directory_iterator_filter::INCLUDE; break;
				case 'e': filter_method = ANT_directory_iterator_filter::EXCLUDE; break;
				default: exit(printf("Filter method must be one of [ie], given '%c'\n", *(command + 7)));
				}
			filter_filename = argv[++param];
			}
		else if (strncmp(command, "iscrub:", 7) == 0)
			this->scrub(command + 7);
		else if (*command == 'S')
			segment(command + 1);
		else if (strcmp(command, "?") == 0)
			help();
		else if (strcmp(command, "h") == 0)
			help();
		else if (strcmp(command, "H") == 0)
			help();
		else if (*command == 'k')
			term_removal(command + 1);
		else if (*command == 'K')
			{
			if ((static_prune_point = atoll(command + 1)) <= 0)
				static_prune_point = LLONG_MAX;
			}
		else if (*command == 'N')
			{
			reporting_frequency = atol(command + 1);
			if (reporting_frequency == 0)
				reporting_frequency = LLONG_MAX;
			}
		else if (*command == 'C')
			{
			document_compression_scheme = NONE;
			document_compression(command + 1);
			}
		else if (*command == 'c')
			{
			compression_scheme = 0;
			compression(command + 1);
			}
		else if (strcmp(command, "findex") == 0)
			index_filename = argv[++param];
		else if (strcmp(command, "fdoclist") == 0)
			doclist_filename = argv[++param];
		else if (strcmp(command, "Inverted") == 0)
			inversion_type = INVERTED_FILE;
		else if (strncmp(command, "Ilmptfidf", 9) == 0)
			{
			inversion_extras |= ANT_memory_index::PUURULA_LENGTH_VECTORS | ANT_memory_index::PUURULA_LENGTH_VECTORS_TFIDF;
			ANT_indexer_param_block_rank::get_one_parameter(command + 9, &puurula_length_g);
			}
		else if (strncmp(command, "Ilmp", 4) == 0)
			{
			if (command[4] == '\0' || command[4] == ':')
				{
				inversion_extras |= ANT_memory_index::PUURULA_LENGTH_VECTORS;
				ANT_indexer_param_block_rank::get_one_parameter(command + 4, &puurula_length_g);
				}
			else
				printf("Unknown -Ilmp parameter ('-%s'), ignoring\n", command);
			}
		else if (strcmp(command, "pregen") == 0)
			{
			char *field_type, *field_name;

			if (param + 2 >= argc)
				exit(printf("Not enough arguments after '-pregen', expected two arguments.\n"));

			field_name = argv[++param];
			field_type = argv[++param];

			if (!add_pregen_field(field_name, field_type))
				exit(printf("Unknown pregen field type '%s'\n", field_type));
			}
		else if (strcmp(command, "vc") == 0)
			compression_validation = TRUE;
		else if (*command == 'R')
			{
			readability_measure = 0;
			readability(command + 1);
			}
		else if (*command == 's')
			{
			statistics = 0;
			stats(command + 1);
			}
		else if (strstr(command, "Topsig") != NULL)
			topsig(command + 7);
		else if (*command == 'Q')
			{
			quantization = TRUE;
			if (!set_ranker(command + 1))
				exit(printf("Bad ranking function or ranking parameters '%s'\n", command + 1));
			}
		else if (*command == 'q')
			{
			quantization = TRUE;
			quantization_automatic = TRUE;

			if (*(command + 1) == '-')
				quantization = FALSE;
			else if (*(command + 1) != '\0')
				{
				quantization_bits = atol(command + 1);
				if (quantization_bits < 2 || quantization_bits > 16)
					exit(printf("Have to quantize into range 2--16 bits inclusive\n"));
				quantization_automatic = FALSE;
				}
			}
		else if (*command == 't')
			term_expansion(command + 1, FALSE);
		else if (strcmp(command, "nologo") == 0)
			logo = FALSE;
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

return param;		// first parameter that isn't a command line switch
}
