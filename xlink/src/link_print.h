/*
 * link_print.h
 *
 *  Created on: Oct 27, 2010
 *      Author: monfee
 */

#ifndef LINK_PRINT_H_
#define LINK_PRINT_H_

#include <string>
#include <set>

#include <strings.h>


namespace QLINK {

	struct ltstr
	{
	  bool operator()(const char* s1, const char* s2) const
	  {
		return strcasecmp(s1, s2) < 0;
	  }
	};

	class link_print {
	public:
		enum {YEAR, DATE, DAY, MONTH, DAY_or_MONTH, DECADE, CENTURY, NUMBER, NONE};
	public:
		static const std::string target_format;

		static const char CHINESE_YEAR[];
		static const char CHINESE_CENTURY[];
		static const char CHINESE_MONTH[];
		static const char CHINESE_DAY[];
		static const char CHINESE_DATE[];
		static const char CHINESE_DECADE[];

		static const char ENGLISH_YEAR[];
		static const char ENGLISH_CENTURY[];
		static const char *ENGLISH_DAY_MONTH[];
		static const char ENGLISH_MONTH[];
		static const char ENGLISH_DATE[];
		static const char ENGLISH_DECADE[];
		static std::set<const char*, ltstr> ENGLISH_DAY_MONTH_SET;

		static bool is_number_or_chronological_link(const char *term, const char *lang);
	};
}
#endif /* LINK_PRINT_H_ */
