/*
 * ltw_topic.cpp
 *
 *  Created on: Sep 9, 2009
 *      Author: monfee
 */

#include "ltw_topic.h"
#include "xml2txt.h"
#include "corpus_txt.h"
#include "sys_file.h"
#include "ant_link_parts.h"
#include "string_utils.h"

#include <stdlib.h>

using namespace QLINK;

ltw_topic::ltw_topic(const char *file) : topic(file)
{
	text_of_xml_ = NULL;
}

ltw_topic::~ltw_topic()
{
	if (text_of_xml_)
		delete [] text_of_xml_;
		//free(text_of_xml_);
}

char *ltw_topic::get_text()
{
	if (!text_of_xml_)
		xml_to_text();
	return text_of_xml_; /*? text_of_xml_ : content_;*/
}

void ltw_topic::xml_to_text()
{
	//text_of_xml_ = xml2txt::instance().convert(content_);
	//text_of_xml_ = sys_file::read_entire_file(corpus_txt::instance().id2docpath(id_).c_str());
	if (id_ > 0) {
		text_of_xml_ = xml2txt::instance().gettext(filename_, NULL, content_);
		//text_of_xml_ = xml2txt::instance().gettext(id_, content_);
	}
	else
		text_of_xml_ = xml2txt::instance().gettearatext(name_, content_);

	if (!text_of_xml_) {
		//text_of_xml_ = new char[strlen(content_) + 1];
		//strcpy(text_of_xml_, content_);
		//string_clean(text_of_xml_, 0, 0);
		text_of_xml_ = xml2txt::instance().clean_tags(content_, 0);
	}

	/*
	 *
	 *
	 */
	std::string text_file = number_to_string(id_) + ".txt";
//	sys_file::write(text_of_xml_, text_file.c_str());
}

long QLINK::ltw_topic::get_term_len(long  offset, char *term/*, bool is_cjk_lang*/)
{
	char *start, *end, *where_to = term;
	long term_len = 0, char_len;
	long target_term_len = strlen(term);

	char buffer[1024];

	end = start = content_ + offset;

	while (*where_to != '\0') {
		char_len = language::utf8_bytes(where_to);
//		strncpy(buffer, start, char_len);

		end += char_len;

		if (*where_to == ' ') {
			while (*where_to == ' ')
				++where_to;
			while (*end == ' ')
				++end;
		}
		else {
			if (isspace(*end)) {
//				if (is_cjk_lang) {
					end = start;
					break;
//				}
			}
		}
		if (*end == '<') {
			while (*end != '\0' && *end != '>')
				++end;
			if (*end != '\0')
				++end;
		}

		where_to += char_len;
//		if (term_len >= target_term_len)
//			break;
	}
	term_len =  end - start;
	return term_len;
}


