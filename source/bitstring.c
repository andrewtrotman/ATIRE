/*
	BITSTRING.C
	-----------
*/
#include <string.h>
#include <stdlib.h>
#include "bitstring.h"
#include "str.h"
#include "fundamental_types.h"

#define BITS_PER_WORD 64		// 64 bits to a word

typedef uint64_t ANT_op;

/*
	How many bits are set in byte with value equal to index.
*/
static unsigned char bits_set_in[] = {
	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
	} ;

/*
	ANT_COUNT_BITS_IN()
	-------------------
*/
long long ANT_count_bits_in(unsigned char *ch, long long bytes_long)
{
long long total, here;

total = 0;
for (here = 0; here < bytes_long; here++)
	total += bits_set_in[*ch++];

return total;
}

/*
	ANT_BITSTRING::ANT_BITSTRING()
	------------------------------
*/
ANT_bitstring::ANT_bitstring()
{
bits = NULL;
bits_long = bytes_long = chunks_long = 0;
}

/*
	ANT_BITSTRING::~ANT_BITSTRING()
	-------------------------------
*/
ANT_bitstring::~ANT_bitstring()
{
delete [] bits;
}

/*
	ANT_BITSTRING::SET_LENGTH()
	---------------------------
*/
void ANT_bitstring::set_length(long long len_in_bits)
{
long long old_bytes_long, old_chunks_long;

old_bytes_long = bytes_long;
old_chunks_long = chunks_long;

unsafe_set_length(len_in_bits);

if (chunks_long != old_chunks_long)
	{
	bits = strrenew(bits, old_bytes_long, bytes_long);
	if (bytes_long > old_bytes_long)
		memset(bits + old_bytes_long, 0, (size_t)(bytes_long - old_bytes_long));
	}
}

/*
	ANT_BITSTRING::UNSAFE_SET_LENGTH()
	----------------------------------
	This resets the lengh of the bitstring but not the length of the underlying
	buffer that holds the bits.  Its only useful if you're going to resize within
	the bounds that the object was first created with.  It also doesn't zerof the
	new stuff (because there is no new stuff)
*/
void ANT_bitstring::unsafe_set_length(long long len_in_bits)
{
long long new_chunks_long;

new_chunks_long = (len_in_bits - 1) / BITS_PER_WORD + 1;
if (new_chunks_long != chunks_long)
	{
	chunks_long = new_chunks_long;
	bytes_long = chunks_long * (BITS_PER_WORD / 8);
	}

bits_long = len_in_bits;
}

/*
	ANT_BITSTRING::ZERO()
	---------------------
*/
void ANT_bitstring::zero(void)
{
memset(bits, 0, (size_t)bytes_long);
}

/*
	ANT_BITSTRING::ONE()
	--------------------
	Set all bits to 1 (and pad end with zeros)
*/
void ANT_bitstring::one(void)
{
long long clearing, last_bit;
memset(bits, 0xFF, (size_t)bytes_long);

last_bit = bytes_long * 8;
for (clearing = bits_long; clearing < last_bit; clearing++)
	unsafe_unsetbit(clearing);
}

/*
	ANT_BITSTRING::OPERATION()
	--------------------------
*/
void ANT_bitstring::operation(int op, ANT_bitstring *a, ANT_bitstring *b, ANT_bitstring *c)
{
ANT_op *aa, *bb, *cc;
long long longest, here;

longest = b->bits_long > c->bits_long ? b->bits_long : c->bits_long;
a->set_length(longest);
b->set_length(longest);
c->set_length(longest);

aa = (ANT_op *)a->bits;
bb = (ANT_op *)b->bits;
cc = (ANT_op *)c->bits;

if (op == OR)
	for (here = 0; here < chunks_long; here++)
		*aa++ = *bb++ | *cc++;
else if (op == AND)
	for (here = 0; here < chunks_long; here++)
		*aa++ = *bb++ & *cc++;
else if(op == XOR)
	for (here = 0; here < chunks_long; here++)
		*aa++ = *bb++ ^ *cc++;
else if (op == AND_NOT)
	for (here = 0; here < chunks_long; here++)
		*aa++ = *bb++ & ~*cc++;
}

/*
	ANT_BITSTRING::INDEX()
	----------------------
*/
long long ANT_bitstring::index(long long which)
{
long long old_total, total, here, my_bit, bit;
unsigned char *ch;

total = 0;
ch = bits;
for (here = 0; here < bytes_long; here++)
	{
	old_total = total;
	total += bits_set_in[*ch];
	if (total >= which)
		{
		my_bit = which - old_total;
		for (bit = 0; bit < 8; bit++)
			if (*ch &  1 << bit)
				{
				my_bit--;
				if (my_bit == 0)
					return here * 8 + bit;
				}
		}
	ch++;
	}

return -1;		// no such bit set.
}

