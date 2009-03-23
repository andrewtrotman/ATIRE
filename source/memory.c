/*
	MEMORY.C
	--------
*/
#ifdef _MSC_VER
	#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

/*
	ANT_MEMORY::ANT_MEMORY()
	------------------------
*/
ANT_memory::ANT_memory(size_t block_size_for_allocation)
{
#ifdef _MSC_VER
	OSVERSIONINFO os_info;
	SYSTEM_INFO hardware_info;

	os_info.dwOSVersionInfoSize = sizeof(os_info);
	GetVersionEx(&os_info);
	has_large_pages = os_info.dwMajorVersion >= 6;		// version 6 is Vista & Server2008 - prior to this there was no large page support
	large_page_size = has_large_pages ? GetLargePageMinimum() : 0;
	GetSystemInfo(&hardware_info);
	short_page_size = hardware_info.dwPageSize;

//	printf("Large Page Size: %lld Small Page Size:%lld\n", (long long)large_page_size, (long long)short_page_size);
#else
	short_page_size = large_page_size = 4096;		// use 4K blocks by default (as this is the Pentium small page size)
#endif

chunk_end = at = chunk = NULL;
used = 0;
this->block_size = block_size_for_allocation;
}

/*
	ANT_MEMORY::~ANT_MEMORY()
	-------------------------
*/
ANT_memory::~ANT_memory()
{
#ifdef _MSC_VER
	if (chunk != NULL)
		VirtualFree(chunk, 0, MEM_RELEASE);
#else
	if (chunk != NULL)
		free(chunk);
#endif
}

#ifdef _MSC_VER
	/*
		ANT_MEMORY::SET_PRIVILEGE()
		---------------------------
	*/
	long ANT_memory::set_privilege(char *priv_name, long enable)
	{
	HANDLE token;
	TOKEN_PRIVILEGES tp;
	long success = FALSE;

	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token);

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;
	  
	if (LookupPrivilegeValue(NULL, priv_name, &tp.Privileges[0].Luid) == 0)
		CloseHandle(token);
	else
		if (AdjustTokenPrivileges( token, FALSE, &tp, 0, NULL, 0) == ERROR_NOT_ALL_ASSIGNED)
			CloseHandle(token);
		else
			success = TRUE;

	return success;
	}
#endif

/*
	ANT_MEMORY::ALLOC()
	-------------------
	Allocate large pages from the OS (and if this fails then use small pages).  Under Windows large pages can fail if:
	1. we are out of memory
	2. the memory is fragmented
	3. we don't have permissions to allocate large pages of memory
	4. we're not on Vista (or later).

	to check whether or not you have permission to allocate large pages see:
	Control Panel->Administrative Tools->Local Security Settings->->User Rights Assignment->Lock pages in memory
*/
void *ANT_memory::alloc(size_t *size)
{
#ifdef _MSC_VER
	size_t bytes = 0;
	void *answer = NULL;
	/*
		First try using large page memory blocks
	*/
	if (has_large_pages)
		if (set_privilege(SE_LOCK_MEMORY_NAME, TRUE))		// try and get permission from the OS to allocate large pages
			{
			bytes = large_page_size * ((*size + large_page_size - 1) / large_page_size);
			answer = VirtualAlloc(NULL, bytes, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
			set_privilege(SE_LOCK_MEMORY_NAME, FALSE);		// drop back to the initial security level
			}
	/*
		If we don't have large pages or large pages fail then fall back to small pages
	*/
	if (answer == NULL)
		{
		bytes = short_page_size * ((*size + short_page_size - 1) / short_page_size);
		answer = VirtualAlloc(NULL, bytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		if (answer != NULL)
			VirtualLock(answer, bytes);		// lock the pages in memory so that it can't page to disk
		else
			bytes = 0;		// couldn't allocate any memory.
		}
	/*
		If we still can't allocate memory then we're run out (ans so return NULL)
	*/
	*size = bytes;		// number of bytes we allocated
	return answer;
#else
	return ::malloc(*size);
#endif
}

