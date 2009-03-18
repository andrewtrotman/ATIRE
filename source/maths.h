/*
	MATHS.H
	-------
*/

#ifndef __MATHS_H__
#define __MATHS_H__
#ifdef _MSC_VER
	#include <crtdefs.h>
#else
	#include <stddef.h>
#endif


/*
	SIGN()
	------
*/
inline int sign(char a)
{
return a < 0 ? -1 : a > 0 ? 1 : 0;
}

inline int sign(short a)
{
return a < 0 ? -1 : a > 0 ? 1 : 0;
}

inline int sign(int a)
{
return a < 0 ? -1 : a > 0 ? 1 : 0;
}

inline int sign(long a)
{
return a < 0 ? -1 : a > 0 ? 1 : 0;
}

inline int sign(long long a)
{
return a < 0 ? -1 : a > 0 ? 1 : 0;
}

inline int sign(float a)
{
return a < 0 ? -1 : a > 0 ? 1 : 0;
}

inline int sign(double a)
{
return a < 0 ? -1 : a > 0 ? 1 : 0;
}

inline int sign(size_t a)
{
return a < 0 ? -1 : a > 0 ? 1 : 0;
}

#endif __MATHS_H__
