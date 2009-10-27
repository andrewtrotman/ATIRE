/*
	DIRECTORY_ITERATOR_FILE.C
	-------------------------
*/
#include <new>
#include <string.h>
#include "pragma.h"
#include "directory_iterator_file.h"

/*
	ANT_DIRECTORY_ITERATOR_FILE::FIRST()
	------------------------------------
*/
char *ANT_directory_iterator_file::first(char *wildcard)
{
document_end = document_start = file;

return next();
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ANT_DIRECTORY_ITERATOR_FILE::NEXT()
	-----------------------------------
*/
char *ANT_directory_iterator_file::next(void)
{
char *document_id_start, *document_id_end;

if ((document_start = strstr(document_end, "<DOC")) != NULL)
	if ((document_id_start = strstr(document_start, "<DOCNO>")) != NULL)
		if ((document_id_end = strstr(document_id_start += 7, "</DOCNO>")) != NULL)
			if ((document_end = strstr(document_id_end, "</DOC>")) != NULL)
				{
				strncpy(filename, document_id_start, document_id_end - document_id_start);
				filename[document_id_end - document_id_start] = '\0';
				return filename;
				}
return NULL;
}

/*
	ANT_DIRECTORY_ITERATOR_FILE::READ_ENTIRE_FILE()
	-----------------------------------------------
*/
char *ANT_directory_iterator_file::read_entire_file(long long *len)
{
char *result;

*len = document_end - document_start;
result = new (std::nothrow) char [(size_t)(*len + 1)];
strncpy(result, document_start, (size_t)*len);
result[(size_t)*len] = '\0';

return result;
}

