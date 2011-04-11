/*
 * algorithm_ltara_base.h
 *
 *  Created on: Oct 17, 2009
 *      Author: monfee
 */

#ifndef ALGORITHM_LTARA_BASE_H_
#define ALGORITHM_LTARA_BASE_H_

#include "algorithm_ant_link_this_and_page_name.h"

namespace QLINK {

	class algorithm_ltara_base: public QLINK::algorithm_ant_link_this_and_page_name
	{
	public:
		typedef std::map<long,  char *>			id_name_map_t;

	private:
		id_name_map_t							id_name_map_;

	public:
//		algorithm_ltara_base(links *links_list);
		algorithm_ltara_base(ltw_task *task);
		virtual ~algorithm_ltara_base();

	private:
		void load_names();
		void init();
	};

}

#endif /* ALGORITHM_LTARA_BASE_H_ */
