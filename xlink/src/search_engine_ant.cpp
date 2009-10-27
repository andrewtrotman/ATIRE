/*
 * search_engine_ant.cpp
 *
 *  Created on: Sep 30, 2009
 *      Author: monfee
 */

#include "search_engine_ant.h"

#include <ant_api.h>

using namespace QLINK;

#define FALSE 0;

search_engine_ant::search_engine_ant(const char *doclist_filename, const char *index_filename)
{
	docids_ = NULL;

	ant_ = ant_easy_init();
	ant_params(ant_)->logo = FALSE;
	ant_params(ant_)->stats = NONE;
	ant_params(ant_)->output = stderr;
	ant_params(ant_)->doclist_filename = doclist_filename;
	ant_params(ant_)->index_filename = index_filename;

	ant_setup(ant_);
}

search_engine_ant::~search_engine_ant()
{
	ant_free(ant_);
}

void search_engine_ant::search(const char * term)
{
	docids_ = ant_search(ant_, &hits_, (char *)term);
}
