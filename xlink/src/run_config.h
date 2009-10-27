/*
 * run_config.h
 *
 *  Created on: Aug 3, 2009
 *      Author: monfee
 */

#ifndef RUN_CONFIG_H_
#define RUN_CONFIG_H_

#include "config.h"
#include <string>

	namespace QLINK {

	/*
	 * LTW run config
	 */
	class run_config : public config {
	public:
		run_config(char *name);
		virtual ~run_config();

		std::string get_algorithm_outgoing_name();
		std::string get_algorithm_incoming_name();
	};

}
#endif /* RUN_CONFIG_H_ */
