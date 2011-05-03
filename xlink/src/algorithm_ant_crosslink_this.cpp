/*
 * algorithm_ant_crosslink_this.cpp
 *
 *  Created on: Apr 30, 2011
 *      Author: monfee
 */

#include "algorithm_ant_crosslink_this.h"
#include "translation.h"
#include "ltw_task.h"
#include "ant_link_term.h"

#include <string.h>

namespace QLINK
{

algorithm_ant_crosslink_this::algorithm_ant_crosslink_this(ltw_task *task) : find_anchors_with_this_(task), algorithm_ant_link_this::algorithm_ant_link_this(task), algorithm_out(task)
{

}

algorithm_ant_crosslink_this::~algorithm_ant_crosslink_this()
{

}

int algorithm_ant_crosslink_this::init_params(int argc, char *argv[])
{
int index_argv_param = 0;
char *index_file = NULL;

find_anchors_with_this_.set_lowercase(TRUE);

int next_argv_param = find_anchors_with_this_.init_params(argc, argv);

for (index_argv_param = 1; index_argv_param < argc; index_argv_param++)
	{
	if (strncmp(argv[index_argv_param], "-crossindex", 11) == 0)
		{
			index_file = strchr(argv[index_argv_param], ':') + 1;

			if (index_file != NULL)
				read_index(index_file, &terms_in_index);
			break;
		}
	}
return next_argv_param;
}

ANT_link_term *algorithm_ant_crosslink_this::find_term_in_list(const char *value)
{
	return find_anchors_with_this_.find_term_in_list(value);
}

void algorithm_ant_crosslink_this::add_link(ANT_link_term *term, char **term_list)
{
std::string result = translation::instance().translate(term->term, (std::string(ltw_task_->get_source_lang()) + "|" + std::string(ltw_task_->get_target_lang())).c_str());

ANT_link_term *crossterm = NULL;
if ((crossterm = algorithm_ant_link_this::find_term_in_list(result.c_str())) != NULL)
	algorithm_ant_link_this::add_link(crossterm, term_list);
}

}
