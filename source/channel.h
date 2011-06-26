/*
	CHANNEL.H
	---------
*/
#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <stdio.h>
#include <string.h>

/*
	class ANT_CHANNEL
	-----------------
*/
class ANT_channel
{
protected:
	virtual long block_write(char *source, long length) = 0;		// returns bytes written
	virtual char *block_read(char *into, long length) = 0;
	virtual char *getsz(char terminator = '\0') = 0;

public:
	ANT_channel() {}
	virtual ~ANT_channel() {}

	long write(char *source, long length) 		{ return block_write(source, length); }
	long write(char *source) 					{ return block_write(source, strlen(source)); }

	long write(short source) 					{ return write((long long)source); }
	long write(int source) 						{ return write((long long)source); }
	long write(long source) 					{ return write((long long)source); }
	long write(long long source) 				{ char buffer[32]; sprintf(buffer, "%lld", source); return write(buffer); }

	long write(unsigned short source) 			{ return write((unsigned long long)source); }
	long write(unsigned int source) 			{ return write((unsigned long long)source); }
	long write(unsigned long source) 			{ return write((unsigned long long)source); }
	long write(unsigned long long source) 		{ char buffer[32]; sprintf(buffer, "%ulld", source); return write(buffer); }

	char *read(char *destination, long length) 	{ return block_read(destination, length); }
	char *gets(void) 							{ return getsz('\n'); }
	long puts(char *string) 					{ return block_write(string, strlen(string)) < 0 ? 0 : block_write("\n", 1); }
} ;

#endif /* CHANNEL_H_ */
