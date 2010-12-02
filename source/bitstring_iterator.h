
/*
	BITSTRING_ITERATOR.H
	--------------------
*/
#ifndef BITSTRING_ITERATOR_H_
#define BITSTRING_ITERATOR_H_

class ANT_bitstring;

/*
	class ANT_BITSTRING_ITERATOR
	----------------------------
*/
class ANT_bitstring_iterator
{
private:
	ANT_bitstring *bitstring;
	long byte_pos, bit_pos;

public:
	ANT_bitstring_iterator(ANT_bitstring *string);
	~ANT_bitstring_iterator();
	long first(void) { return next(); }
	long next(void);
} ;



#endif /* BITSTRING_ITERATOR_H_ */
