/*
 * uniseg_string.h
 *
 *  Created on: Dec 16, 2009
 *      Author: monfee
 */

#ifndef UNISEG_STRING_H_
#define UNISEG_STRING_H_

#include <vector>
#include <string>
#include <string.h>
#include <assert.h>

typedef std::string	 							string_type;
typedef std::vector<string_type>				string_array;

#include "encoding_factory.h"
#include "encoding_utf8.h"
typedef UNISEG_encoding									uniseg_encoding;
typedef UNISEG_encoding_utf8							uniseg_encoding_utf8;
typedef UNISEG_encoding_factory							uniseg_encoding_factory;

inline std::string get_first_utf8char(const unsigned char* c)
{
	size_t num_of_bytes = uniseg_encoding_utf8::test_utf8char(c);
//	char *tmp_utf8str = new char[num_of_bytes + 1];
//	strncpy(tmp_utf8str, (char*)c, num_of_bytes);
//	tmp_utf8str[num_of_bytes] = '\0';
	std::string a_char(c, c + num_of_bytes);
//	delete [] tmp_utf8str;
	return a_char;
}

typedef std::string::iterator							entity_iterator;

inline size_t get_utf8_string_length(const char* c, size_t length)
{
	size_t count = 0;
	size_t num_of_bytes = 0;
	unsigned char *next = (unsigned char *)c;
	size_t actual_len = 0;
	for (; count < length; ) {
		num_of_bytes = uniseg_encoding_utf8::test_utf8char(next);
		if (num_of_bytes <= 0)
			num_of_bytes = 1;
		next += num_of_bytes;
		actual_len++;
		count += num_of_bytes;
	}
	return actual_len;
}

inline size_t get_utf8_string_length(std::string& source)
{
	get_utf8_string_length(source.c_str(), source.length());
}

inline void to_string_array(const char *source, string_array& sa)
{
	const unsigned char *start = (const unsigned char *)source;
	while (*start != '\0') {
		sa.push_back(get_first_utf8char(start));
		start += sa.back().length();
	}
}

inline void to_string_array(const string_type& source, string_array& sa)
{
	to_string_array(source.c_str(), sa);
}

inline void array_to_string(string_array& ca, /*long lang, */string_type& dest, int idx = 0, int len = -1)
{
	if (len == -1)
		len = ca.size();

	assert(idx >= 0);
	assert(len <= (int)ca.size());
	string_type sep;
//	if (lang != uniseg_encoding::CHINESE)
//		sep = " ";
	int count = 0;
	for (int i = idx; i < idx + len; i++) {
		if (count > 0)
			dest.append(sep);
		dest.append(ca[i]);
		count++;
	}
}
#endif /* UNISEG_STRING_H_ */
