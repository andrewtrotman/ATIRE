/*
	HASH_SUPERFAST.H
	----------------
	Paul Hsieh's SuperFastHash, adapted to conform to ATIRE coding standards (mostly).

	Original from: http://www.azillionmonkeys.com/qed/hash.html
*/

#ifndef HASH_SUPERFAST_
#define HASH_SUPERFAST_

#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8) + (uint32_t)(((const uint8_t *)(d))[0]))

/*
	ANT_HASH_SUPERFAST_32()
	-----------------------
*/
static inline unsigned long ANT_hash_superfast_32(char *string, size_t length)
{
uint32_t hash = length, tmp;
int rem = length & 3;
length >>= 2;

for (; length > 0; length--)
	{
	hash += get16bits(string);
	tmp = (get16bits(string + 2) << 11) ^ hash;
	hash = (hash << 16) ^ tmp;
	string += 2 * sizeof(uint16_t);
	hash += hash >> 11;
	}

switch (rem)
	{
	case 3:
		hash += get16bits(string);
		hash ^= hash << 16;
		hash ^= ((signed char)string[sizeof(uint16_t)]) << 18;
		hash += hash >> 11;
		break;
	case 2:
		hash += get16bits(string);
		hash ^= hash << 11;
		hash += hash >> 17;
		break;
	case 1:
		hash += (signed char)*string;
		hash ^= hash << 10;
		hash += hash >> 1;
	}

hash ^= hash << 3;
hash += hash >> 5;
hash ^= hash << 4;
hash += hash >> 17;
hash ^= hash << 25;
hash += hash >> 6;

return hash;
}

/*
	ANT_HASH_SUPERFAST_8()
	----------------------
*/
static inline unsigned long ANT_hash_superfast_8(ANT_string_pair *string)
{
return ANT_hash_superfast_32(string->string(), string->length()) & 0xFF;
}

/*
	ANT_HASH_SUPERFAST_16()
	-----------------------
*/
static inline unsigned long ANT_hash_superfast_16(ANT_string_pair *string)
{
return ANT_hash_superfast_32(string->string(), string->length()) & 0xFFFF;
}

/*
	ANT_HASH_SUPERFAST_24()
	-----------------------
*/
static inline unsigned long ANT_hash_superfast_24(ANT_string_pair *string)
{
return ANT_hash_superfast_32(string->string(), string->length()) & 0xFFFFFF;
}

/*
	ANT_HASH_SUPERFAST_32()
	-----------------------
*/
static inline unsigned long ANT_hash_superfast_32(ANT_string_pair *string)
{
return ANT_hash_superfast_32(string->string(), string->length());
}

#endif
