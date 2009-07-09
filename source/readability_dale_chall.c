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
words_encountered = new word[initial_size];
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
unsigned long i;

if (token == NULL)
	return token;

last_was_title = ANT_isupper(*token->start);

while (ANT_parser_readability::issentenceend(token->start[token->length()-1]))
	{
	number_of_sentences++;
	token->string_length--;
	}

for (i = 0; i < token->length(); i++)
	token->start[i] = ANT_tolower(token->start[i]);

return token;
}

/*
	ANT_READABILITY_DALE_CHALL::WORD_CMP()
	--------------------------------------
*/
int ANT_readability_dale_chall::word_cmp(const void *a, const void *b)
{
	word *x = (word *)a;
	word *y = (word *)b;
	return x->node->string.true_strcmp(&y->node->string);
}

/*
	ANT_READABILITY_DALE_CHALL::SCORE()
	-----------------------------------
*/
long ANT_readability_dale_chall::score()
{
word *prev, *curr;
unsigned long istitle = 0;
unsigned int term_frequency;
unsigned long wordlist_position = 0;
unsigned int i = 0;
int comparison_prev, comparison_curr;

//printf("\n");
//printf("Number of sentences: %ld\n", number_of_sentences);
//printf("Number of words: %ld\n", number_of_words);
//printf("Number of words in wordlist: %ld\n", ANT_readability_dale_chall_wordlist_length);
//printf("\n");

// sort the list of terms encountered
qsort(words_encountered, number_of_words, sizeof(*words_encountered), ANT_readability_dale_chall::word_cmp);

prev = &words_encountered[0];
istitle += prev->istitle;

for (i = 1; i < number_of_words; i++)
	{
	curr = &words_encountered[i];

	/*
		If we're pointing to the same hashnode as before then just update
		the number of times it's been seen as title case and move onto the
		next one.
	*/
	if (prev->node == curr->node)
		{
		istitle += curr->istitle;
		prev = curr;
		}
	/*
		Otherwise, we have finished looking at all occurrences of the previous
		term so we need to deal with it.
	*/
	else
		{
		term_frequency = prev->node->tf_list_tail->data[prev->node->tf_node_used - 1];
		
		/*
			Compare the term to those in the familiar wordlist.
			If it's not in there, then nu += istitle ? 1 : term_frequency
		*/
		while (wordlist_position < ANT_readability_dale_chall_wordlist_length && 
		(comparison_prev = prev->node->string.strcmp(ANT_readability_dale_chall_wordlist[wordlist_position])) > 0 && // want the wordlist to come after previous (string > wordlist)
		(comparison_curr = curr->node->string.strcmp(ANT_readability_dale_chall_wordlist[wordlist_position])) > 0) // and before current (string < wordlist)
			{
			wordlist_position++;
			}
		if (comparison_prev == 0)
			{
			// found a familiar word
			// skip over the word in the wordlist
			wordlist_position++;
			}
		else
			{
			// found an unfamiliar word
			number_of_unfamiliar_words += istitle ? 1 : term_frequency;
			}
		
		// reset things for the new word
		prev = curr;
		istitle = prev->istitle;
		}
	}

/*
	Finally, deal with the last term in the list, it won't be dealt with until now even if it's repeated.
*/
while (wordlist_position < ANT_readability_dale_chall_wordlist_length && (comparison_prev = prev->node->string.strcmp(ANT_readability_dale_chall_wordlist[wordlist_position])) > 0)
	{
	wordlist_position++;
	}
if (comparison_prev != 0)
	{
	// found an unfamiliar word
	number_of_unfamiliar_words += istitle ? 1 : term_frequency;
	}

// avoid division by zero errors
if (number_of_words == 0)
	number_of_words = 1;
if (number_of_sentences == 0)
	number_of_sentences = 1;

//printf("\n");
//printf("Score = (0.049 * %ld / %ld) + (15.79 * %ld / %ld) + 3.6365\n", number_of_words, number_of_sentences, number_of_unfamiliar_words, number_of_words);
//printf("Score = %.4f\n", (0.049 * number_of_words / number_of_sentences) + (15.79 * number_of_unfamiliar_words / number_of_words) + 3.6365);
return (long)(1000 * ((0.049 * number_of_words / number_of_sentences) + (15.79 * number_of_unfamiliar_words / number_of_words) + 3.6365));
}

/*
	ANT_READABILITY_DALE_CHALL::SET_DOCUMENT()
	------------------------------------------
*/
void ANT_readability_dale_chall::set_document(unsigned char *document)
{
parser->set_document(document);
number_of_sentences = number_of_words = number_of_unfamiliar_words = 0;
// don't reset the wordlist - might need expanding again, cheaper to just reuse
// the same space from the beginning
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
	word *new_words = new word[size * 2];
	memcpy(new_words, words_encountered, sizeof(word) * size);
	size *= 2;
	delete [] words_encountered;
	words_encountered = new_words;
	}

// add this hash node/word to the list of encountered ones
words_encountered[number_of_words].node = node;
words_encountered[number_of_words].istitle = last_was_title;
number_of_words++;
}

/*
	ANT_READABILITY_DALE_CHALL::INDEX()
	-----------------------------------
*/
void ANT_readability_dale_chall::index(long long docno, ANT_memory_index *index)
{
ANT_string_pair measure_name("~dalechall", 10);
index->set_document_readability(docno, score(), &measure_name);
}
