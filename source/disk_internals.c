/*
	DISK_INTERNALS.C
	----------------
*/
#include "disk_internals.h"

/*
	ANT_DISK_INTERNALS::ANT_DISK_INTERNALS()
	----------------------------------------
*/
ANT_disk_internals::ANT_disk_internals()
{
#ifdef _MSC_VER
	file_list = INVALID_HANDLE_VALUE;
#endif
}

/*
	ANT_DISK_INTERNALS::~ANT_DISK_INTERNALS()
	-----------------------------------------
*/
ANT_disk_internals::~ANT_disk_internals()
{
}
