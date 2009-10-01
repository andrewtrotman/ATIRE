/*
	BITSTREAM.H
	-----------
*/
#ifndef BITSTREAM_H_
#define BITSTREAM_H_

#include "fundamental_types.h"

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE (!FALSE)
#endif

/*
	class ANT_BITSTREAM
	--------------------
*/
class ANT_bitstream
{
protected:
	long long total_bits;

	uint32_t *stream;
	long long stream_length, stream_pos;

	long bit_pos;
	uint32_t buffer;

	long failed;

protected:
	inline void push_buffer(void);

public:
	ANT_bitstream();

	inline void push_zero(void);
	inline void push_zeros(long long length);
	inline void push_one(void);
	inline void push_bits(unsigned long long bits, long length);		// push up-to 64 bits
	long long eof(void);								// clean up and return the length in bytes
	void rewind(unsigned char *destination = 0, long long destination_length = 0);		// destination_length is in units of bytes

	inline long get_bit(void);
	inline unsigned long long get_bits(long bits);					// get up-to 64 bits
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
inline void ANT_bitstream::push_bits(unsigned long long bits, long length)
{
while (length-- > 0)
	if (bits & ((unsigned long long)1 << length))
		push_one();
	else
		push_zero();
}

/*
	VOID ANT_BITSTREAM::PUSH_ZEROS()
	--------------------------------
*/
inline void ANT_bitstream::push_zeros(long long length)
{
while (length-- > 0)
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
	failed = TRUE;
	return;
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
long ans;

ans = (stream[bit_pos >> 5] >> (bit_pos & 31)) & 0x01;
bit_pos++;

return ans;
}

/*
	ANT_BITSTREAM::GET_BITS()
	-------------------------
*/
inline unsigned long long ANT_bitstream::get_bits(long bits)
{
uint64_t ans = 0;

while (bits-- > 0)
	ans = (ans << 1) | get_bit();

return ans;
}

#endif  /* BITSTREAM_H_ */
