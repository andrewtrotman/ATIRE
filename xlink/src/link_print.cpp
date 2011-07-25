/*
 * link_print.cpp
 *
 *  Created on: Oct 28, 2010
 *      Author: monfee
 */

#include "link_print.h"
#include "string_utils.h"

#include <string>
#include <iostream>

using namespace std;

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

	const char link_print::ENGLISH_YEAR[] = "year";
	const char link_print::ENGLISH_CENTURY[] = "century";
	const char link_print::ENGLISH_MONTH[] = "month"; // month
//	const char *link_print::ENGLISH_DAY[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday", 0x0}; // week day, monday, tuesday
//	const char *link_print::ENGLISH_MONTH[] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
	const char *link_print::ENGLISH_DAY_MONTH[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December", ""};
	std::set<const char*, ltstr> link_print::ENGLISH_DAY_MONTH_SET(ENGLISH_DAY_MONTH, ENGLISH_DAY_MONTH + 20);
//	const char link_print::ENGLISH_DATE[] = {(char)0xe6, (char)0x97, (char)0xa5, 0x0}; //日
//	const char link_print::ENGLISH_DECADE[] = {(char)0xe5, (char)0xb9, (char)0xb4, (char)0xe4, (char)0xbb, (char)0xa3, 0x0}; //niandai 年代

//	struct link_print_proxy
//	{
//	public:
//		link_print_proxy() {
//			link_print::ENGLISH_DAY_MONTH_SET =
//		}
//	};
//
//	link_print_proxy link_print_instance;

	bool link_print::is_number_or_chronological_link(const char *term, const char *lang)
	{
		int type = NONE;
		bool ret = false;
		long number = atol(term);
		char *word, *term_copy;
		word = term_copy = strdup(term);
		char *temp;
		bool has_space = false;
		std::string number_str = number_to_string(number);

		if (number > 0) {
			if (number_str.length() == strlen(term))
				type = NUMBER;
			else
				word += number_str.length();

			if (word) {
				if (strcmp(lang, "zh") == 0) {
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
				else if (strcmp(lang, "en") == 0) {
					if (strncasecmp(word, "st", 2) == 0 || strncasecmp(word, "nd", 2) == 0 || strncasecmp(word, "rd", 2) == 0 || strncasecmp(word, "th", 2) == 0)
						word +=2;

					if (isspace(*word)) {
						has_space = true;
						while (isspace(*word))
							++word;
					}

					if (!has_space) {
						if (number > 999 && number < 3000) {
							type = YEAR;
							if (strcasecmp(word, "s") == 0)
								type = DECADE;

						}
						else if (number < 32) {
							type = DATE;
						}
					}
					else {
						if (number > 0) {
							if (number < 32 && link_print::ENGLISH_DAY_MONTH_SET.find(word) != link_print::ENGLISH_DAY_MONTH_SET.end())
								type = DATE;
							else if (number < 31 && strcasecmp(word, ENGLISH_CENTURY) == 0)
								type = CENTURY;
						}
					}

//					else if (number < 31 && memcmp(word, ENGLISH_CENTURY, strlen(ENGLISH_CENTURY)) == 0) {
//						type = CENTURY;
//					}


				}
			}
		}
		else {
			if (strcmp(lang, "en") == 0) {
				temp = word;
				while (isalpha(*temp))
					++temp;

				if (isspace(*temp)) {
					has_space = true;
					*temp = '\0';
					++temp;
					while (isspace(*temp))
						(++temp);
				}
				number = atol(temp);

				if (link_print::ENGLISH_DAY_MONTH_SET.find(word) != link_print::ENGLISH_DAY_MONTH_SET.end()) {
					if (!has_space)
						type = DAY_or_MONTH;
					else {
						if (number > 0) {
							if ((number < 32 && (*word == '\0' || *word == ' ')) || (number < 32 && (strncasecmp(word, "st", 2) == 0 || strncasecmp(word, "nd", 2) == 0 || strncasecmp(word, "th", 2) == 0))) {
								type = DATE;
							}
							else
								type = NONE;
						}
					}
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
			case DAY_or_MONTH:
				ret = true;
				break;
			default:
				break;
		}

		free(term_copy);

		if (ret)
			cerr << "[INFO - Chronological Link]: " << term << endl;

		return ret;
	}
}
