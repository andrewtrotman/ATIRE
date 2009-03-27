/*
	DISK_INTERNALS.C
	----------------
*/
#include <stdio.h>
#include "disk_internals.h"

/*
	ANT_DISK_INTERNALS::ANT_DISK_INTERNALS()
	----------------------------------------
*/
ANT_disk_internals::ANT_disk_internals()
{
#ifdef _MSC_VER
	file_list = INVALID_HANDLE_VALUE;
#else
	file_list = NULL;
#endif
}

ANT_disk_internals::~ANT_disk_internals() {
}
