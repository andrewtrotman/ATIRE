/*
	DIRECTORY_ITERATOR_TSV.C
	------------------------
*/
#include <string.h>
#include <new>
#include "directory_iterator_tsv.h"
#include "str.h"

/*
	ANT_DIRECTORY_ITERATOR_TSV::READ_LINE()
	---------------------------------------
*/
unsigned char *ANT_directory_iterator_tsv::read_line()
{
unsigned char *into = buffer - 1;

*buffer = '\0';

do
	{
	into++;
	if (source->read(into, 1) != 1)
		return NULL; // EOF
	}
while (*into != '\n' && *into != '\r');

*into = '\0';

return buffer;
}

/*
	ANT_DIRECTORY_ITERATOR_TSV::FIRST()
	-----------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_tsv::first(ANT_directory_iterator_object *object)
{
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
unsigned char *file = read_line();
unsigned char *filename_start, *filename_end;

if (file == NULL)
	return NULL;

while (ANT_isspace(*file))
	file++;

filename_start = filename_end = file;
while (*filename_end != '\t')
	filename_end++;

object->filename = strnnew((char *)filename_start, filename_end - filename_start);

// skip over the url
file = filename_end + 1;
while (*file != '\t')
	file++;
file++;

object->length = strlen(file);

if (get_file)
	{
	object->file = new (std::nothrow) char [(size_t)(object->length + 1)];
	memcpy(object->file, file, object->length);
	object->file[object->length] = '\0';
	}

return object;
}
