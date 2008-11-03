/*
	HASH_TABLE.H
	------------
*/

#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#include "string_pair.h"

extern unsigned char ANT_hash_table[];

/*
	ANT_HASH()
	----------
*/
inline static unsigned int ANT_hash(char *string, long length, unsigned int seed)
{
unsigned char *ch;
long pos;

ch = (unsigned char *)string;

for (pos = 0; pos < length; pos++)
	seed = ANT_hash_table[seed ^ *ch++];

return seed;
}

/*
	ANT_HASH_8()
	------------
*/
inline static unsigned int ANT_hash_8(ANT_string_pair *string)
{
return ANT_hash(string->string(), string->length(), (unsigned char)(string->length() & 0xFF));
}

/*
	ANT_HASH_16()
	-------------
*/
inline static unsigned long ANT_hash_16(ANT_string_pair *string)
{
long hash1, hash2;

hash1 = ANT_hash_8(string);
hash2 = string->length() <= 1 ? 0 : ANT_hash(string->string() + 1, string->length() - 1, (unsigned char)((string->length() - 1) & 0xFF));

return (hash1 << 8) + hash2;
}

/*
	ANT_HASH_24()
	-------------
*/
inline static unsigned long ANT_hash_24(ANT_string_pair *string)
{
long hash1, hash2, hash3;

hash1 = ANT_hash_8(string);
hash2 = string->length() <= 1 ? 0 : ANT_hash(string->string() + 1, string->length() - 1, (unsigned char)((string->length() - 1) & 0xFF));
hash3 = string->length() <= 2 ? 0 : ANT_hash(string->string() + 2, string->length() - 2, (unsigned char)((string->length() - 2) & 0xFF));
return (hash1 << 16) + (hash2 << 8) + hash3;
}

/*
	FROM:http://www.azillionmonkeys.com/qed/hash.html
*/

#define get16bits(d) (*((const unsigned short *) (d)))

inline static unsigned long SuperFastHash (const char * data, int len) {
unsigned long hash = len, tmp;
int rem;

    if (len <= 0 || data == NULL) return 0;

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (unsigned short);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += get16bits (data);
                hash ^= hash << 16;
                hash ^= data[sizeof (unsigned short)] << 18;
                hash += hash >> 11;
                break;
        case 2: hash += get16bits (data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += *data;
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
}


#endif __HASH_TABLE_H__
