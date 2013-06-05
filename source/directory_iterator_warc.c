/*
	DIRECTORY_ITERATOR_WARC.C
	-------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <new>
#include "pragma.h"
#include "str.h"
#include "ctypes.h"
#include "directory_iterator_warc.h"
#include "maths.h"

/*
	ANT_DIRECTORY_ITERATOR_WARC::FIND_STRING()
	------------------------------------------
*/
unsigned char *ANT_directory_iterator_warc::find_string(char *string, long string_length)
{
unsigned char *into, *end;

/*
	Do a series of gets's until we find the given string
*/
buffer[string_length] = '\0';		// prevent early accidental termination
end = buffer + sizeof(buffer) - 1;
do
	do
		{
		*buffer = '\0';
		into = buffer - 1;
		do
			{
			into++;
			if (source->read(into, 1) != 1)
				return NULL;		// at EOF
			}
		while (*into != '\n' && into < end);
		*into = '\0';

		/*
			In the case of buffer overflow we read to end of line and star over.  This shouldn't happen very often,
			but it does happen because there are some very long WARC-Target-URI in the TREC ClueWeb09 collection
			in documents such as clueweb09-en0000-05-10880 which as a 1486 character WARC-Target-URI!
		*/
		if (into == end)
			{
			do
				if (source->read(into, 1) != 1)
					return NULL;		// at EOF
			while (*into != '\n');
			continue;
			}
		}
	while (buffer[string_length] != ':');			// check for the ':' in the right place
while (strncmp((char *)buffer, string, string_length) != 0);	// now check for the TREC-ID

return buffer + string_length + 1;
}

/*
	ANT_DIRECTORY_ITERATOR_WARC::NEXT()
	-----------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_warc::next(ANT_directory_iterator_object *object)
{
unsigned char *filename, *file_length;

/*
	Get and store the filename
*/
if ((filename = find_string("WARC-TREC-ID", 12)) == NULL)
	return NULL;

while (ANT_isspace(*filename))
	filename++;

object->filename = strnew((char *)filename);

/*
	Get and store the document length
*/
if ((file_length = find_string("Content-Length", 14)) == NULL)
	return NULL;
object->length = atoll(file_length);

/*
	Get the file
*/
if (get_file)
	read_entire_file(object);

return object;
}

/*
	ANT_DIRECTORY_ITERATOR_WARC::READ_ENTIRE_FILE()
	-----------------------------------------------
*/
void ANT_directory_iterator_warc::read_entire_file(ANT_directory_iterator_object *object)
{
object->file = new (std::nothrow) char [(size_t)(object->length + 1)];
source->read((unsigned char *)object->file, object->length);
object->file[object->length] = '\0';			// NULL terminate the contents of the file.
}

