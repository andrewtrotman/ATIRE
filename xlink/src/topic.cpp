/*
 * topic.cpp
 *
 *  Created on: Aug 16, 2009
 *      Author: monfee
 */

#include "topic.h"
#include "ant_link_parts.h"
#include "sys_file.h"
#include "application_out.h"

#include <libgen.h>
#include <stdio.h>
#include <string.h>

#define TOPIC_SIGNITURE_ID "	<topic file=\"%d\" name=\"%s\">\n"
#define TOPIC_SIGNITURE_ID_NAME "	<topic file=\"%s\" name=\"%s\">\n"

using namespace QLINK;

topic::topic(const char *filename)
{
	id_ = -1;
	strcpy(name_, "");
	strcpy(filename_, "");

	set_filename(filename);
	content_ = sys_file::read_entire_file(filename);
	set_content(content_);
}

topic::~topic()
{
	if (content_)
		delete [] content_;
}

void topic::set_filename(const char *filename)
{
	strcpy(filename_, filename);
}

void topic::set_content(char *file)
{
	content_ = file;

	id_ = get_doc_id(file);
	get_doc_name(file, name_);
	if (strlen(name_) <= 0) {
		char filename[1024];
		char *end = NULL;
		strcpy(filename, filename_);
		strcpy(name_, basename(filename));
		end = strchr(name_, '.');
		if (end != NULL)
			name_[end - name_] = '\0';
	}

}

void topic::print_header()
{
	char buf[255];
	if (id_ > 0)
		sprintf(buf, TOPIC_SIGNITURE_ID, id_, name_);
	else
		sprintf(buf, TOPIC_SIGNITURE_ID_NAME, name_, name_);
//	else
//		fprintf(stderr, "Topic ID or ID_NAME error");
	aout << buf;
}

void topic::print_footer()
{
	//puts("	</topic>");
	aout << "\t</topic>\n";
}
