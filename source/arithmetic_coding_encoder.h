/*
	ARITHMETIC_CODING_ENCODER.H
	---------------------------
*/
#include "arithmetic_coding.h"

#include <stdio.h>
#include <limits>
#include <limits.h>
#include <cassert>

#undef max

#include "arithmetic_model.h"

/*
	class ANT_ARITHMETIC_CODING_ENCODER
	-----------------------------------
*/
template<typename T> 
class ANT_arithmetic_coding_encoder : ANT_arithmetic_coding
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
	ANT_arithmetic_coding_encoder(ANT_arithmetic_model *acm)
	{
	this->acm = acm;

	acm->clear_context();

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

	acm->update(sym);

	return bits_to_go > 0;
	}
};

