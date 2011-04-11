/*
 * task.h
 *
 *  Created on: Aug 16, 2009
 *      Author: monfee
 */

#ifndef __LTW_TASK_H__
#define __LTW_TASK_H__

//#include "outgoing_links.h"
//#include "incoming_links.h"
//#include "links.h"

#include <string>
#include <map>

#include "ir_task.h"
#include "algorithm.h"

namespace QLINK {
	class algorithm_out;
	//class algorithm_in;
	class algorithm_bep;
	class links;

	class ltw_task : public ir_task
	{
	public:
		enum ltw_task_type { LTW_TASK_NONE = 0,
								LTW_LTW_F2F = 1,
								LTW_LTW_A2B = 2,
								LTW_LTAra_A2B = 3,
								LTW_LTAraTW_A2B = 4
								};

		enum ltw_algorithm { LTW_NONE = 0,
							 LTW_ANT = 1,
							  LTW_TANG = 2,
							  LTW_INCOMING_SEARCH_TN = 3,
							  LTW_A2B_INCOMING_SEARCH_TN = 4,
							  LTW_ANT_AND_PAGE = 5,
							  LTW_TEARA_BASE = 6,
							  LTW_PAGE_NAME = 7};
	    //NUMBER_OF_LTW_ALGORITHMS
	    typedef std::map<std::string,ltw_algorithm> ltw_algorithm_map;
	    typedef std::map<std::string,ltw_task_type> ltw_task_type_map;
	public:
	    static ltw_algorithm_map alorithm_names;
	    static ltw_task_type_map task_names;
	    static int topic_param_start;

	protected:
	    std::string name_;
	    algorithm_out *algor_out_;
	    algorithm_in *algor_in_;
	    algorithm_bep *algor_bep_;
	    std::string algor_out_name_;
	    std::string algor_in_name_;
	    links *outgoings_;
	    links *incomings_;

	    long links_to_print_;
	    long beps_to_print_;

	    std::string source_lang_;

	public:
	    ltw_task(std::string & name, std::string & out_algor_name, std::string & in_algor_name);
	    virtual ~ltw_task();
	    virtual void init();
	    virtual void perform();
	    virtual void print_links();
	    bool is_f2f_task();
	    bool is_a2b_task();
	    void set_alorithm_bep(std::string name);
	    ltw_task_type get_task_type();

	    algorithm_out *get_algor_out() const { return algor_out_; }

		void set_links_to_print(long links_to_print) { links_to_print_ = links_to_print; }
		long links_to_print() { return links_to_print_; }

		void set_beps_to_print(long beps_to_print) { beps_to_print_ = beps_to_print; }
		long beps_to_print() { return beps_to_print_; }

	    std::string get_source_lang() const
	    {
	        return source_lang_;
	    }

	    void set_source_lang(std::string source_lang_)
	    {
	        this->source_lang_ = source_lang_;
	    }

	private:
		ltw_algorithm get_algorithm_outgoing_type();
		ltw_algorithm get_algorithm_incoming_type();

	};

	inline ltw_task::ltw_task_type ltw_task::get_task_type()
	{
		return task_names[name_];
	}

	inline ltw_task::ltw_algorithm ltw_task::get_algorithm_outgoing_type()
	{
		return alorithm_names[algor_out_name_];
	}

	inline ltw_task::ltw_algorithm ltw_task::get_algorithm_incoming_type()
	{
		return alorithm_names[algor_in_name_];
	}

	inline bool ltw_task::is_f2f_task() { return get_task_type() == LTW_LTW_F2F; }
	inline bool ltw_task::is_a2b_task()
	{
		return get_task_type() == LTW_LTW_A2B
						|| get_task_type() == LTW_LTAra_A2B
								|| get_task_type() == LTW_LTAraTW_A2B;
	}
}

#endif /* __LTW_TASK_H__ */
