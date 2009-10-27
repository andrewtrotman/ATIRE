/*
 * algorithm_ltw_incoming_search_tn.h
 *
 *  Created on: Aug 17, 2009
 *      Author: monfee
 */

#ifndef ALGORITHM_LTW_INCOMING_SEARCH_TN_H_
#define ALGORITHM_LTW_INCOMING_SEARCH_TN_H_

#include "algorithm.h"
#include <ant_api.h>

namespace QLINK {

	/*
	 * to creat the incoming links by searching topic name
	 */
	class algorithm_ltw_incoming_search_tn: public algorithm
	{
	protected:
		char **docids_;
		long long hits_;

	private:
		ANT *ant_;

	public:
		algorithm_ltw_incoming_search_tn();
		virtual ~algorithm_ltw_incoming_search_tn();

		virtual void process_topic(ltw_topic *a_topic);

	private:
		void init_search_engine();
	};

}
#endif /* ALGORITHM_LTW_INCOMING_SEARCH_TN_H_ */
