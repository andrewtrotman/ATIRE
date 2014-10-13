/*
	HASH_HEADER_COLLAPSE.H
	----------------------
	Examples assume 24-bit hash.

	Uses the first characters of the string if the string is <= 4 (37^4 < 2^24, 37^5 > 2^24).

	If the string is longer, then it selectively uses different characters from the string to keep the different
	prefixes, but disambiguate (roughly), the suffixes of common prefixes. This might introduce _some_ collisions
	but they are for longer terms, which we do not care about as much (they have a smaller df/cf).

	However, because the lead of the prefix combined with the suffix could be equivalent to an already found 4-char term,
	then we shift it along a bit. We can do this shift a number of times, 2^24 > 8 * 37^4, however, as we want to keep
	some of the prefix, in practice we only do this a few times.

	Below is a map of length to the characters extracted from the string:
	1   : 0
	2   : 0, 1
	3   : 0, 1, 2
	4   : 0, 1, 2, 3
	5   : 0,    2, 3, 4
	6   : 0,       3, 4, 5
	7   : 0,          4, 5, 6
	8   : 0,          4,    6, 7
	9   : 0,          4,       7, 8
	10+ : 0,          4,       7,    9

	We then shift by the range 37^4:
		once to stop terms > 4 colliding with < 4
		once to stop terms > 7 colliding with < 7
		once to stop terms > 9 colliding with < 9

	This variant prefers to keep as much of the end of the string as possible, while still keeping the root
	of the prefix. Testing showed this to have more potential than keeping as much of the early string as
	possible.

	This does allow for 1 bit of the length to be used (in the case of 24-bit hash), further separating
	any collisions in the higher ranges.
*/

#ifndef HASH_HEADER_COLLAPSE_
#define HASH_HEADER_COLLAPSE_

#include "string_pair.h"

extern unsigned char ANT_header_hash_encode[];

/*
	ANT_HASH_HEADER_COLLAPSE_8()
	----------------------------
*/
static inline unsigned long ANT_hash_header_collapse_8(ANT_string_pair *string)
{
/*#warning Hash header collapse 8-bit variant is not fully implemented yet*/
unsigned long ans;

ans = ANT_header_hash_encode[(*string)[0]]; // range 0..36

/*
	Top 3 bits are the bottom 3 bits of the string length
*/
ans |= (string->length() & 0x07) << 5;

return ans;
}

/*
	ANT_HASH_HEADER_COLLAPSE_24()
	-----------------------------
*/
static inline unsigned long ANT_hash_header_collapse_24(ANT_string_pair *string)
{
unsigned long ans = (string->length() & 0x01) << 23;
const long base = 37;

switch (string->length())
	{
	default: // >= 10
		ans += ANT_header_hash_encode[(*string)[0]] * base * base * base;
		ans += ANT_header_hash_encode[(*string)[4]] * base * base;
		ans += ANT_header_hash_encode[(*string)[7]] * base;
		ans += ANT_header_hash_encode[(*string)[9]];
		ans += base * base * base * base; // avoid colliding <= 9
		ans += base * base * base * base; // avoid colliding <= 7
		ans += base * base * base * base; // avoid colliding <= 4
		break;
	case 9:
		ans += ANT_header_hash_encode[(*string)[0]] * base * base * base;
		ans += ANT_header_hash_encode[(*string)[4]] * base * base;
		ans += ANT_header_hash_encode[(*string)[7]] * base;
		ans += ANT_header_hash_encode[(*string)[8]];
		ans += base * base * base * base; // avoid colliding <= 7
		ans += base * base * base * base; // avoid colliding <= 4
		break;
	case 8:
		ans += ANT_header_hash_encode[(*string)[0]] * base * base * base;
		ans += ANT_header_hash_encode[(*string)[4]] * base * base;
		ans += ANT_header_hash_encode[(*string)[6]] * base;
		ans += ANT_header_hash_encode[(*string)[7]];
		ans += base * base * base * base; // avoid colliding <= 7
		ans += base * base * base * base; // avoid colliding <= 4
		break;
	case 7:
		ans += ANT_header_hash_encode[(*string)[0]] * base * base * base;
		ans += ANT_header_hash_encode[(*string)[4]] * base * base;
		ans += ANT_header_hash_encode[(*string)[5]] * base;
		ans += ANT_header_hash_encode[(*string)[6]];
		ans += base * base * base * base; // avoid colliding <= 4
		break;
	case 6:
		ans += ANT_header_hash_encode[(*string)[0]] * base * base * base;
		ans += ANT_header_hash_encode[(*string)[3]] * base * base;
		ans += ANT_header_hash_encode[(*string)[4]] * base;
		ans += ANT_header_hash_encode[(*string)[5]];
		ans += base * base * base * base; // avoid colliding <= 4
		break;
	case 5:
		ans += ANT_header_hash_encode[(*string)[0]] * base * base * base;
		ans += ANT_header_hash_encode[(*string)[2]] * base * base;
		ans += ANT_header_hash_encode[(*string)[3]] * base;
		ans += ANT_header_hash_encode[(*string)[4]];
		ans += base * base * base * base; // avoid colliding <= 4
		break;
	case 4:
		ans += ANT_header_hash_encode[(*string)[0]] * base * base * base;
		ans += ANT_header_hash_encode[(*string)[1]] * base * base;
		ans += ANT_header_hash_encode[(*string)[2]] * base;
		ans += ANT_header_hash_encode[(*string)[3]];
		break;
	case 3:
		ans += ANT_header_hash_encode[(*string)[0]] * base * base * base;
		ans += ANT_header_hash_encode[(*string)[1]] * base * base;
		ans += ANT_header_hash_encode[(*string)[2]] * base;
		break;
	case 2:
		ans += ANT_header_hash_encode[(*string)[0]] * base * base * base;
		ans += ANT_header_hash_encode[(*string)[1]] * base * base;
		break;
	case 1:
		ans += ANT_header_hash_encode[(*string)[0]] * base * base * base;
		break;
	}
	return ans;
}

/*
	ANT_HASH_HEADER_COLLAPSE_32()
	-----------------------------
*/
static inline unsigned long ANT_hash_header_collapse_32(ANT_string_pair *string)
{
/*#warning Hash header collapse 32-bit variant not fully implemented yet*/
/*
	Because we're special casing numbers, we only need a base-27 number
	In which case we can use the first 5 characters to encode the number
*/
unsigned long ans = 0;
const long base = 37;
const long num_start = base * base * base * base * base;

/*
	Numbers get distributed across the space for the 6th digit, ensuring that all 
	words <= 5 bytes long are unique
*/
if (ANT_isdigit((*string)[0]))
	return num_start + (ANT_atoul(string->start, string->length()) % (0x100000000 - num_start));

switch (string->length())
	{
	default: ans = ans * base + ANT_header_hash_encode[(*string)[5]];
	case 5 : ans = ans * base + ANT_header_hash_encode[(*string)[4]];
	case 4 : ans = ans * base + ANT_header_hash_encode[(*string)[3]];
	case 3 : ans = ans * base + ANT_header_hash_encode[(*string)[2]];
	case 2 : ans = ans * base + ANT_header_hash_encode[(*string)[1]];
	case 1 : ans = ans * base + ANT_header_hash_encode[(*string)[0]];
	}

return ans + ((string->length() & 0x03) << 30);	// top 2 bits are the length
}

#endif
