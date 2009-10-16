/*
	DIRECTORY_ITERATOR_MULTIPLE.H
	-----------------------------
*/
#include <stdio.h>
#include <string.h>
#include "directory_iterator_multiple.h"

/*
	ANT_DIRECTORY_ITERATOR_MULTIPLE::ANT_DIRECTORY_ITERATOR_MULTIPLE()
	------------------------------------------------------------------
*/
ANT_directory_iterator_multiple::ANT_directory_iterator_multiple() : ANT_directory_iterator()
{
sources_length = sources_used = 0;
sources = NULL;
filename = NULL;
file = NULL;
length = NULL;
}

/*
	ANT_DIRECTORY_ITERATOR_MULTIPLE::~ANT_DIRECTORY_ITERATOR_MULTIPLE()
	-------------------------------------------------------------------
*/
ANT_directory_iterator_multiple::~ANT_directory_iterator_multiple()
{
delete [] sources;
delete [] filename;
delete [] file;
delete [] length;
}

/*
	ANT_DIRECTORY_ITERATOR_MULTIPLE::ADD_ITERATOR()
	-----------------------------------------------
*/
void ANT_directory_iterator_multiple::add_iterator(ANT_directory_iterator *iterator)
{
ANT_directory_iterator **memory;

if (sources_used >= sources_length)
	{
	memory = new ANT_directory_iterator *[sources_length += 10];
	memcpy(memory, sources, sources_used * sizeof(ANT_directory_iterator *));
	delete [] sources;
	sources = memory;

	delete [] filename;
	filename = new char *[sources_length];

	delete [] file;
	file = new char *[sources_length];

	delete [] length;
	length = new long long [sources_length];
	}
sources[sources_used] = iterator;
sources_used++;
}

/*
	ANT_DIRECTORY_ITERATOR_MULTIPLE::FIRST()
	----------------------------------------
*/
char *ANT_directory_iterator_multiple::first(char *wildcard)
{
current_source = 0;

#pragma omp parallel for num_threads(4)
for (long current = 0; current < sources_used; current++)
	{
	filename[current] = sources[current]->first(wildcard);
	file[current] = sources[current]->read_entire_file(&length[current]);
	}

return filename[current_source];
}

/*
	ANT_DIRECTORY_ITERATOR_MULTIPLE::NEXT()
	---------------------------------------
*/
char *ANT_directory_iterator_multiple::next(void)
{
current_source++;

if (current_source >= sources_used)
	{
#pragma omp parallel for num_threads(4)
	for (long current = 0; current < sources_used; current++)
		{
		filename[current] = sources[current]->next();
		file[current] = sources[current]->read_entire_file(&length[current]);
		}
	current_source = 0;
	}

return filename[current_source];
}


/*
	ANT_DIRECTORY_ITERATOR_MULTIPLE::READ_ENTIRE_FILE()
	---------------------------------------------------
*/
char *ANT_directory_iterator_multiple::read_entire_file(long long *length)
{
*length = this->length[current_source];
return file[current_source];
}
