/*
 * string_utils.h
 *
 *  Created on: Oct 7, 2009
 *      Author: monfee
 */

#ifndef STRING_UTILS_H_
#define STRING_UTILS_H_

#include <string>
#include <stdexcept>
#include <algorithm>
#include <cctype>

template<typename StringT>
inline void std_tolower(StringT& s1, StringT& s2) {
	std::transform(s1.begin(), s1.end(), s2.begin(), static_cast < int(*)(int) > (std::tolower));
}

template<typename StringT>
inline void std_tolower(StringT& s1) {
	std_tolower(s1, s1);
}

inline void trim_right(std::string &source, const std::string &t)
{
	source.erase(source.find_last_not_of(t)+1);
}
inline void trim_left(std::string &source, const std::string &t)
{
	source.erase(0, source.find_first_not_of(t));
}

inline std::string::iterator trim(std::string& source, std::string::iterator str_it)
{
	while (str_it != source.end() && (*str_it) == ' ')
		str_it = source.erase(str_it);

	while (str_it != source.end() && *str_it != ' ' && !isalnum(*str_it)) {
		//str_it++;
		// skip ()
		if ((*str_it) == '(') {
			str_it = source.erase(str_it);
			while (str_it != source.end() && (*str_it) != ')')
				str_it = source.erase(str_it);

			if ((*str_it) == ')')
				str_it = source.erase(str_it);
			continue;
		}

		str_it = source.erase(str_it);
	}
	return str_it;
}
#endif /* STRING_UTILS_H_ */
