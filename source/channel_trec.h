/*
	CHANNEL_TREC.H
	--------------
*/
#ifndef CHANNEL_TREC_H_
#define CHANNEL_TREC_H_

#include "stop_word.h"
#include "channel.h"

/*
	class ANT_CHANNEL_TREC
	----------------------
*/
class ANT_channel_trec : public ANT_channel
{
private:
	ANT_channel *in_channel;
	ANT_stop_word stopper;
	char *buffer;
	long read;
	char *tag;

protected:
	virtual long long block_write(char *source, long long length);
	virtual char *block_read(char *into, long long length);
	virtual char *getsz(char terminator = '\0');

public:
	ANT_channel_trec(ANT_channel *in, char *taglist);
	virtual ~ANT_channel_trec();
} ;

#endif
