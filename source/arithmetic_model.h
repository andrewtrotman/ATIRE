/*
	ARITHMETIC_MODEL.H
	------------------
*/
#ifndef ARITHMETIC_MODEL_H_
#define ARITHMETIC_MODEL_H_

/*
	class ANT_ARITHMETIC_MODEL
	--------------------------
*/
class ANT_arithmetic_model
{
public:
	int nsym;

	unsigned int *freq;
	unsigned int *cfreq;

public:
	virtual ~ANT_arithmetic_model() {}

	virtual void clear_context(void) {}

	virtual void update(int sym) = 0;
	virtual void text_render(void) = 0;
};

#endif /* ARITHMETIC_MODEL_H_ */
