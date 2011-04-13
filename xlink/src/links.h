/*
 * links.h
 *
 *  Created on: Aug 16, 2009
 *      Author: monfee
 */

#ifndef LINKS_H_
#define LINKS_H_

#include "link.h"
//#include "ltw_topic.h"
//#include <stdlib.h>
//#include <algorithm>
#include <vector>

#define MAX_LINKS_IN_FILE (1024 * 1024)

namespace QLINK {
	class ltw_topic;
	class algorithm_bep;
	class algorithm;

	class links
	{
	public:
		enum link_type { INCOMING_LINK_TYPE, OUTGOING_LINK_TYPE };
		typedef links 				*maker_t();

	protected:
		//std::vector<link *>		list_;
		//link 						*all_links_in_file; //[MAX_LINKS_IN_FILE];
		std::vector<link *>			all_links_in_file_;
		//long long 					all_links_in_file_length;

		//link 						*links_in_orphan[MAX_LINKS_IN_FILE];
		std::vector<link *>			links_in_orphan_;
		//long 						links_in_orphan_length;

		ltw_topic					*topic_;
		link_type					type_;

		long					links_to_print_;  // for f2f the number of links, for a2b the number of anchors to print
		long					beps_to_print_;	  // a2b only, up to # beps per anchor can be specified

		long 						mode_;            // mode for print year or not
		//bool						to_clean_;
		//bool						link_managed_;    // to indicate whether the links in the container from other containers

		//maker_t						*link_maker_;
		algorithm_bep 				*bep_algor_;
		algorithm					*algorithm_;

	public:
		links();
		links(ltw_topic *a_topic);
		virtual ~links();

		std::vector<link *>& all_links() { return all_links_in_file_; }
		std::vector<link *>& orphan_links() { return links_in_orphan_; }
		long long all_links_length() { return all_links_in_file_.size(); }
		//long long *all_linksall_links_in_file_.size()all_links_in_file_length; }
		long orphan_links_length() { return links_in_orphan_.size(); }
		//long *orphan_links_length_ptr() { return &links_in_orphan_length; }

		void set_links_to_print(long links_to_print) { links_to_print_ = links_to_print; }
		long links_to_print() { return links_to_print_; }

		void set_beps_to_print(long beps_to_print) { beps_to_print_ = beps_to_print; }
		long beps_to_print() { return beps_to_print_; }

		link *push_link(char *place_in_file, long offset, char *buffer, long docid, double gamma, ANT_link_term *node = 0);
		link *push_link(char *buffer, long docid, ANT_link_term *node);
		link *push_link(link *lnk);
		void sort_links();
		void sort_links_by_term();

		virtual void print();
		virtual void print_header();
		virtual void print_footer();

		void type(link_type type) { type_ = type; }
		link_type type() { return type_; }

		void mode(long what) { mode_ = what; }
		long mode() { return mode_; }

		void sort_orphan();

//		void require_cleanup() { to_clean_ = true; }
//		void set_link_managed() { link_managed_ = true; }

		link *create_new_orphan_link();
		link *create_new_link();

		link *find(const char *term);
		link *find_orphan(const char *term);

		void set_bep_algorithm(algorithm_bep *bep_algor) { bep_algor_ = bep_algor; }
		void set_algorithm(algorithm *algor) { algorithm_ = algor; }
		virtual void find_anchor() {}
		//long find_bep(link *lnk);

	protected:
		virtual void print_links(long orphan_docid, const char *orphan_name, long links_to_print = 250, long max_targets_per_anchor = 1, long mode = 1);
		virtual void print_anchors(long orphan_docid, const char *orphan_name) {};
		virtual void print_link_tag_header() {};
		virtual void print_link_tag_footer() {};

		virtual void init_links() {}

		void deduplicate_links(void);

		virtual link *link_maker();

	private:
		virtual void init();
	};

	inline link *links::link_maker() {
		return new link;
	}
}

#endif /* LINKS_H_ */
