#ifndef HASH_HEADER_EXPERIMENTAL_
#define HASH_HEADER_EXPERIMENTAL_

extern unsigned char ANT_header_hash_encode[];
extern unsigned char ANT_header_hash_encode_27[];

/*
	ANT_HEADER_HASH_8()
	-------------------
	8-bit version of the header hash function - this one uses the first character and the
	length of the string.
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

/*
	ANT_HEADER_HASH_24()
	--------------------
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
	ANT_HEADER_HASH_32()
	--------------------
*/
static inline unsigned long ANT_hash_header_experimental_32(ANT_string_pair *string)
{
// TODO: Adapt this for the base27 version
/*
	This code assumes a 37 character alphabet (a..z,A-Z,0..9,(~_@-)) and treats the string as a base 37 integer
	Numbers cause problems with this, especially increasing sequences because they end up with the indexer's direct
	tree chain in the hash table reducing to a linked list!  Numbers are now encoded as the number itself.
*/
uint32_t ans;
size_t len;
const long base = 37;

if (ANT_isdigit((*string)[0]))
	return ANT_atoul(string->start, string->length());

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
