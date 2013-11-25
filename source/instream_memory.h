/*
	INSTREAM_MEMORY.H
	-----------------
*/
#ifndef INSTREAM_MEMORY_H_
#define INSTREAM_MEMORY_H_

#include <stdio.h>
#include "instream.h"

/*
	class ANT_INSTREAM_MEMORY
	-------------------------
*/
class ANT_instream_memory : public ANT_instream
{
private:
	long long bytes_read;
	char *file;
	long long file_length;

public:
	ANT_instream_memory(char *source, long long length);
	virtual ~ANT_instream_memory() {}

	virtual long long read(unsigned char *buffer, long long bytes);
} ;

#endif /* INSTREAM_MEMORY_H_ */
