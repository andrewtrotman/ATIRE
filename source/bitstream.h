/*
	BITSTREAM.H
	-----------
*/
#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

#include <stdlib.h>

/*
	class ANT_BITSTREAM
	--------------------
*/
class ANT_bitstream
{
protected:
	long total_bits;

	unsigned long *stream;
	long stream_length, stream_pos;

	long bit_pos;
	unsigned long buffer;

	long decomp_bit_pos;

protected:
	inline void push_buffer(void);

public:
	ANT_bitstream();
	virtual ~ANT_bitstream();

	inline void push_zero(void);
	inline void push_one(void);
	inline void push_bits(unsigned long bits, long length);
	long eof(void);

	inline long get_bit(void);
	inline unsigned long get_bits(long bits);

	void text_render(void);
} ;

/*
	ANT_BITSTREAM::PUSH_ZERO()
	--------------------------
*/
inline void ANT_bitstream::push_zero(void)
{
if (++bit_pos >= 32)
	push_buffer();

total_bits++;
}


/*
	ANT_BITSTREAM::PUSH_ONE()
	-------------------------
*/
inline void ANT_bitstream::push_one(void)
{
buffer |= ((unsigned long)1) << bit_pos;

if (++bit_pos >= 32)
	push_buffer();

total_bits++;
}

/*
	ANT_BITSTREAM::PUSH_BITS()
	--------------------------
*/
inline void ANT_bitstream::push_bits(unsigned long bits, long length)
{
while (length-- > 0)
	if (bits & (1 << length))
		push_one();
	else
		push_zero();
}

/*
	ANT_BITSTREAM::PUSH_BUFFER()
	-----------------------------
*/
inline void ANT_bitstream::push_buffer(void)
{
if (stream_pos >= stream_length)
	{
	stream_length = stream_length * 2 + 1;
	stream = (unsigned long *)realloc(stream, sizeof(*stream) * (stream_length));
	}
stream[stream_pos] = buffer;
stream_pos++;
buffer = 0;
bit_pos = 0;
}

/*
	ANT_BITSTREAM::GET_BIT()
	------------------------
*/
inline long ANT_bitstream::get_bit(void)
{
decomp_bit_pos++;
return (stream[decomp_bit_pos >> 5] >> (decomp_bit_pos & 31)) & 0x01;
}

/*
	ANT_BITSTREAM::GET_BITS()
	-------------------------
*/
inline unsigned long ANT_bitstream::get_bits(long bits)
{
unsigned long ans = 0;

while (bits-- > 0)
	ans = (ans << 1) | get_bit();

return ans;
}

#endif __BITSTREAM_H__
