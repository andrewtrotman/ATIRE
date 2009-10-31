/*
	DIRECTORY_ITERATOR_MULTIPLE.C
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
	memory = new ANT_directory_iterator *[sources_length += 8];
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

for (long current = 0; current < sources_used; current++)
	if ((filename[current] = sources[current]->first(wildcard)) == NULL)
		file[current] = NULL;
	else
		file[current] = sources[current]->read_entire_file(&length[current]);

/*
	Find the first non-empty stream
*/
for (current_source = 0; current_source < sources_used; current_source++)
	if (filename[current_source] != NULL)
		return filename[current_source];

/*
	All sources returned EOF
*/
current_source = 0;
return NULL;
}

/*
	ANT_DIRECTORY_ITERATOR_MULTIPLE::NEXT()
	---------------------------------------
*/
char *ANT_directory_iterator_multiple::next(void)
{
/*
	Find the next non_empty stream
*/
current_source++;
while (current_source < sources_used)
	{
	if (filename[current_source] != NULL)
		break;
	current_source++;
	}

if (current_source >= sources_used)
	{
	for (long current = 0; current < sources_used; current++)
		{
		/*
			Re-fill the non-empty streams
		*/
		if (filename[current] != NULL)
			if ((filename[current] = sources[current]->next()) == NULL)
				file[current] = NULL;
			else
				file[current] = sources[current]->read_entire_file(&length[current]);
		}
	/*
		Find the first non-empty stream
	*/
	for (current_source = 0; current_source < sources_used; current_source++)
		if (filename[current_source] != NULL)
			return filename[current_source];

	/*
		All sources returned EOF
	*/
	current_source = 0;
	return NULL;
	}

return filename[current_source];
}


/*
	ANT_DIRECTORY_ITERATOR_MULTIPLE::READ_ENTIRE_FILE()
	---------------------------------------------------
*/
char *ANT_directory_iterator_multiple::read_entire_file(long long *length)
{
if (filename[current_source] != NULL)
	{
	*length = this->length[current_source];
	return file[current_source];
	}
return NULL;		// no such file as at EOF of all streams
}
