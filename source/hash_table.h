/*
	HASH_TABLE.H
	------------
*/
#ifndef HASH_TABLE_H_
#define HASH_TABLE_H_

#include "string_pair.h"
#include "fundamental_types.h"

extern unsigned char ANT_hash_table[];
extern unsigned char ANT_header_hash_encode[];

/*
	ANT_RANDOM_HASH_8()
	-------------------
*/
static inline unsigned int ANT_random_hash_8(char *string, size_t length, unsigned int seed)
{
unsigned char *ch;
size_t pos;

ch = (unsigned char *)string;

for (pos = 0; pos < length; pos++)
	seed = ANT_hash_table[seed ^ *ch++];

return seed;
}

/*
	ANT_RANDOM_HASH_24()
	--------------------
*/
static inline unsigned long ANT_random_hash_8_24(ANT_string_pair *string)
{
long hash1, hash2, hash3;

hash1 = ANT_random_hash_8(string->string(), string->length(), (unsigned char)(string->length() & 0xFF));
hash2 = string->length() <= 1 ? 0 : ANT_random_hash_8(string->string() + 1, string->length() - 1, (unsigned char)((string->length() - 1) & 0xFF));
hash3 = string->length() <= 2 ? 0 : ANT_random_hash_8(string->string() + 2, string->length() - 2, (unsigned char)((string->length() - 2) & 0xFF));
return (hash1 << 16) + (hash2 << 8) + hash3;
}

/*
	ANT_RANDOM_HASH_64()
	-------------------
*/
static inline uint64_t ANT_random_hash_64(char *string, size_t length)
{
unsigned char *ch, *seed, *end;
uint64_t result;

result = 0;		// this will initialise all eight bytes
seed = (unsigned char *)&result;	// now take a pointer to the array of eight bytes so that we can address each individually
ch = (unsigned char *)string;
end = ch + length;

next_eight_bytes:
	if (ch >= end)
		return result;
	seed[0] = ANT_hash_table[seed[0] ^ *ch++];
	if (ch >= end)
		return result;
	seed[1] = ANT_hash_table[seed[1] ^ *ch++];
	if (ch >= end)
		return result;
	seed[2] = ANT_hash_table[seed[2] ^ *ch++];
	if (ch >= end)
		return result;
	seed[3] = ANT_hash_table[seed[3] ^ *ch++];
	if (ch >= end)
		return result;
	seed[4] = ANT_hash_table[seed[4] ^ *ch++];
	if (ch >= end)
		return result;
	seed[5] = ANT_hash_table[seed[5] ^ *ch++];
	if (ch >= end)
		return result;
	seed[6] = ANT_hash_table[seed[6] ^ *ch++];
	if (ch >= end)
		return result;
	seed[7] = ANT_hash_table[seed[7] ^ *ch++];
goto next_eight_bytes;
}

/*
	ANT_RANDOM_HASH_24()
	--------------------
*/
static inline uint32_t ANT_random_hash_24(char *string, size_t length)
{
unsigned char *ch, *seed, *end;
uint32_t result;

result = 0;		// this will initialise all four bytes
seed = (unsigned char *)&result;	// now take a pointer to the array of four bytes so that we can address each individually
ch = (unsigned char *)string;
end = ch + length;

while (ch < end)
	{
	seed[0] = ANT_hash_table[seed[0] ^ *ch++];
	if (ch >= end)
		break;
	seed[1] = ANT_hash_table[seed[1] ^ *ch++];
	if (ch >= end)
		break;
	seed[2] = ANT_hash_table[seed[2] ^ *ch++];
	}

seed[3] = seed[0];

return result & 0xFFFFFF;
}

/*
	ANT_MEMORY_INDEX::ANT_HEADER_HASH_24()
	--------------------------------------
*/
static inline unsigned long ANT_header_hash_24(ANT_string_pair *string)
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

if (ANT_isdigit((*string)[0]))
	return ANT_atoul(string->start, string->length()) % 0x1000000;

if (((*string)[0] & 0x80) != 0)
	return ANT_random_hash_24(string->string(), string->length());

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
	ANT_HEADER_HASH_8()
	-------------------
	8-bit version of the header hash function - this one uses the first character and the
	length of the string.
*/
static inline unsigned long ANT_header_hash_8(ANT_string_pair *string)
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
	ANT_HASH_24()
	-------------
*/
static inline unsigned long ANT_hash_24(ANT_string_pair *string)
{
#ifndef HASHER
	#error "HASHER must be defined so a hash_table function can be chosen"
#else
	#if HASHER == RANDOM_HASHER
		return ANT_random_hash_8_24(string);
	#elif HASHER == HEADER_HASHER
		return ANT_header_hash_24(string);
	#else
		#error "Don't know which hash function to use - aborting"
	#endif
#endif
}

/*
	ANT_HASH_8()
	------------
*/
static inline unsigned long ANT_hash_8(ANT_string_pair *string)
{
#ifndef HASHER
	#error "HASHER must be defined so a hash_table function can be chosen"
#else
	#if HASHER == RANDOM_HASHER
		return ANT_random_hash_8(string);
	#elif HASHER == HEADER_HASHER
		return ANT_header_hash_8(string);
	#else
		#error "Don't know which hash function to use - aborting"
	#endif
#endif
}

#endif  /* HASH_TABLE_H_ */
