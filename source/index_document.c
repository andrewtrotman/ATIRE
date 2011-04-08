/*
	INDEX_DOCUMENT.C
	----------------
*/
#include "btree_iterator.h"
#include "index_document.h"
#include "memory_indexer.h"
#include "stem.h"
#include "readability_factory.h"
#include "directory_iterator_object.h"
#include "string_pair.h"

/*
	INDEX_DOCUMENT()
	----------------
*/
long index_document(ANT_memory_indexer *indexer, ANT_stem *stemmer, long segmentation, ANT_readability_factory *readability, long long doc, ANT_directory_iterator_object *current_file)
{
char term[MAX_TERM_LENGTH + 1], token_stem_internals[MAX_TERM_LENGTH + 1];
ANT_string_pair *token;
long terms_in_document, length_of_token, length_of_previous_token, is_previous_token_chinese;
char *previous_token_start;

/*
	Initialise
*/
terms_in_document = 0;

/*
	Initialise the Chinese parser
*/
previous_token_start = NULL;
length_of_previous_token = 0;
is_previous_token_chinese = FALSE;

/*
	Index the file
*/
readability->set_document((unsigned char *)current_file->file);
while ((token = readability->get_next_token()) != NULL)
	{
//	printf("%*.*s\n", token->string_length, token->string_length, token->start);
	/*
	 * a bit redudant, the code below.
	 * I think the original code from revision 656 should be fine, except the chinese handling part
	 */
	if (ANT_islower(token->start[0]))
		{
		terms_in_document++;
		if (stemmer == NULL || token->string_length <= 3)
			readability->handle_node(indexer->add_term(token, doc));			// indexable the term
		else
			{
			token->strncpy(term, MAX_TERM_LENGTH);
			stemmer->stem(term, token_stem_internals);
			ANT_string_pair token_stem(token_stem_internals);
			readability->handle_node(indexer->add_term(&token_stem, doc));			// indexable the stem of the term
			}
		}
	else if (ANT_isdigit(token->start[0]))
		{
		terms_in_document++;
		readability->handle_node(indexer->add_term(token, doc));			// indexable term
		}
	else if (ANT_isupper(token->start[0]))
		readability->handle_node(indexer->add_term(token, doc));			// open tag
	else if ((token->start[0] & 0x80) && (ANT_parser::ischinese(token->start) || ANT_parser::iseuropean(token->start)))
		{
		terms_in_document++; // keep counting number of terms, but for dual indexing only number of the words is recorded
		readability->handle_node(indexer->add_term(token, doc));

		if (ANT_parser::ischinese(token->start))
			{
			if ((segmentation & ANT_parser::DOUBLE_SEGMENTATION) == ANT_parser::DOUBLE_SEGMENTATION && token->string_length > 4) // (> 4) means more than one character
				{
				// move a character forward, try not to re-index the last single character in previous bigram
				length_of_token = ANT_parser::utf8_bytes(token->start);
				if ((segmentation & ANT_parser::BIGRAM_SEGMENTATION) == ANT_parser::BIGRAM_SEGMENTATION && is_previous_token_chinese && previous_token_start != NULL && (token->start + length_of_token) == (previous_token_start + length_of_previous_token))
					{
					previous_token_start = token->start;
					length_of_previous_token = token->string_length;
					token->start += length_of_token;
					token->string_length -= length_of_token;
					}
				else
					{
					previous_token_start = token->start;
					length_of_previous_token = token->string_length;
					}

				while (token->string_length > 0)								// chinese
					{
					length_of_token = ANT_parser::utf8_bytes(token->start);
					ANT_string_pair next_character(token->start, length_of_token);
					readability->handle_node(indexer->add_term(&next_character, doc));
					token->start += length_of_token;
					token->string_length -= length_of_token;
					}
				}
			is_previous_token_chinese = TRUE;
			continue;
			}
		}
	is_previous_token_chinese = FALSE;
	previous_token_start = NULL;
	}

if (terms_in_document != 0)
	{
	indexer->set_document_length(doc, terms_in_document);
	readability->index(indexer);
	}

return terms_in_document;
}
