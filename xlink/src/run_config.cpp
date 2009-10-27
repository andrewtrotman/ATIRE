/*
 * run_config.cpp
 *
 *  Created on: Aug 3, 2009
 *      Author: monfee
 */

#include "run_config.h"

using namespace QLINK;


run_config::run_config(char *name) : config(name) {

}

run_config::~run_config() {
}

std::string run_config::get_algorithm_outgoing_name()
{
	return get_value("algorithm_outgoing");
}

std::string run_config::get_algorithm_incoming_name()
{
	return get_value("algorithm_incoming");
}
