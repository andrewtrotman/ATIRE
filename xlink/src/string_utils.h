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
#include <sstream>

#include <assert.h>
#include <string.h>

inline char *strlower(char *string)
{
unsigned char *ch;

for (ch = (unsigned char *)string; *ch != '\0'; ch++)
	*ch = tolower(*ch);

return string;
}

inline char hex_to_byte(std::string s)
{
	static std::string Mask = "0123456789ABCDEF";
	assert(s.length() == 2);
	char c = 0;
	char byte1 = Mask.find(s[0]) * 16;
	char byte2 = Mask.find(s[1]);
	return  byte1 + byte2;
}

template<typename T = int>
inline T string_to_number(std::string s, T b = 10)
{
	static std::string Mask = "0123456789ABCDEF";
	b = b > 1 && b <= 16 ? b : 10;
	T no = 0;
	std::string::size_type pos;
	for (int i = 0; i < s.length(); i++)
		no = (pos = Mask.find(s[i])) != std::string::npos ? b * no + pos : no;
	return no;
}

template<typename T = int>
inline std::string number_to_string(T val)
{
	std::stringstream ss;
	ss << val;
	return ss.str();
}

inline std::string byte_to_string(int i, int b = 10)
{
	static std::string Mask = "0123456789ABCDEF";
	b = b > 1 && b <= 16 ? b : 10;
	std::string s = "";
	return (s += i < b ? "" : byte_to_string(i / b, b)) + Mask[i % b];
}


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

inline void trim_non_alnum(std::string& source)
{
	std::string::iterator str_it = source.begin();
	while (str_it != source.end() && !isalnum(*str_it))
		str_it = source.erase(str_it);

	if (str_it == source.end())
		return;

	str_it = source.end();
	--str_it;
	while (str_it != source.begin() && (!isalnum(*str_it) && *str_it != '.'))
		--str_it;

	++str_it;
	source.erase(str_it, source.end());
}

template <typename T>
void find_and_replace( T& source, const T& find, const T& replace )
{
        size_t j;
        for (;(j = source.find( find )) != T::npos;)
        {
                source.replace( j, find.length(), replace );
        }
}


inline void convert_encoded_unicode(char *buf, std::string secret)
{
	//std::string::size_type pos = 0;
	for (int i = 0; i < secret.length();) { // ((pos = secret.find('%', pos)) != std::string::npos) {
		char c = secret[i];
		if (c == '%') {
			std::string hex;
			hex.append(1,secret[i + 1]);
			hex.append(1,secret[i + 2]);
			i += 3;
			*buf = hex_to_byte(hex);
		}
		else {
			*buf = c;
			++i;
		}
		++buf;
	}
	*buf = '\0';
}

inline std::string increase_file_num(std::string& filename) {
	std::string::size_type pos;
	pos = filename.find_last_of(".");
	std::string ext;
	if (pos == std::string::npos)
		pos = filename.length();
	else
		ext = std::string(filename, pos);
		//--pos;

	int i;
	for (i = pos - 1; i > 0; --i)
		if (!isdigit(filename[i]))
			break;
	std::string newfile(filename, 0, i);

	int number = 0;
	std::string str_num(filename, i, pos);
	if (str_num.length() > 0)
		number = strtol(str_num.c_str(), NULL, 10);
	++number;

	std::ostringstream stream;
	stream << newfile << number << ext;
	newfile = stream.str();
}

inline static void string_clean_tag(std::string& file)
{
std::string::iterator ch = file.begin();

/*
	remove XML tags and remove all non-alnum (but keep case)
*/
while (ch != file.end())
	{
	if (*ch == '<')			// then remove the XML tags
		{
		while (ch != file.end() && *ch != '>') {
			*ch = ' ';
			++ch;
		}

		if (ch == file.end())
			break;
		*ch = ' ';
		++ch;
		}
	else
		++ch;
	}
}


#endif /* STRING_UTILS_H_ */
