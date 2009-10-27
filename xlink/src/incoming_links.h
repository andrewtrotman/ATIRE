/*
 * incoming_links.h
 *
 *  Created on: Aug 16, 2009
 *      Author: monfee
 */

#ifndef INCOMING_LINKS_H_
#define INCOMING_LINKS_H_

#include "links.h"

namespace QLINK {

	class incoming_links : public links
	{
	public:
		incoming_links();
		incoming_links(ltw_topic *a_topic);
		virtual ~incoming_links();

		virtual void print_header();
		virtual void print_footer();

		void find_anchor();

	protected:
		void print_link_tag_header();
		void print_link_tag_footer();

		virtual void init_links();

		void print_links(long orphan_docid, const char *orphan_name, long links_to_print = 1, long max_targets_per_anchor = 250, long mode = 1);
		void print_anchors(long orphan_docid, const char *orphan_name);

		virtual link *link_maker();

	private:
		virtual void init();
	};

	inline void incoming_links::print_link_tag_header() { printf("		<linkfrom>"); }
	inline void incoming_links::print_link_tag_footer() { printf("</linkfrom>\n"); }
}

#endif /* INCOMING_LINKS_H_ */
