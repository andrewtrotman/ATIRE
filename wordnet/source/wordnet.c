/*
	WORDNET.C
	---------
*/
#include <stdio.h>
#include <stdlib.h>
#include <new>
#include "wordnet.h"
#include "thesaurus_rootnode.h"
#include "thesaurus_relationship.h"
#include "../../source/fundamental_types.h"
#include "../../source/file.h"
#include "../../source/file_memory.h"

/*
	ANT_WORDNET::ANT_WORDNET()
	--------------------------
*/
ANT_wordnet::ANT_wordnet(char *filename) : ANT_thesaurus(filename)
{
size_t file_tail_length;
uint64_t root_start, id_wordnet, length_of_longest_leaf, bytes_in_longest_leaf;
uint32_t id_version, id_ant;
long long length_of_file, current;
char *position;

leaf_buffer = NULL;
root = NULL;
root_buffer = NULL;
root_length_in_terms = 0;
synset = NULL;

file = new ANT_file();
if (file->open(filename, "rb") == false)
	{
	file->close();
	delete file;
	file = NULL;
	printf("Warning: Cannot open thesaurus file:%s (ignoring)\n", filename);

	return;
	}

file_tail_length = sizeof(id_ant) + sizeof(id_version) + sizeof(id_wordnet) + sizeof(root_start) + sizeof(root_length_in_terms) + sizeof(length_of_longest_leaf) + sizeof(bytes_in_longest_leaf);
if ((length_of_file = file->file_length()) > file_tail_length)
	{
	file->seek(length_of_file - file_tail_length);
	file->read(&root_start);
	file->read(&root_length_in_terms);
	file->read(&length_of_longest_leaf);
	file->read(&bytes_in_longest_leaf);

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
		leaf_buffer = new (std::nothrow) char [bytes_in_longest_leaf];
		synset = new (std::nothrow) ANT_thesaurus_relationship[length_of_longest_leaf + 1];

		file->seek(root_start);
		file->read(root_buffer, length_of_file - root_start);

		if (root != NULL && root_buffer != NULL && leaf_buffer != NULL)
			{
			for (current = 0; current < root_length_in_terms; current++)
				{
				root[current].name = position;
				while (*position != '\0')
					position++;
				position++;			// skip over the '\0'
				root[current].start = ANT_get_long_long(position);
				position += sizeof(uint64_t);
				root[current].length = ANT_get_long_long(position);
				position += sizeof(uint64_t);
				}
			root[root_length_in_terms].name = NULL;
			root[root_length_in_terms].start = root[root_length_in_terms].length = 0;
			}
		}

	return;				// this is the successful return, all other exit points are errors
	}

printf("Warning: The WORDNET thesaurus file appears be corrupt (ignoring)\n");
}

/*
	ANT_WORDNET::~ANT_WORDNET()
	---------------------------
*/
ANT_wordnet::~ANT_wordnet()
{
file->close();
delete file;

delete [] root;
delete [] root_buffer;
delete [] synset;
delete [] leaf_buffer;
}

/*
	ANT_WORDNET::GET_SYNSET()
	-------------------------
*/
ANT_thesaurus_relationship *ANT_wordnet::get_synset(char *term, long long *terms_in_synset)
{
long long terms = 0;
char *leaf_end, *current;
ANT_thesaurus_rootnode *got;
ANT_thesaurus_relationship *current_term, *head;

if ((got = (ANT_thesaurus_rootnode *)bsearch(term, root, root_length_in_terms, sizeof(*root), ANT_thesaurus_rootnode::string_compare)) == NULL)
	head = NULL;
else
	{
	file->seek(got->start);
	file->read(leaf_buffer, got->length);

	leaf_end = leaf_buffer + got->length;
	current_term = head = synset;
	terms = 0;
	for (current = leaf_buffer; current < leaf_end; current++)
		{
		terms++;
		current_term->relationship = *current++;						// first is the relationship;
		current_term->term = current;									// then the term
		current = strchr(current, '\0');								// skip over the term
		current_term++;
		}
	current_term->relationship = ANT_thesaurus_relationship::SENTINAL;	// end of the list
	current_term->term = NULL;											// is not a string
	}

if (terms_in_synset != NULL)
	*terms_in_synset = terms;

return head;
}

