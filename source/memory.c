/*
	MEMORY.C
	--------
*/
#ifdef _MSC_VER
	#include <windows.h>
#else
	#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

#if _WIN32_WINNT < 0x0600			// prior to Vista
	/*
		If you compile under XP then GetLargePageMinimum() doesn't exist; it is first seen in
		Server 2003 and Vista.  So, if you have a version of the compiler from the XP era then
		we need to fake this method because you might compile under XP and run under Vista (or
		later) which would cause the run-time check to succeed so a function stub is needed.
		The solution is to require Microsoft C/C++ version 15.00 or later, but some existing
		ANT users don't have that.

		The hack is to set the large page size to the small page size (4096 bytes) in the case 
		of compiled under XP but run under Vista.
	*/
	#define GetLargePageMinimum() (4096)
#endif

#ifndef LARGE_MEMORY_PAGES
	#define GetLargePageMinimum() (4096)
#endif

/*
	ANT_MEMORY::ANT_MEMORY()
	------------------------
*/
ANT_memory::ANT_memory(long long block_size_for_allocation)
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
#else
	if ((short_page_size = large_page_size = sysconf(_SC_PAGESIZE)) <= 0)
		short_page_size = large_page_size = 4096;		// use 4K blocks by default (as this is the Pentium small page size)
#endif
//printf("Large Page Size: %lld Small Page Size:%lld\n", (long long)large_page_size, (long long)short_page_size);

chunk_end = at = chunk = NULL;
used = 0;
allocated = 0;
this->block_size = block_size_for_allocation;
}

/*
	ANT_MEMORY::~ANT_MEMORY()
	-------------------------
*/
ANT_memory::~ANT_memory()
{
char *killer;

while (chunk != NULL)
	{
	killer = chunk;
	chunk = *(char **)chunk;
	#ifdef _MSC_VER
		#ifdef PURIFY
			::free(killer);
		#else
			VirtualFree(killer, 0, MEM_RELEASE);
		#endif
	#else
		free(killer);
	#endif
	}
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
void *ANT_memory::alloc(long long *size)
{
#ifdef _MSC_VER
	#ifdef PURIFY
		return ::malloc((size_t)*size);
	#else
		void *answer = NULL;
		long long bytes = 0;

		/*
			First try using large page memory blocks
		*/
		#ifdef LARGE_MEMORY_PAGES
			if (has_large_pages)
				if (set_privilege(SE_LOCK_MEMORY_NAME, TRUE))		// try and get permission from the OS to allocate large pages
					{
					bytes = large_page_size * ((*size + large_page_size - 1) / large_page_size);
					answer = VirtualAlloc(NULL, (size_t)bytes, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
					set_privilege(SE_LOCK_MEMORY_NAME, FALSE);		// drop back to the initial security level
					}
		#endif
		/*
			If we don't have large pages or large pages fail then fall back to small pages
		*/
		if (answer == NULL)
			{
			bytes = short_page_size * ((*size + short_page_size - 1) / short_page_size);
			answer = VirtualAlloc(NULL, (size_t)bytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			if (answer != NULL)
				{
				/*
					This code is commented out at the moment because when I ran this over the
					INEX Wikipedia 2009 collection it caused a VISTA hang!  It looks like you
					can't lock more than 4GB of memory - but this is a guess.
				*/
	//			VirtualLock(answer, (size_t)bytes);		// lock the pages in memory so that it can't page to disk
				}
			else
				bytes = 0;		// couldn't allocate any memory.
			}
		/*
			If we still can't allocate memory then we're run out (ans so return NULL)
		*/
		*size = bytes;		// number of bytes we allocated
		return answer;
	#endif
#else
	return ::malloc((size_t)*size);
#endif
}

/*
	ANT_MEMORY::GET_CHAINED_BLOCK()
	-------------------------------
	The bytes parameter is passed to this routine simply so that we can be sure to
	allocate at least that number of bytes.
*/
void *ANT_memory::get_chained_block(long long bytes)
{
char **chain;
long long request;

#ifdef PURIFY
	request = bytes + sizeof(*chain);
#else
	if (bytes > block_size)
		block_size = bytes;			// extend the largest allocate block size

	request = block_size + sizeof(*chain);
#endif
if ((chain = (char **)alloc(&request)) == NULL)
	return NULL;

/*
	Create a linked list of large blocks of memory.
*/
*chain = chunk;
chunk = (char *)chain;

/*
	Mark the current location we are to allocate from (which we call at)
	and the end of the block (chunk_end)
*/
at = chunk + sizeof(*chain);
chunk_end = chunk + request;		// request is the amount it allocated (which might be more than we asked for)

/*
	This is the amount we have allocated in total
*/
allocated += request;

return chunk;
}
