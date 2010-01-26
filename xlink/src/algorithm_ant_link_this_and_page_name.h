/*
 * algorithm_ant_link_this_and_page_name.h
 *
 *  Created on: Oct 6, 2009
 *      Author: monfee
 */

#ifndef ALGORITHM_ANT_LINK_THIS_AND_PAGE_NAME_H_
#define ALGORITHM_ANT_LINK_THIS_AND_PAGE_NAME_H_

#include "algorithm_ant_link_this.h"
#include "algorithm_page_name.h"

namespace QLINK {
	//class ANT_link_term;

	class algorithm_ant_link_this_and_page_name: public QLINK::algorithm_ant_link_this, public QLINK::algorithm_page_name
	{
	private:
		links			*k_links_;

	public:
		algorithm_ant_link_this_and_page_name(links *links_list);
		algorithm_ant_link_this_and_page_name();
		virtual ~algorithm_ant_link_this_and_page_name();

	protected:
		void process_terms(char **term_list, const char *source);
		void merge_links();

	private:
		void init();
	};

}

#endif /* ALGORITHM_ANT_LINK_THIS_AND_PAGE_NAME_H_ */
