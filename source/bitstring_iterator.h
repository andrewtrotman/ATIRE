/*
	BITSTRING_ITERATOR.H
	--------------------
*/

#ifndef __BITSTRING_ITERATOR_H__
#define __BITSTRING_ITERATOR_H__

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
	long next(void);
} ;


#endif __BITSTRING_ITERATOR_H__
