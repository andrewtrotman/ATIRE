/*
	DIRECTORY_ITERATOR_FILE_BUFFERED.C
	----------------------------------
*/
#include <new>
#include <string.h>
#include <stdio.h>
#include "pragma.h"
#include "str.h"
#include "instream.h"
#include "directory_iterator_file_buffered.h"

#ifdef _MSC_VER
	#define strstr	strcasestr
#endif

/*
	ANT_DIRECTORY_ITERATOR_FILE_BUFFERED::ANT_DIRECTORY_ITERATOR_FILE_BUFFERED()
	----------------------------------------------------------------------------
*/
ANT_directory_iterator_file_buffered::ANT_directory_iterator_file_buffered(ANT_instream *instream, long get_file) : ANT_directory_iterator("", get_file) 
{
document_start = document_end = NULL;

source = instream;

read_buffer = new char [buffer_size + 1];
*read_buffer = read_buffer[buffer_size] = '\0';			// null terminate the end of the buffer, and mark it as empty too
read_buffer_used = buffer_size;
this->auto_file_id = 0;
}

/*
	ANT_DIRECTORY_ITERATOR_FILE_BUFFERED::~ANT_DIRECTORY_ITERATOR_FILE_BUFFERED()
	-----------------------------------------------------------------------------
*/
ANT_directory_iterator_file_buffered::~ANT_directory_iterator_file_buffered()
{
delete [] read_buffer;
}

/*
	ANT_DIRECTORY_ITERATOR_FILE_BUFFERED::READ()
	--------------------------------------------
*/
long long ANT_directory_iterator_file_buffered::read(char *destination, long long length)
{
long long got;

if ((got = source->read((unsigned char *)destination, length)) < length)
	destination[got] = '\0';

return got;
}

/*
	ANT_DIRECTORY_ITERATOR_FILE_BUFFERED::NEXT()
	--------------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_file_buffered::next(ANT_directory_iterator_object *object)
{
char *start, *document_id_start, *document_id_end;
long long bytes_read;
char *file_id_buffer = new char[sizeof(long) * 8 + 1];

start = read_buffer + read_buffer_used;

/*
	Get the start tag
*/
if ((document_start = strcasestr(start, "<DOC")) == NULL)
	{
	/*
		We might be at the end of a buffer and half-way through a tag so we copy the remainder of the file to the
		start of the buffer and full the remainder
	*/
	memmove(read_buffer, read_buffer + read_buffer_used, buffer_size - read_buffer_used);
	bytes_read = read(read_buffer + (buffer_size - read_buffer_used), read_buffer_used);
	if ((bytes_read == 0) || (document_start = strcasestr(read_buffer, "<DOC")) == NULL)
		return NULL;		// we are either at end of file of have a document that is too long to index (so pretend EOF)
	}

/*
	Get the end tag
*/
if ((document_end = strcasestr(document_start, "</DOC>")) == NULL)
	{
	/*
		This happens when we move find the start tag in the buffer, but the end tag is
		not in memoruy.  We play the same game as above and shift the start tag to the
		start of the buffer
	*/
	memmove(read_buffer, document_start, buffer_size - (document_start - read_buffer));
	bytes_read = read(read_buffer + buffer_size - (document_start - read_buffer), document_start - read_buffer);

	document_start = read_buffer;
	if ((bytes_read == 0) || (document_end = strcasestr(document_start, "</DOC>")) == NULL)
		return NULL;		// we are either at end of file of have a document that is too long to index (so pretend EOF)
	}
document_end += 6;			// skip to end of tag

/*
	Take a note of how far through the buffer we are
*/
read_buffer_used = document_end - read_buffer;

/*
	Now get the DOCID (or in the case of NTCIR, the id=
*/

if (!auto_file_id )
	{
	if (*(document_start + 4) == '>')
		{
		document_id_start = strcasestr(document_start, "<DOCNO>");
		if (document_id_start != NULL)
			document_id_end = strcasestr(document_id_start += 7, "</DOCNO>");
		else
			++auto_file_id;
		}
	else
		{
		document_id_start = strstr(document_start, "id=\"");
		document_id_end = strchr(document_id_start += 4, '"');
		if (document_id_end)
			document_start = strchr(document_id_end, '>') + 1;
		}
	}
else

/*
	Copy the id into the document object and get the document
*/
object->file = NULL;
if (document_id_end == NULL && !auto_file_id)
	object->filename = strnew("Unknown");
else
	{
	if (auto_file_id) {
		sprintf(file_id_buffer, "%d", auto_file_id++);
		object->filename = strnew(file_id_buffer);
	}
	else
		object->filename = strnnew(document_id_start, document_id_end - document_id_start);

	if (get_file)
		read_entire_file(object);
	}
delete file_id_buffer;
return object;
}

/*
	ANT_DIRECTORY_ITERATOR_FILE_BUFFERED::READ_ENTIRE_FILE()
	--------------------------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_file_buffered::read_entire_file(ANT_directory_iterator_object *object)
{
object->length = document_end - document_start;
object->file = new (std::nothrow) char [(size_t)(object->length + 1)];
memcpy(object->file, document_start, (size_t)object->length);
object->file[(size_t)object->length] = '\0';

return object;
}
