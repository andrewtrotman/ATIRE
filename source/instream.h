/*
	INSTREAM.H
	----------
*/
#ifndef INSTREAM_H_
#define INSTREAM_H_

class ANT_memory;

/*
	class ANT_INSTREAM
	------------------
*/
class ANT_instream
{
protected:
	ANT_memory *memory;
	ANT_instream *source;

public:
	ANT_instream(ANT_memory *memory, ANT_instream *source = 0) { this->memory = memory; this->source = source; }
	virtual ~ANT_instream() {}

	virtual long long read(unsigned char *buffer, long long bytes) = 0;
} ;


#endif /* INSTREAM_H_ */
