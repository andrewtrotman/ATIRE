/*
 * algorithm_ltw_incoming_a2b_search_tn.h
 *
 *  Created on: Sep 14, 2009
 *      Author: monfee
 */

#ifndef ALGORITHM_LTW_INCOMING_A2B_SEARCH_TN_H_
#define ALGORITHM_LTW_INCOMING_A2B_SEARCH_TN_H_

#include "algorithm_ltw_incoming_search_tn.h"

namespace QLINK {
	class link;
	class ltw_topic;
	class algorithm_bep;

	class algorithm_ltw_incoming_a2b_search_tn: public QLINK::algorithm_ltw_incoming_search_tn
	{
//	private:
//		algorithm_bep	*bep_algor_;

	public:
		algorithm_ltw_incoming_a2b_search_tn(ltw_task *task);
		virtual ~algorithm_ltw_incoming_a2b_search_tn();

//		void process_topic(ltw_topic *a_topic);
//
//	private:
//		void find_anchor_and_bep(ltw_topic *a_topic, link* curr_link);
//		void process_links(ltw_topic *a_topic);
	};

}

#endif /* ALGORITHM_LTW_INCOMING_A2B_SEARCH_TN_H_ */
