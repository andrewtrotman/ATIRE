/*
 * ltw.cpp
 *
 *  Created on: Aug 3, 2009
 *      Author: monfee
 */

#include <iostream>

#include "ltw_run.h"
#include "cmdline_options.h"
#include <string.h>

using namespace QLINK;


void create_run(int argc, char **argv)
{
	char *config_file = NULL;

	int index_argv_param = 1;
	int next_argv_param = 0;
	for (; index_argv_param < argc; index_argv_param++) {
		char *what = argv[index_argv_param];
		if (strncmp(what, "-conf", 5) == 0) {
			next_argv_param = index_argv_param + 1;
			if (*argv[next_argv_param] != '-')
				config_file = argv[++index_argv_param];
		} else if (strncmp(what, "-index", 6) == 0) {
			++index_argv_param;
		}else if (*what == '-') {

//			if (!strchr(what, ':'))
				index_argv_param++;
		} else
			break;
	}
	ltw_task::topic_param_start = index_argv_param;

	ltw_run *run = NULL;
	if (config_file)
		run = new ltw_run(config_file);
	else
		run = new ltw_run("ltw.conf");
	run->create();
	delete run;
}

int main(int argc, char **argv)
{
//    if ( argc == 1 ) {
//        std::cout
//            << "Usage: "
//            << argv[ 0 ]
//            << " run_config_file"
//            << std::endl
//            ;
//        return 1;
//    }
    cmdline_options::argc = argc;
    cmdline_options::argv = argv;

	create_run(argc, argv);
	return 0;
}
