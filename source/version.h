/*
	VERSION.H
	---------
*/
#ifndef VERSION_H_
#define VERSION_H_

#include "fundamental_types.h"

extern long ANT_version;		// version number in BCD
extern char *ANT_version_string;

static const uint32_t ANT_file_signature = 0x54505341UL;	//"ASPT" (Intel Byte Order)
static const uint64_t ANT_file_signature_index = 0x494e444558000000ULL; // "INDEX" (Intel Byte Order)

void ANT_credits(void);

#endif  /* VERSION_H_ */
