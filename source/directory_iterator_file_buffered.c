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

doc_tag = new char*[2];
doc_tag[0] = NULL;
doc_tag[1] = NULL;
docno_tag = new char*[2];
docno_tag[0] = NULL;
docno_tag[1] = NULL;
set_tags("DOC", "DOCNO");
}

/*
	ANT_DIRECTORY_ITERATOR_FILE_BUFFERED::~ANT_DIRECTORY_ITERATOR_FILE_BUFFERED()
	-----------------------------------------------------------------------------
*/
ANT_directory_iterator_file_buffered::~ANT_directory_iterator_file_buffered()
{
delete [] read_buffer;
free_tag();
delete [] doc_tag;
delete [] docno_tag;
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
char *start, *document_id_start = NULL, *document_id_end = NULL;
long long bytes_read;
char file_id_buffer[24];		// large enough to hold a 64-bit sequence number

start = read_buffer + read_buffer_used;

/*
	Get the start tag
*/
if ((document_start = strstr(start, doc_tag[0])) == NULL)
	{
	/*
		We might be at the end of a buffer and half-way through a tag so we copy the remainder of the file to the
		start of the buffer and full the remainder
	*/
	memmove(read_buffer, read_buffer + read_buffer_used, buffer_size - read_buffer_used);
	bytes_read = read(read_buffer + (buffer_size - read_buffer_used), read_buffer_used);
	if ((bytes_read == 0) || (document_start = strstr(read_buffer, doc_tag[0])) == NULL)
		return NULL;		// we are either at end of file of have a document that is too long to index (so pretend EOF)
	}

/*
	Get the end tag
*/
if ((document_end = strstr(document_start, doc_tag[1])) == NULL)
	{
	/*
		This happens when we move find the start tag in the buffer, but the end tag is
		not in memoruy.  We play the same game as above and shift the start tag to the
		start of the buffer
	*/
	memmove(read_buffer, document_start, buffer_size - (document_start - read_buffer));
	bytes_read = read(read_buffer + buffer_size - (document_start - read_buffer), document_start - read_buffer);

	document_start = read_buffer;
	if ((bytes_read == 0) || (document_end = strstr(document_start, doc_tag[1])) == NULL)
		return NULL;		// we are either at end of file of have a document that is too long to index (so pretend EOF)
	}
document_end += strlen(doc_tag[1]);			// skip to end of tag

/*
	Take a note of how far through the buffer we are
*/
read_buffer_used = document_end - read_buffer;

/*
	Now get the DOCID (or in the case of NTCIR, the id=)
*/
if (!auto_file_id)
	{
	if (*(document_start + strlen(doc_tag[0])) == '>')
		{
		document_id_start = strstr(document_start, docno_tag[0]);
		if (document_id_start != NULL)
			document_id_end = strstr(document_id_start += strlen(docno_tag[0]), docno_tag[1]);
		else
			auto_file_id++;
		}
	else
		{
		document_id_start = strstr(document_start, "id=\"");
		document_id_end = strchr(document_id_start += strlen(doc_tag[0]), '"');
		if (document_id_end)
			document_start = strchr(document_id_end, '>') + 1;
		}
	}
if (auto_file_id)
	{
	document_id_start = document_id_end = file_id_buffer;
	document_id_end += sprintf(file_id_buffer, "%ld", auto_file_id++);
	}

/*
	Copy the id into the document object and get the document
*/
object->file = NULL;
if (document_id_end == NULL)
	object->filename = strnew("Unknown");
else
	{
	object->filename = strnnew(document_id_start, document_id_end - document_id_start);

	if (get_file)
		read_entire_file(object);
	}

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

/*
	ANT_DIRECTORY_ITERATOR_FILE_BUFFERED::FREE_TAG()
	------------------------------------------------
 */
void ANT_directory_iterator_file_buffered::free_tag()
{
for (int i = 0; i < 2; ++i)
	{
	delete doc_tag[i];
	delete docno_tag[i];
	}
}

/*
	ANT_DIRECTORY_ITERATOR_FILE_BUFFERED::SET_TAGS()
	------------------------------------------------
 */
void ANT_directory_iterator_file_buffered::set_tags(char *doc_name, char *docno_name)
{
free_tag();
doc_tag[0] = new char[strlen(doc_name) + 2];   //"<" \0
doc_tag[1] = new char[strlen(doc_name) + 4];  // "</>" plus \0
docno_tag[0] = new char[strlen(docno_name) + 3];  //"<DOCNO>",  extra 3 bytes include < > \0
docno_tag[1] = new char[strlen(docno_name) + 4];  // </DOCNO>", extra 3 bytes include </ > \0
sprintf(doc_tag[0], "<%s", doc_name);
sprintf(doc_tag[1], "</%s>", doc_name);
sprintf(docno_tag[0], "<%s>", docno_name);
sprintf(docno_tag[1], "</%s>", docno_name);
}
