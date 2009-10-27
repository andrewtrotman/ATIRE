/*
 * algorithm_config.h
 *
 *  Created on: Aug 4, 2009
 *      Author: monfee
 */

#ifndef ALGORITHM_CONFIG_H_
#define ALGORITHM_CONFIG_H_

/*
 *
 */
#include "config.h"

namespace QLINK {
	class algorithm_config: public config {
	public:
		algorithm_config(const char *name);
		algorithm_config();
		virtual ~algorithm_config();
	};
}

#endif /* ALGORITHM_CONFIG_H_ */
