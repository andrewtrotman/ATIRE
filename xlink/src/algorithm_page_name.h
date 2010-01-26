/*
 * algorithm_page_name.h
 *
 *  Created on: Jan 26, 2010
 *      Author: monfee
 */

#ifndef ALGORITHM_PAGE_NAME_H_
#define ALGORITHM_PAGE_NAME_H_

#include "algorithm.h"
#include "wikipedia.h"
#include "ant_link_term.h"

#include <map>
#include <vector>
#include <string>

namespace QLINK
{

	class algorithm_page_name: virtual public QLINK::algorithm
	{
	public:
		algorithm_page_name();
		virtual ~algorithm_page_name();

	public:
		typedef	std::vector<wiki_entry>							wiki_entry_array;
		//typedef std::map<std::string,  wiki_entry_array>		page_map_t;
		typedef std::map<std::string,  ANT_link_term *>			page_map_t;

	protected:
		page_map_t		names_map_;
		links			*g_links_;

	protected:
		void process_terms(char **term_list, const char *source);
		void search_anchor_by_page_nam(links* lx, char **term_list, const char *source);

		//wiki_entry_array *find_page(char *name);
		//void create_posting(wiki_entry_array& wea, link* lnk);
		void calculate_g_links_score(double max, double min);
		void load_names();

	private:
		void init();
	};

}

#endif /* ALGORITHM_PAGE_NAME_H_ */
