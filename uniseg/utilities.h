/*
 * utilities.h
 *
 *  Created on: Oct 26, 2008
 *      Author: monfee
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <string>
#include <cassert>
#include <string.h>
#include <stdlib.h>
#include <vector>

template<typename T, typename ST>
inline ST array_to_string(const T& arr, int idx, int len) {
	ST tmp_str;
	assert(idx >= 0);
	assert(len <= (int)arr.size());
	for (int i = idx; i < (idx + len); i++)
		tmp_str.append(arr[i]);
	return tmp_str;
}

template<typename T, typename ST>
inline ST array_to_string(const T& arr) {
	return array_to_string<T, ST>(arr, 0, arr.size());
}

template<typename T>
inline void chars_to_array(const char* source, T& arr) {
	for (int i = 0; i < strlen(source); i++)
		arr[i] = source[i];
}

inline void chars_to_array(const char* source, std::vector<int>& arr) {
	char tmp = 0;
	for (int i = 0; i < strlen(source); i++) {
		tmp = source[i];
		arr.push_back(atoi(&tmp));
		//++tmp;
	}
}

inline std::string getext(std::string& source) {
	return source.substr(source.find_last_of(".") + 1);
}

inline unsigned int bytes_to_int32(char *bytes)
{
	unsigned int shift = 0;
	char *start = bytes;
	int count = 0;
	while (/*start && */count < 4) {
		unsigned int shiftbits = count * 8;
		unsigned char byte = (unsigned char)(*start);
		unsigned int ret = byte << shiftbits;
		shift |= ret;
		start++;
		count++;
	}
	if (shift >= 4294967181 && shift < 4294967295)
		std::cerr << "there must be a error somewhere" << std::endl;
	//unsigned int plus = (unsigned int)(*(bytes + 3) << 24) + (unsigned int)(*(bytes + 2) << 16) + (unsigned int)(*(bytes + 1) << 8) + (unsigned int)(*bytes);
	return shift;
}

inline void int32_to_bytes(int val, char *bytes)
{
	bytes[0] = (int)val;
	bytes[1] = (int)(val >> 8);
	bytes[2] = (int)(val >> 16);
	bytes[3] = (int)(val >> 24);
}

#endif /* UTILITIES_H_ */
