/*
	MATHS.H
	-------
*/
#ifndef MATHS_H_
#define MATHS_H_
#ifdef _MSC_VER
	#include <crtdefs.h>
#else
	#include <stddef.h>
#endif
#include <stdlib.h>
#include <math.h>

/*
	ANT_SIGN()
	----------
*/
template <class Type> int ANT_sign(Type a) { return a < 0 ? -1 : a > 0 ? 1 : 0; }

/*
	ANT_FLOOR_LOG2()
	----------------
*/
static inline unsigned long ANT_floor_log2(unsigned long long x)
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
static inline unsigned long ANT_ceiling_log2(unsigned long long x)
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

/*
	ANT_LOG_TO_BASE()
	-----------------
*/
static inline double ANT_log_to_base(double base, double value)
{
return log(value) / log(base);
}

/*
	ANT_LOG2()
	----------
*/
static inline double ANT_log2(double value)
{
static double log2 = log(2.0);

return log(value) / log2;
}

/*
	ANT_POW2_ZERO()
	---------------
	return 2^power (except that 2^0=0)
*/
static inline unsigned long ANT_pow2_zero(long power)
{
extern unsigned long ANT_powers_of_two_zero[];

return ANT_powers_of_two_zero[power];
}

/*
	ANT_POW2_ZERO_64()
	------------------
	64 bit verison of ANT_pow2_zero()
*/
static inline unsigned long long ANT_pow2_zero_64(long long power)
{
extern unsigned long long ANT_powers_of_two_long_long_zero[];

return ANT_powers_of_two_long_long_zero[power];
}

/*
	ANT_POW2()
	----------
*/
static inline unsigned long ANT_pow2(long power)
{
extern unsigned long ANT_powers_of_two[];

return ANT_powers_of_two[power];
}

/*
	ANT_POW2_64()
	-------------
	64-bit verison of ANT_POW2()
*/
static inline unsigned long long ANT_pow2_64(long long power)
{
extern unsigned long long ANT_powers_of_two_long_long[];

return ANT_powers_of_two_long_long[power];
}


/*
	ANT_MAX()
	---------
	For ANT_MAX we violate the ANT coding rule that says no templates
*/
template <class Type> Type ANT_max(Type first, Type second) { return first > second ? first : second; }
template <class Type> Type ANT_max(Type first, Type second, Type third) { return ANT_max(ANT_max(first, second), third); }

/*
	ANT_MIN()
	---------
	For ANT_MIN we violate the ANT coding rule that says no templates
*/
template <class Type> Type ANT_min(Type first, Type second) { return first < second ? first : second; }
template <class Type> Type ANT_min(Type first, Type second, Type third) { return ANT_min(ANT_min(first, second), third); }

/*
	ATOLL()
	-------
	atol() for long long integers
*/
#ifdef _MSC_VER
	static inline long long atoll(const char *string) { return _atoi64(string); }
#endif
static inline long long atoll(const unsigned char *string) { return atoll((const char *)string); }

#endif  /* MATHS_H_ */
