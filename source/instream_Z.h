/*
	INSTREAM_Z.H
	------------
*/
#ifndef INSTREAM_Z_H_
#define INSTREAM_Z_H_

#include "instream.h"

class ANT_file;
class ANT_memory;

/*
	class ANT_INSTREAM_Z
	--------------------
*/
class ANT_instream_Z : public ANT_instream
{
private:
	char *filename;
	unsigned char *binary;
	unsigned char *text;

	size_t bytes_read;
	size_t file_length;

public:
	ANT_instream_Z(ANT_memory *memory, char *filename);
	virtual ~ANT_instream_Z();

	virtual long long read(unsigned char *buffer, long long bytes);
} ;

#endif

