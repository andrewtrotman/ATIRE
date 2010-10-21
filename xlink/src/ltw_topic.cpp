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
	if (id_ > 0)
		text_of_xml_ = xml2txt::instance().gettext(id_, content_);
	else
		text_of_xml_ = xml2txt::instance().gettearatext(name_, content_);

	if (!text_of_xml_) {
		text_of_xml_ = new char[strlen(content_) + 1];
		strcpy(text_of_xml_, content_);
		//string_clean(text_of_xml_, 0, 0);
		xml2txt::instance().clean_tags(text_of_xml_, 0);
	}
}
