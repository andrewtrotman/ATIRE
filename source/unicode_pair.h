/*
	UNICODE_PAIR.H
	--------------
*/

#ifndef UNICODE_PAIR_H_
#define UNICODE_PAIR_H_

/*
	class ANT_UNICODE_PAIR
	----------------------
*/
class ANT_UNICODE_pair
{
public:
	long source, target;		// for example source == character, target == uppercase
} ;

enum ANT_UNICODE_chartype {
		CT_LETTER, CT_NUMBER, CT_PUNCTUATION, CT_SEPARATOR, CT_OTHER, CT_MARK
};

struct ANT_UNICODE_decomposition
{
	long source;
	char const * target;
};

#endif
