/*
	PREGEN_FILE_HEADER.H
	--------------------
*/
#ifndef PREGEN_FILE_HEADER_H_
#define PREGEN_FILE_HEADER_H_

#include "fundamental_types.h"

#define PREGEN_FILE_VERSION 2

/*
	class ANT_PREGEN_FILE_HEADER
	----------------------------
*/
class ANT_pregen_file_header
{
public:
	uint32_t version;
	uint32_t doc_count;
	uint32_t pregen_t_size;
	uint32_t field_type;
	uint32_t field_name_length;
};

#endif /* PREGEN_FILE_HEADER_H_ */

