/*
	FUNDAMENTAL_TYPES.H
	-------------------
	Some C/C++ compilers (i.e. Microsoft Visual Studio 2008 ) don't have the C99 stdint.h, so we 
	have to make the parts we need ourselves
*/

#ifndef __FUNDAMENTAL_TYPES_H__
#define __FUNDAMENTAL_TYPES_H__

#ifdef _MSC_VER
	typedef char int8_t;
	typedef unsigned char uint8_t;

	typedef short int16_t;
	typedef unsigned short uint16_t;

	typedef long int32_t;
	typedef unsigned long uint32_t;

	typedef long long int64_t;
	typedef unsigned long long uint64_t;
#else
	#include <stdint.h>
#endif

#endif __FUNDAMENTAL_TYPES_H__

