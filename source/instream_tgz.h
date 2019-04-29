/*
	INSTREAM_TGZ.H
	--------------
*/
#ifndef INSTREAM_TGZ_H_
#define INSTREAM_TGZ_H_

#include "instream.h"

class ANT_file;
class ANT_memory;
class ANT_directory_iterator;

/*
	class ANT_INSTREAM_TGZ
	----------------------
*/
class ANT_instream_tgz : public ANT_instream
{
private:
	char *filename;
	long long bytes_read;						// total bytes read (allowing for multiple files)
	ANT_directory_iterator *source;
	char *current_file;
	long long current_file_length;
	char *current_file_pointer;
	ANT_instream *deflater;

public:
	ANT_instream_tgz(ANT_memory *memory, char *filename);
	virtual ~ANT_instream_tgz();

	virtual long long read(unsigned char *buffer, long long bytes);
} ;

#endif 
