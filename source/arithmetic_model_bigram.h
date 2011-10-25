/* Derived from http://www.cipr.rpi.edu/~wheeler/ac/. Modified to encode/decode to a fixed-sized
 * integer instead of a file, and translate to C++ */

#ifndef ARITHMETIC_MODEL_BIGRAM_H_
#define ARITHMETIC_MODEL_BIGRAM_H_

#include "arithmetic_model.h"

class ANT_arithmetic_model_bigram : public ANT_arithmetic_model
{
private:
	unsigned int prev_symbol;

	void set_context(int symplusone);
	void update_context(int sym);

public:
	unsigned int *bigram_freq;
	unsigned int *bigram_cfreq;

	int adapt;

	virtual void print();
	virtual void update(int sym);
	virtual void clear_context();

	ANT_arithmetic_model_bigram(int nsym, unsigned int *ifreq, int adapt);
	virtual ~ANT_arithmetic_model_bigram();
};

#endif
