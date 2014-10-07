#ifndef HASH_RANDOM_
#define HASH_RANDOM_

#include "fundamental_types.h"
#include "string_pair.h"

extern unsigned char ANT_hash_table[];

/*
	ANT_HASH_RANDOM_8()
	-------------------
*/
static inline unsigned int ANT_hash_random_8(char *string, size_t length, unsigned int seed)
{
unsigned char *ch;
size_t pos;

ch = (unsigned char *)string;

for (pos = 0; pos < length; pos++)
	seed = ANT_hash_table[seed ^ *ch++];

return seed;
}

/*
	ANT_HASH_RANDOM_8()
	-------------------
*/
static inline unsigned int ANT_hash_random_8(ANT_string_pair *string)
{
return ANT_hash_random_8(string->string(), string->length(), (unsigned char)(string->length() & 0xFF));
}

/*
	ANT_HASH_RANDOM_24()
	--------------------
	Uses ANT_random_hash_8 on the whole string, excluding the first
	character and excluding the first two characters to generate
	three hashes which are combined.
*/
static inline unsigned long ANT_hash_random_24(ANT_string_pair *string)
{
long hash1, hash2, hash3;

hash1 = ANT_hash_random_8(string->string(), string->length(), (unsigned char)(string->length() & 0xFF));
hash2 = string->length() <= 1 ? 0 : ANT_hash_random_8(string->string() + 1, string->length() - 1, (unsigned char)((string->length() - 1) & 0xFF));
hash3 = string->length() <= 2 ? 0 : ANT_hash_random_8(string->string() + 2, string->length() - 2, (unsigned char)((string->length() - 2) & 0xFF));
return (hash1 << 16) + (hash2 << 8) + hash3;
}

/*
	ANT_HASH_RANDOM_32()
	--------------------
*/
static inline unsigned long ANT_hash_random_32(ANT_string_pair *string)
{
long hash1, hash2, hash3, hash4;

hash1 = ANT_hash_random_8(string->string(), string->length(), (unsigned char)(string->length() & 0xFF));
hash2 = string->length() <= 1 ? 0 : ANT_hash_random_8(string->string() + 1, string->length() - 1, (unsigned char)((string->length() - 1) & 0xFF));
hash3 = string->length() <= 2 ? 0 : ANT_hash_random_8(string->string() + 2, string->length() - 2, (unsigned char)((string->length() - 2) & 0xFF));
hash4 = string->length() <= 3 ? 0 : ANT_hash_random_8(string->string() + 3, string->length() - 3, (unsigned char)((string->length() - 3) & 0xFF));
return (hash1 << 24) + (hash2 << 16) + (hash3 << 8) + hash4;
}

/*
	ANT_HASH_RANDOM_STEP_24()
	-------------------------
	Uses the same logic as ANT_random_hash_8, but generates three hashes
	on the 1,4,7... 2,5,8,... 3,6,9,.. characters which are then
	combined togehter for the final hash.
*/
static inline uint32_t ANT_hash_random_step_24(char *string, size_t length)
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
	ANT_HASH_RANDOM_HASH_STEP_24()
	------------------------------
*/
static inline uint32_t ANT_hash_random_step_24(ANT_string_pair *string)
{
return ANT_hash_random_step_24(string->string(), string->length());
}

/*
	ANT_HASH_RANDOM_STEP_32()
	-------------------------
*/
static inline uint32_t ANT_hash_random_step_32(char *string, size_t length)
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
	if (ch >= end)
		break;
	seed[3] = ANT_hash_table[seed[3] ^ *ch++];
	}

return result;
}

/*
	ANT_HASH_RANDOM_STEP_32()
	-------------------------
*/
static inline uint32_t ANT_hash_random_step_32(ANT_string_pair *string)
{
return ANT_hash_random_step_32(string->string(), string->length());
}

/*
	ANT_HASH_RANDOM_STEP_64()
	-------------------------
*/
static inline uint64_t ANT_hash_random_step_64(char *string, size_t length)
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

#endif
