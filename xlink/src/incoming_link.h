/*
 * incoming_link.h
 *
 *  Created on: Aug 3, 2009
 *      Author: monfee
 */

#ifndef INCOMING_LINK_H_
#define INCOMING_LINK_H_

#include "link.h"

namespace QLINK {

	/*
	 *
	 */
	class incoming_link: public link {
	public:
		incoming_link();
		virtual ~incoming_link();

		void print_header();
		void print_footer();

		//void print_anchor(long beps_to_print = 1);
		//void print_bep(long beps_to_print = 1);
	};

	inline void incoming_link::print_header() {  }
	inline void incoming_link::print_footer() {  }
//	inline void incoming_link::print_header() { printf("<linkfrom>\n"); }
//	inline void incoming_link::print_footer() { printf("</linkfrom>\n"); }
}

#endif /* INCOMING_LINK_H_ */
