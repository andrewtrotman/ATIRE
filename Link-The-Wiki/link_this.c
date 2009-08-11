/*
	LINK_THIS.C
	-----------
	Written (w) 2008 by Andrew Trotman, University of Otago
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../source/disk.h"
#include "link_parts.h"

#define REMOVE_ORPHAN_LINKS 1

#ifdef REMOVE_ORPHAN_LINKS
	#define ADD_ORPHAN_LINKS 1
	#define SUBTRACT_ORPHAN_LINKS (-1)
#endif

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

#ifndef _MSC_VER
#define _strdup strdup
#endif

#define MODE_NO_4_DIGIT_NUMBERS 1

#ifdef INEX_ARCHIVE
	#define COLLECTION_LINK_TAG_NAME "collectionlink"
	#define TOPIC_SIGNITURE "<topic file=\"%d.xml\" name=\"%s\"><outgoing>\n"
#else
	#define COLLECTION_LINK_TAG_NAME "link"
	#define TOPIC_SIGNITURE "<topic file=\"%d\" name=\"%s\"><outgoing>\n"
#endif

class ANT_link_posting
{
public:
	long docid;						// target document
	long link_frequency;			// number of times the document occurs as the target of a link
	long doc_link_frequency;		// number of documents in which the phrase links to docid
public:
	static int compare(const void *a, const void *b);
} ;

class ANT_link_term
{
public:
	char *term;
	long postings_length;			// which is also the number of documents pointed to.
	ANT_link_posting *postings;
	long total_occurences;			// number of times the phrase occurs as a link in the collection
	long collection_frequency;		// number of times the phrase occurs in the colleciton
	long document_frequency;		// number of documents in which the phrase occurs
public:
	static int compare(const void *a, const void *b);
} ;

class ANT_link
{
public:
	char *place_in_file;
	char *term;
	double gamma;
	long target_document;
	ANT_link_term *link_term;

public:
	static int compare(const void *a, const void *b);
	static int final_compare(const void *a, const void *b);
	static int string_target_compare(const void *a, const void *b);
} ;

#define MAX_LINKS_IN_FILE (1024 * 1024)
ANT_link all_links_in_file[MAX_LINKS_IN_FILE];
long long all_links_in_file_length = 0;

ANT_link links_in_orphan[MAX_LINKS_IN_FILE];
long links_in_orphan_length = 0;

long lowercase_only;					// are we in lowercase or mixed-case matching mode?

static char buffer[1024 * 1024];

/*
	COUNT_CHAR()
	------------
*/
long count_char(char *buffer, char what)
{
char *ch;
long times;

times = 0;
for (ch = buffer; *ch != '\0'; ch++)
	if (*ch == what)
		times++;

return times;
}

/*
	ANT_LINK::COMPARE()
	-------------------
*/
int ANT_link::compare(const void *a, const void *b)
{
ANT_link *one, *two;
double diff;
long cmp;

one = (ANT_link *)a;
two = (ANT_link *)b;

diff = two->gamma - one->gamma;
if (diff < 0)
	return -1;
else if (diff > 0)
	return 1;
else
	{
	if ((cmp = strcmp(one->term, two->term)) == 0)
		return one->place_in_file > two->place_in_file ? 1 : one->place_in_file == two->place_in_file ? 0 : -1;
	else
		return cmp;
	}
}

/*
	ANT_LINK::FINAL_COMPARE()
	-------------------------
*/
int ANT_link::final_compare(const void *a, const void *b)
{
ANT_link *one, *two;
double diff;

one = (ANT_link *)a;
two = (ANT_link *)b;

diff = two->gamma - one->gamma;
if (diff < 0)
	return -1;
else if (diff > 0)
	return 1;
else
	return one->place_in_file > two->place_in_file ? 1 : one->place_in_file == two->place_in_file ? 0 : -1;
}

/*
	ANT_LINK::STRING_TARGET_COMPARE()
	---------------------------------
*/
int ANT_link::string_target_compare(const void *a, const void *b)
{
ANT_link *one, *two;
int cmp;

one = (ANT_link *)a;
two = (ANT_link *)b;

if ((cmp = strcmp(one->term, two->term)) == 0)
	cmp = one->target_document - two->target_document;

return cmp;
}

/*
	ANT_LINK_TERM::COMPARE()
	------------------------
*/
int ANT_link_term::compare(const void *a, const void *b)
{
ANT_link_term *one, *two;

one = (ANT_link_term *)a;
two = (ANT_link_term *)b;

return strcmp(one->term, two->term);
}

/*
	ANT_LINK_POSTING::COMPARE()
	---------------------------
*/
int ANT_link_posting::compare(const void *a, const void *b)
{
ANT_link_posting *one, *two;

one = (ANT_link_posting *)a;
two = (ANT_link_posting *)b;

return two->link_frequency > one->link_frequency ? 1 : two->link_frequency == one->link_frequency ? 0 : -1;
}

/*
	READ_INDEX()
	------------
*/
ANT_link_term *read_index(char *filename, long *terms_in_collection)
{
FILE *fp;
ANT_link_term *all_terms, *term;
/**
 * there is something wrong with the long type for sscanf in reading the number of terms on 64-bit Linux
 */
int unique_terms, postings, current;
char *term_end, *from;
char tmp[6];

if ((fp = fopen(filename, "rb")) == NULL)
	exit(printf("Cannot index file:%s\n", filename));

fgets(buffer, sizeof(buffer), fp);
sscanf(buffer, "%d %s", &unique_terms, tmp);
term = all_terms = new ANT_link_term [unique_terms];

while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
	term_end = strchr(buffer, ':');
	postings = count_char (term_end, '>');
	term->term = strnnew(buffer, term_end - buffer);
	term->postings_length = postings;
	term->postings = new ANT_link_posting[term->postings_length];
	term->total_occurences = 0;

	sscanf(term_end + 2, "%d,%d", &(term->document_frequency), &(term->collection_frequency));
//	term_end = strchr(term_end + 1, ';';
	from = term_end;
	for (current = 0; current < postings; current++)
		{
		from = strchr(from, '<') + 1;
		sscanf(from, "%d,%d,%d", &(term->postings[current].docid), &(term->postings[current].doc_link_frequency), &(term->postings[current].link_frequency));
		term->total_occurences += term->postings[current].link_frequency;
		}
	if (term->postings_length > 1)
		qsort(term->postings, term->postings_length, sizeof(term->postings[0]), ANT_link_posting::compare);

	term++;
	}

*terms_in_collection = unique_terms;
return all_terms;
}

/*
	PUSH_LINK()
	-----------
*/
void push_link(char *place_in_file, char *buffer, long docid, double gamma, ANT_link_term *node)
{
ANT_link *current;

current = all_links_in_file + all_links_in_file_length;
current->place_in_file = place_in_file;
current->term = _strdup(buffer);
current->gamma = gamma;
current->target_document = docid;
current->link_term = node;

all_links_in_file_length++;
if (all_links_in_file_length >= MAX_LINKS_IN_FILE)
	exit(printf("Too many links in this file (aborting)\n"));
}

/*
	DEDUPLICATE_LINKS()
	-------------------
*/
void deduplicate_links(void)
{
ANT_link *from, *to;

from = to = all_links_in_file + 1;

while (from < all_links_in_file + all_links_in_file_length)
	{
	if (strcmp(from->term, (from - 1)->term) != 0)
		{
		*to = *from;
		from++;
		to++;
		}
	else
		from++;
	}
all_links_in_file_length = to - all_links_in_file;
}

/*
	PRINT_HEADER()
	--------------
*/
void print_header(char *runname)
{
#ifdef INEX_ARCHIVE
	printf("<inex-submission participant-id=\"4\" run-id=\"%s\" task=\"LinkTheWiki\" format=\"FOL\"><details><machine><cpu>Pentium 4</cpu><speed>2992 MHz</speed><cores>2</cores><hyperthreads>1</hyperthreads><memory>1GB</memory></machine><time>TBC</time></details><description>This is the description</description><collections><collection>wikipedia</collection></collections>", runname);
#else
	printf("<inexltw-submission participant-id=\"4\" run-id=\"%s\" task=\"LTW_F2F\" format=\"FOL\"><details><machine><cpu>Pentium 4</cpu><speed>2992 MHz</speed><cores>2</cores><hyperthreads>1</hyperthreads><memory>1GB</memory></machine><time>TBC</time></details><description>This is the description</description><collections><collection>wikipedia</collection></collections>", runname);
#endif
}

/*
	PRINT_FOOTER()
	--------------
*/
void print_footer(void)
{
#ifdef INEX_ARCHIVE
	puts("</inex-submission>");
#else
	puts("</inexltw-submission>");
#endif
}

/*
	PRINT_LINKS()
	-------------
*/
void print_links(long orphan_docid, char *orphan_name, long links_to_print, long max_targets_per_anchor, long mode)
{
long *links_already_printed;
long current, links_already_printed_length, links_printed;
long result, current_anchor, forget_it, has_link, anchors_printed;
//char *orphan_name = "Unknown";
long array_size = links_to_print * max_targets_per_anchor * 2;
long stop_sign = 0;

long links_already_printed_array[array_size]; // = new long [array_size];
links_already_printed = links_already_printed_array;

#ifdef REMOVE_ORPHAN_LINKS
links_already_printed[0] = orphan_docid;		// fake having already printed the oprhan itself.
links_already_printed_length = 1;
#endif

printf(TOPIC_SIGNITURE, orphan_docid, orphan_name);
result = 0;
links_printed = 0;
#ifdef INEX_ARCHIVE
while ((result < all_links_in_file_length) && (links_printed < links_to_print))
	{
	if (mode & MODE_NO_4_DIGIT_NUMBERS)
		if ((strlen(all_links_in_file[result].link_term->term) == 4) && (atol(all_links_in_file[result].link_term->term) > 999))
			{
			result++;		// in the case of a 4 digit number we ignore as we're probably a year
			continue;
			}

	has_link = FALSE;
	anchors_printed = current_anchor = 0;
	while ((current_anchor < all_links_in_file[result].link_term->postings_length) && (anchors_printed < max_targets_per_anchor))
		{
		forget_it = FALSE;
		for (current = 0; (current < links_already_printed_length) && (current < array_size); current++)
			if (links_already_printed[current] == all_links_in_file[result].link_term->postings[current_anchor].docid)
				forget_it = TRUE;
		if (!forget_it)
			{
			links_already_printed[links_already_printed_length] = all_links_in_file[result].link_term->postings[current_anchor].docid;
			links_already_printed_length++;
			if (!has_link)
				printf("<link><anchor><file>%d.xml</file><offset>0</offset><length>0</length></anchor>\n", orphan_docid);
			printf("<linkto><file>%d.xml</file><bep>0</bep></linkto>\n", all_links_in_file[result].link_term->postings[current_anchor].docid);
			anchors_printed++;
			if (/*anchors_printed >= max_targets_per_anchor || */links_already_printed_length >= array_size)
				{
				stop_sign = 1;
				break;
				}
//			printf("%d:%d:%d:%d:%d\n", orphan_docid, all_links_in_file[result].link_term->postings[current_anchor].docid, count_char(all_links_in_file[result].term, ' ') + 1, strlen(all_links_in_file[result].term), (long)(all_links_in_file[result].gamma * 100));
			//has_link = TRUE;
			}
		current_anchor++;
		}
	if (has_link)
		{
		printf("</link>\n");
		links_printed++;
		}
	result++;

	if (stop_sign)
		break;
	} // while
#else
printf("<linkto>");
while ((result < all_links_in_file_length) && (links_printed < links_to_print))
	{
	if (strlen(all_links_in_file[result].link_term->term) == 0)
		{
		result++;		// in the case of empty term
		continue;
		}

	if (mode & MODE_NO_4_DIGIT_NUMBERS)
		if ((strlen(all_links_in_file[result].link_term->term) == 4) && (atol(all_links_in_file[result].link_term->term) > 999))
			{
			result++;		// in the case of a 4 digit number we ignore as we're probably a year
			continue;
			}

	has_link = FALSE;
	anchors_printed = current_anchor = 0;
	while ((current_anchor < all_links_in_file[result].link_term->postings_length) && (anchors_printed < max_targets_per_anchor))
		{
		forget_it = FALSE;
		for (current = 0; (current < links_already_printed_length) && (current < array_size); current++)
			if (links_already_printed[current] == all_links_in_file[result].link_term->postings[current_anchor].docid)
				forget_it = TRUE;
		if (!forget_it)
			{
			links_already_printed[links_already_printed_length] = all_links_in_file[result].link_term->postings[current_anchor].docid;
			if (links_already_printed_length > 1)
				printf(", ");
			printf("%d", all_links_in_file[result].link_term->postings[current_anchor].docid);
			anchors_printed++;
			links_already_printed_length++;
			if (/*anchors_printed >= max_targets_per_anchor || */links_already_printed_length >= array_size)
				{
				stop_sign = 1;
				break;
				}
			}
		current_anchor++;
		}
	result++;

	if (stop_sign)
		break;
	} // while
printf("</linkto>\n");
#endif


#ifdef INEX_ARCHIVE
		puts("</outgoing><incoming><link><anchor><file>654321.xml</file><offset>445</offset><length>462</length></anchor><linkto><bep>1</bep></linkto></link></incoming>");
#else
		puts("</outgoing>");
#endif
puts("</topic>");

//delete [] links_already_printed;
}

/*
	FIND_TERM_IN_LIST()
	-------------------
*/
ANT_link_term *find_term_in_list(char *value, ANT_link_term *list, long list_length, long this_docid)
{
long low, high, mid;

low = 0;
high = list_length;
while (low < high)
	{
	mid = (low + high) / 2;
	if (strcmp(list[mid].term, value) < 0)
		low = mid + 1;
	else
		high = mid;
	}

#ifdef REMOVE_ORPHAN_LINKS
/*
	remove terms that are in the anchor list but have no postings because the only document used the anchor was this one.
*/
while (low < list_length)
	if (list[low].postings[0].doc_link_frequency == 0)		// we've been deleted so this is a miss
		low++;
	else
		break;
/*
	remove terms that point to this document
*/
while (low < list_length)
	if (list[low].postings_length == 1 && list[low].postings[0].docid == this_docid)		// we point to this document so we delete it
		low++;
	else
		break;
#endif

if ((low < list_length) && (strcmp(value, list[low].term) == 0))
	return &list[low];		// match
else
	{
	if (low < list_length)
		return &list[low];		// not found in list but not after the last term in the list
	else
		return NULL;
	}
}

/*
	GENERATE_COLLECTION_LINK_SET()
	------------------------------
*/
void generate_collection_link_set(char *original_file)
{
char *file, *pos, *end, *copy, *slash, *target_end;
long id;

links_in_orphan_length = 0;
file = _strdup(original_file);
pos = strstr(file, "<"COLLECTION_LINK_TAG_NAME);
while (pos != NULL)
	{
	end = strstr(pos, "</"COLLECTION_LINK_TAG_NAME);
	pos = strstr(pos, "xlink:href=");
	if (pos != NULL && pos < end)
		{
		pos = strchr(pos, '"');
		pos++;
		target_end = strchr(pos, '"');

		while ((slash = strpbrk(pos, "\\/")) && slash < target_end)
			pos = slash + 1;

		id = atol(pos);
		pos = strchr(pos, '>');

		copy = strnnew(pos + 1, end - pos - 1);
		string_clean(copy, lowercase_only);

		links_in_orphan[links_in_orphan_length].term = copy;
		links_in_orphan[links_in_orphan_length].gamma = 0;
		links_in_orphan[links_in_orphan_length].target_document = id;

		links_in_orphan_length++;
		if (links_in_orphan_length >= MAX_LINKS_IN_FILE)
			exit(printf("Too many links present in orphan a priori\n"));
		}
	pos = strstr(end, "<"COLLECTION_LINK_TAG_NAME);
	}
delete [] file;

qsort(links_in_orphan, links_in_orphan_length, sizeof(*links_in_orphan), ANT_link::string_target_compare);
}

/*
	ADD_OR_SUBTRACT_ORPHAN_LINKS()
	------------------------------
*/
void add_or_subtract_orphan_links(long add_or_subtract, ANT_link_term *link_index, long terms_in_index)
{
long current, posting;
ANT_link_term key, *found;

for (current = 0; current < links_in_orphan_length; current++)
	{
	if (current > 0)
		if (strcmp(links_in_orphan[current].term, links_in_orphan[current - 1].term) == 0)		// same term
			if (links_in_orphan[current].target_document == links_in_orphan[current - 1].target_document)	// same target
				continue;			// don't do dupicate links as we are only computing the new DF.

	key.term = links_in_orphan[current].term;
	found = (ANT_link_term *)bsearch(&key, link_index, terms_in_index, sizeof(*link_index), ANT_link_term::compare);
	if (found != NULL)
		{
		for (posting = 0; posting < found->postings_length; posting++)
			if (found->postings[posting].docid == links_in_orphan[current].target_document)
				found->postings[posting].doc_link_frequency = found->postings[posting].doc_link_frequency + add_or_subtract;

		if (found->postings_length > 1)
			qsort(found->postings, found->postings_length, sizeof(found->postings[0]), ANT_link_posting::compare);
		}
	}
}

/*
	ISPROPPER_NOUN()
	----------------
*/
long ispropper_noun(char *phrase)
{
char *ch;

if (!isupper(*phrase))			// first letter of first word
	return FALSE;

for (ch = phrase; *ch != '\0'; ch++)
	if (isspace(*ch))
		if (!isupper(*(ch + 1)))
			return FALSE;

return TRUE;
}

/*
	USAGE()
	-------
*/
void usage(char *exename)
{
exit(printf("Usage:%s [-lowercase] [-noyears] [-runname:name] [-anchors:<n>] [-targets:<n>] [-propernounboost:<n.n>] <index> <file_to_link> ...\n", exename));
}

/*
	MAIN()
	------
*/
int main(int argc, char *argv[])
{
static char *seperators = " ";
ANT_disk disk;
char *file, *token, *where_to;
char **term_list, **first, **last, **current;
ANT_link_term *link_index, *index_term, *last_index_term;
long terms_in_index, orphan_docid, param, noom, index_argv_param;
double gamma, numerator, denominator;
long targets_per_link = 1, anchors_per_run = 250, print_mode = 0;
char *runname = "Unknown";
char orphan_name[2048];
double proper_noun_boost = 0.0;
long num_of_processed_topic = 0;

if (argc < 3)
	usage(argv[0]);		// and exit

lowercase_only = FALSE;

for (index_argv_param = 1; *argv[index_argv_param] == '-'; index_argv_param++)
	{
	if (strcmp(argv[index_argv_param], "-lowercase") == 0)
		lowercase_only = TRUE;
	else if (strcmp(argv[index_argv_param], "-noyears") == 0)
		print_mode |= MODE_NO_4_DIGIT_NUMBERS;
	else if (strncmp(argv[index_argv_param], "-runname:", 9) == 0)
		runname = strchr(argv[index_argv_param], ':') + 1;
	else if (strncmp(argv[index_argv_param], "-propernounboost:", 17) == 0)
		proper_noun_boost = atof(strchr(argv[index_argv_param], ':') + 1);
	else if (strncmp(argv[index_argv_param], "-targets:", 8) == 0)
		{
		targets_per_link = atoi(strchr(argv[index_argv_param], ':') + 1);
		if (targets_per_link <= 0)
			usage(argv[0]);
		}
	else if (strncmp(argv[index_argv_param], "-anchors:", 8) == 0)
		{
		anchors_per_run = atoi(strchr(argv[index_argv_param], ':') + 1);
		if (anchors_per_run <= 0)
			usage(argv[0]);
		}
	else
		usage(argv[0]);		// and exit
	}

link_index = read_index(argv[index_argv_param], &terms_in_index);

print_header(runname);

for (param = index_argv_param + 1; param < argc; param++)
	for (file = disk.read_entire_file(disk.get_first_filename(argv[param])); file != NULL; file = disk.read_entire_file(disk.get_next_filename()))
		{
		all_links_in_file_length = 0;
		orphan_docid = get_doc_id(file);
		get_doc_name(file, orphan_name);
#ifdef REMOVE_ORPHAN_LINKS
		generate_collection_link_set(file);
		add_or_subtract_orphan_links(SUBTRACT_ORPHAN_LINKS, link_index, terms_in_index);
#endif
		string_clean(file, lowercase_only);

		current = term_list = new char *[strlen(file)];		// this is the worst case by far
		for (token = strtok(file, seperators); token != NULL; token = strtok(NULL, seperators))
			*current++ = token;
		*current = NULL;

		for (first = term_list; *first != NULL; first++)
			{
			where_to = buffer;
			last_index_term = NULL;
			for (last = first; *last != NULL; last++)
				{
				if (where_to == buffer)
					{
					strcpy(buffer, *first);
					where_to = buffer + strlen(buffer);
					}
				else
					{
					*where_to++ = ' ';
					strcpy(where_to, *last);
					where_to += strlen(*last);
					}

				index_term = find_term_in_list(buffer, link_index, terms_in_index, orphan_docid);

				if (index_term == NULL)
					break;									// we're after the last term in the list

				if (strcmp(buffer, index_term->term) == 0)
					last_index_term = index_term;			// we're a term in the list, but might be a longer one so keep looking

				if (strncmp(buffer, index_term->term, strlen(buffer)) != 0)
					break;									// we can't be a substring so we're done
				}

			if (last_index_term != NULL)
				{
				noom = 0;
				denominator = (double)last_index_term->document_frequency;
#ifdef REMOVE_ORPHAN_LINKS
				denominator--;
				if (last_index_term->postings[noom].docid == orphan_docid)			// not alowed to use links that point to the orphan
					noom = 1;
#endif
				numerator = (double)last_index_term->postings[noom].doc_link_frequency;

#ifdef LYCOS_EXTENSIONS
				/*
					Lycos rule for short anchors
				*/
				if (strlen(last_index_term->term) < 5 || strlen(last_index_term->term) > 60)
					denominator = 1000000;
				/*
				*/
				/*
					Lycos rule for links occuring fewer than 5 times
				*/
					if (denominator < 5)
						denominator = 1000000;				// so make the link unimportant
				/*
				*/
#endif
				gamma = numerator / denominator;
				if (ispropper_noun(buffer))
					gamma += proper_noun_boost;
				push_link(*first, last_index_term->term, last_index_term->postings[0].docid, gamma, last_index_term);
//				printf("%s -> %d (gamma = %2.2f / %2.2f)\n", last_index_term->term, last_index_term->postings[0].docid, numerator, denominator);
				}
			}

		qsort(all_links_in_file, (size_t)all_links_in_file_length, sizeof(*all_links_in_file), ANT_link::compare);

		deduplicate_links();
		qsort(all_links_in_file, (size_t)all_links_in_file_length, sizeof(*all_links_in_file), ANT_link::final_compare);

		print_links(orphan_docid, orphan_name, anchors_per_run, targets_per_link, print_mode);

		delete [] file;
		delete [] term_list;
#ifdef REMOVE_ORPHAN_LINKS
		add_or_subtract_orphan_links(ADD_ORPHAN_LINKS, link_index, terms_in_index);
#endif
		num_of_processed_topic++;
		}
fprintf(stderr, "Total %d topics processed\n", num_of_processed_topic);
print_footer();

return 0;
}

