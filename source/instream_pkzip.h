/*
	INSTREAM_PKZIP.H
	----------------
*/
#ifndef INSTREAM_PKZIP_H_
#define INSTREAM_PKZIP_H_

#include "instream.h"

class ANT_file;
class ANT_memory;
class ANT_directory_iterator;
/*
	class ANT_INSTREAM_PKZIP
	------------------------
*/
class ANT_instream_pkzip : public ANT_instream
{
private:
	char *filename;
	long long bytes_read;						// total bytes read (allowing for multiple files)
	ANT_directory_iterator *source;
	char *current_file;
	long long current_file_length;
	char *current_file_pointer;

public:
	ANT_instream_pkzip(ANT_memory *memory, char *filename);
	virtual ~ANT_instream_pkzip();

	virtual long long read(unsigned char *buffer, long long bytes);
} ;

#endif 
