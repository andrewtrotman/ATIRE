/*
 * algorithm_config.cpp
 *
 *  Created on: Aug 4, 2009
 *      Author: monfee
 */

#include "algorithm_config.h"

using namespace QLINK;


algorithm_config::algorithm_config() : config("algorithm.conf") {

}

algorithm_config::algorithm_config(const char *name) : config(name) {

}

algorithm_config::~algorithm_config() {

}
