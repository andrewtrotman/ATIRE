/*
	ARITHMETIC_MODEL_UNIGRAM.H
	--------------------------
*/
#ifndef ARITHMETIC_MODEL_UNIGRAM_H_
#define ARITHMETIC_MODEL_UNIGRAM_H_

#include "arithmetic_model.h"

/*
	class ANT_ARITHMETIC_MODEL_UNIGRAM
	----------------------------------
*/
class ANT_arithmetic_model_unigram : public ANT_arithmetic_model
{
public:
	int adapt;

public:
	ANT_arithmetic_model_unigram(int nsym, const unsigned int *ifreq, int adapt);
	virtual ~ANT_arithmetic_model_unigram();

	virtual void text_render(void);
	virtual void update(int sym);
};


#endif /* ARITHMETIC_MODEL_UNIGRAM_H_ */
