/*
	FUNDAMENTAL_TYPES.H
	-------------------
	Some C/C++ compilers (i.e. Microsoft Visual Studio 2008) don't have the C99 stdint.h, so we
	have to make the parts we need ourselves
*/

#ifndef FUNDAMENTAL_TYPES_H_
#define FUNDAMENTAL_TYPES_H_

#ifdef _MSC_VER
	#if (_MSC_VER < 1700)
		#define inline __forceinline
	#endif

	typedef signed char int8_t;
	typedef unsigned char uint8_t;

	typedef signed short int16_t;
	typedef unsigned short uint16_t;

	typedef int int32_t;
	typedef unsigned int uint32_t;

	#define UINT32_MAX  0xffffffff

	typedef signed long long int64_t;
	typedef unsigned long long uint64_t;
#else
	#include <stdint.h>

	#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

/*
	Functions to convert from on-disk format to machine specific in-memory format.
*/
/*
	ANT_GET_UNSIGNED_LONG_LONG()
	----------------------------
*/
inline unsigned long long ANT_get_unsigned_long_long(unsigned char *from)
{
#ifdef __arm__
	return
		(((uint64_t)*(from + 0)) <<  0) |
		(((uint64_t)*(from + 1)) <<  8) |
		(((uint64_t)*(from + 2)) << 16) |
		(((uint64_t)*(from + 3)) << 24) |
		(((uint64_t)*(from + 4)) << 32) |
		(((uint64_t)*(from + 5)) << 40) |
		(((uint64_t)*(from + 6)) << 48) |
		(((uint64_t)*(from + 7)) << 56) ;
#else
	return *((uint64_t *)from);
#endif
}

/*
	ANT_GET_UNSIGNED_LONG()
	-----------------------
*/
inline unsigned long ANT_get_unsigned_long(unsigned char *from)
{
#ifdef __arm__
	return
		(((uint32_t)*(from + 0)) <<  0) |
		(((uint32_t)*(from + 1)) <<  8) |
		(((uint32_t)*(from + 2)) << 16) |
		(((uint32_t)*(from + 3)) << 24);
#else
	return *((uint32_t *)from);
#endif
}

/*
	ANT_GET_UNSIGNED_SHORT()
	------------------------
*/
inline unsigned short ANT_get_unsigned_short(unsigned char *from)
{
#ifdef __arm__
	return
		(((uint16_t)*(from + 0)) <<  0) |
		(((uint16_t)*(from + 1)) <<  8);
#else
	return *((uint16_t *)from);
#endif
}

/*
	ANT_GET_LONG_LONG()
	-------------------
*/
inline long long ANT_get_long_long(unsigned char *from)
{
#ifdef __arm__
	return (int64_t)(
		(((uint64_t)*(from + 0)) <<  0) |
		(((uint64_t)*(from + 1)) <<  8) |
		(((uint64_t)*(from + 2)) << 16) |
		(((uint64_t)*(from + 3)) << 24) |
		(((uint64_t)*(from + 4)) << 32) |
		(((uint64_t)*(from + 5)) << 40) |
		(((uint64_t)*(from + 6)) << 48) |
		(((uint64_t)*(from + 7)) << 56)
		);
#else
	return *((int64_t *)from);
#endif
}

/*
	ANT_GET_LONG()
	--------------
*/
inline long ANT_get_long(unsigned char *from)
{
#ifdef __arm__
	return (int32_t)(
		(((uint32_t)*(from + 0)) <<  0) |
		(((uint32_t)*(from + 1)) <<  8) |
		(((uint32_t)*(from + 2)) << 16) |
		(((uint32_t)*(from + 3)) << 24)
		);
#else
	return *((int32_t *)from);
#endif
}

/*
	ANT_GET_SHORT()
	---------------
*/
inline short ANT_get_short(unsigned char *from)
{
#ifdef __arm__
	return (int16_t)(
		(((uint16_t)*(from + 0)) <<  0) |
		(((uint16_t)*(from + 1)) <<  8)
		);
#else
	return *((int16_t *)from);
#endif
}

/*
	And again with signed char type
*/
inline unsigned long long ANT_get_unsigned_long_long(char *from) { return ANT_get_unsigned_long_long((unsigned char *)from); }
inline unsigned long ANT_get_unsigned_long(char *from) { return ANT_get_unsigned_long((unsigned char *)from); }
inline unsigned short ANT_get_unsigned_short(char *from) { return ANT_get_unsigned_short((unsigned char *)from); }

inline long long ANT_get_long_long(char *from) { return ANT_get_long_long((unsigned char *)from); }
inline long ANT_get_long(char *from) { return ANT_get_long((unsigned char *)from); }
inline short ANT_get_short(char *from) { return ANT_get_short((unsigned char *)from); }

#endif  /* FUNDAMENTAL_TYPES_H_ */

