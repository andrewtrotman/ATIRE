/*
 * link.h
 *
 *  Created on: Aug 3, 2009
 *      Author: monfee
 */

#ifndef LINK_H_
#define LINK_H_

#include <string>

#include "ant_link.h"

namespace QLINK {

	/*
	 *
	 */
	class link : public ANT_link {
	private:
		bool						to_clean_;

	public:
		link();
		virtual ~link();

		virtual void print_target(long anchor = 0);
		virtual void print_anchor(long beps_to_print = 5, bool id_or_name = true);
		virtual void print_bep(long beps_to_print = 1);
		virtual void print();
		virtual void print_header() {}
		virtual void print_footer() {};

		void require_cleanup() { to_clean_ = true; }

	public:
		static bool compare(const link *a, const link *b);
		static bool final_compare(const link *a, const link *b);
		static bool string_target_compare(const link *a, const link *b);
		static bool term_compare(const link *a, const link *b);

		bool operator==(const link& rhs);
		bool operator==(const char *term);
	};

	class link_string_target_compare {
	public:
		const bool operator()(const link *a, const link * b) const;
	};

	class link_compare {
	public:
		const bool operator()(const link *a, const link * b) const;
	};
}
#endif /* LINK_H_ */
