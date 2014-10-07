/*
	HASH_HEADER.H
	-------------
*/

#ifndef HASH_MATT_
#define HASH_MATT_

#ifndef FALSE
	#define FALSE (0)
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

#include "hash_random.h"
#include "fundamental_types.h"
#include "string_pair.h"

extern unsigned char ANT_matt_hash_encode[];

static inline long ascii(ANT_string_pair *string, size_t len) {
	unsigned char ans = (*string)[0];
	if (len > 1) ans |= (*string)[1];
	if (len > 2) ans |= (*string)[2];
	return ans & 0x80;
}

/*
	ANT_HASH_MATT_8()
	-----------------
*/
static inline unsigned long ANT_hash_matt_8(ANT_string_pair *string)
{
unsigned long ans;

/*
	Bottom 6 bits are the hash of the first character
*/
ans = ANT_matt_hash_encode[(*string)[0]];		// in the range 0..37

/*
	Top 2 bits are the bottom 2 bits of the string length
*/
ans |= (string->length() & 0x03) << 6;

return ans;
}

/*
	ANT_HASH_MATT_UNIQ()
	--------------------
	Determine whether a value is unique or not
*/
static inline unsigned long ANT_hash_matt_unique(unsigned long value)
{
#if HASHER == MATT_N
const long base = 26;
#else
const long base = 36;
#endif
const long offset = base*base*base + base*base + base + 1;
return value < offset;
}

/*
	ANT_HASH_MATT_24()
	------------------
*/
static inline unsigned long ANT_hash_matt_24(ANT_string_pair *string)
{
#if HASHER == MATT_N
static const long base = 26;
#else
static const long base = 36;
#endif

static const long offset = base*base*base + base*base + base + 1;
static const long remain = (1 << 24) - offset;

unsigned long ans = 0;
size_t len;

/*
	Use a header hash if they are less than 3 bytes long and all
	ascii, otherwise randomly hash over the remaining space

	Ordered from least likely to most likely to succeed to minimise time
*/
#if HASHER == MATT_N
if ((len = string->length()) < 4 && !ANT_isdigit((*string)[0]) && ascii(string, len))
#else
if ((len = string->length()) < 4 && ascii(string, len))
#endif
	{
	if (len < 4)
		ans = (ans * base) + ANT_matt_hash_encode[(*string)[2]];
	if (len < 3)
		ans = (ans * base) + ANT_matt_hash_encode[(*string)[1]];
	return (ans * base) + ANT_matt_hash_encode[(*string)[0]];
	}

return offset + (ANT_hash_random_24(string) % remain);
}

/*
	ANT_HASH_MATT_32()
	------------------
*/
static inline unsigned long ANT_hash_matt_32(ANT_string_pair *string)
{
/*
	This code assumes a 37 character alphabet (a..z,A-Z,0..9,(~_@-)) and treats the string as a base 37 integer
	Numbers cause problems with this, especially increasing sequences because they end up with the indexer's direct
	tree chain in the hash table reducing to a linked list!  Numbers are now encoded as the number itself.
*/
#warning hash matt 32 == header hash 32
uint32_t ans;
size_t len;
const long base = 0x26;

#if HASHER == HEADER_NUM
	if (ANT_isdigit((*string)[0]))
		return ANT_atoul(string->start, string->length());
#endif

ans = (ANT_matt_hash_encode[(*string)[0]]) * base * base * base * base;

if ((len = string->length()) > 1)
	ans += (ANT_matt_hash_encode[(*string)[1]]) * base * base * base;
if (len > 2)
	ans += (ANT_matt_hash_encode[(*string)[2]]) * base * base;
if (len > 3)
	ans += (ANT_matt_hash_encode[(*string)[3]]) * base;
if (len > 4)
	ans += (ANT_matt_hash_encode[(*string)[4]]);

ans += (len & 0x1F) << 27; // top 5 bits are the length

return ans;
}

#endif
