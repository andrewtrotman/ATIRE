/*
	DIRECTORY_ITERATOR_CSV.C
	------------------------
*/
#include <new>
#include <string.h>
#include "pragma.h"
#include "directory_iterator_csv.h"
#include "ctypes.h"

/*
	ANT_DIRECTORY_ITERATOR_CSV::NEXT()
	----------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_csv::next(ANT_directory_iterator_object *object)
{
document_start = document_end;
while (ANT_isspace(*document_start))
	*document_start++;

document_end = document_start;
while (*document_end != '\n' && *document_end != '\0')
	document_end++;

if (*document_start != '\0')
	{
	strncpy(object->filename, document_start, document_end - document_start);
		object->filename[document_end - document_start] = '\0';
	if (get_file)
		read_entire_file(object);
	return object;
	}

return NULL;
}
