/*
	THESAURUS_WORDNET.C
	-------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <new>
#include "thesaurus_wordnet.h"
#include "thesaurus_rootnode.h"
#include "thesaurus_relationship.h"
#include "fundamental_types.h"
#include "file.h"
#include "file_memory.h"

/*
	ANT_THESAURUS_WORDNET::ANT_THESAURUS_WORDNET()
	----------------------------------------------
*/
ANT_thesaurus_wordnet::ANT_thesaurus_wordnet(char *filename) : ANT_thesaurus(filename)
{
size_t file_tail_length;
uint64_t root_start, id_wordnet, length_of_longest_leaf, bytes_in_longest_leaf;
uint32_t id_version, id_ant;
size_t length_of_file;
unsigned long long current;
char *position;

/*
	Initialise
*/
leaf_buffer = NULL;
root = NULL;
root_buffer = NULL;
root_length_in_terms = 0;
synset = NULL;

/*
	Its necessary to use an ANT_file_memory object rather than an ANT_file object
	because query expansion requires the expanded terms to be statically allocated
	and the only way to do that is to use the strings in the file themselves
*/
file = new ANT_file_memory();
if (file->open(filename, "rb") == false)
	{
	file->close();
	delete file;
	file = NULL;
	printf("Warning: Cannot open thesaurus file:%s (ignoring)\n", filename);

	return;
	}

/*
	Read the header and verify that we have a thesaurus file
*/
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
		position = root_buffer = new (std::nothrow) char [(size_t)(length_of_file - root_start)];
		root = new (std::nothrow) ANT_thesaurus_rootnode[(size_t)(root_length_in_terms + 1)];
		leaf_buffer = new (std::nothrow) char [(size_t)bytes_in_longest_leaf];
		synset = new (std::nothrow) ANT_thesaurus_relationship[(size_t)(length_of_longest_leaf + 1)];

		/*
			Load the root node of the tree
		*/
		file->seek(root_start);
		file->read(root_buffer, length_of_file - root_start);

		/*
			Now decode the header
		*/
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
	ANT_THESAURUS_WORDNET::~ANT_THESAURUS_WORDNET()
	-----------------------------------------------
*/
ANT_thesaurus_wordnet::~ANT_thesaurus_wordnet()
{
file->close();
delete file;

delete [] root;
delete [] root_buffer;
delete [] synset;
delete [] leaf_buffer;
}

/*
	ANT_THESAURUS_WORDNET::GET_SYNSET()
	-----------------------------------
*/
ANT_thesaurus_relationship *ANT_thesaurus_wordnet::get_synset(char *term, long long *terms_in_synset)
{
long long terms = 0;
char *leaf_end, *leaf_start, *current;
ANT_thesaurus_rootnode *got;
ANT_thesaurus_relationship *current_term, *head;

if (root == NULL)
	head = NULL;
else if ((got = (ANT_thesaurus_rootnode *)bsearch(term, root, (size_t)root_length_in_terms, sizeof(*root), ANT_thesaurus_rootnode::string_compare)) == NULL)
	head = NULL;
else
	{
	leaf_start = leaf_buffer;
	file->seek(got->start);

	//file->direct_read((unsigned char **)&leaf_start, got->length);
	leaf_start = (char *)file->read_return_ptr((unsigned char *)leaf_start, got->length);

	leaf_end = leaf_start + got->length;
	current_term = head = synset;
	terms = 0;
	for (current = leaf_start; current < leaf_end; current++)
		{
		current_term->relationship = *current++;						// first is the relationship;
		current_term->term = current;									// then the term
		current = strchr(current, '\0');								// skip over the term

		if (allowable_relationship(current_term->relationship))
			{
			current_term++;
			terms++;
			}
		}
	current_term->relationship = 0;	// end of the list
	current_term->term = NULL;		// is not a string
	}

if (terms_in_synset != NULL)
	*terms_in_synset = terms;

return head;
}

