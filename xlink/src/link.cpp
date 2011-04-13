/*
 * link.cpp
 *
 *  Created on: Aug 3, 2009
 *      Author: monfee, Andrew Trotman
 */

#include "link.h"
#include "link_print.h"
#include "ant_link_term.h"
#include "ant_link_posting.h"
#include "ant_link_parts.h"
#include "application_out.h"
#include "sys_file.h"
#include "corpus.h"
#include "algorithm.h"

#include <stdlib.h>
#include <string.h>

#include <string>
#include <iostream>

using namespace QLINK;
using namespace std;


link::link() {
	place_in_file = NULL;
	term = NULL;
	link_term = NULL;
	to_clean_ = false;
	gamma = 0.0;
	offset = 0;
	target_document = -1;
}

link::~link() {
	// segment fault if run below code
	// found out later
	if (term) {
		free(term);
		term = NULL;
	}
	if (to_clean_) {
		delete link_term;
		link_term = NULL;
	}
}

void link::print()
{
	print_header();
	//printf("%d", target_document);
	aout << target_document;
	print_footer();
}


bool link::print_target(long anchor, algorithm *algor)
{
	bool ret = true;
	if (algor != NULL)
		ret = algor->is_valid_link(link_term->postings[anchor]->docid);

	if (ret) {
		print_header();
		//printf("%d", link_term->postings[anchor]->docid);
		aout << link_term->postings[anchor]->docid;
		print_footer();
	}
	return ret;
}

bool link::print_anchor(long beps_to_print, bool id_or_name, algorithm *algor)
{
	char buf[1024 * 10];
	int count = 0;

	bool ret = false;
	if (algor != NULL) {
		if (link_term->postings.size() > 0)
			for (int i = 0; i < link_term->postings.size(); i++) {
				if (algor->is_valid_link(link_term->postings[i]->docid)) {
					ret = true;
					break;
				}
			}
		else
			ret = algor->is_valid_link(target_document);
	}
	if (ret) {
		sprintf(buf, "\t\t\t<anchor offset=\"%d\" length=\"%d\" name=\"%s\">\n", offset, strlen(term), term);
		aout << buf;
		const char *format = link_print::target_format.c_str();
		if (link_term->postings.size() > 0) {
			for (int i = 0; i < link_term->postings.size(); i++) {
				if (link_term->postings[i]->docid < 0 && id_or_name)
					continue;

				unsigned long id = 0;
				if (id_or_name)
					id = link_term->postings[i]->docid;
				else
					id = atoi(link_term->postings[i]->desc);

#ifdef CROSSLINK
				string filename = corpus::instance().id2docpath(id);
				if (!sys_file::exist(filename.c_str())) {
					cerr << "No target file found:" << filename << endl;
					continue;
				}
				std::string target_title = corpus::instance().gettitle(filename);
				sprintf(buf, format, link_term->postings[i]->offset, target_lang, target_title.c_str(), id);
#else
				sprintf(buf, format, link_term->postings[i]->offset, id);
#endif
				aout << buf;
				++count;
				if (count >= beps_to_print)
					break;
			}
		}
		else {
			sprintf(buf, format, 0, target_document);
			aout << buf;
		}
		//puts("\t\t\t</anchor>\n");
		aout << "\t\t\t</anchor>\n";
	}
	return ret;
}

void link::print_bep(long beps_to_print)
{
	char buf[255];
	if (!link_term) {
		sprintf(buf, "\t\t\t<bep offset=\"%d\">\n", 0);
		aout << buf;
		sprintf(buf, "\t\t\t\t<fromanchor offset=\"%d\" length=\"%d\" file=\"%d\">%s</fromanchor>\n",
				offset, strlen(term), target_document, term);
		aout << buf;
//		puts("\t\t\t</bep>\n");
		aout << "\t\t\t</bep>\n";
	}
	else {
		int count = 0;
		for (int i = 0; i < link_term->postings.size(); i++) {
			if (link_term->postings[i]->docid < 0)
				continue;

			sprintf(buf, "\t\t\t<bep offset=\"%d\">\n", link_term->postings[i]->offset);
			aout << buf;
			sprintf(buf, "\t\t\t\t<fromanchor offset=\"%d\" length=\"%d\" file=\"%d\">%s</fromanchor>\n",
					offset, strlen(term), target_document, term);
			aout << buf;
			//puts("\t\t\t</bep>\n");
			aout << "\t\t\t</bep>\n";
			++count;
			if (count >= beps_to_print)
				break;
		}
	}
}

/*
	LINK::COMPARE()
	-------------------
*/
bool link::compare(const link *one, const link *two)
{
	//link *one = NULL, *two = NULL;
	double diff = 0.0;
	bool cmp = false;

	//one = (link *)a;
	//two = (link *)b;

	diff = two->gamma - one->gamma;
	if (diff < 0)
		return false;
	else if (diff > 0)
		return true;

	if ((cmp = strcmp(one->term, two->term)) == 0)
		return one->place_in_file > two->place_in_file;
		//return one->place_in_file > two->place_in_file ? 1 : one->place_in_file == two->place_in_file ? 0 : -1;

	return cmp;
}

/*
	LINK::FINAL_COMPARE()
	-------------------------
*/
bool link::final_compare(const link *one, const link *two)
{
//link *one = NULL, *two = NULL;
double diff = 0.0;

//one = (link *)a;
//two = (link *)b;

diff = two->gamma - one->gamma;
if (diff < 0)
	return true;
else if (diff > 0)
	return false;
else
	return one->place_in_file > two->place_in_file; // ? 1 : one->place_in_file == two->place_in_file ? 0 : -1;
}

/*
	LINK::STRING_TARGET_COMPARE()
	---------------------------------
*/
bool link::string_target_compare(const link *one, const link *two)
{
//link *one = NULL, *two = NULL;
bool cmp = false;

//one = (link *)a;
//two = (link *)b;

if ((cmp = strcmp(one->term, two->term)) == 0)
	cmp = one->target_document > two->target_document;

return cmp;
}

/*
	LINK::TERM_COMPARE()
	---------------------------------
*/
bool link::term_compare(const link *one, const link *two)
{
//link *one = NULL, *two = NULL;
bool cmp;

//one = (link *)a;
//two = (link *)b;
int ret = 0;
long one_term_count = count_char(one->term, ' ') + 1;
long two_term_count = count_char(two->term, ' ') + 1;

if (one_term_count == two_term_count) {
	if (strlen(one->term) == strlen(two->term)) {
		if ((ret = strcmp(one->term, two->term)) == 0)
			cmp = one->target_document > two->target_document;
		else
			cmp = ret > 0;
	}
	else
		cmp = strlen(one->term) > strlen(two->term);
}
else
	cmp = one_term_count > two_term_count;

return cmp;
}

bool link::operator==(const link& rhs)
{
	return strcmp(rhs.term, this->term) == 0;
}

bool link::operator==(const char *term)
{
	int cmp = strcmp(this->term, term);
	return cmp == 0;
}

const bool link_string_target_compare::operator()(const link *a, const link * b) const
{
	// check for 0
	if (a == 0)
		return b == 0; // if b is also 0, then they are equal, hence b is not > than a
	else if (b == 0)
		return true;
	else {
		int cmp = 0;

		if ((cmp = strcmp(a->term, b->term)) == 0)
			cmp = a->target_document > b->target_document;

		return cmp > 0;
	}
}

const bool link_compare::operator() (const link *one, const link * two) const
{
	//link *one = NULL, *two = NULL;
	double diff = 0.0;

	//one = (link *)a;
	//two = (link *)b;

	diff = two->gamma - one->gamma;
	if (diff < 0)
		return true;
	else if (diff > 0)
		return false;
	else
		return two->target_document > one->target_document;
		//return one->place_in_file > two->place_in_file ? true : one->place_in_file == two->place_in_file ? one->target_document < two->target_document : false;
}

const bool term_compare::operator()(const link *one, const link *two) const
{
//link *one = NULL, *two = NULL;
bool cmp;

//one = (link *)a;
//two = (link *)b;
int ret = 0;
long one_term_count = count_char(one->term, ' ') + 1;
long two_term_count = count_char(two->term, ' ') + 1;

if (one_term_count == two_term_count) {
	if (strlen(one->term) == strlen(two->term)) {
		if ((ret = strcmp(one->term, two->term)) == 0)
			cmp = one->target_document > two->target_document;
		else
			cmp = ret > 0;
	}
	else
		cmp = strlen(one->term) > strlen(two->term);
		//cmp = one->target_document < two->target_document;
}
else
	cmp = one_term_count > two_term_count;

return cmp;
}
