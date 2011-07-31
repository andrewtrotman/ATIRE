/*
	NUMBERS.H
	---------
*/
#ifndef NUMBERS_H_
#define NUMBERS_H_

#include "string_pair.h"

extern ANT_string_pair_constant ANT_itosp_table[];
extern long ANT_atosp_table_max;

/*
	ANT_ATOSP()
	-----------
*/
inline ANT_string_pair *ANT_atosp(ANT_string_pair *destination, long value)
{
if (value < ANT_atosp_table_max)
	{
	*destination = ANT_itosp_table[value];
	return destination;
	}
else
	return NULL;
}

#endif /* NUMBERS_H_ */
