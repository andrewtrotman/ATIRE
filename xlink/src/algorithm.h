/*
 * algorithm.h
 *
 *  Created on: Aug 3, 2009
 *      Author: monfee
 */

#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include "links.h"

namespace QLINK {
	class algorithm_config;
	class ltw_task;

	/*
	 *
	 */
	class algorithm {
	public:
		enum run_mode { WITH_CMDLINE_OPTIONS, SIMPLE };

	protected:
		run_mode 				mode_;
		int						topic_param_start_;

		links					*links_;

		/*
		 * pointers for the convenient access members of links
		 */
//		ANT_link *all_links_in_file; //[MAX_LINKS_IN_FILE];
//		long long *all_links_in_file_length;
//
//		ANT_link *links_in_orphan; //[MAX_LINKS_IN_FILE];
//		long *links_in_orphan_length;
		algorithm_config		*config_;
		const char 				*text_;
		const char 				*xml_;
		long 					lowercase_only;					// are we in lowercase or mixed-case matching mode?
		long					stopword_no_;
		char					*source_;
		char		 			*current_term_;
		char 					buffer_[1024 * 1024];

		long 					orphan_docid_;

		ltw_task				*ltw_task_;

		bool 					use_utf8_token_matching_; // which means that matching without spaces, all the spaces in string will be removed
		bool 					crosslink_;

		char					**token_address_;
		long					current_index_;
	public:
		algorithm(ltw_task *task);
		algorithm() {}
		virtual ~algorithm();

		virtual int init_params(int argc, char *argv[]);
		virtual int init_params();

		virtual void process_topic(char *filename) {}
		virtual void process_topic_content(char *file) {}
		virtual void process_topic(ltw_topic *a_topic);
		virtual void process_links(ltw_topic *a_topic, algorithm *bep_algor);
		virtual void process_links(const char *file, algorithm *bep_algor) {}
		virtual void process_topic_text();

		int topic_param_start() { return topic_param_start_; }
		void set_links_container(links *container);
		void set_task(ltw_task *task) { ltw_task_ = task; }

		virtual bool has_crosslink(unsigned long id) { return true; }
		virtual unsigned long get_crosslink(unsigned long id) { return id; }

	protected:
		virtual void process_terms(char **term_list, const char *source) {}

	private:
		void init();
		void clear_token_address();
	};

	typedef algorithm	algorithm_in;
}

#endif /* ALGORITHM_H_ */
