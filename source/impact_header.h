/*
	IMPACT_HEADER.H
	--------------------------
*/
#ifndef IMPACT_HEADER_H_
#define IMPACT_HEADER_H_

#include "compress.h"
#include <stdint.h>

/*
	class ANT_IMPACT_HEADER
	----------------------------------
*/
class ANT_impact_header
{
public:
	static const uint32_t NUM_OF_QUANTUMS = 256;
	long long info_size, header_size;
	ANT_compressable_integer *header_buffer;
	uint64_t postings_chain;
	uint32_t the_quantum_count;
	uint32_t beginning_of_the_postings;
	ANT_compressable_integer *impact_value_start, *impact_value_ptr;
	ANT_compressable_integer *doc_count_start, *doc_count_ptr, *doc_count_trim_ptr;
	ANT_compressable_integer *impact_offset_start, *impact_offset_ptr;
};

#endif  /* IMPACT_HEADER_H_ */

