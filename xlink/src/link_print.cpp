/*
 * link_print.cpp
 *
 *  Created on: Oct 28, 2010
 *      Author: monfee
 */

#include "link_print.h"
#include "string_utils.h"

#include <string>

namespace QLINK {
#ifdef CROSSLINK
	const std::string QLINK::link_print::target_format = "\t\t\t\t<tofile bep_offset=\"%d\" lang=\"%s\" title=\"%s\">%d</tofile>\n";
#else
	const std::string QLINK::link_print::target_format = "\t\t\t\t<tobep offset=\"%d\">%d</tobep>\n";
#endif

	const char link_print::CHINESE_YEAR[] = {(char)0xe5, (char)0xb9, (char)0xb4, 0x0};
	const char link_print::CHINESE_CENTURY[] = {(char)0xe4, (char)0xb8, (char)0x96, (char)0xe7, (char)0xba, (char)0xaa, 0x0};
	const char link_print::CHINESE_MONTH[] = {(char)0xe6, (char)0x9c, (char)0x88, 0x0}; // month
	const char link_print::CHINESE_DAY[] = {(char)0xe6, (char)0x98, (char)0x9f, (char)0xe6, (char)0x9c, (char)0x9f, 0x0}; // week day, monday, tuesday
	const char link_print::CHINESE_DATE[] = {(char)0xe6, (char)0x97, (char)0xa5, 0x0}; //日
	const char link_print::CHINESE_DECADE[] = {(char)0xe5, (char)0xb9, (char)0xb4, (char)0xe4, (char)0xbb, (char)0xa3, 0x0}; //niandai 年代

	bool link_print::is_number_or_chronological_link(const char *term)
	{
		int type = NONE;
		bool ret;
		long number = atol(term);
		const char *word = NULL;

		std::string number_str = number_to_string(number);

		if (number > 0) {
			if (number_str.length() == strlen(term))
				type = NUMBER;
			else
				word = term + number_str.length();

			if (word) {
				if (number > 999 && number < 3000 && memcmp(word, CHINESE_YEAR, strlen(CHINESE_YEAR)) == 0) {
					type = YEAR;
				}
				else if (number < 13 && memcmp(word, CHINESE_MONTH, strlen(CHINESE_MONTH)) == 0) {
					type = MONTH;
				}
				else if (number < 32 && memcmp(word, CHINESE_DATE, strlen(CHINESE_DATE)) == 0) {
					type = DATE;
				}
				else if (number < 100 && memcmp(word, CHINESE_DECADE, strlen(CHINESE_DECADE)) == 0) {
					type = DECADE;
				}
				else if (number < 31 && memcmp(word, CHINESE_CENTURY, strlen(CHINESE_CENTURY)) == 0) {
					type = CENTURY;
				}
			}
		}

		switch (type) {
			case YEAR:
			case NUMBER:
			case MONTH:
			case DAY:
			case CENTURY:
			case DATE:
				ret = true;
				break;
			default:
				break;
		}

		return ret;
	}
}
