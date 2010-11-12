/*
	BITSTRING.H
	-----------
*/
#ifndef BITSTRING_H_
#define BITSTRING_H_

/*
	Routines to count bits in integer types.
*/
extern long ANT_count_bits_in(unsigned char *ch, long bytes_long);

inline long ANT_count_bits(long long val) { return ANT_count_bits_in((unsigned char *)&val, sizeof(val)); }
inline long ANT_count_bits(long val) { return ANT_count_bits_in((unsigned char *)&val, sizeof(val)); }

/*
	class ANT_BITSTRING
	-------------------
*/
class ANT_bitstring
{
friend class ANT_bitstring_iterator;

protected:
	enum { OR, XOR, AND, AND_NOT };

private:
	unsigned char *bits;
	long bits_long;
	long bytes_long;
	long chunks_long;

protected:
	virtual void operation(int op, ANT_bitstring *a, ANT_bitstring *b, ANT_bitstring *c);		// a = b op c

public:
	ANT_bitstring();
	virtual ~ANT_bitstring();

	void set_length(long len_in_bits);
	long get_length(void) 			{ return bits_long; }
	void unsafe_setbit(long pos) 	{ bits[pos >> 3] |= 1 << (pos & 7); }
	void unsafe_unsetbit(long pos)	{ bits[pos >> 3] &= ~(1 << (pos & 7)); }
	long unsafe_getbit(long pos)	{ return (bits[pos >> 3] >> (pos & 7)) & 0x01; }

	void bit_or(ANT_bitstring *ans, ANT_bitstring *with) 		{ operation(OR, ans, this, with); }
	void bit_xor(ANT_bitstring *ans, ANT_bitstring *with) 		{ operation(XOR, ans, this, with); }
	void bit_and(ANT_bitstring *ans, ANT_bitstring *with) 		{ operation(AND, ans, this, with); }
	void bit_and_not(ANT_bitstring *ans, ANT_bitstring *with) 	{ operation(AND_NOT, ans, this, with); }

	long count_bits(void) { return ANT_count_bits_in(bits, bytes_long); }
	long index(long which);
	void zero(void);
	void one(void);
} ;


#endif /* BITSTRING_H_ */

