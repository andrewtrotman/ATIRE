/*
 * algorithm_page_name.h
 *
 *  Created on: Jan 26, 2010
 *      Author: monfee
 */

#ifndef ALGORITHM_PAGE_NAME_H_
#define ALGORITHM_PAGE_NAME_H_

#include "algorithm_out.h"
#include "wikipedia.h"
#include "ant_link_term.h"

#include <map>
#include <vector>
#include <string>

namespace QLINK
{

	class algorithm_page_name: virtual public QLINK::algorithm_out
	{
	public:
		typedef	std::vector<wiki_entry>							wiki_entry_array;
		//typedef std::map<std::string,  wiki_entry_array>		page_map_t;
		typedef std::map<std::string,  ANT_link_term *>			page_map_t;
		typedef std::vector<ANT_link_term *>					page_title_t;

	protected:
		page_map_t		names_map_;
		page_title_t	name_array_;
		bool			loaded_;

	private:
		void init();

	protected:
		void process_terms(char **term_list, const char *source);
		void process_terms(links *lx, char **term_list, const char *source);
//		virtual void recommend_anchors(links* lx, char **term_list, const char *source);

		//wiki_entry_array *find_page(char *name);
		//void create_posting(wiki_entry_array& wea, link* lnk);
		void load_names();

		virtual void add_link(ANT_link_term *term, char **term_list, long offset);
		virtual ANT_link_term *find_term_in_list(const char *value);
	public:
		algorithm_page_name(ltw_task *task);
		virtual ~algorithm_page_name();
	};

}

#endif /* ALGORITHM_PAGE_NAME_H_ */
