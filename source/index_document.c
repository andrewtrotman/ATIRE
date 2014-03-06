/*
	INDEX_DOCUMENT.C
	----------------
*/
#include <math.h>
#include "maths.h"
#include "btree_iterator.h"
#include "index_document.h"
#include "memory_indexer.h"
#include "stem.h"
#include "readability_factory.h"
#include "directory_iterator_object.h"
#include "string_pair.h"
#include "parser_token.h"
#include "unicode.h"

/*
	ANT_INDEX_DOCUMENT::INDEX_DOCUMENT()
	------------------------------------
*/
long ANT_index_document::index_document(ANT_memory_indexer *indexer, ANT_stem *stemmer, long segmentation, ANT_readability_factory *readability, long long doc, unsigned char *file)
{
char term[MAX_TERM_LENGTH + 1], token_stem_internals[MAX_TERM_LENGTH + 1];
ANT_parser_token *token;
long terms_in_document, length_of_token, is_previous_token_chinese;
size_t length_of_previous_token;
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
readability->set_document(file);
while ((token = readability->get_next_token()) != NULL)
	{
	//	printf("%*.*s\n", token->string_length, token->string_length, token->start);
	/*
	 * a bit redudant, the code below.
	 * I think the original code from revision 656 should be fine, except the chinese handling part
	 */
	/*
		Discard super long terms that search can't handle
	*/
	if (token->length() > MAX_TERM_LENGTH)
		continue;
	switch (token->type)
		{
		case TT_WORD:
			terms_in_document++;

			if (ischinese(token->start))
				{
				readability->handle_node(indexer->add_term(token, doc));
				
				if ((segmentation & ANT_parser::DOUBLE_SEGMENTATION) == ANT_parser::DOUBLE_SEGMENTATION && token->string_length > 4) // (> 4) means more than one character
					{
					// move a character forward, try not to re-index the last single character in previous bigram
					length_of_token = utf8_bytes(token->start);
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
						length_of_token = utf8_bytes(token->start);
						ANT_string_pair next_character(token->start, length_of_token);
						readability->handle_node(indexer->add_term(&next_character, doc));
						token->start += length_of_token;
						token->string_length -= length_of_token;
						}
					}
				is_previous_token_chinese = TRUE;
				}
			else
				{
				is_previous_token_chinese = FALSE;
				previous_token_start = NULL;

				if ((stopword_mode & ANT_memory_index::PRUNE_STOPWORDS_BEFORE_INDEXING) && indexer->stopwords->isstop(token->normalized_pair()->string(), token->normalized_pair()->length()))
					break;

				if (stemmer == NULL || token->string_length <= 3)
					readability->handle_node(indexer->add_term(token->normalized_pair(), doc));			// indexable the term
				else
					{
					token->normalized_pair()->strncpy(term, MAX_TERM_LENGTH);
					stemmer->stem(term, token_stem_internals);
					ANT_string_pair token_stem(token_stem_internals);
					readability->handle_node(indexer->add_term(&token_stem, doc));				// indexable the stem of the term
					}
				}
			break;
		case TT_NUMBER:
			if ((stopword_mode & ANT_memory_index::PRUNE_NUMBERS) != 0)
				break;
			terms_in_document++;
			readability->handle_node(indexer->add_term(token->normalized_pair(), doc));			// indexable term
			break;
		case TT_TAG_OPEN:
			if ((stopword_mode & ANT_memory_index::PRUNE_TAGS) == 0)
				readability->handle_node(indexer->add_term(token, doc));						// open tag
			readability->handle_tag(token, TRUE);
			break;
		case TT_TAG_CLOSE:
			readability->handle_tag(token, FALSE);
			break;
		case TT_PUNCTUATION:
			//no-op
			break;
		default:
			//no-op
			break;
		}
	}

if (terms_in_document != 0)
	{
	/*
		Set the true length
	*/
	indexer->set_document_length(doc, terms_in_document);
	readability->index(indexer, doc);
	}

return terms_in_document;
}
