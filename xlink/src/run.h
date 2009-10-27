/*
 * run.h
 *
 *  Created on: Aug 3, 2009
 *      Author: monfee
 */

#ifndef RUN_H_
#define RUN_H_

#include "system_info.h"
#include "run_config.h"
#include <string>

namespace QLINK {

	/*
	 *
	 */
	class run {
	private:
		run_config		run_conf_;

	public:
		std::string		affiliation;
		std::string		id;
		std::string		run_id;
		std::string		run_name;
		std::string		task;
		std::string		header;
		std::string		footer;
		system_info		system_information;

	private:
		void init();

	protected:
		virtual void print_header();
		virtual void print_footer();
		//virtual void print() = 0;

	public:
		run(char *configfile);
		virtual ~run();

		virtual void create() = 0;
		virtual void print() = 0;

		const run_config& get_config() const { return run_conf_; }
		run_config& get_config() { return run_conf_; }
	};

}

#endif /* RUN_H_ */
