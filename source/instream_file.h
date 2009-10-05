/*
	INSTREAM_FILE.H
	---------------
*/
#ifndef INSTREAM_FILE_H_
#define INSTREAM_FILE_H_

#include "instream.h"

class ANT_file;
class ANT_memory;

/*
	class ANT_INSTREAM_FILE
	-----------------------
*/
class ANT_instream_file : public ANT_instream
{
private:
	ANT_file *file;
	char *filename;
	long long file_length, bytes_read;

public:
	ANT_instream_file(ANT_memory *memory, char *filename);
	virtual ~ANT_instream_file();

	virtual long long read(unsigned char *buffer, long long bytes);
} ;

#endif /* INSTREAM_FILE_H_ */
