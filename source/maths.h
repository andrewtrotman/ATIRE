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
#include <limits.h>
#include <limits>

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

/*
	ANT_RAND_XORSHIFT64()
	---------------------
	64-bit xor-shift random number generator according to:
	Marsaglia, G., (2003), Xorshift RNGs, Journal of Statistical Software 8(14):1-6
*/
inline unsigned long long ANT_rand_xorshift64(unsigned long long *seed)
{
//static unsigned long long seed = 88172645463325252LL;

*seed ^= (*seed << 13);
*seed ^= (*seed >> 7);
return (*seed ^= (*seed << 17));
}

/*
	ANT_compiletime_floor_log_to_base
	---------------------------------
	Code for computing logs of arbitrary bases at compile-time. This allows logs to be computed as part of constant expressions.
*/
template <unsigned long long n, unsigned int base>
struct ANT_compiletime_floor_log_to_base
{
enum { value = n < base ? 0 : 1 + ANT_compiletime_floor_log_to_base<n / base, base>::value };
};

template <unsigned int base>
struct ANT_compiletime_floor_log_to_base<0, base>
{
enum { value = 0 };
};

/*
	ANT_compiletime_ispowerof2
	--------------------------
*/
template <int n>
struct ANT_compiletime_ispowerof2
{
enum { value = !(n & (n-1)) };
};

/*
	ANT_compiletime_floor_log2
	--------------------------
*/
template <unsigned long long n>
struct ANT_compiletime_floor_log2
{
enum { value = 1 + ANT_compiletime_floor_log2<(n >> 1)>::value };
};

template<>
struct ANT_compiletime_floor_log2<0>
{
enum { value = 0 };
};

template<>
struct ANT_compiletime_floor_log2<1>
{
enum { value = 0 };
};

/*
	ANT_compiletime_pow
	-------------------
*/
template <unsigned int base, unsigned int exponent>
struct ANT_compiletime_pow
{
static const unsigned long long value = base * ANT_compiletime_pow<base, exponent - 1>::value;
};

template<unsigned int base>
struct ANT_compiletime_pow<base, 1>
{
static const unsigned long long value = base;
};

template<unsigned int base>
struct ANT_compiletime_pow<base, 0>
{
static const unsigned long long value = 1;
};

/*
	ANT_compiletime_int_max
	-----------------------
	This is needed because numeric_limits<T>::max() is unavailable at compile-time:
*/
template <typename T>
struct ANT_compiletime_int_max
{
static const T value = (T) (std::numeric_limits<T>::is_signed ? ~(T) (1ULL << (sizeof(T) * CHAR_BIT - 1)) : ~0ULL);
};

/*
	ANT_compiletime_int_floor_log_to_base
	-------------------------------------
	How many 'base' digits would fit into an integer of type T? This is required in addition to ANT_compiletime_floor_log_to_base,
	because we need to be able to compute it precisely for the largest supported integral type, too.
*/
template <typename T, int base>
struct ANT_compiletime_int_floor_log_to_base
{
/*
	Avoid creating the value 1 << sizeof(T) * CHAR_BIT, which can't fit in T. The only case where this is important
	is when 'base' is a power of two (and so could fit an integer number of times into T), otherwise we can take the
	log of 1 << sizeof(T) * CHAR_BIT - 1 instead.
*/
enum { value = (ANT_compiletime_ispowerof2<base>::value ? (sizeof(T) * CHAR_BIT - (std::numeric_limits<T>::is_signed ? 1 : 0)) / ANT_compiletime_floor_log2<base>::value : ANT_compiletime_floor_log_to_base<ANT_compiletime_int_max<T>::value, base>::value) };
};

/*
	ANT_compiletime_int_floor_log_to_base_has_remainder
	---------------------------------------------------
	Is there a remainder when computing the log of the integer type T (i.e. 2^(num_bits_in_T)) to the given base?
*/
template <typename T, int base>
struct ANT_compiletime_int_floor_log_to_base_has_remainder
{
/* 
	The only way to avoid a remainder is to have base be a power of two where the number of bits required to represent it fits exactly into T
*/
enum { value = (ANT_compiletime_ispowerof2<base>::value ? ((sizeof(T) * CHAR_BIT - (std::numeric_limits<T>::is_signed ? 1 : 0)) % ANT_compiletime_floor_log2<base>::value) != 0 : 1) };
};

/*
	ANT_compiletime_int_floor_log_to_base_remainder
	-----------------------------------------------
	Computes 2^(num_bits_in_T) / pow(base, floor(log_base(2^(num_bits_in_T), base))))
*/
template <typename T, int base, int has_remainder>
struct ANT_compiletime_int_floor_log_to_base_remainder
{
enum { value = ANT_compiletime_int_max<T>::value / ANT_compiletime_pow<base, ANT_compiletime_int_floor_log_to_base<T, base>::value>::value };
};

template <typename T, int base>
struct ANT_compiletime_int_floor_log_to_base_remainder<T, base, 0>
{
enum { value = 1 } ;
};

#endif  /* MATHS_H_ */
