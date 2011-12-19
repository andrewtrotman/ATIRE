/*
	ARITHMETIC_CODING_DECODER.H
	---------------------------
*/
#ifndef ARITHMETIC_CODING_DECODER_H_
#define ARITHMETIC_CODING_DECODER_H_

#include <stdio.h>
#include <limits>
#include <limits.h>
#include <cassert>

#undef max
#include "arithmetic_model.h"

/*
	class ANT_ARITHMETIC_DECODER
	----------------------------
*/
template<typename T> 
class ANT_arithmetic_decoder : ANT_arithmetic_coding
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

	acm->clear_context();
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

	acm->update(sym);

	return sym;
	}

	void done();
};


#endif /* ARITHMETIC_CODING_DECODER_H_ */
