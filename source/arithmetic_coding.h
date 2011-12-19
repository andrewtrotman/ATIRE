/*
	ARITHMETIC_CODING.H
	-------------------
	Derived from http://www.cipr.rpi.edu/~wheeler/ac/. Modified to encode/decode to a fixed-sized
	integer instead of a file, and translate to C++
*/
#ifndef ARITHMETIC_CODING_H_
#define ARITHMETIC_CODING_H_

/*
	class ANT_ARITHMETIC_CODING
	---------------------------
*/
class ANT_arithmetic_coding
{
protected:
	static const unsigned int Code_value_bits = 16;
	static const unsigned long Top_value = (((long)1<<Code_value_bits)-1);
	static const unsigned long First_qtr = (Top_value/4+1);
	static const unsigned long Half = (2*First_qtr);
	static const unsigned long Third_qtr = (3*First_qtr);
};

#endif /* ARITHMETIC_CODING_H_ */
