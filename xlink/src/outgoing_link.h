/*
 * outgoing_link.h
 *
 *  Created on: Aug 3, 2009
 *      Author: monfee
 */

#ifndef OUTGOING_LINK_H_
#define OUTGOING_LINK_H_

/*
 *
 */
#include "link.h"

	namespace QLINK {

	class outgoing_link : public link {
	public:
		outgoing_link();
		virtual ~outgoing_link();

		void print_header();
		void print_footer();

		//void print_anchor(long beps_to_print = 5);
	};
	inline void outgoing_link::print_header() {  }
	inline void outgoing_link::print_footer() {  }
//	inline void outgoing_link::print_header() { printf("<linkto>\n"); }
//	inline void outgoing_link::print_footer() { printf("</linkto>\n"); }

}

#endif /* OUTGOING_LINK_H_ */
