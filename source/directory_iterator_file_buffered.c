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
#include "stats.h"
#include "memory.h"

long ANT_directory_iterator_file_buffered::tid = 0;

/*
	ANT_DIRECTORY_ITERATOR_FILE_BUFFERED::ANT_DIRECTORY_ITERATOR_FILE_BUFFERED()
	----------------------------------------------------------------------------
*/
ANT_directory_iterator_file_buffered::ANT_directory_iterator_file_buffered(ANT_instream *instream, long get_file) : ANT_directory_iterator("", get_file) 
{
wait_input_time = 0;
wait_output_time = 0;
process_time = 0;
clock = new ANT_stats(new ANT_memory);

message = new char[50];
sprintf(message, "ANT_directory_iterator_file_buffered %ld ", ANT_directory_iterator_file_buffered::tid++);

document_start = document_end = NULL;

source = instream;

primary_read_buffer = new char [buffer_size + 1];
*primary_read_buffer = primary_read_buffer[buffer_size] = '\0';			// null terminate the end of the buffer, and mark it as empty too
primary_read_buffer_used = buffer_size;

secondary_read_buffer = new char [buffer_size + 1];
*secondary_read_buffer = secondary_read_buffer[buffer_size] = '\0';
secondary_read_buffer_used = buffer_size;

//read(primary_read_buffer, primary_read_buffer_used);

this->auto_file_id = 0;

doc_tag = new char*[2];
doc_tag[0] = NULL;
doc_tag[1] = NULL;
docno_tag = new char*[2];
docno_tag[0] = NULL;
docno_tag[1] = NULL;
set_tags("DOC", "DOCNO");

//printf("%sstart_upstream %lld\n", message, clock->start_timer());
}

/*
	ANT_DIRECTORY_ITERATOR_FILE_BUFFERED::~ANT_DIRECTORY_ITERATOR_FILE_BUFFERED()
	-----------------------------------------------------------------------------
*/
ANT_directory_iterator_file_buffered::~ANT_directory_iterator_file_buffered()
{
//clock->print_time(message, wait_input_time, " input");
//clock->print_time(message, wait_output_time, " upstream");
//clock->print_time(message, process_time, " process");
//printf("%send_upstream %lld\n", message, clock->start_timer());

free_tag();

delete [] primary_read_buffer;
delete [] secondary_read_buffer;

delete [] doc_tag;
delete [] docno_tag;

delete source;
delete clock;
}

/*
	ANT_DIRECTORY_ITERATOR_FILE_BUFFERED::READ()
	--------------------------------------------
*/
long long ANT_directory_iterator_file_buffered::read(char *destination, long long length)
{
long long got;

//long long now = clock->start_timer();
printf("%send_process %lld\n", message, clock->start_timer());
//printf("%sstart_wait %lld\n", message, now);
//printf("%sstart_input_wait %lld\n", message, now);
if ((got = source->read((unsigned char *)destination, length)) < length)
	destination[got] = '\0';

//printf("%send_wait %lld\n", message, now);
printf("%sstart_process %lld\n", message, clock->start_timer());
//wait_input_time += clock->stop_timer(now);
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

//static long long now = clock->start_timer();

//printf("%send_upstream %lld\n", message, clock->start_timer());

//printf("%send_upstream_wait %lld\n", message, clock->start_timer());
//wait_output_time += clock->stop_timer(now);
//now = clock->start_timer();
printf("%sstart_process %lld\n", message, clock->start_timer());

start = primary_read_buffer + primary_read_buffer_used;

/*
	Get the start tag
*/
if ((document_start = strstr(start, doc_tag[0])) == NULL)
	{
	/*
		We might be at the end of a buffer and half-way through a tag so we copy the remainder of the file to the
		start of the buffer and full the remainder
	*/
	memmove(primary_read_buffer, primary_read_buffer + primary_read_buffer_used, buffer_size - primary_read_buffer_used);
	//process_time += clock->stop_timer(now);
	bytes_read = read(primary_read_buffer + (buffer_size - primary_read_buffer_used), primary_read_buffer_used);
	//now = clock->start_timer();
	if ((bytes_read == 0) || (document_start = strstr(primary_read_buffer, doc_tag[0])) == NULL)
		{
printf("%send_process %lld\n", message, clock->start_timer());
//		process_time += clock->stop_timer(now);
//printf("%sstart_upstream %lld\n", message, clock->start_timer());
		return NULL;		// we are either at end of file of have a document that is too long to index (so pretend EOF)
		}
	}

/*
	Get the end tag
*/
if ((document_end = strstr(document_start, doc_tag[1])) == NULL)
	{
	/*
		This happens when we move find the start tag in the buffer, but the end tag is
		not in memory.  We play the same game as above and shift the start tag to the
		start of the buffer
	*/
	memmove(primary_read_buffer, document_start, buffer_size - (document_start - primary_read_buffer));
	//process_time += clock->stop_timer(now);
	bytes_read = read(primary_read_buffer + buffer_size - (document_start - primary_read_buffer), document_start - primary_read_buffer);
	//now = clock->start_timer();

	document_start = primary_read_buffer;
	if ((bytes_read == 0) || (document_end = strstr(document_start, doc_tag[1])) == NULL)
		{
printf("%send_process %lld\n", message, clock->start_timer());
//		process_time += clock->stop_timer(now);
//printf("%sstart_upstream %lld\n", message, clock->start_timer());
		return NULL;		// we are either at end of file of have a document that is too long to index (so pretend EOF)
		}
	}
document_end += strlen(doc_tag[1]);			// skip to end of tag

/*
	Take a note of how far through the buffer we are
*/
primary_read_buffer_used = document_end - primary_read_buffer;

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
else
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

printf("%send_process %lld\n", message, clock->start_timer());
//process_time += clock->stop_timer(now);
//printf("%sstart_upstream %lld\n", message, clock->start_timer());
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
