/*
 * ltw_topic.h
 *
 *  Created on: Sep 9, 2009
 *      Author: monfee
 */

#ifndef LTW_TOPIC_H_
#define LTW_TOPIC_H_

#include "topic.h"
#include "ltw_task.h"

namespace QLINK {
	//class ltw_task;

	class ltw_topic: public QLINK::topic
	{
	private:
		ltw_task	*ltw_task_;
		char		*text_of_xml_;

	public:
		ltw_topic(const char *file);
		virtual ~ltw_topic();

		void set_ltw_task(ltw_task *a_task) { ltw_task_ = a_task; task(static_cast<ir_task *>(ltw_task_)); }
		ltw_task *get_ltw_task() { return ltw_task_; }

		char *get_text();
		void xml_to_text();

		long get_term_len(long offset, char *term, bool is_cjk_lang);
	};

}

#endif /* LTW_TOPIC_H_ */
