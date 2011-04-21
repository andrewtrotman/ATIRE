/*
 * algorithm_ant_link_this.cpp
 *
 *  Created on: Aug 11, 2009
 *      Author: Andrew Troman
 *      Modified by Eric
 */

#include "algorithm_ant_link_this.h"
#include "ant_link_parts.h"
#include "sys_file.h"
#include "assert.h"
#include "outgoing_link.h"
#include "ltw_topic.h"
#include "algorithm_bep.h"
#include "corpus.h"
#include "corpus_txt.h"
#include "xml2txt.h"
#include "ant_link_term.h"
#include "ant_link_posting.h"
#include "language.h"
#include "template_algo.h"
#include "template_comparor.h"
#include "run_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <iostream>

#ifdef INEX_ARCHIVE
	#define COLLECTION_LINK_TAG_NAME "collectionlink"
	#define TOPIC_SIGNITURE "<topic file=\"%d.xml\" name=\"%s\"><outgoing>\n"
#else
	#define COLLECTION_LINK_TAG_NAME "link"
	#define TOPIC_SIGNITURE "<topic file=\"%d\" name=\"%s\"><outgoing>\n"
#endif

using namespace QLINK;
using namespace std;

//char algorithm_ant_link_this::buffer_[1024 * 1024];

algorithm_ant_link_this::algorithm_ant_link_this(ltw_task *task) : algorithm_out(task)
{
	init_variables();
}

//algorithm_ant_link_this::algorithm_ant_link_this(links *links_list)
//{
//	set_links_container(links_list);
//	init_variables();
//}

algorithm_ant_link_this::~algorithm_ant_link_this()
{
//	for (long i = 0; i < link_index_.size(); i++) {
//		delete link_index_[i];
//	}
	delete [] link_index_;
}

void algorithm_ant_link_this::init_variables()
{
	//link_index = NULL;
	lowercase_only = FALSE;

	string crosslink_table = run_config::instance().get_value("crosslink_table");
	if (crosslink_table.length() > 0) {
		crosslink_ = true;
		load_crosslink_table(crosslink_table);
	}
}

void algorithm_ant_link_this::load_crosslink_table(std::string& filename)
{
	ifstream myfile (filename.c_str());

	if (myfile.is_open()) {
		while (! myfile.eof()) {
			string line;
			getline (myfile, line);
			string::size_type pos = line.find_first_of(':');
			if (pos != string::npos) {
				unsigned long doc_id = atol(line.c_str());
//				if (!corpus::instance().exist(doc_id))
//					continue;
				unsigned long target_doc_id = atol(line.c_str() + pos + 1);
				crosslink_table_[doc_id] = target_doc_id;
				//crosslink_table_.insert(make_pair(doc_id, target_doc_id));
			}
		}
		myfile.close();
	}
}

bool algorithm_ant_link_this::has_crosslink(unsigned long id)
{
	map<unsigned long, unsigned long>::iterator it = crosslink_table_.find(id);
	return (it != crosslink_table_.end() && corpus::instance().exist(it->second));
}

unsigned long algorithm_ant_link_this::get_crosslink(unsigned long id)
{
	if (has_crosslink(id))
		return crosslink_table_[id];
	return 0;
}
/*
	READ_INDEX()
	------------
*/
void algorithm_ant_link_this::read_index(const char *filename, long *terms_in_collection)
{
FILE *fp = NULL;
ANT_link_term /**all_terms = NULL, */*term = NULL;
/**
 * there is something wrong with the long type for sscanf in reading the number of terms on 64-bit Linux
 */
int unique_terms = 0, postings = 0, current = 0;
char *term_end = NULL, *from = NULL;
char tmp[6];
int id = 0, cf = 0, lf = 0;

strcpy(tmp, "");

if ((fp = fopen(filename, "rb")) == NULL) {
	/*exit(*/fprintf(stderr, "Cannot open index file:%s\n", filename)/*)*/;
	return;
}

fgets(buffer_, sizeof(buffer_), fp);
sscanf(buffer_, "%d %s", &unique_terms, tmp);
term = link_index_ = new ANT_link_term [unique_terms];

long count = 0;
while (fgets(buffer_, sizeof(buffer_), fp) != NULL)
	{
	count++;
	//term = new ANT_link_term;

	term_end = strchr(buffer_, ':');
	postings = count_char (term_end, '>');
	//term->term = strnnew(buffer_, term_end - buffer_);
	term->term = strndup(buffer_, term_end - buffer_);
	if (!term->term) {
		fprintf(stderr, "reading term error at %d, should be load %d terms", count, unique_terms);
		unique_terms = count;
		break;
	}

	//term->postings_length = postings;
	//term->postings = new ANT_link_posting[term->postings_length];
	term->total_occurences = 0;

	sscanf(term_end + 2, "%d,%d", &(term->document_frequency), &(term->collection_frequency));
//	term_end = strchr(term_end + 1, ';';
	from = term_end;
	for (current = 0; current < postings; current++)
		{
		from = strchr(from, '<') + 1;
		//sscanf(from, "%u,%u,%u", &(term->postings[current].docid), &(term->postings[current].doc_link_frequency), &(term->postings[current].link_frequency));
		sscanf(from, "%d,%d,%d", &id, &cf, &lf);

		term->postings.push_back(new ANT_link_posting);
		ANT_link_posting& curr_posting = *term->postings[current];
		curr_posting.docid = id;
		curr_posting.doc_link_frequency = cf;
		curr_posting.link_frequency = lf;
		if (curr_posting.docid < 0)
			fprintf(stderr, "Error, negative id: %d\n", term->postings[current]->docid);
		term->total_occurences += curr_posting.link_frequency;
		}

	//if (strlen(term->term) > 0) {
		//link_index_.push_back(term);
		if (term->postings.size() > 1)
			std::sort(term->postings.begin(), term->postings.end(), pointer_greater_than<ANT_link_posting>());
			//bubble_sort1<vector<ANT_link_posting *> >(term->postings);
			//std::sort(term->postings.begin(), term->postings.end(), ANT_link_posting::compare);
			//qsort(term->postings, term->postings_length, sizeof(term->postings[0]), ANT_link_posting::compare);

//		fprintf(stderr, "\n");
	//}
		//debug
//		if (strcmp(term->term, "railway station") == 0) {
//			fprintf(stderr, "%s:", term->term);
//			for (int i = 0; i < term->postings.size(); i++)
//				fprintf(stderr, "(%d, %d)", term->postings[i]->docid, term->postings[i]->link_frequency);
//			fprintf(stderr, "\n");
//		}
	term++;

	// for debug
//	if (count > 50000)
//		break;
	}

//assert(count == unique_terms);
*terms_in_collection = count;
//return all_terms;
}

/*
	FIND_TERM_IN_LIST()
	-------------------
*/
ANT_link_term *algorithm_ant_link_this::find_term_in_list(const char *value/*, ANT_link_term *list, long list_length, long this_docid*/)
{
//	link_index_, terms_in_index, orphan_docid_
long low, high, mid;

low = 0;
high = terms_in_index;
while (low < high)
	{
	mid = (low + high) / 2;
	if (strcmp(link_index_[mid].term, value) < 0)
		low = mid + 1;
	else
		high = mid;
	}

#ifdef REMOVE_ORPHAN_LINKS
/*
	remove terms that are in the anchor list but have no postings because the only document used the anchor was this one.
*/
while (low < terms_in_index)
	if (link_index_[low].postings[0]->doc_link_frequency == 0)		// we've been deleted so this is a miss
		low++;
	else
		break;
/*
	remove terms that point to this document
*/
while (low < terms_in_index)
	if (link_index_[low].postings.size() == 1 && link_index_[low].postings[0]->docid == orphan_docid_)		// we point to this document so we delete it
		low++;
	else
		break;
#endif

if ((low < terms_in_index) && (strcmp(value, link_index_[low].term) == 0))
	return &link_index_[low];		// match
else
	{
	if (low < terms_in_index)
		return &link_index_[low];		// not found in list but not after the last term in the list
	else
		return NULL;
	}
}

/*
	GENERATE_COLLECTION_LINK_SET()
	------------------------------
*/
void algorithm_ant_link_this::generate_collection_link_set(char *original_file)
{
char *file, *pos, *end, *copy, *slash, *target_end;
long id;

std::vector<link *>& links_in_orphan = links_->orphan_links();
//long *links_in_orphan_length = links_->orphan_links_length_ptr();

//*links_in_orphan_length = 0;
file = _strdup(original_file);
link *orphan_link = NULL;

pos = strstr(file, "<"COLLECTION_LINK_TAG_NAME);
int count = 0;
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

		copy = strndup(pos + 1, end - pos - 1);
		string_clean(copy, lowercase_only);

//		links_in_orphan_[*links_in_orphan_length]->term = copy;
//		links_in_orphan_[*links_in_orphan_length]->gamma = 0;
//		links_in_orphan_[*links_in_orphan_length]->target_document = id;
		if (copy && strlen(copy) > 0 && !links_->find_orphan(copy)) {
				orphan_link = links_->create_new_orphan_link();
				orphan_link->term = copy;
				orphan_link->gamma = 0;
				orphan_link->target_document = id;
				//(*links_in_orphan_length)++;
				++count;
		}
		else
			free(copy);

		if (links_->orphan_links_length() >= MAX_LINKS_IN_FILE)
			exit(printf("Too many links present in orphan a priori\n"));
		}
	pos = strstr(end, "<"COLLECTION_LINK_TAG_NAME);
	}

//link *link_ptr;
//fprintf(stderr, "found %d links", count);
//for (int i = 0; i < links_in_orphan.size(); i++) {
//	link_ptr = links_in_orphan[i];
//	printf ("#%d: %s, %f, %d\n", i, link_ptr->term, link_ptr->gamma, link_ptr->target_document);
//}

//qsort(links_in_orphan, *links_in_orphan_length, sizeof(*links_in_orphan), ANT_link::string_target_compare);
links_->sort_orphan();
free(file);
}

/*
	ADD_OR_SUBTRACT_ORPHAN_LINKS()
	------------------------------
*/
void algorithm_ant_link_this::add_or_subtract_orphan_links(long add_or_subtract, ANT_link_term *link_index, long terms_in_index)
{
long current, posting;
ANT_link_term *key, *found;

key = new ANT_link_term;

//link **links_in_orphan = links_->orphan_links();
//long *links_in_orphan_length = links_->orphan_links_length_ptr();
std::vector<link *>& links_in_orphan = links_->orphan_links();

for (current = 0; current < links_->orphan_links_length(); current++)
	{
	if (current > 0)
		if (strcmp(links_in_orphan[current]->term, links_in_orphan[current - 1]->term) == 0)		// same term
			if (links_in_orphan[current]->target_document == links_in_orphan[current - 1]->target_document)	// same target
				continue;			// don't do dupicate links as we are only computing the new DF.

	key->term = links_in_orphan[current]->term;
	found = (ANT_link_term *)bsearch(key, link_index_, terms_in_index, sizeof(*link_index_), ANT_link_term::compare);
	if (found != NULL)
		{
		for (posting = 0; posting < found->postings.size(); posting++)
			if (found->postings[posting]->docid == links_in_orphan[current]->target_document) {
				found->postings[posting]->doc_link_frequency = found->postings[posting]->doc_link_frequency + add_or_subtract;
				//fprintf(stderr, "add %d to anchor %s\n", add_or_subtract, key->term);
			}

		if (found->postings.size() > 1)
			std::sort(found->postings.begin(), found->postings.end(), pointer_greater_than<ANT_link_posting>());
			//qsort(found->postings, found->postings.size(), sizeof(found->postings[0]), ANT_link_posting::compare);
		}
	}

key->term = NULL;
delete key;
}

/*
	ISPROPPER_NOUN()
	----------------
*/
long algorithm_ant_link_this::ispropper_noun(char *phrase)
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
void algorithm_ant_link_this::usage(char *exename)
{
exit(printf("Usage:%s [-lowercase] [-noyears] [-runname:name] [-anchors:<n>] [-targets:<n>] [-propernounboost:<n.n>] <index> <file_to_link> ...\n", exename));
}

/*
	MAIN()
	------
*/
int algorithm_ant_link_this::init_params(int argc, char *argv[])
{
targets_per_link = 1, anchors_per_run = 250, print_mode = 0;
proper_noun_boost = 0.0;
//long num_of_processed_topic = 0;

if (argc < 3)
	usage(argv[0]);		// and exit

lowercase_only = FALSE;
int index_argv_param = 0;
char *index_file = NULL;

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
	else if (strncmp(argv[index_argv_param], "-conf", 5) == 0)
	{
		int next_argv_param = index_argv_param;
		next_argv_param++;
		if (*argv[next_argv_param] != '-')
			index_argv_param = next_argv_param;
	}
	else if (strncmp(argv[index_argv_param], "-index", 6) == 0)
	{
		int next_argv_param = index_argv_param;
		next_argv_param++;
		if (*argv[next_argv_param] != '-')
			index_file = argv[next_argv_param];
	}
//	else
//		usage(argv[0]);		// and exit
	}

//if (!index_file) {
//	fprintf(stderr, "the index file must be specified");
//	exit(-1);
//}

/*link_index = */read_index(index_file, &terms_in_index);

//print_header(runname);

return index_argv_param + 1;
}

//void algorithm_ant_link_this::process_topic(char *filename)
//{
//
//	char *file = NULL;
//	sys_file disk;
//
//	disk.list(filename);
//	char *name = NULL;
//	while ((name = disk.next()) != NULL)
//		{
//		file = disk.read_entire_file(name);
//		process_topic_content(file);
//		}
//
//}

void algorithm_ant_link_this::process_topic_text()
{
	algorithm::process_topic_text();

#ifdef REMOVE_ORPHAN_LINKS
	add_or_subtract_orphan_links(ADD_ORPHAN_LINKS, link_index_, terms_in_index);
#endif
}

void algorithm_ant_link_this::process_topic(ltw_topic *a_topic)
{
	char *xml = a_topic->get_content();
	orphan_docid_ = get_doc_id(xml);
	get_doc_name(xml, orphan_name_);


#ifdef REMOVE_ORPHAN_LINKS
	generate_collection_link_set(xml);
	add_or_subtract_orphan_links(SUBTRACT_ORPHAN_LINKS, link_index_, terms_in_index);
#endif

	algorithm::process_topic(a_topic);
}

void algorithm_ant_link_this::process_terms(char **term_list, const char *source)
{
	recommend_anchors(links_, term_list, source);
	fprintf(stderr, "Total %d links found\n", links_->all_links_length());
	links_->sort_links();
}

//void algorithm_ant_link_this::recommend_anchors(links* lx, char **term_list, const char *source)
//{
//	if (terms_in_index <= 0)
//		return;
//
//	char *where_to;
//	char **first, **last;
//	ANT_link_term *index_term, *last_index_term;
//	double gamma, numerator, denominator;
//	long /*terms_in_index, orphan_docid, */param, noom, index_argv_param;
//	long targets_per_link = 1, anchors_per_run = 250, print_mode = 0;
//	const char *runname = "Unknown";
//
//	double proper_noun_boost = 0.0;
//	long num_of_processed_topic = 0;
//	long offset = 0;
//	char *place;
//
//	long term_len = 0;
//	long links_count = 0;
//	bool is_stopword = false;
//
//	for (first = term_list; *first != NULL; first++)
//		{
//		where_to = buffer_;
//		last_index_term = NULL;
//
//		if (strcasecmp(buffer_, "railway station") == 0)
//			offset = 0;
//
//		for (last = first; *last != NULL; last++)
//			{
//			if (where_to == buffer_)
//				{
//				strcpy(buffer_, *first);
//				where_to = buffer_ + strlen(buffer_);
//				}
//			else
//				{
//				*where_to++ = ' ';
//				strcpy(where_to, *last);
//				where_to += strlen(*last);
//				}
//
////			vector<ANT_link_term *>::iterator iter = std::find_if(link_index_.begin(), link_index_.end(), term_part_equal(buffer_));
////			if (iter != link_index_.end())
////				index_term = *iter;
////			else
////				break;
//			index_term = find_term_in_list(buffer_);
//
//			if (index_term == NULL)
//				break;									// we're after the last term in the list
//
//			if (strcmp(buffer_, index_term->term) == 0)
//				last_index_term = index_term;			// we're a term in the list, but might be a longer one so keep looking
//
//			if (strncmp(buffer_, index_term->term, strlen(buffer_)) != 0)
//				break;									// we can't be a substring so we're done
//			}
//
//		if (last_index_term != NULL) {
//			noom = 0;
//			denominator = (double)last_index_term->document_frequency;
//#ifdef REMOVE_ORPHAN_LINKS
//			denominator--;
//			if (last_index_term->postings[noom]->docid == orphan_docid_)			// not alowed to use links that point to the orphan
//				noom = 1;
//#endif
//			numerator = (double)last_index_term->postings[noom]->doc_link_frequency;
//			gamma = numerator / denominator;
//			if (ispropper_noun(last_index_term->term))
//				gamma += proper_noun_boost;
//			place = *first;
//			offset = place - source;
//			term_len = strlen(last_index_term->term);
//
//			fprintf(stderr, "%s -> %d (gamma = %2.2f / %2.2f)", last_index_term->term, last_index_term->postings[0]->docid, numerator, denominator);
//
////			is_stopword = false;
////			if (!strpbrk(last_index_term->term, "- "))
////				is_stopword = language::isstopword(last_index_term->term);
////
////			if (!is_stopword) {
//				strncpy(buffer_, offset + text_, term_len);
//				buffer_[term_len] = '\0';
////				if (strcmp(last_index_term->term, "the church of england") == 0)
////					fputs("I got you", stderr);
//				//if (!lx->find(buffer_)) {
//					//lx->push_link(*first, offset, buffer_, last_index_term->postings[0]->docid, gamma, last_index_term);
//				if (!lx->find(last_index_term->term)) {
//					lx->push_link(*first, offset, last_index_term->term, last_index_term->postings[0]->docid, gamma, last_index_term);
//					// debug
////					fprintf(stderr, "found a %s anchor\n", buffer__);
//					links_count++;
//				}
////				else
////					fprintf(stderr, "Duplicated");
//				fprintf(stderr, "\n");
//
////			}
//
//		}
//	}
//	// debug
//	if (links_count <= 0)
//		fprintf(stderr, "added %d links\n", links_count);
//}

void algorithm_ant_link_this::add_link(ANT_link_term *term, char **term_list)
{
	double gamma, numerator, denominator;
	long /*terms_in_index, orphan_docid, */param, noom, index_argv_param;
	long targets_per_link = 1, anchors_per_run = 250, print_mode = 0;
	const char *runname = "Unknown";

	double proper_noun_boost = 0.0;
	long num_of_processed_topic = 0;
	long offset = 0;
	char *place;

	long term_len = 0;
	long links_count = 0;
	bool is_stopword = false;

	noom = 0;
	denominator = (double)term->document_frequency;
#ifdef REMOVE_ORPHAN_LINKS
	denominator--;
	if (term->postings[noom]->docid == orphan_docid_)			// not alowed to use links that point to the orphan
		noom = 1;
#endif
	numerator = (double)term->postings[noom]->doc_link_frequency;
	gamma = numerator / denominator;
	if (ispropper_noun(term->term))
		gamma += proper_noun_boost;
//	place = current_term_;
//	offset = place - source_;
//	term_len = strlen(term->term);

#ifdef DEBUG
	fprintf(stderr, "%s -> %d (gamma = %2.2f / %2.2f)\n", term->term, term->postings[0]->docid, numerator, denominator);
#endif

//			is_stopword = false;
//			if (!strpbrk(term->term, "- "))
//				is_stopword = language::isstopword(term->term);
//
//			if (!is_stopword) {
//		strncpy(buffer_, offset + text_, term_len);
//		buffer_[term_len] = '\0';
//				if (strcmp(term->term, "the church of england") == 0)
//					fputs("I got you", stderr);
		//if (!lx->find(buffer_)) {
			//lx->push_link(*first, offset, buffer_, term->postings[0]->docid, gamma, term);
	offset = assign_link_term(term, term_list);
	if (!links_->find(term->term)) {
		links_->push_link(current_term_, offset, buffer_, term->postings[0]->docid, gamma, term);
		// debug
//					fprintf(stderr, "found a %s anchor\n", buffer__);
		links_count++;
	}
//				else
//					fprintf(stderr, "Duplicated");
//	fprintf(stderr, "\n");
}
