#ifndef __QTYPE_H__
#define __QTYPE_H__

#include "word.h"
typedef Word::string_type							string_type;
typedef Word::word_type								word_type;
typedef Word::word_ptr_type							word_ptr_type;
typedef Word::string_array							string_array;

typedef const word_ptr_type							const_word_ptr;

#include <map>
typedef std::map<string_type, word_ptr_type>		freq_type;
typedef std::map<string_type, word_ptr_type>		word_map;

typedef std::vector<word_ptr_type>					array_type;
typedef std::vector<array_type >					array_array_type;

#include <encoding_factory.h>
#include <encoding_utf8.h>
typedef ANT_encoding								uniseg_encoding;
typedef ANT_encoding_utf8							uniseg_encoding_utf8;
typedef ANT_encoding_factory						uniseg_encoding_factory;

#include <string.h>
inline std::string get_first_utf8char(unsigned char* c)
{
	size_t num_of_bytes = uniseg_encoding_utf8::test_utf8char(c);
	char *tmp_utf8str = new char[num_of_bytes + 1];
	strncpy(tmp_utf8str, (char*)c, num_of_bytes);
	tmp_utf8str[num_of_bytes] = '\0';
	std::string a_char(tmp_utf8str);
	delete tmp_utf8str;
	return a_char;
}

#include <string>
typedef std::string::iterator						entity_iterator;

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


#endif /* __QTYPE_H_ */
