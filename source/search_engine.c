/*
	SEARCH_ENGINE.C
	---------------
*/
#include <stdio.h>
#include <stdlib.h>
#include "file.h"

class ANT_search_engine
{
private:
	ANT_memory *memory;
	ANT_file *index;
public:
	ANT_search_engine();
	~ANT_search_engine();
};

/*
	ANT_SEARCH_ENGINE::ANT_SEARCH_ENGINE()
	--------------------------------------
*/
ANT_search_engine::ANT_search_engine(ANT_memory *memory)
{
long long end;

this->memory = memory;
index = new ANT_file;
if (index->open("index.aspt", "rb") == 0)
	exit(printf("Cannot open index file:index.aspt\n"));

end = index->filelength();
index->seek(end - sizeof(long long));
index->read(&term_header, sizeof(term_header);
index->seek(term_header);
block = memory->malloc(end - term_header);
/*
	block should be in the form:
		B_NODE_PREFIX_SIZE '\0' offset
		until we get to the end of the block.
*/
}

/*
	ANT_SEARCH_ENGINE::~ANT_SEARCH_ENGINE()
	---------------------------------------
*/
ANT_search_engine::~ANT_search_engine()
{
index->close();
delete index;
}


