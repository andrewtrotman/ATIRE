/*
	CHANNEL_FILE.H
	--------------
*/
#ifndef CHANNEL_FILE_H_
#define CHANNEL_FILE_H_

#include <stdio.h>
#include "channel.h"

class ANT_instream;
class ANT_memory;

/*
	class ANT_CHANNEL_FILE
	----------------------
*/
class ANT_channel_file : public ANT_channel
{
private:
	char *filename;
	FILE *outfile;
	ANT_instream *infile;
	ANT_memory *memory;
	long eof;

protected:
	virtual long long block_write(char *source, long long length);
	virtual char *block_read(char *into, long long length);
	virtual char *getsz(char terminator = '\0');

public:
	ANT_channel_file(char *filename = NULL);
	virtual ~ANT_channel_file();
} ;

#endif /* CHANNEL_FILE_H_ */
