/*
	HASH_HEADER.H
	-------------
*/

#ifndef HASH_HEADER_
#define HASH_HEADER_

#include "fundamental_types.h"
#include "string_pair.h"

extern unsigned char ANT_header_hash_encode[];

/*
	ANT_HEADER_HASH_8()
	-------------------
	8-bit version of the header hash function - this one uses the first character and the
	length of the string.
*/
static inline unsigned long ANT_hash_header_8(ANT_string_pair *string)
{
unsigned long ans;

/*
	Bottom 6 bits are the hash of the first character
*/
ans = ANT_header_hash_encode[(*string)[0]];		// in the range 0..36

/*
	Top 2 bits are the bottom 2 bits of the string length
*/
ans |= (string->length() & 0x03) << 6;

return ans;
}

/*
	ANT_HEADER_HASH_24()
	--------------------
*/
static inline unsigned long ANT_hash_header_24(ANT_string_pair *string)
{
/*
	This code assumes a 37 character alphabet (a..z,A-Z,0..9,(~_@-)) and treats the string as a base 37 integer
	and encodes the length in the top 3 bits.  Numbers cause problems with this, especially increasing sequences
	because they end up with the indexer's direct tree chain in the hash table reducing to a linked list!  Numbers
	are now encoded as the sumber itself.
*/
unsigned long ans;
size_t len;
const long base = 37;

#if HASHER == HEADER_NUM
	if (ANT_isdigit((*string)[0]))
		return ANT_atoul(string->start, string->length()) % 0x1000000;
#endif

ans = ANT_header_hash_encode[(*string)[0]] * base * base * base;

if ((len = string->length()) > 1)
	ans += (ANT_header_hash_encode[(*string)[1]]) * base * base;
if (len > 2)
	ans += (ANT_header_hash_encode[(*string)[2]]) * base;
if (len > 3)
	ans += (ANT_header_hash_encode[(*string)[3]]);

ans += (string->length() & 0x07) << 21;	// top 3 bits are the length

return ans;
}

/*
	ANT_HEADER_HASH_32()
	--------------------
*/
static inline unsigned long ANT_hash_header_32(ANT_string_pair *string)
{
/*
	This code assumes a 37 character alphabet (a..z,A-Z,0..9,(~_@-)) and treats the string as a base 37 integer
	Numbers cause problems with this, especially increasing sequences because they end up with the indexer's direct
	tree chain in the hash table reducing to a linked list!  Numbers are now encoded as the number itself.
*/
uint32_t ans;
size_t len;
const long base = 37;

#if HASHER == HEADER_NUM
	if (ANT_isdigit((*string)[0]))
		return ANT_atoul(string->start, string->length());
#endif

ans = (ANT_header_hash_encode[(*string)[0]]) * base * base * base * base;

if ((len = string->length()) > 1)
	ans += (ANT_header_hash_encode[(*string)[1]]) * base * base * base;
if (len > 2)
	ans += (ANT_header_hash_encode[(*string)[2]]) * base * base;
if (len > 3)
	ans += (ANT_header_hash_encode[(*string)[3]]) * base;
if (len > 4)
	ans += (ANT_header_hash_encode[(*string)[4]]);

ans += (len & 0x1F) << 27; // top 5 bits are the length

return ans;
}

#endif
