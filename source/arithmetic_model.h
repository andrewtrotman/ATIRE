#ifndef ARITHMETIC_MODEL_H_
#define ARITHMETIC_MODEL_H_

class ANT_arithmetic_model
{
public:
	int nsym;

	unsigned int *freq;
	unsigned int *cfreq;

	virtual void clear_context() {};

	virtual void update(int sym) = 0;
	virtual void print() = 0;

	virtual ~ANT_arithmetic_model() {};

};

#endif
