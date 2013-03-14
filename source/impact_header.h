/*
	IMPACT_HEADER.H
	---------------
*/
#ifndef IMPACT_HEADER_H_
#define IMPACT_HEADER_H_

#include "compress.h"
#include "fundamental_types.h"

typedef uint64_t postings_chain_type;
typedef uint64_t chain_length_type;
typedef uint32_t quantum_count_type;
//typedef  uint16_t chain_length_type;
//typedef uint8_t quantum_count_type;
typedef uint32_t beginning_of_the_postings_type;

/*
	class ANT_IMPACT_HEADER
	-----------------------
*/
class ANT_impact_header
{
public:
	static const uint32_t NUM_OF_QUANTUMS = 1 << 16;
	static const long long INFO_SIZE = sizeof(postings_chain_type) + sizeof(chain_length_type)
													+ sizeof(quantum_count_type) + sizeof(beginning_of_the_postings_type);

	static const unsigned char pos_postings_chain = 0;
	static const unsigned char pos_chain_length = sizeof(postings_chain_type);
	static const unsigned char pos_quantum_count = sizeof(postings_chain_type) + sizeof(chain_length_type);
	static const unsigned char pos_beginning_of_the_postings = sizeof(postings_chain_type) + sizeof(chain_length_type) + sizeof(quantum_count_type);

public:
	long long header_size;
	ANT_compressable_integer *header_buffer;
	postings_chain_type postings_chain;
	chain_length_type chain_length;
	quantum_count_type the_quantum_count;
	beginning_of_the_postings_type beginning_of_the_postings;
	ANT_compressable_integer *impact_value_start, *impact_value_ptr;
	ANT_compressable_integer *doc_count_start, *doc_count_ptr, *doc_count_trim_ptr;
	ANT_compressable_integer *impact_offset_start, *impact_offset_ptr;
	long long sum;

	static inline void set_postings_chain(unsigned char *buffer, postings_chain_type chain_value)
	{
	*(postings_chain_type *)(buffer+pos_postings_chain) = chain_value;
	}

	static inline void set_chain_length(unsigned char *buffer, chain_length_type length_value)
	{
	*(chain_length_type *)(buffer+pos_chain_length) = length_value;
	}

	static inline void set_quantum_count(unsigned char *buffer, quantum_count_type count_value)
	{
	*(quantum_count_type *)(buffer+pos_quantum_count) = count_value;
	}

	static inline void set_beginning_of_the_postings(unsigned char *buffer, beginning_of_the_postings_type beginning_value)
	{
	*(beginning_of_the_postings_type *)(buffer+pos_beginning_of_the_postings) = beginning_value;
	}

	inline void set_INFO(unsigned char *buffer)
	{
	*(postings_chain_type *)(buffer+pos_postings_chain) = this->postings_chain;
	*(chain_length_type *)(buffer+pos_chain_length) = this->chain_length;
	*(quantum_count_type *)(buffer+pos_quantum_count) = this->the_quantum_count;
	*(beginning_of_the_postings_type *)(buffer+pos_beginning_of_the_postings) = this->beginning_of_the_postings;
	}

	static inline postings_chain_type get_postings_chain(unsigned char *buffer)
	{
	return  *(postings_chain_type *)(buffer+pos_postings_chain);
	}

	static inline chain_length_type get_chain_length(unsigned char *buffer)
	{
	return *(chain_length_type *)(buffer+pos_chain_length);
	}

	static inline quantum_count_type get_quantum_count(unsigned char *buffer)
	{
	return *(quantum_count_type *)(buffer+pos_quantum_count);
	}

	static inline beginning_of_the_postings_type get_beginning_of_the_postings(unsigned char *buffer)
	{
	return *(beginning_of_the_postings_type *)(buffer+pos_beginning_of_the_postings);
	}
};

#endif  /* IMPACT_HEADER_H_ */

