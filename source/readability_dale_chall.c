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
words_encountered = new char*[initial_size];
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
char *stoken;

if (token == '\0') return token;

stoken = token->str();

number_of_sentences += strcountchr(stoken, '.') + strcountchr(stoken, '?') + strcountchr(stoken, '!');

add_token(stoken);

//printf("%p --> \"", token);
//token->text_render();
//printf("\"\t%ld\t%p\n", number_of_sentences, &stoken);

return token;
}

/*
	ANT_READABILITY_DALE_CHALL::SCORE()
	-----------------------------------
*/
long ANT_readability_dale_chall::score()
{
char *token;
unsigned long start_of_lowercase = 0;

//printf("\n");
//printf("Number of sentences: %ld\n", number_of_sentences);
//printf("Number of words: %ld\n", number_of_words);

// sort the list of terms encountered
//qsort(words_encountered, number_of_words, sizeof(*words_encountered), scmp);

// find the first word that starts with lower case
//token = words_encountered[start_of_lowercase];
//while (ANT_isupper(token[0]))
//	{
//	start_of_lowercase++;
//	token = words_encountered[start_of_lowercase];
//	}

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
//	token = words_encountered[i];
//	if (i == start_of_lowercase)
//		printf("--------------------\n");
//	printf("%p --> \"%s\"\n", &token, token);
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
words_encountered = new char *[initial_size];
}

/*
	ANT_READABILITY_DALE_CHALL::ADD_TOKEN()
	----------------------------------------
*/
void ANT_readability_dale_chall::add_token(char *token)
{
if (number_of_words == size)
	{
	/*
		Expand the size of the list of hash nodes, to twice the size it currently it
	*/
	char **new_words = new char*[size * 2];
	memcpy(new_words, words_encountered, sizeof(char *) * size);
	size *= 2;
	delete [] words_encountered;
	words_encountered = new_words;
	}

// add this hash node/word to the list of encountered ones
words_encountered[number_of_words++] = token;
}
