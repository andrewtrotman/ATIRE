/*
 * algorithm_config.cpp
 *
 *  Created on: Aug 4, 2009
 *      Author: monfee
 */

#include "algorithm_config.h"

using namespace QLINK;

const char *algorithm_config::DEFAULT_CONFIG_FILENAME = "algorithm.conf";

algorithm_config::algorithm_config() : config(DEFAULT_CONFIG_FILENAME) {

}

algorithm_config::algorithm_config(const char *name) : config(name) {

}

algorithm_config::~algorithm_config() {

}
