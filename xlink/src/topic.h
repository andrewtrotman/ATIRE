/*
 * topic.h
 *
 *  Created on: Aug 16, 2009
 *      Author: monfee
 */

#ifndef TOPIC_H_
#define TOPIC_H_

#include "ir_task.h"

namespace QLINK {

	class topic
	{
	protected:
		char		filename_[1024];
		char 		*content_;
		long		id_;
		//char		id_name_[1024];
		char		name_[1024];

		ir_task		*task_;

	public:
		topic(const char *filename);
		virtual ~topic();

		void set_filename(const char *filename);
		void set_content(char *file);
		char *get_content() { return content_; }
		long get_id() { return id_; }
		const char *get_name() const { return name_; }

		void task(ir_task *task) { task_ = task; }
		ir_task *task() { return task_; }

		virtual void print_header();
		virtual void print_footer();
	};

}

#endif /* TOPIC_H_ */
