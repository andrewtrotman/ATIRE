/*
 * incoming_links.cpp
 *
 *  Created on: Aug 16, 2009
 *      Author: monfee
 */

#include "incoming_links.h"
#include "incoming_link.h"
#include "ltw_topic.h"
#include "algorithm_bep.h"
#include "xml2txt.h"
#include "corpus.h"
#include "corpus_txt.h"
#include "sys_file.h"
#include "ant_link_parts.h"
#include "ant_link_term.h"
#include "ant_link_posting.h"
#include "application_out.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace QLINK;


incoming_links::incoming_links(ltw_topic *a_topic) : links(a_topic)
{
	init();
}

incoming_links::incoming_links()
{
	init();
}

incoming_links::~incoming_links()
{
}

inline QLINK::link *incoming_links::link_maker() {
	return new incoming_link;
}

void incoming_links::init_links()
{
	//links_in_orphan = new incoming_link[MAX_LINKS_IN_FILE];
//	all_links_in_file = new incoming_link[MAX_LINKS_IN_FILE];
//
//	for (int i = 0; i < MAX_LINKS_IN_FILE; i++) {
//		links_in_orphan[i] = new incoming_link;
//	}

	//all_links_in_file_length = 0;
	//links_in_orphan_length = 0;
}

void incoming_links::init() {
	type(INCOMING_LINK_TYPE);

	if (topic_->get_ltw_task()->is_a2b_task()) {
		links_to_print_ = 250;
		beps_to_print_ = 1;
	}

	init_links();
}

void incoming_links::print_header()
{
//	puts("	<incoming>");
	aout << "	<incoming>\n";
}

void incoming_links::print_footer()
{
//	puts("	</incoming>\n");
	aout << "	</incoming>\n";
}

void incoming_links::print_links(long orphan_docid, const char *orphan_name, long links_to_print, long max_targets_per_anchor, long mode)
{
	long links_printed = 0;
	long result = 0;

	print_link_tag_header();
	incoming_link *current_link = NULL;
	while ((result < all_links_in_file_.size()) && (links_printed < links_to_print_)) {
		current_link = reinterpret_cast<incoming_link *>(all_links_in_file_[result]);
		if (!current_link->term || strlen(current_link->term) == 0) {
			result++;		// in the case of empty term
			continue;
		}
		if (links_printed > 0)
			aout << ", ";
			//printf(", ");
		//printf("%d", current_link->target_document);
		current_link->print();
		links_printed++;
		result++;
	}

	print_link_tag_footer();

}


void incoming_links::print_anchors(long orphan_docid, const char *orphan_name)
{
	long links_printed = 0;
	long result = 0;

	incoming_link *current_link = NULL;
	while ((result < all_links_in_file_.size()) && (links_printed < links_to_print_)) {
		current_link = reinterpret_cast<incoming_link *>(all_links_in_file_[result]);
		if (!current_link->term || strlen(current_link->term) == 0) {
			result++;		// in the case of empty term
			continue;
		}

		current_link->print_bep();
		links_printed++;
		result++;
	}
}

void incoming_links::find_anchor()
{
	if (!bep_algor_)
		return;

	//this->require_cleanup();
	link* curr_link = NULL;
	for (int i = 0; i < all_links_length() && i < links_to_print_; i++) {
		curr_link = all_links()[i];
		std::string doc = corpus::instance().id2docpath(curr_link->target_document);
		char *content = sys_file::read_entire_file(doc.c_str());

		if (!content) {
			curr_link->target_document = 0 - curr_link->target_document;
			continue;
		}
	//	char *text = sys_file::read_entire_file(corpus_txt::instance().id2docpath(curr_link->target_document).c_str());
		//char *text = xml2txt::instance().convert(content);
		char *text = xml2txt::instance().gettext(curr_link->target_document, content);
		char *textcopy = strdup(text);

		char title[1024];
		char term[1024];
		strcpy(term, this->topic_->get_name());
		get_doc_name(content, title);
		string_tolower(title);
		string_tolower(textcopy);
		string_tolower(term);

		if (!curr_link->link_term) {
			curr_link->link_term = new ANT_link_term;
			//curr_link->link_term->postings_length = 1;
			ANT_link_posting *posting = new ANT_link_posting;
			posting->docid = this->topic_->get_id();
			curr_link->link_term->postings.push_back(posting); //[curr_link->link_term->postings_length];
			curr_link->link_term->total_occurences = 0;
			curr_link->require_cleanup();
		}

		// get the anchor
		char *where = NULL;
		long title_offset = strlen(title) + 2;
		char *source = textcopy + title_offset;
		long offset = 0;
		int term_len = find_phrase(source, term, &offset);
		//long offset = where - source;
		curr_link->offset = title_offset + offset;
		//free(curr_link->term);
		curr_link->term = strndup(text + curr_link->offset, term_len);

		// debug
	//	if (curr_link->target_document == 214711)
	//		puts("I got you");

		// get the bep
		if (bep_algor_) {
			bep_algor_->setsource(text);
			bep_algor_->set_anchor_offset(curr_link->offset);
			curr_link->link_term->postings[0]->offset = bep_algor_->find_bep(this->topic_, title);
		}
		//free(text);
		delete [] content;
		delete [] text;
		free(textcopy);
	}
}

