/*
	DIRECTORY_ITERATOR_INTERNALS.C
	------------------------------
*/
#include <stdio.h>
#include "directory_iterator_internals.h"

/*
	ANT_DIRECTORY_ITERATOR_INTERNALS::ANT_DIRECTORY_ITERATOR_INTERNALS()
	--------------------------------------------------------------------
*/
ANT_directory_iterator_internals::ANT_directory_iterator_internals()
{
#ifdef _MSC_VER
	file_list = INVALID_HANDLE_VALUE;
#else
	file_list = NULL;
#endif
}

