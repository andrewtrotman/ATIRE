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
	Doesn't really read a line, returns the beginning of the next line
	and sets up for the next line so that it won't contain any extraneous
	characters
*/
long long ANT_directory_iterator_tsv::read_line()
{
long long remain, beginning;
unsigned char *nl;
char *n = strchr((char *)buffer + position, '\n');
char *r = strchr((char *)buffer + position, '\r');

// if we didn't find a \n or \r then move the end of the buffer
// to the beginning and get fill the rest of the buffer
if (n == NULL && r == NULL)
	{
	remain = buffer_size - position;
	memcpy(buffer, buffer + position, remain);

	// try to read more from the source
	source->read(buffer + remain, buffer_size - remain);

	position = 0;

	n = strchr((char *)buffer + position, '\n');
	r = strchr((char *)buffer + position, '\r');
	}

if (n == NULL && r == NULL)
	return -1;
else if (n == NULL)
	nl = (unsigned char *)r;
else if (r == NULL)
	nl = (unsigned char *)n;
else
	nl = (unsigned char *)(n > r ? r : n);

// replace the new line with a nul so that strlen blah work
*nl = '\0';

beginning = position;

// skip over this "line"
position = (nl - buffer) + 1;

// skip over any extra newlines we might have found (\r\n pairs)
while (position < buffer_size && buffer[position] != '\0' && (buffer[position] == '\r' || buffer[position] == '\n'))
	position++;

return beginning;
}

/*
	ANT_DIRECTORY_ITERATOR_TSV::FIRST()
	-----------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_tsv::first(ANT_directory_iterator_object *object)
{
position = 0;
buffer = new unsigned char[buffer_size];
source->read(buffer, buffer_size);

return next(object);
}

/*
	ANT_DIRECTORY_ITERATOR_TSV::NEXT()
	----------------------------------
	Reads line by line, file format for Djeord provided anchors:
	<filename>\t<url>\t<anchor>\t...\t<anchor>
*/
ANT_directory_iterator_object *ANT_directory_iterator_tsv::next(ANT_directory_iterator_object *object)
{
unsigned char *file;
unsigned char *filename_start, *filename_end;

long long begin = read_line();

if (begin == -1)
	return NULL;

filename_start = buffer + begin;
filename_end = strchr(filename_start, '\t');
file = strchr(filename_end + 1, '\t');

//if ((filename_end = strchr(filename_start, '\t')) == NULL)
//	return NULL;
//
//if ((file = strchr(filename_end + 1, '\t')) == NULL)
//	return NULL;

//file = strchr(filename_end + 1, '\t');
//if (file == NULL)
//	return NULL;

printf("%d %d\n", strlen(file), position - begin);

object->filename = strnnew((char *)filename_start, filename_end - filename_start);
object->length = strlen(file);
if (get_file)
	{
	object->file = new (std::nothrow) char[(size_t)(object->length + 1)];
	strcpy(object->file, (char *)file);
	}

return object;
}
