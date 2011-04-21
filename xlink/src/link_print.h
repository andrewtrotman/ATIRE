/*
 * link_print.h
 *
 *  Created on: Oct 27, 2010
 *      Author: monfee
 */

#ifndef LINK_PRINT_H_
#define LINK_PRINT_H_

#include <string>

namespace QLINK {
	class link_print {
	public:
		enum {YEAR, DATE, DAY, MONTH, DECADE, CENTURY, NUMBER, NONE};
	public:
		static const std::string target_format;

		static const char CHINESE_YEAR[];
		static const char CHINESE_CENTURY[];
		static const char CHINESE_MONTH[];
		static const char CHINESE_DAY[];
		static const char CHINESE_DATE[];
		static const char CHINESE_DECADE[];

		static bool is_number_or_chronological_link(const char *term);
	};
}
#endif /* LINK_PRINT_H_ */
