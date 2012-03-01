/*
	IMPACT_HEADER.H
	---------------
*/
#ifndef IMPACT_HEADER_H_
#define IMPACT_HEADER_H_

#include "compress.h"
#include "fundamental_types.h"

/*
	class ANT_IMPACT_HEADER
	-----------------------
*/
class ANT_impact_header
{
public:
	static const uint32_t NUM_OF_QUANTUMS = 256;
	static const long long INFO_SIZE = 2 * sizeof(uint64_t) + 2 * sizeof(uint32_t);

public:
	long long header_size;
	ANT_compressable_integer *header_buffer;
	uint64_t postings_chain;
	uint64_t chain_length;
	uint32_t the_quantum_count;
	uint32_t beginning_of_the_postings;
	ANT_compressable_integer *impact_value_start, *impact_value_ptr;
	ANT_compressable_integer *doc_count_start, *doc_count_ptr, *doc_count_trim_ptr;
	ANT_compressable_integer *impact_offset_start, *impact_offset_ptr;
	long long sum;
};

#endif  /* IMPACT_HEADER_H_ */

