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

#include <stdio.h>      /* defines printf for tests */
#include <time.h>       /* defines time_t for timings in the test */
#include <stdint.h>     /* defines uint32_t etc */
#ifdef linux
	#include <sys/param.h>  /* attempt to define endianness */
	#include <endian.h>    /* attempt to define endianness */
#endif

/*
 * My best guess at if you are big-endian or little-endian.  This may
 * need adjustment.
 */
#if (defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && \
     __BYTE_ORDER == __LITTLE_ENDIAN) || \
    (defined(i386) || defined(__i386__) || defined(__i486__) || \
     defined(__i586__) || defined(__i686__) || defined(vax) || defined(MIPSEL))
# define HASH_LITTLE_ENDIAN 1
# define HASH_BIG_ENDIAN 0
#elif (defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && \
       __BYTE_ORDER == __BIG_ENDIAN) || \
      (defined(sparc) || defined(POWERPC) || defined(mc68000) || defined(sel))
# define HASH_LITTLE_ENDIAN 0
# define HASH_BIG_ENDIAN 1
#else
# define HASH_LITTLE_ENDIAN 0
# define HASH_BIG_ENDIAN 0
#endif

#define hashsize(n) ((uint32_t)1<<(n))
#define hashmask(n) (hashsize(n)-1)
#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

/*
-------------------------------------------------------------------------------
mix -- mix 3 32-bit values reversibly.

This is reversible, so any information in (a,b,c) before mix() is
still in (a,b,c) after mix().

If four pairs of (a,b,c) inputs are run through mix(), or through
mix() in reverse, there are at least 32 bits of the output that
are sometimes the same for one pair and different for another pair.
This was tested for:
* pairs that differed by one bit, by two bits, in any combination
  of top bits of (a,b,c), or in any combination of bottom bits of
  (a,b,c).
* "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
  the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
  is commonly produced by subtraction) look like a single 1-bit
  difference.
* the base values were pseudorandom, all zero but one bit set, or 
  all zero plus a counter that starts at zero.

Some k values for my "a-=c; a^=rot(c,k); c+=b;" arrangement that
satisfy this are
    4  6  8 16 19  4
    9 15  3 18 27 15
   14  9  3  7 17  3
Well, "9 15 3 18 27 15" didn't quite get 32 bits diffing
for "differ" defined as + with a one-bit base and a two-bit delta.  I
used http://burtleburtle.net/bob/hash/avalanche.html to choose 
the operations, constants, and arrangements of the variables.

This does not achieve avalanche.  There are input bits of (a,b,c)
that fail to affect some output bits of (a,b,c), especially of a.  The
most thoroughly mixed value is c, but it doesn't really even achieve
avalanche in c.

This allows some parallelism.  Read-after-writes are good at doubling
the number of bits affected, so the goal of mixing pulls in the opposite
direction as the goal of parallelism.  I did what I could.  Rotates
seem to cost as much as shifts on every machine I could lay my hands
on, and rotates are much kinder to the top and bottom bits, so I used
rotates.
-------------------------------------------------------------------------------
*/
#define mix(a,b,c) \
{ \
  a -= c;  a ^= rot(c, 4);  c += b; \
  b -= a;  b ^= rot(a, 6);  a += c; \
  c -= b;  c ^= rot(b, 8);  b += a; \
  a -= c;  a ^= rot(c,16);  c += b; \
  b -= a;  b ^= rot(a,19);  a += c; \
  c -= b;  c ^= rot(b, 4);  b += a; \
}

/*
-------------------------------------------------------------------------------
final -- final mixing of 3 32-bit values (a,b,c) into c

Pairs of (a,b,c) values differing in only a few bits will usually
produce values of c that look totally different.  This was tested for
* pairs that differed by one bit, by two bits, in any combination
  of top bits of (a,b,c), or in any combination of bottom bits of
  (a,b,c).
* "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
  the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
  is commonly produced by subtraction) look like a single 1-bit
  difference.
* the base values were pseudorandom, all zero but one bit set, or 
  all zero plus a counter that starts at zero.

These constants passed:
 14 11 25 16 4 14 24
 12 14 25 16 4 14 24
and these came close:
  4  8 15 26 3 22 24
 10  8 15 26 3 22 24
 11  8 15 26 3 22 24
-------------------------------------------------------------------------------
*/
#define final(a,b,c) \
{ \
  c ^= b; c -= rot(b,14); \
  a ^= c; a -= rot(c,11); \
  b ^= a; b -= rot(a,25); \
  c ^= b; c -= rot(b,16); \
  a ^= c; a -= rot(c,4);  \
  b ^= a; b -= rot(a,14); \
  c ^= b; c -= rot(b,24); \
}


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
	ANT_RANDOM_HASH_8()
	-------------------
*/
static inline unsigned int ANT_random_hash_8(ANT_string_pair *string)
{
return ANT_random_hash_8(string->string(), string->length(), (unsigned char)(string->length() & 0xFF));
}

/*
	ANT_RANDOM_HASH_8_24()
	----------------------
	Uses ANT_random_hash_8 on the whole string, excluding the first
	character and excluding the first two characters to generate
	three hashes which are combined.
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
	ANT_RANDOM_HASH_8_32()
	----------------------
*/
static inline unsigned long ANT_random_hash_8_32(ANT_string_pair *string)
{
long hash1, hash2, hash3, hash4;

hash1 = ANT_random_hash_8(string->string(), string->length(), (unsigned char)(string->length() & 0xFF));
hash2 = string->length() <= 1 ? 0 : ANT_random_hash_8(string->string() + 1, string->length() - 1, (unsigned char)((string->length() - 1) & 0xFF));
hash3 = string->length() <= 2 ? 0 : ANT_random_hash_8(string->string() + 2, string->length() - 2, (unsigned char)((string->length() - 2) & 0xFF));
hash4 = string->length() <= 3 ? 0 : ANT_random_hash_8(string->string() + 3, string->length() - 3, (unsigned char)((string->length() - 3) & 0xFF));
return (hash1 << 24) + (hash2 << 16) + (hash3 << 8) + hash4;
}

/*
	ANT_RANDOM_HASH_24()
	--------------------
	Uses the same logic as ANT_random_hash_8, but generates three hashes
	on the 1,4,7... 2,5,8,... 3,6,9,.. characters which are then
	combined togehter for the final hash.
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
	ANT_RANDOM_HASH_24()
	--------------------
*/
static inline uint32_t ANT_random_hash_24(ANT_string_pair *string)
{
return ANT_random_hash_24(string->string(), string->length());
}

/*
	ANT_RANDOM_HASH_32()
	--------------------
*/
static inline uint32_t ANT_random_hash_32(char *string, size_t length)
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
	ANT_RANDOM_HASH_32()
	--------------------
*/
static inline uint32_t ANT_random_hash_32(ANT_string_pair *string)
{
return ANT_random_hash_32(string->string(), string->length());
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
	ANT_HEADER_HASH_24()
	--------------------
*/
static inline unsigned long ANT_header_hash_24(ANT_string_pair *string)
{
/*
	This code assumes a 37 character alphabet (a..z,A-Z,0..9,(~_@-)) and treats the string as a base 37 integer
	and encodes the length in the top 3 bits.  Numbers cause problems with this, especially increasing sequences
	because they end up with the indexer's direct tree chain in the hash table reducing to a linked list!  Numbers
	are now encoded as the sumber itself.

	UNICODE strings now appear to use the random_hash_24 method too.
*/
#ifndef EXPERIMENTAL_HASH
unsigned long ans;
size_t len;
const long base = 37;

#ifdef HEADER_SPECIAL_CASE_NUMBER
if (ANT_isdigit((*string)[0]))
	return ANT_atoul(string->start, string->length()) % 0x1000000;
#endif

#ifdef HEADER_SPECIAL_CASE_UTF
if (((*string)[0] & 0x80) != 0)
	return ANT_random_hash_24(string->string(), string->length());
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
#else
/*
	If the length is bigger than 4, we can't encode the base-37 version directly into the hashtable
	so use random hashing distributed evenly over the remainder of the space. If it is fewer than 4
	characters, then it will fit directly as the base-37 encoded version, so use that.
*/
unsigned long ans = 0;
const long base = 37;

#ifdef HEADER_SPECIAL_CASE_NUMBER
if (ANT_isdigit((*string)[0]))
	return ANT_atoul(string->start, string->length()) % 0x1000000;
#endif

switch (string->length())
	{
	case 4: ans = ans * base + ANT_header_hash_encode[(*string)[3]];
	case 3: ans = ans * base + ANT_header_hash_encode[(*string)[2]];
	case 2: ans = ans * base + ANT_header_hash_encode[(*string)[1]];
	case 1: return ans * base + ANT_header_hash_encode[(*string)[0]];
	default: return base * base * base * base + (ANT_random_hash_24(string) % ((1 << 24) - (base * base * base * base)));
	}

#endif
}

/*
	ANT_HEADER_HASH_32()
	--------------------
*/
static inline unsigned long ANT_header_hash_32(ANT_string_pair *string)
{
/*
	This code assumes a 37 character alphabet (a..z,A-Z,0..9,(~_@-)) and treats the string as a base 37 integer
	Numbers cause problems with this, especially increasing sequences because they end up with the indexer's direct
	tree chain in the hash table reducing to a linked list!  Numbers are now encoded as the number itself.
*/
uint32_t ans;
size_t len;
const long base = 37;

#ifdef HEADER_SPECIAL_CASE_NUMBER
if (ANT_isdigit((*string)[0]))
	return ANT_atoul(string->start, string->length());
#endif

#ifdef HEADER_SPECIAL_CASE_UTF
if (((*string)[0] & 0x80) != 0)
	return ANT_random_hash_32(string);
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

/*
	ANT_SUPERFASTHASH_8()
	---------------------
	Paul Hsieh's SuperFastHash
	www.azillionmonkeys.com/qed/hash.html
*/
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8) + (uint32_t)(((const uint8_t *)(d))[0]))

static inline unsigned long ANT_superfasthash_32(char *string, size_t length)
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

static inline unsigned long ANT_superfasthash_8(ANT_string_pair *string)
{
return ANT_superfasthash_32(string->string(), string->length()) & 0xFF;
}
static inline unsigned long ANT_superfasthash_24(ANT_string_pair *string)
{
return ANT_superfasthash_32(string->string(), string->length()) & 0xFFFFFF;
}
static inline unsigned long ANT_superfasthash_32(ANT_string_pair *string)
{
return ANT_superfasthash_32(string->string(), string->length());
}

static inline uint32_t ANT_lookup3hash_32(const void *key, size_t length)
{
uint32_t a,b,c;                                          /* internal state */
union { const void *ptr; size_t i; } u;     /* needed for Mac Powerbook G4 */

/* Set up the internal state */
a = b = c = 0xdeadbeef + ((uint32_t)length);

u.ptr = key;
if (HASH_LITTLE_ENDIAN && ((u.i & 0x3) == 0))
	{
	const uint32_t *k = (const uint32_t *)key;         /* read 32-bit chunks */
	const uint8_t  *k8;

	/*------ all but last block: aligned reads and affect 32 bits of (a,b,c) */
	while (length > 12)
		{
		a += k[0];
		b += k[1];
		c += k[2];
		mix(a,b,c);
		length -= 12;
		k += 3;
		}

	/*----------------------------- handle the last (probably partial) block */
	/* 
	 * "k[2]&0xffffff" actually reads beyond the end of the string, but
	 * then masks off the part it's not allowed to read.  Because the
	 * string is aligned, the masked-off tail is in the same word as the
	 * rest of the string.  Every machine with memory protection I've seen
	 * does it on word boundaries, so is OK with this.  But VALGRIND will
	 * still catch it and complain.  The masking trick does make the hash
	 * noticably faster for short strings (like English words).
	 */
#ifndef VALGRIND
	switch(length)
		{
		case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
		case 11: c+=k[2]&0xffffff; b+=k[1]; a+=k[0]; break;
		case 10: c+=k[2]&0xffff; b+=k[1]; a+=k[0]; break;
		case 9 : c+=k[2]&0xff; b+=k[1]; a+=k[0]; break;
		case 8 : b+=k[1]; a+=k[0]; break;
		case 7 : b+=k[1]&0xffffff; a+=k[0]; break;
		case 6 : b+=k[1]&0xffff; a+=k[0]; break;
		case 5 : b+=k[1]&0xff; a+=k[0]; break;
		case 4 : a+=k[0]; break;
		case 3 : a+=k[0]&0xffffff; break;
		case 2 : a+=k[0]&0xffff; break;
		case 1 : a+=k[0]&0xff; break;
		case 0 : return c;              /* zero length strings require no mixing */
		}
#else /* make valgrind happy */
	k8 = (const uint8_t *)k;
	switch(length)
		{
		case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
		case 11: c+=((uint32_t)k8[10])<<16;  /* fall through */
		case 10: c+=((uint32_t)k8[9])<<8;    /* fall through */
		case 9 : c+=k8[8];                   /* fall through */
		case 8 : b+=k[1]; a+=k[0]; break;
		case 7 : b+=((uint32_t)k8[6])<<16;   /* fall through */
		case 6 : b+=((uint32_t)k8[5])<<8;    /* fall through */
		case 5 : b+=k8[4];                   /* fall through */
		case 4 : a+=k[0]; break;
		case 3 : a+=((uint32_t)k8[2])<<16;   /* fall through */
		case 2 : a+=((uint32_t)k8[1])<<8;    /* fall through */
		case 1 : a+=k8[0]; break;
		case 0 : return c;
		}
#endif /* !valgrind */
	}
else if (HASH_LITTLE_ENDIAN && ((u.i & 0x1) == 0))
	{
	const uint16_t *k = (const uint16_t *)key;         /* read 16-bit chunks */
	const uint8_t  *k8;

	/*--------------- all but last block: aligned reads and different mixing */
	while (length > 12)
		{
		a += k[0] + (((uint32_t)k[1])<<16);
		b += k[2] + (((uint32_t)k[3])<<16);
		c += k[4] + (((uint32_t)k[5])<<16);
		mix(a,b,c);
		length -= 12;
		k += 6;
		}

	/*----------------------------- handle the last (probably partial) block */
	k8 = (const uint8_t *)k;
	switch(length)
		{
		case 12: c+=k[4]+(((uint32_t)k[5])<<16);
						 b+=k[2]+(((uint32_t)k[3])<<16);
						 a+=k[0]+(((uint32_t)k[1])<<16);
						 break;
		case 11: c+=((uint32_t)k8[10])<<16;     /* fall through */
		case 10: c+=k[4];
						 b+=k[2]+(((uint32_t)k[3])<<16);
						 a+=k[0]+(((uint32_t)k[1])<<16);
						 break;
		case 9 : c+=k8[8];                      /* fall through */
		case 8 : b+=k[2]+(((uint32_t)k[3])<<16);
						 a+=k[0]+(((uint32_t)k[1])<<16);
						 break;
		case 7 : b+=((uint32_t)k8[6])<<16;      /* fall through */
		case 6 : b+=k[2];
						 a+=k[0]+(((uint32_t)k[1])<<16);
						 break;
		case 5 : b+=k8[4];                      /* fall through */
		case 4 : a+=k[0]+(((uint32_t)k[1])<<16);
						 break;
		case 3 : a+=((uint32_t)k8[2])<<16;      /* fall through */
		case 2 : a+=k[0];
						 break;
		case 1 : a+=k8[0];
						 break;
		case 0 : return c;                     /* zero length requires no mixing */
		}
	}
else
	{                        /* need to read the key one byte at a time */
	const uint8_t *k = (const uint8_t *)key;

	/*--------------- all but the last block: affect some 32 bits of (a,b,c) */
	while (length > 12)
		{
		a += k[0];
		a += ((uint32_t)k[1])<<8;
		a += ((uint32_t)k[2])<<16;
		a += ((uint32_t)k[3])<<24;
		b += k[4];
		b += ((uint32_t)k[5])<<8;
		b += ((uint32_t)k[6])<<16;
		b += ((uint32_t)k[7])<<24;
		c += k[8];
		c += ((uint32_t)k[9])<<8;
		c += ((uint32_t)k[10])<<16;
		c += ((uint32_t)k[11])<<24;
		mix(a,b,c);
		length -= 12;
		k += 12;
		}

	/*-------------------------------- last block: affect all 32 bits of (c) */
	switch(length)                   /* all the case statements fall through */
		{
		case 12: c+=((uint32_t)k[11])<<24;
		case 11: c+=((uint32_t)k[10])<<16;
		case 10: c+=((uint32_t)k[9])<<8;
		case 9 : c+=k[8];
		case 8 : b+=((uint32_t)k[7])<<24;
		case 7 : b+=((uint32_t)k[6])<<16;
		case 6 : b+=((uint32_t)k[5])<<8;
		case 5 : b+=k[4];
		case 4 : a+=((uint32_t)k[3])<<24;
		case 3 : a+=((uint32_t)k[2])<<16;
		case 2 : a+=((uint32_t)k[1])<<8;
		case 1 : a+=k[0];
						 break;
		case 0 : return c;
		}
	}

final(a,b,c);
return c;
}

static inline unsigned long ANT_lookup3hash_8(ANT_string_pair *string)
{
return ANT_lookup3hash_32(string->string(), string->length()) & 0xFF;
}
static inline unsigned long ANT_lookup3hash_24(ANT_string_pair *string)
{
return ANT_lookup3hash_32(string->string(), string->length()) & 0xFFFFFF;
}
static inline unsigned long ANT_lookup3hash_32(ANT_string_pair *string)
{
return ANT_lookup3hash_32(string->string(), string->length());
}

/*
	ANT_HASH_8()
	------------
*/
static inline unsigned long ANT_hash_8(ANT_string_pair *string)
{
#ifndef HASHER
	#error "HASHER must be defined so a hash_table function can be chosen"
#elif HASHER == RANDOM_HASHER || HASHER == RANDOM_STEP_HASHER
	return ANT_random_hash_8(string);
#elif HASHER == HEADER_HASHER
	return ANT_header_hash_8(string);
#elif HASHER == SUPERFAST_HASHER
	return ANT_superfasthash_8(string);
#elif HASHER == LOOKUP3_HASHER
	return ANT_lookup3hash_8(string);
#else
	#error "Don't know which hash function to use - aborting"
#endif
}

/*
	ANT_HASH_24()
	-------------
*/
static inline unsigned long ANT_hash_24(ANT_string_pair *string)
{
#ifndef HASHER
	#error "HASHER must be defined so a hash_table function can be chosen"
#elif HASHER == RANDOM_HASHER
	return ANT_random_hash_8_24(string);
#elif HASHER == RANDOM_STEP_HASHER
	return ANT_random_hash_24(string);
#elif HASHER == HEADER_HASHER
	return ANT_header_hash_24(string);
#elif HASHER == SUPERFAST_HASHER
	return ANT_superfasthash_24(string);
#elif HASHER == LOOKUP3_HASHER
	return ANT_lookup3hash_24(string);
#else
	#error "Don't know which hash function to use - aborting"
#endif
}

/*
	ANT_HASH_32()
	-------------
*/
static inline unsigned long ANT_hash_32(ANT_string_pair *string)
{
#ifndef HASHER
	#error "HASHER must be defined so a hash_table function can be chosen"
#elif HASHER == RANDOM_HASHER
	return ANT_random_hash_8_32(string);
#elif HASHER == RANDOM_STEP_HASHER
	return ANT_random_hash_32(string);
#elif HASHER == HEADER_HASHER
	return ANT_header_hash_32(string);
#elif HASHER == SUPERFAST_HASHER
	return ANT_superfasthash_32(string);
#elif HASHER == LOOKUP3_HASHER
	return ANT_lookup3hash_32(string);
#else
	#error "Don't know which hash function to use - aborting"
#endif
}

#endif  /* HASH_TABLE_H_ */
