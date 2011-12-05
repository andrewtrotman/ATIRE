/*
	FUNDAMENTAL_TYPES.H
	-------------------
	Some C/C++ compilers (i.e. Microsoft Visual Studio 2008) don't have the C99 stdint.h, so we 
	have to make the parts we need ourselves
*/

#ifndef FUNDAMENTAL_TYPES_H_
#define FUNDAMENTAL_TYPES_H_

#ifdef _MSC_VER
	#define inline __forceinline

	typedef char int8_t;
	typedef unsigned char uint8_t;

	typedef short int16_t;
	typedef unsigned short uint16_t;

	typedef long int32_t;
	typedef unsigned long uint32_t;

	#define UINT32_MAX  0xffffffff

	typedef long long int64_t;
	typedef unsigned long long uint64_t;
#else
	#include <stdint.h>

	#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

/*
	Functions to convert from on-disk format to machine specific in-memory format.
*/
inline long long ANT_get_long_long(unsigned char *from) { return *((int64_t *)from); }
inline long long ANT_get_long_long(char *from) { return *((int64_t *)from); }
inline unsigned long long ANT_get_unsigned_long_long(unsigned char *from) { return *((uint64_t *)from); }
inline unsigned long long ANT_get_unsigned_long_long(char *from) { return *((uint64_t *)from); }

inline long ANT_get_long(unsigned char *from) { return *((int32_t *)from); }
inline long ANT_get_long(char *from) { return *((int32_t *)from); }
inline unsigned long ANT_get_unsigned_long(unsigned char *from) { return *((uint32_t *)from); }
inline unsigned long ANT_get_unsigned_long(char *from) { return *((uint32_t *)from); }

inline short ANT_get_short(unsigned char *from) { return *((int16_t *)from); }
inline short ANT_get_short(char *from) { return *((int16_t *)from); }
inline unsigned short ANT_get_unsigned_short(unsigned char *from) { return *((uint16_t *)from); }
inline unsigned short ANT_get_unsigned_short(char *from) { return *((uint16_t *)from); }

#endif  /* FUNDAMENTAL_TYPES_H_ */

