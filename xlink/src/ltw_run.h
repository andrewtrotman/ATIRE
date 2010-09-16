/*
 * ltw_run.h
 *
 *  Created on: Aug 3, 2009
 *      Author: monfee
 */

#ifndef LTW_RUN_H_
#define LTW_RUN_H_

#include "run.h"
#include "ltw_task.h"

namespace QLINK {

	/*
	 * Link-the-Wiki run
	 */
	class ltw_run : public run {
	private:
		ltw_task		*task_;

	public:
		ltw_run(char *configfile);
		virtual ~ltw_run();

		void create();
		void print();
		ltw_task *get_task() { return task_; }

	protected:
		void init();
		std::string get_home(const char *name);

		void print_header();
		void print_links(long orphan_docid, char *orphan_name, long links_to_print, long max_targets_per_anchor, long mode);

	private:

	};

}
#endif /* LTW_RUN_H_ */
