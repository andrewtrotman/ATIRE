/*
	HASH_TABLE.H
	------------
*/
#ifndef HASH_TABLE_H_
#define HASH_TABLE_H_

#include "string_pair.h"
#include "fundamental_types.h"

#ifndef HASHER
	#error "HASHER must be defined so a hash_table function can be chosen"
#elif HASHER == RANDOM || HASHER == RANDOM_STEP
	#include "hash_random.h"
#elif HASHER == HEADER || HASHER == HEADER_NUM
	#include "hash_header.h"
#elif HASHER == HEADER_EXP
	#include "hash_header_experimental.h"
#elif HASHER == HEADER_COLLAPSE
	#include "hash_header_collapse.h"
#elif HASHER == SUPERFAST
	#include "hash_superfast.h"
#elif HASHER == LOOKUP3
	#include "hash_lookup3.h"
#elif HASHER == MATT || HASHER == MATT_N
	#include "hash_matt.h"
#else
	#error "Don't know which hash function to use - aborting"
#endif

/*
	ANT_HASH_8()
	------------
*/
static inline unsigned long ANT_hash_8(ANT_string_pair *string)
{
#if HASHER == RANDOM
	return ANT_hash_random_8(string);
#elif HASHER == RANDOM_STEP
	return ANT_hash_random_8(string);
#elif HASHER == HEADER || HASHER == HEADER_NUM
	return ANT_hash_header_8(string);
#elif HASHER == HEADER_EXP
	return ANT_hash_header_experimental_8(string);
#elif HASHER == SUPERFAST
	return ANT_hash_superfast_8(string);
#elif HASHER == LOOKUP3
	return ANT_hash_lookup3_8(string);
#elif HASHER == MATT || HASHER == MATT_N
	return ANT_hash_matt_8(string);
#else
	return -1;
#endif
}

#if 0
/*
	ANT_HASH_16()
	-------------
*/
static inline unsigned long ANT_hash_16(ANT_string_pair *string)
{
#if HASHER == RANDOM
	return ANT_hash_random_16(string);
#elif HASHER == RANDOM_STEP
	return ANT_hash_random_16(string);
#elif HASHER == HEADER || HASHER == HEADER_NUM
	return ANT_hash_header_16(string);
#elif HASHER == HEADER_EXP
	return ANT_hash_header_experimental_16(string);
#elif HASHER == SUPERFAST
	return ANT_hash_superfast_16(string);
#elif HASHER == LOOKUP3
	return ANT_hash_lookup3_16(string);
#elif HASHER == MATT || HASHER == MATT_N
	return ANT_hash_matt_16(string);
#else
	return -1;
#endif
}
#endif

/*
	ANT_HASH_24()
	-------------
*/
static inline unsigned long ANT_hash_24(ANT_string_pair *string)
{
#if HASHER == RANDOM
	return ANT_hash_random_24(string);
#elif HASHER == RANDOM_STEP
	return ANT_hash_random_step_24(string);
#elif HASHER == HEADER || HASHER == HEADER_NUM
	return ANT_hash_header_24(string);
#elif HASHER == HEADER_EXP
	return ANT_hash_header_experimental_24(string);
#elif HASHER == HEADER_COLLAPSE
	return ANT_hash_header_collapse_24(string);
#elif HASHER == SUPERFAST
	return ANT_hash_superfast_24(string);
#elif HASHER == LOOKUP3
	return ANT_hash_lookup3_24(string);
#elif HASHER == MATT || HASHER == MATT_N
	return ANT_hash_matt_24(string);
#else
	return -1;
#endif
}

/*
	ANT_HASH_32()
	-------------
*/
static inline unsigned long ANT_hash_32(ANT_string_pair *string)
{
#if HASHER == RANDOM
	return ANT_hash_random_32(string);
#elif HASHER == RANDOM_STEP
	return ANT_hash_random_step_32(string);
#elif HASHER == HEADER || HASHER == HEADER_NUM
	return ANT_hash_header_32(string);
#elif HASHER == HEADER_EXP
	return ANT_hash_header_experimental_32(string);
#elif HASHER == SUPERFAST
	return ANT_hash_superfast_32(string);
#elif HASHER == LOOKUP3
	return ANT_hash_lookup3_32(string);
#elif HASHER == MATT || HASHER == MATT_N
	return ANT_hash_matt_32(string);
#else
	return -1;
#endif
}

#endif  /* HASH_TABLE_H_ */
