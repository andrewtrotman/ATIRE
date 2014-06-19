/*
	HASH_HEADER_EXPERIMENTAL.H
	--------------------------
*/

#ifndef HASH_HEADER_EXPERIMENTAL_
#define HASH_HEADER_EXPERIMENTAL_

#include "string_pair.h"

extern unsigned char ANT_header_hash_encode_27[];

/*
	ANT_HASH_HEADER_EXPERIMENTAL_8()
	--------------------------------
*/
static inline unsigned long ANT_hash_header_experimental_8(ANT_string_pair *string)
{
unsigned long ans;

ans = ANT_header_hash_encode_27[(*string)[0]]; // range 0..26

/*
	Top 3 bits are the bottom 3 bits of the string length
*/
ans |= (string->length() & 0x07) << 5;

return ans;
}

#ifdef NEVER
/*
	ANT_HASH_HEADER_EXPERIMENTAL_16()
	---------------------------------
*/
static inline unsigned long ANT_hash_header_experimental_16(ANT_string_pair *string)
{
/*
	Because we're special casing numbers, we only need a base-27 number
	In which case we can use the first 5 characters to encode the number
*/
unsigned long ans = 0;
const long base = 27;
const long num_start = base * base;

/*
	Numbers get distributed across the space for the 5th digit, ensuring that all 
	words <= 4 bytes long are unique
*/
if (ANT_isdigit((*string)[0]))
	return num_start + (ANT_atoul(string->start, string->length()) % (0x10000 - num_start));

switch (string->length())
	{
	default: ans = ans * base + ANT_header_hash_encode_27[(*string)[2]];
	case 2 : ans = ans * base + ANT_header_hash_encode_27[(*string)[1]];
	case 1 : ans = ans * base + ANT_header_hash_encode_27[(*string)[0]];
	}
}
#endif

/*
	ANT_HASH_HEADER_EXPERIMENTAL_24()
	---------------------------------
*/
static inline unsigned long ANT_hash_header_experimental_24(ANT_string_pair *string)
{
/*
	Because we're special casing numbers, we only need a base-27 number
	In which case we can use the first 5 characters to encode the number
*/
unsigned long ans = 0;
const long base = 27;
const long num_start = base * base * base * base;

/*
	Numbers get distributed across the space for the 5th digit, ensuring that all 
	words <= 4 bytes long are unique
*/
if (ANT_isdigit((*string)[0]))
	return num_start + (ANT_atoul(string->start, string->length()) % (0x1000000 - num_start));

/*
	While there are ~2M unused buckets from this, there aren't enough unused buckets
	to dedicate any bits to the length
*/
switch (string->length())
	{
	default:  ans = ans * base + ANT_header_hash_encode_27[(*string)[4]];
	case 4 :  ans = ans * base + ANT_header_hash_encode_27[(*string)[3]];
	case 3 :  ans = ans * base + ANT_header_hash_encode_27[(*string)[2]];
	case 2 :  ans = ans * base + ANT_header_hash_encode_27[(*string)[1]];
	case 1 : return ans * base + ANT_header_hash_encode_27[(*string)[0]];
	}
}

/*
	ANT_HASH_HEADER_EXPERIMENTAL_32()
	---------------------------------
*/
static inline unsigned long ANT_hash_header_experimental_32(ANT_string_pair *string)
{
/*
	Because we're special casing numbers, we only need a base-27 number
	In which case we can use the first 5 characters to encode the number
*/
unsigned long ans = 0;
const long base = 27;
const long num_start = base * base * base * base * base;

/*
	Numbers get distributed across the space for the 6th digit, ensuring that all 
	words <= 5 bytes long are unique
*/
if (ANT_isdigit((*string)[0]))
	return num_start + (ANT_atoul(string->start, string->length()) % (0x100000000 - num_start));

switch (string->length())
	{
	default: ans = ans * base + ANT_header_hash_encode_27[(*string)[5]];
	case 5 : ans = ans * base + ANT_header_hash_encode_27[(*string)[4]];
	case 4 : ans = ans * base + ANT_header_hash_encode_27[(*string)[3]];
	case 3 : ans = ans * base + ANT_header_hash_encode_27[(*string)[2]];
	case 2 : ans = ans * base + ANT_header_hash_encode_27[(*string)[1]];
	case 1 : ans = ans * base + ANT_header_hash_encode_27[(*string)[0]];
	}

return ans + ((string->length() & 0x03) << 30);	// top 2 bits are the length
}

#endif
