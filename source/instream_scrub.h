/*
	INSTREAM_SCRUB.H
	-----------------
*/
#ifndef INSTREAM_SCRUB_H_
#define INSTREAM_SCRUB_H_

#include "instream.h"

/*
	class ANT_INSTREAM_SCRUB
	------------------------
*/
class ANT_instream_scrub : public ANT_instream
{
private:
	long long scrubbing;
	ANT_instream *source;

public:
	ANT_instream_scrub(ANT_memory *memory, ANT_instream *source, long long scrubbing);
	virtual ~ANT_instream_scrub() {};

	virtual long long read(unsigned char *data, long long size);

} ;

#endif /* INSTREAM_SCRUB_H_ */
