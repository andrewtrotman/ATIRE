/*
	DIRECTORY_ITERATOR_FILE.C
	-------------------------
*/
#include <new>
#include <string.h>
#include "pragma.h"
#include "str.h"
#include "directory_iterator_file.h"

/*
	ANT_DIRECTORY_ITERATOR_FILE::ANT_DIRECTORY_ITERATOR_FILE()
	----------------------------------------------------------
*/
ANT_directory_iterator_file::ANT_directory_iterator_file(char *file, long get_file) : ANT_directory_iterator("", get_file)
{
initialise();
this->file = file;
}

/*
	ANT_DIRECTORY_ITERATOR_FILE::ANT_DIRECTORY_ITERATOR_FILE()
	----------------------------------------------------------
*/
ANT_directory_iterator_file::ANT_directory_iterator_file(ANT_directory_iterator *source, long get_file) : ANT_directory_iterator("", get_file)
{
initialise();
this->source = source;
}

/*
	ANT_DIRECTORY_ITERATOR_FILE::INITIALISE()
	-----------------------------------------
*/
void ANT_directory_iterator_file::initialise(void)
{
source = NULL;
document_start = document_end = file = NULL;
}

/*
	ANT_DIRECTORY_ITERATOR_FILE::GET_NEXT_FILE()
	--------------------------------------------
*/
char *ANT_directory_iterator_file::get_next_file(void)
{
ANT_directory_iterator_object object, *success;

if (source != NULL)
	{
	if (file == NULL)
		success = source->first(&object);
	else
		success = source->next(&object);

	if (success != NULL)
		{
		delete [] file;
		file = object.file;
		return document_end = document_start = file;
		}
	}

return NULL;
}

/*
	ANT_DIRECTORY_ITERATOR_FILE::FIRST()
	------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_file::first(ANT_directory_iterator_object *object)
{
document_end = document_start = file;
return next(object);
}

/*
	ANT_DIRECTORY_ITERATOR_FILE::NEXT()
	-----------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_file::next(ANT_directory_iterator_object *object)
{
char *document_id_start = NULL, *document_id_end = NULL;

if (document_end == NULL)
	if (get_next_file() == NULL)
		return NULL;

if ((document_start = strstr(document_end, "<DOC")) != NULL)
	{
	if (*(document_start + 4) == '>')
		{
		document_id_start = strstr(document_start, "<DOCNO>");
		document_id_end = strstr(document_id_start += 7, "</DOCNO>");
		}
	else
		{
		document_id_start = strstr(document_start, "id=\"");
		document_id_end = strchr(document_id_start += 4, '"');
		if (document_id_end)
			document_start = strchr(document_id_end, '>') + 1;
		}

	if (document_id_end != NULL && (document_end = strstr(document_id_end, "</DOC>")) != NULL)
		{
		document_end += 6;
		object->filename = strnnew(document_id_start, document_id_end - document_id_start);
		if (get_file)
			read_entire_file(object);
		return object;
		}
	}

if (get_next_file() == NULL)
	return NULL;
else
	return next(object);
}

/*
	ANT_DIRECTORY_ITERATOR_FILE::READ_ENTIRE_FILE()
	-----------------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_file::read_entire_file(ANT_directory_iterator_object *object)
{
object->length = document_end - document_start;
object->file = new (std::nothrow) char [(size_t)(object->length + 1)];
memcpy(object->file, document_start, (size_t)object->length);
object->file[(size_t)object->length] = '\0';

return object;
}

