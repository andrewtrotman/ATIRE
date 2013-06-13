/*
	DIRECTORY_ITERATOR_TSV.C
	------------------------
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <new>
#include "directory_iterator_tsv.h"
#include "str.h"

/*
	ANT_DIRECTORY_ITERATOR_TSV::READ_LINE()
	---------------------------------------
*/
long long ANT_directory_iterator_tsv::read_line()
{
long long remain, beginning;
unsigned char *nl;

char *n = strchr((char *)buffer + position, '\n');
char *r = NULL;

// if we didn't find it, then move the end to the beginning, refill the end
// and check again
if (n == NULL && r == NULL)
	{
	end_of_buffer = remain = buffer_size - position;
	memcpy(buffer, buffer + position, remain);

	// try to read more from the source
	end_of_buffer += source->read(buffer + remain, buffer_size - remain);

	n = strchr((char *)buffer, '\n');
	r = NULL;
	position = 0;
	}

if (n == NULL && r == NULL)
	nl = buffer + end_of_buffer + 1; // force the fail later
else if (n == NULL)
	nl = (unsigned char *)r;
else if (r == NULL)
	nl = (unsigned char *)n;
else
	nl = (unsigned char *)(n > r ? r : n);

// because we have a reused buffer, if we don't read anything new into the end, we could
// refind a nl that we've already used, so check against the end of buffer
if ((nl - buffer) > end_of_buffer)
	return -1;

// replace the new line with a nul so that strlen blah work
*nl = '\0';

beginning = position;

// skip over this "line"
position = (nl - buffer) + 1;

// skip over any extra newlines we might have found
while (position < end_of_buffer && buffer[position] != '\0' && buffer[position] == '\n')
	position++;

return beginning;
}

/*
	ANT_DIRECTORY_ITERATOR_TSV::FIRST()
	-----------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_tsv::first(ANT_directory_iterator_object *object)
{
end_of_buffer = position = 0;

buffer = new unsigned char[buffer_size];

end_of_buffer = source->read(buffer, buffer_size);

return next(object);
}

/*
	ANT_DIRECTORY_ITERATOR_TSV::NEXT()
	----------------------------------
	Reads line by line, file format for Djeord provided anchors:
	<filename>\t<url>\t<anchor>\t...\t<anchor>\n
*/
ANT_directory_iterator_object *ANT_directory_iterator_tsv::next(ANT_directory_iterator_object *object)
{
unsigned char *file;
unsigned char *filename_start, *filename_end;

long long begin = read_line();

if (begin == -1)
	return NULL;

filename_start = buffer + begin;

if ((filename_end = strchr(filename_start, '\t')) == NULL)
	return NULL;

if ((file = strchr(filename_end + 1, '\t')) == NULL)
	return NULL;
file++;

object->filename = strnnew((char *)filename_start, filename_end - filename_start);
object->length = strlen(file);

if (get_file)
	{
	object->file = new (std::nothrow) char[(size_t)(object->length + 1)];
	strcpy(object->file, (char *)file);
	}

return object;
}
