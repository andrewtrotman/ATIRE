/*
	READABILITY_DALE_CHALL.C
	------------------------
*/
#include <stdlib.h>
#include "str.h"
#include "readability_dale_chall.h"
#include "readability_dale_chall_wordlist.h"

ANT_string_pair ANT_readability_dale_chall::measure_name("~dalechall");

/*
	ANT_READABILITY_DALE_CHALL::ANT_READABILITY_DALE_CHALL()
	--------------------------------------------------------
*/
ANT_readability_dale_chall::ANT_readability_dale_chall()
{
number_of_words = number_of_sentences = number_of_unfamiliar_words = 0;
/*
	Biggest document in wiki07/08 is 65003 so we set the constant to something larger
	so that in that collection (and smaller) it never needs to expand.  In larger
	collections it will automatically expand.
*/
size = 65005; 
words_encountered = new word[size];
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
	ANT_READABILITY_DALE_CHALL::HANDLE_TOKEN()
	------------------------------------------
*/
void ANT_readability_dale_chall::handle_token(ANT_parser_token *token)
{
char *str;

if (token == NULL)
	return;

/*
	If we've been given just sentence endings, add them, then get next token
*/
if (ANT_parser_readability::issentenceend(*token->start))
	{
	number_of_sentences += token->length();
	return;
	}

/*
	If we've been given a tag, remove the starting < and pass it on
*/
if (*token->start == '<')
	return;

last_was_title = ANT_isupper(*token->start);

str = token->str();
number_of_sentences += strcountchr(str, '.');
number_of_sentences += strcountchr(str, '!');
number_of_sentences += strcountchr(str, '?');
delete str;
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
long ANT_readability_dale_chall::score(void)
{
word *prev, *curr;
unsigned int i;
unsigned long istitle;
unsigned long wordlist_position = 0;
unsigned long term_frequency;
int comparison = 1;			// non-zero

/*
	Blank (empty) document therefore no score.
*/
if (number_of_words == 0)
	return 0;

/* 
	sort the list of terms encountered
*/
qsort(words_encountered, number_of_words, sizeof(*words_encountered), ANT_readability_dale_chall::word_cmp);

prev = &words_encountered[0];
istitle = prev->istitle;

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

		/*
			Compare the term to those in the familiar wordlist.

			While we have words in the wordlist, play "catch up" 
			to the term we're looking at.
		*/
		while (wordlist_position < ANT_readability_dale_chall_wordlist_length && 
			(comparison = prev->node->string.true_strcmp(ANT_readability_dale_chall_wordlist[wordlist_position])) > 0)
			{
			wordlist_position++;
			}

		if (comparison == 0) 
			{
			/*
				found a familiar word so skip over it in the wordlist
			*/
			wordlist_position++;
			}
		else
			{
			term_frequency = (long)prev->node->term_frequency;
			/*
				if it's only occured as title-case then it's a name
				cheap/fugly approximaion
			*/
			number_of_unfamiliar_words += istitle == term_frequency ? 1 : term_frequency;
			}
		
		/*
			reset things for the next word
		*/
		prev = curr;
		istitle = prev->istitle;
		}
	}

/*
	Finally, deal with the last term in the list, it won't be dealt with until now even if it's repeated.
*/
while (wordlist_position < ANT_readability_dale_chall_wordlist_length && 
	(comparison = prev->node->string.true_strcmp(ANT_readability_dale_chall_wordlist[wordlist_position])) > 0)
	wordlist_position++;

if (comparison != 0)
	{
	term_frequency = (long)prev->node->term_frequency;
	number_of_unfamiliar_words += istitle == term_frequency ? 1 : term_frequency;
	}

/*
	avoid division by zero errors
*/
if (number_of_sentences == 0)
	number_of_sentences = 1;

return (long)(1000 * ((0.049 * number_of_words / number_of_sentences) + (15.79 * number_of_unfamiliar_words / number_of_words) + 3.6365));
}

/*
	ANT_READABILITY_DALE_CHALL::HANDLE_NODE()
	-----------------------------------------
*/
void ANT_readability_dale_chall::handle_node(ANT_memory_indexer_node *node)
{
/*
	If we've been given a tag or number to deal with, then just ignore it.
*/
if (ANT_isupper(node->string[0]) || ANT_isdigit(node->string[0]))
	return;

/*
	If it's on the suffix list then ignore it.

	Couldn't get bsearch to work - it was comparing node->string with garbage.
*/
for (unsigned long i = 0; i < ANT_readability_dale_chall_suffixlist_length; i++)
	if (node->string.true_strcmp(ANT_readability_dale_chall_suffixlist[i]) == 0)
		return;

/*
	If we've run out of room then expand the size of the list of hash nodes to 
	twice the size it currently is
*/
if (number_of_words == size)
	{
	word *new_words = new word[size * 2];
	memcpy(new_words, words_encountered, sizeof(*words_encountered) * size);
	size *= 2;
	delete [] words_encountered;
	words_encountered = new_words;
	}

/*
	Add the word to our list of words
*/
words_encountered[number_of_words].node = node;
words_encountered[number_of_words].istitle = last_was_title;
number_of_words++;
}

/*
	ANT_READABILITY_DALE_CHALL::INDEX()
	-----------------------------------
*/
void ANT_readability_dale_chall::index(ANT_memory_indexer *index, long long doc)
{
index->set_document_detail(&measure_name, score());

number_of_sentences = number_of_words = number_of_unfamiliar_words = 0;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
