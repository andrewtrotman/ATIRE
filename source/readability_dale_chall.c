/*
	READABILITY_DALE_CHALL.C
	------------------------
*/
#include <stdlib.h>
#include "str.h"
#include "readability_dale_chall.h"
#include "readability_dale_chall_wordlist.h"

/*
	ANT_READABILITY_DALE_CHALL::ANT_READABILITY_DALE_CHALL()
	--------------------------------------------------------
*/
ANT_readability_dale_chall::ANT_readability_dale_chall()
{
number_of_words = number_of_sentences = number_of_unfamiliar_words = 0;
size = initial_size;
words_encountered = new ANT_memory_index_hash_node *[initial_size];
}

/*
	ANT_READABILITY_DALE_CHALL::~ANT_READABILITY_DALE_CHALL()
	---------------------------------------------------------
*/
ANT_readability_dale_chall::~ANT_readability_dale_chall()
{
delete [] words_encountered;
}

/*
	ANT_READABILITY_DALE_CHALL::GET_NEXT_TOKEN()
	--------------------------------------------
*/
ANT_string_pair *ANT_readability_dale_chall::get_next_token()
{
ANT_string_pair *token = parser->get_next_token();
//char *stoken = token->str();
//
//number_of_sentences += strcountchr(stoken, '.') + strcountchr(stoken, '?') + strcountchr(stoken, '!');
//
// Getting a segfault with the above and can't track the cause down

return token;
}

/*
	ANT_READABILITY_DALE_CHALL::SCORE()
	-----------------------------------
*/
long ANT_readability_dale_chall::score()
{
ANT_string_pair *token;
unsigned long start_of_lowercase = 0;

//printf("\nNumber of sentences: %ld\n", number_of_sentences);

// sort the list of terms encountered
qsort(words_encountered, number_of_words, sizeof(*words_encountered), ANT_memory_index_hash_node::term_compare);

// find the first word that starts with lower case
token = &words_encountered[start_of_lowercase]->string;
while (ANT_isupper(token->start[0]))
	{
	start_of_lowercase++;
	token = &words_encountered[start_of_lowercase]->string;
	}

// for each uppercase word, see if it appears as a lowercase word
// if it does then check the lowercase version for familiarity
//        if lower is familiar: skip over all title
//        if lower is unfamiliar: add one to all title
// if it no lower, then we have a title (approximation) and nu++
//        skip over any other occurences of this titlecase word
// now zoom through the lowercase words and see if the word is unfamiliar or not
// and count the number of times that it appears
//for (unsigned int i = 0; i < number_of_words; i++)
//	{
//	token = &words_encountered[i]->string;
//	if (i == start_of_lowercase)
//		printf("--------------------\n");
//	printf("%p --> ", token);
//	token->text_render();
//	printf("\n");
//	}

// (0.049 * (nw / ns)) + (15.79 * (nu / nw)) + 3.6365
return 0;
}

/*
	ANT_READABILITY_DALE_CHALL::SET_DOCUMENT()
	------------------------------------------
*/
void ANT_readability_dale_chall::set_document(unsigned char *document)
{
parser->set_document(document);
number_of_sentences = number_of_words = number_of_unfamiliar_words = 0;

// Reset the list of words for this document
delete [] words_encountered;
words_encountered = new ANT_memory_index_hash_node *[initial_size];
}

/*
	ANT_READABILITY_DALE_CHALL::ADD_NODE()
	--------------------------------------
*/
void ANT_readability_dale_chall::add_node(ANT_memory_index_hash_node *node)
{
if (number_of_words == size)
	{
	/*
		Expand the size of the list of hash nodes, to twice the size it currently it
	*/
	ANT_memory_index_hash_node **new_words = new ANT_memory_index_hash_node *[size * 2];
	memcpy(new_words, words_encountered, sizeof(ANT_memory_index_hash_node *) * size);
	size *= 2;
	delete [] words_encountered;
	words_encountered = new_words;
	}

// add this hash node/word to the list of encountered ones
words_encountered[number_of_words++] = node;
}
