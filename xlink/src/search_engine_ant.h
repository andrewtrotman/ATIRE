/*
 * search_engine_ant.h
 *
 *  Created on: Sep 30, 2009
 *      Author: monfee
 */

#ifndef SEARCH_ENGINE_ANT_H_
#define SEARCH_ENGINE_ANT_H_

#include "search_engine.h"

typedef void ANT;

class search_engine_ant: public search_engine
{
private:
	ANT	*ant_;

public:
	search_engine_ant(const char *doclist_filename = "doclist.aspt", const char *index_filename = "index.aspt");
	virtual ~search_engine_ant();

	void search(const char * term);
};

#endif /* SEARCH_ENGINE_ANT_H_ */
