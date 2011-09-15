/* Derived from http://www.cipr.rpi.edu/~wheeler/ac/. Modified to encode/decode to a fixed-sized
 * integer instead of a file, and translate to C++ */

#ifndef AC_HEADER
#define AC_HEADER

#include <stdio.h>
#include "fundamental_types.h"
#include <limits>
#include <limits.h>
#include <cassert>

#undef max
#undef min

class ANT_arithmetic_coding
{
protected:
	static const unsigned int Code_value_bits = 16;
	static const unsigned long Top_value = (((long)1<<Code_value_bits)-1);
	static const unsigned long First_qtr = (Top_value/4+1);
	static const unsigned long Half = (2*First_qtr);
	static const unsigned long Third_qtr = (3*First_qtr);
	static const unsigned long Max_frequency = 16383;
};

class ANT_arithmetic_model : ANT_arithmetic_coding
{
public:
	int nsym;
	int *freq;
	int *cfreq;
	int adapt;

	void update(int sym)
	{
	int i;

	if (cfreq[0] == Max_frequency)
		{
		int cum = 0;
		cfreq[nsym] = 0;
		for (i = nsym - 1; i >= 0; i--)
			{
			freq[i] = (freq[i] + 1) / 2;
			cum += freq[i];
			cfreq[i] = cum;
			}
		}

	freq[sym] += 1;
	for (i = sym; i >= 0; i--)
		cfreq[i] += 1;
	}

	ANT_arithmetic_model(int nsym, int *ifreq, int adapt)
	{
	int i;

	this->nsym = nsym;
	this->adapt = adapt;

	freq = (int *) calloc(nsym, sizeof(int));
	cfreq = (int *) calloc(nsym + 1, sizeof(int));
	assert(freq && cfreq);

	if (ifreq)
		{
		cfreq[nsym] = 0;
		for (i = nsym - 1; i >= 0; i--)
			{
			freq[i] = ifreq[i];
			cfreq[i] = cfreq[i + 1] + freq[i];
			}
		if (cfreq[0] > Max_frequency)
			{
			fprintf(stderr, "arithmetic coder model max frequency exceeded");
			exit(-1);
			}
		}
	else
		{
		for (i = 0; i < nsym; i++)
			{
			freq[i] = 1;
			cfreq[i] = nsym - i;
			}
		cfreq[nsym] = 0;
		}
	}

	~ANT_arithmetic_model()
	{
	free(freq);
	free(cfreq);
	}
};

template<typename T> class ANT_arithmetic_decoder : ANT_arithmetic_coding
{
private:
	T buffer; //Input buffer
	unsigned long value;
	int bits_to_go;
	int garbage_bits;
	unsigned long low, high;
	ANT_arithmetic_model *acm;

	int input_bit()
	{
	int t;

	if (bits_to_go == 0)
		{
		garbage_bits++;
		//assert(garbage_bits <= Code_value_bits - 2);

		return 0;
		}

	t = (buffer & ((T) 1 << (sizeof(buffer) * CHAR_BIT - 1))) != 0 ? 1 : 0;
	buffer <<= 1;
	bits_to_go--;

	return t;
	}

public:
	ANT_arithmetic_decoder(ANT_arithmetic_model *acm, T buffer)
	{
	this->buffer = std::numeric_limits<T>::max() - buffer;
	this->acm = acm;

	bits_to_go = sizeof(buffer) * CHAR_BIT;
	garbage_bits = 0;

	value = 0;
	for (int i = 1; i <= Code_value_bits; i++)
		value = (value << 1) | input_bit();

	low = 0;
	high = Top_value;

	}

	int decode_symbol()
	{
	unsigned long range;
	unsigned long cum;
	long sym;

	range = (long) (high - low) + 1;
	cum = (((long) (value - low) + 1) * acm->cfreq[0] - 1) / range;

	for (sym = 0; acm->cfreq[sym + 1] > cum; sym++)
		/* do nothing */;

	assert(!(sym<0||sym>=acm->nsym));

	high = low + (range * acm->cfreq[sym]) / acm->cfreq[0] - 1;
	low = low + (range * acm->cfreq[sym + 1]) / acm->cfreq[0];

	for (;;)
		{
		if (high < Half)
			{
			/* do nothing */
			}
		else if (low >= Half)
			{
			value -= Half;
			low -= Half;
			high -= Half;
			}
		else if (low >= First_qtr && high < Third_qtr)
			{
			value -= First_qtr;
			low -= First_qtr;
			high -= First_qtr;
			}
		else break;

		low = low * 2;
		high = high * 2 + 1;
		value = (value << 1) | input_bit();
		}

	if (acm->adapt)
		acm->update(sym);

	return sym;
	}

	void done();
};

template<typename T> class ANT_arithmetic_encoder : ANT_arithmetic_coding
{
private:
	long fbits;
	T buffer;
	int bits_to_go;
	long low, high;
	ANT_arithmetic_model *acm;

	void output_bit(int bit)
	{
	if (bits_to_go > 0)
		{
		buffer <<= 1;
		if (bit)
			buffer |= 0x01;
		bits_to_go--;
		}
	}

	void bit_plus_follow(int bit)
	{
	output_bit(bit);
	while (fbits > 0)
		{
		output_bit(!bit);
		fbits--;
		}
	}

public:
	ANT_arithmetic_encoder(ANT_arithmetic_model *acm)
	{
	this->acm = acm;

	bits_to_go = sizeof(buffer) * CHAR_BIT;

	low = 0;
	high = Top_value;
	fbits = 0;
	buffer = 0;
	}

	T done()
	{
	while (bits_to_go)
		encode_symbol(0);

	fbits++;
	if (low < First_qtr)
		bit_plus_follow(0);
	else
		bit_plus_follow(1);

	return std::numeric_limits<T>::max() - buffer;
	}

	/*
	 * Encode the given symbol, returning non-zero if there is room to encode more symbols
	 * afterwards.
	 */
	int encode_symbol(int sym)
	{
	long range;

	assert(!(sym<0||sym>=acm->nsym));

	range = (long) (high - low) + 1;
	high = low + (range * acm->cfreq[sym]) / acm->cfreq[0] - 1;
	low = low + (range * acm->cfreq[sym + 1]) / acm->cfreq[0];

	for (;;)
		{
		if (high < Half)
			bit_plus_follow(0);
		else if (low >= Half)
			{
			bit_plus_follow(1);
			low -= Half;
			high -= Half;
			}
		else if (low >= First_qtr && high < Third_qtr)
			{
			fbits++;
			low -= First_qtr;
			high -= First_qtr;
			}
		else break;

		low = low * 2;
		high = high * 2 + 1;
		}

	if (acm->adapt)
		acm->update(sym);

	return bits_to_go > 0;
	}
};

#endif
