/*
	RELEVANCE_FEEDBACK.C
	--------------------
*/
#include <stdio.h>
#include "ctypes.h"
#include "relevance_feedback.h"
#include "memory.h"
#include "memory_index.h"
#include "memory_index_one.h"
#include "parser.h"
#include "search_engine.h"
#include "search_engine_result.h"
#include "stem.h"

/*
	ANT_RELEVANCE_FEEDBACK::ANT_RELEVANCE_FEEDBACK()
	------------------------------------------------
*/
ANT_relevance_feedback::ANT_relevance_feedback(ANT_search_engine *engine)
{
search_engine = engine;
memory = NULL;
indexer = NULL;
one = NULL;
parser = NULL;
document_buffer = NULL;
}

/*
	ANT_RELEVANCE_FEEDBACK::~ANT_RELEVANCE_FEEDBACK()
	-------------------------------------------------
*/
ANT_relevance_feedback::~ANT_relevance_feedback()
{
//delete memory;		// this is deleted when "one" is deleted
delete indexer;
delete one;
delete parser;
delete [] document_buffer;
}

/*
	ANT_RELEVANCE_FEEDBACK::REWIND()
	--------------------------------
*/
void ANT_relevance_feedback::rewind(void)
{
if (parser == NULL)
	{
	parser = new ANT_parser;
	memory = new ANT_memory;
	indexer = new ANT_memory_index(NULL);
	one = new ANT_memory_index_one(memory, indexer);
	}
else
	one->rewind();
}

/*
	ANT_RELEVANCE_FEEDBACK::ADD_TO_INDEX()
	--------------------------------------
*/
void ANT_relevance_feedback::add_to_index(char *document)
{
char term[MAX_TERM_LENGTH + 1], token_stem_internals[MAX_TERM_LENGTH + 1];
ANT_parser_token *token;
long long length_in_terms;

parser->set_document((unsigned char *)document);
length_in_terms = one->get_document_length();
while ((token = parser->get_next_token()) != NULL)
	{
	if (token->type == TT_WORD)
		{
		if (search_engine->stemmer == NULL || token->string_length < 3)
			one->add_term(token->normalized_pair());
		else
			{
			token->normalized_pair()->strncpy(term, MAX_TERM_LENGTH);
			search_engine->stemmer->stem(term, token_stem_internals);
			ANT_string_pair token_stem(token_stem_internals);
			one->add_term(&token_stem);
			}
		length_in_terms++;
		}
	}
one->set_document_length(1, length_in_terms);		// reset the length
}

/*
	ANT_RELEVANCE_FEEDBACK::POPULATE()
	----------------------------------
*/
void ANT_relevance_feedback::populate(ANT_search_engine_result *result, long documents_to_examine)
{
long long docid;
long long current, top_n;
double relevance;
unsigned long current_document_length;

rewind();

if (document_buffer == NULL)
	document_buffer = new char [search_engine->get_longest_document_length()];
top_n = documents_to_examine < result->results_list_length ? documents_to_examine : result->results_list_length;

for (current = 0; current < top_n; current++)
	{
	relevance = search_engine->results_list->accumulator_pointers[current]->get_rsv();
	docid = search_engine->results_list->accumulator_pointers[current] - search_engine->results_list->accumulator;

	current_document_length = search_engine->get_longest_document_length();
	if (search_engine->get_document(document_buffer, &current_document_length, docid) != NULL)
		add_to_index(document_buffer);
	}
}

/*
	ANT_RELEVANCE_FEEDBACK::POPULATE()
	----------------------------------
*/
void ANT_relevance_feedback::populate(char *document)
{
rewind();
add_to_index(document);
}