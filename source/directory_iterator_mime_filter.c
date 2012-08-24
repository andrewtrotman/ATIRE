/*
	DIRECTORY_ITERATOR_MIME_FILTER.C
	--------------------------------
	Filters out any mime types (Content-Type) that don't begin with "text" if they exist
	The absence of a Content-Type declaration is taken to mean html
*/
#include <stdio.h>
#include <stdlib.h>
#include "maths.h"
#include "str.h"
#include "disk.h"
#include "directory_iterator_mime_filter.h"

static const char *header = "Content-Type: ";

/*
	ANT_DIRECTORY_ITERATOR_MIME_FILTER::ANT_DIRECTORY_ITERATOR_MIME_FILTER()
	------------------------------------------------------------------------
*/
ANT_directory_iterator_mime_filter::ANT_directory_iterator_mime_filter(ANT_directory_iterator *source, long get_file) : ANT_directory_iterator("", get_file)
{
this->source = source;
}

/*
	ANT_DIRECTORY_ITERATOR_MIME_FILTER::~ANT_DIRECTORY_ITERATOR_MIME_FILTER()
	-------------------------------------------------------------------------
*/
ANT_directory_iterator_mime_filter::~ANT_directory_iterator_mime_filter()
{
delete source;
}

/*
	ANT_DIRECTORY_ITERATOR_MIME_FILTER::FIRST()
	-------------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_mime_filter::first(ANT_directory_iterator_object *object)
{
ANT_directory_iterator_object *t = source->first(object);
char *content_type;

if (t != NULL)
	{
	if ((content_type = strstr(t->file, header)) != NULL)
		content_type += strlen(header);
	
	if (content_type == NULL || strncmp(content_type, "text", 4) == 0)
		return t;
	
	delete [] t->file;
	delete [] t->filename;
	}

return next(object);
}

/*
	ANT_DIRECTORY_ITERATOR_MIME_FILTER::NEXT()
	------------------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_mime_filter::next(ANT_directory_iterator_object *object)
{
ANT_directory_iterator_object *t = source->next(object);
char *content_type;

while (t != NULL)
	{
	if ((content_type = strstr(t->file, header)) != NULL)
		content_type += strlen(header);
	
	if (content_type != NULL && strncmp(content_type, "text", 4) != 0)
		{
		delete [] t->file;
		delete [] t->filename;

		t = source->next(object);
		}
	else
		break;
	}

return t;
}
