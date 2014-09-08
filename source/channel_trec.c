/*
	CHANNEL_TREC.C
	--------------
	This channel reads from another channel assuming that that channel
	is a TREC topic file.  This allows the search engine to directly
	read TREC topic files without pre-processing.
	
	The out channel is what-ever is passed to the constructor of this object
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sstream>
#include "pragma.h"
#include "str.h"
#include "stop_word.h"
#include "ctypes.h"
#include "channel_trec.h"

using namespace std;

/*
	ANT_CHANNEL_TREC::ANT_CHANNEL_TREC()
	------------------------------------
	taglist is a combination of 't','d','n' (title, desc, narr) as a '\0' terminated string.  Its a set
*/
ANT_channel_trec::ANT_channel_trec(ANT_channel *in, char *taglist)
{
tag = strnew(taglist);
in_channel = in;

read = true;
number = -1;
at_eof = false;
}

/*
	ANT_CHANNEL_TREC::~ANT_CHANNEL_TREC()
	------------------------------------
*/
ANT_channel_trec::~ANT_channel_trec()
{
delete [] tag;
delete in_channel;
}

/*
	ANT_CHANNEL_TREC::BLOCK_READ()
	------------------------------
*/
char *ANT_channel_trec::block_read(char *into, long long length)
{
exit(printf("ANT_channel_trec::block_read not implemented (class only supports gets())"));
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ANT_CHANNEL_TREC::BLOCK_WRITE()
	-------------------------------
*/

long long ANT_channel_trec::block_write(char *source, long long length)
{
exit(printf("ANT_channel_trec::block_write not implemented (class only supports gets())"));
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ANT_CHANNEL_TREC::CLEAN()
	-------------------------
*/
char *ANT_channel_trec::clean(long number, ostringstream &raw_query)
{
char *from, *ch;
ostringstream stopped_query;

raw_query << ends;
from = strnew(strlower((char *)raw_query.str().c_str()));
for (ch = from; *ch != '\0'; ch++)
	{
	if (!ANT_isalnum(*ch))
		*ch = ' ';
	else
		*ch = ANT_tolower(*ch);
	}

strip_space_inplace(from);

stopped_query << number << ' ' << from << ends;
delete [] from;

return strnew((char *)stopped_query.str().c_str());
}

/*
	ANT_CHANNEL_TREC::GETSZ()
	-------------------------
*/
char *ANT_channel_trec::getsz(char terminator)
{
long match, old_number;
ostringstream raw_query;

if (at_eof)
	return NULL;

for (;;)
	{
	if (read)
		if ((buffer = in_channel->gets()) == NULL)
			break;		// at end of input

	read = true;
	if (strncmp(buffer, "<num>", 5) == 0)
		{
		old_number = number;
		number = atol(strchr(buffer, ':') + 1);

		if (old_number >= 0)
			return clean(old_number, raw_query);
		}
	else
		{
		match = false;

		if (*buffer == '<')
			{
			if ((strncmp(buffer + 1, "title", 5) == 0) && (strchr(tag, 't') != NULL))
				match = true;
			if ((strncmp(buffer + 1, "desc",  4) == 0) && (strchr(tag, 'd') != NULL))
				match = true;
			if ((strncmp(buffer + 1, "narr",  4) == 0) && (strchr(tag, 'n') != NULL))
				match = true;
			}
		if (match)
			{
			raw_query << strchr(buffer, '>') + 1;			// character after the "<desc>"
			delete [] buffer;
			while ((buffer = in_channel->gets()) != NULL)
				{
				if (*buffer == '<')
					{
					read = false;
					break;
					}
				strip_space_inplace(buffer);
				raw_query << ' ' << buffer;
				delete [] buffer;
				}
			}
		}
	}

at_eof = true;
return clean(number, raw_query);
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
