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
	ANT_SIGN()
	------
*/
inline int ANT_sign(char a) { return a < 0 ? -1 : a > 0 ? 1 : 0; }
inline int ANT_sign(short a) { return a < 0 ? -1 : a > 0 ? 1 : 0; }
inline int ANT_sign(int a) { return a < 0 ? -1 : a > 0 ? 1 : 0; }
inline int ANT_sign(long a) { return a < 0 ? -1 : a > 0 ? 1 : 0; }
inline int ANT_sign(long long a) { return a < 0 ? -1 : a > 0 ? 1 : 0; }
inline int ANT_sign(float a) { return a < 0 ? -1 : a > 0 ? 1 : 0; }
inline int ANT_sign(double a) { return a < 0 ? -1 : a > 0 ? 1 : 0; }


/*
	ANT_FLOOR_LOG2()
	----------------
*/
inline unsigned long ANT_floor_log2(unsigned long long x)
{
extern unsigned long ANT_floor_log2_byte[];
unsigned long sum, mult = 0;

do
	{
	sum = ANT_floor_log2_byte[x & 0xFF] + mult;
	mult += 8;
	x >>= 8;
	}
while (x != 0);

return sum;
}

/*
	ANT_CEILING_LOG2()
	------------------
*/
inline unsigned long ANT_ceiling_log2(unsigned long long x)
{
extern unsigned long ANT_ceiling_log2_byte[];
unsigned long sum, mult = 0;

do
	{
	sum = ANT_ceiling_log2_byte[x & 0xFF] + mult;
	mult += 8;
	x >>= 8;
	}
while (x != 0);

return sum;
}

#endif __MATHS_H__

