/*
	CHANNEL.H
	---------
*/
#ifndef CHANNEL_H_
#define CHANNEL_H_

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

	long write(char *source, long length) { return block_write(source, length); }
	char *read(char *destination, long length) { return block_read(destination, length); }
	char *gets(void) { return getsz('\n'); }
	long puts(char *string)
		{ 
		if (block_write(string, strlen(string)) >= 0)
			return block_write("\n", 1);
		else
			return 0;
		}
} ;

#endif /* CHANNEL_H_ */
