/*
	HASH_TABLE.H
	------------
*/

#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#include "string_pair.h"

extern unsigned char ANT_hash_table[];

/*
	ANT_RANDOM_HASH()
	-----------------
*/
inline static unsigned int ANT_random_hash(char *string, long length, unsigned int seed)
{
unsigned char *ch;
long pos;

ch = (unsigned char *)string;

for (pos = 0; pos < length; pos++)
	seed = ANT_hash_table[seed ^ *ch++];

return seed;
}

/*
	ANT_RANDOM_HASH_24()
	--------------------
*/
inline static unsigned long ANT_random_hash_24(ANT_string_pair *string)
{
long hash1, hash2, hash3;

hash1 = ANT_random_hash(string->string(), string->length(), (unsigned char)(string->length() & 0xFF));
hash2 = string->length() <= 1 ? 0 : ANT_random_hash(string->string() + 1, string->length() - 1, (unsigned char)((string->length() - 1) & 0xFF));
hash3 = string->length() <= 2 ? 0 : ANT_random_hash(string->string() + 2, string->length() - 2, (unsigned char)((string->length() - 2) & 0xFF));
return (hash1 << 16) + (hash2 << 8) + hash3;
}


/*
	ANT_SUPER_FAST_HASH()
	---------------------
	This is Paul Hsieh's SuperFastHash function renamed and layed out
	according to the ANT source code-layout rules (his comments remain).
	For the original source see:http://www.azillionmonkeys.com/qed/hash.html
*/
inline static unsigned long ANT_super_fast_hash(char *data, int len)
{
#define get16bits(d) (*((const unsigned short *) (d)))
unsigned long hash = len, tmp;
int rem;

if (len <= 0 || data == NULL)
	return 0;

rem = len & 3;
len >>= 2;

/* Main loop */
for (;len > 0; len--)
	{
	hash  += get16bits(data);
	tmp    = (get16bits(data + 2) << 11) ^ hash;
	hash   = (hash << 16) ^ tmp;
	data  += 2 * sizeof(unsigned short);
	hash  += hash >> 11;
	}

/* Handle end cases */
switch (rem)
	{
	case 3:
		hash += get16bits(data);
		hash ^= hash << 16;
		hash ^= data[sizeof (unsigned short)] << 18;
		hash += hash >> 11;
		break;
	case 2: 
		hash += get16bits(data);
		hash ^= hash << 11;
		hash += hash >> 17;
		break;
	case 1: 
		hash += *data;
		hash ^= hash << 10;
		hash += hash >> 1;
}

/* Force "avalanching" of final 127 bits */
hash ^= hash << 3;
hash += hash >> 5;
hash ^= hash << 4;
hash += hash >> 17;
hash ^= hash << 25;
hash += hash >> 6;

return hash;
#undef get16bits
}

/*
	ANT_SUPER_FAST_HASH_24()
	------------------------
*/
inline static unsigned long ANT_super_fast_hash_24(ANT_string_pair *string)
{
return ANT_super_fast_hash(string->string(), string->length()) & 0xffffff;
}

/*
	ANT_MEMORY_INDEX::ANT_HEADER_HASH_24()
	--------------------------------------
*/
inline static unsigned long ANT_header_hash_24(ANT_string_pair *string)
{
long ans, len;
const unsigned char base = 'a' - 1;

ans = (string->string()[0] - base) * 27 * 27 * 27;

if ((len = string->length()) > 1)
	ans += (string->string()[1] - base) * 27 * 27;
if (len > 2)
	ans += (string->string()[2] - base) * 27;
if (len > 3)
	ans += (string->string()[3] - base);

ans += (string->length() & 0x0F) << 20;	// top 4 bits are the length

return ans;
}


/*
	ANT_HASH_24()
	-------------
*/
inline static unsigned long ANT_hash_24(ANT_string_pair *string)
{
#ifndef HASHER
	#error "HASHER must be defined so a hash_table function can be chosen"
#else
	#if HASHER == RANDOM_HASHER
		return ANT_random_hash_24(string);
	#elif HASHER == HEADER_HASHER
		return ANT_header_hash_24(string);
	#elif HASHER == FAST_HASHER
		return ANT_super_fast_hash_24(string);
	#else
		#error "Don't know which hash function to use - aborting"
	#endif
#endif
}

#endif __HASH_TABLE_H__
