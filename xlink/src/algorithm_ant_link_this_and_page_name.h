/*
 * algorithm_ant_link_this_and_page_name.h
 *
 *  Created on: Oct 6, 2009
 *      Author: monfee
 */

#ifndef ALGORITHM_ANT_LINK_THIS_AND_PAGE_NAME_H_
#define ALGORITHM_ANT_LINK_THIS_AND_PAGE_NAME_H_

#include "algorithm_ant_link_this.h"
#include "wikipedia.h"
#include "ant_link_term.h"

#include <map>
#include <vector>
#include <string>

namespace QLINK {
	//class ANT_link_term;

	class algorithm_ant_link_this_and_page_name: public QLINK::algorithm_ant_link_this
	{
	public:
		typedef	std::vector<wiki_entry>							wiki_entry_array;
		//typedef std::map<std::string,  wiki_entry_array>		page_map_t;
		typedef std::map<std::string,  ANT_link_term *>			page_map_t;

	protected:
		page_map_t		names_map_;

	private:
		links			*k_links_;
		links			*g_links_;

	public:
		algorithm_ant_link_this_and_page_name(links *links_list);
		algorithm_ant_link_this_and_page_name();
		virtual ~algorithm_ant_link_this_and_page_name();

	protected:
		void process_terms(char **term_list, const char *source);
		void search_anchor_by_page_nam(links* lx, char **term_list, const char *source);
		void merge_links();

	private:
		void init();
		//wiki_entry_array *find_page(char *name);
		//void create_posting(wiki_entry_array& wea, link* lnk);
		void calculate_g_links_score(double max, double min);
		void load_names();
	};

}

#endif /* ALGORITHM_ANT_LINK_THIS_AND_PAGE_NAME_H_ */
