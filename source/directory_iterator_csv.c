/*
	DIRECTORY_ITERATOR_CSV.C
	------------------------
*/
#include <new>
#include <string.h>
#include "pragma.h"
#include "directory_iterator_csv.h"
#include "ctypes.h"
#include "str.h"

/*
	ANT_DIRECTORY_ITERATOR_CSV::NEXT()
	----------------------------------
*/
ANT_directory_iterator_object *ANT_directory_iterator_csv::next(ANT_directory_iterator_object *object)
{
if (file == NULL)
	return NULL;

document_start = document_end;
while (ANT_isspace(*document_start))
	document_start++;

document_end = document_start;
while (*document_end != '\n' && *document_end != '\0')
	document_end++;

if (*document_start != '\0')
	{
	object->filename = strnnew(document_start, document_end - document_start);
	if (get_file)
		read_entire_file(object);
	return object;
	}

return NULL;
}
