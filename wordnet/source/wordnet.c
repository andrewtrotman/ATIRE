/*
	WORDNET.C
	---------
*/
#include <stdio.h>
#include <new>
#include "wordnet.h"
#include "thesaurus_rootnode.h"
#include "../../source/fundamental_types.h"
#include "../../source/file.h"

/*
	ANT_WORDNET::ANT_WORDNET()
	--------------------------
*/
ANT_wordnet::ANT_wordnet(char *filename) : ANT_thesaurus(filename)
{
size_t file_tail_length;
uint64_t root_start, root_length_in_terms, id_wordnet;
uint32_t id_version, id_ant;
long long length_of_file, current;
char *position;

file = new ANT_file();
file->open(filename, "rb");

file_tail_length = sizeof(id_ant) + sizeof(id_version) + sizeof(id_wordnet) + sizeof(root_start) + sizeof(root_length_in_terms);
if ((length_of_file = file->file_length()) > file_tail_length)
	{
	file->seek(length_of_file - file_tail_length);
	file->read(&root_start);
	file->read(&root_length_in_terms);

	file->read(&id_wordnet);
	file->read(&id_version);
	file->read(&id_ant);
	if ((id_ant == ANT_ID_THESAURUS_SIGNATURE_MAJOR) && (id_version == ANT_ID_THESAURUS_SIGNATURE_MINOR) && (id_wordnet == ANT_ID_THESAURUS_WORDNET))
		{
		/*
			At this point we have verified that the file is an ANT WORDNET file with the right version number
		*/
		position = root_buffer = new (std::nothrow) char [length_of_file - root_start];
		root = new (std::nothrow) ANT_thesaurus_rootnode[root_length_in_terms + 1];

		file->seek(root_start);
		file->read(root_buffer, length_of_file - root_start);

		if (root != NULL && root_buffer != NULL)
			{
			for (current = 0; current < root_length_in_terms; current++)
				{
				root[current].name = position;
				while (*position != '\0')
					position++;
				position++;			// skip over the '\0'
				root[current].start = ANT_get_long_long(position);
				position += sizeof(root[current].start);
				root[current].length = ANT_get_long_long(position);
				position += sizeof(root[current].length);
				}
			root[root_length_in_terms].name = NULL;
			root[root_length_in_terms].start = root[root_length_in_terms].length = 0;
/**/
			for (current = 0; current < root_length_in_terms; current++)
				printf("%s (%lld->%lld)\n", root[current].name, root[current].start, root[current].start + root[current].length);

/**/

			}
		}
	}
}
