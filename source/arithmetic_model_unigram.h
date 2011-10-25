#ifndef ARITHMETIC_MODEL_UNIGRAM_H_
#define ARITHMETIC_MODEL_UNIGRAM_H_

#include "arithmetic_model.h"

class ANT_arithmetic_model_unigram : public ANT_arithmetic_model
{
public:
	int adapt;

	virtual void print();
	virtual void update(int sym);

	ANT_arithmetic_model_unigram(int nsym, unsigned int *ifreq, int adapt);
	virtual ~ANT_arithmetic_model_unigram();
};

#endif
