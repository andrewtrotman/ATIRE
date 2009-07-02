#ifndef CONVERT_H_
#define CONVERT_H_

// File: convert.h
#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <stdexcept>
#include <algorithm>
#include <cctype>

class BadConversion : public std::runtime_error {
public:
  BadConversion(const std::string& s)
    : std::runtime_error(s)
    { }
};

template<typename T>
inline std::string stringify(const T& x)
{
  std::ostringstream o;
  if (!(o << x))
    throw BadConversion(std::string("stringify(")
                        + typeid(x).name() + ")");
  return o.str();
}

template<typename T>
inline void convert(const std::string& s, T& x,
                     bool failIfLeftoverChars = true)
{
  std::istringstream i(s);
  char c;
  if (!(i >> x) || (failIfLeftoverChars && i.get(c)))
    throw BadConversion(s);
}

template<typename T>
inline T convertTo(const std::string& s,
                    bool failIfLeftoverChars = true)
{
  T x;
  convert(s, x, failIfLeftoverChars);
  return x;
}

inline int charArrayToInt(char *arr, int size) {
	//unsigned char tempArr[4]={0x00, 0x00, 0x02, 0x07}; //00000000, 00000000, 00000010, 00000111};
	int i = 0, value = 0;

	for(i = 0; i < size; i++)
		value = (value << 8) | arr[i];
	return value;
}

template<typename StringT>
inline void tolower(StringT& s1, StringT& s2) {
	std::transform(s1.begin(), s1.end(), s2.begin(), static_cast < int(*)(int) > (tolower));
}

template<typename StringT>
inline void tolower(StringT& s1) {
	tolower(s1, s1);
}
#endif /*CONVERT_H_*/

