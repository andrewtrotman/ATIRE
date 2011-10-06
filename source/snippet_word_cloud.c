/*
	SNIPPET_WORD_CLOUD.C
	--------------------
*/
#include "ctypes.h"
#include "parser.h"
#include "snippet_word_cloud.h"
#include "relevance_feedback_blind_kl.h"
#include "memory_index_one_node.h"

/*
	ANT_SNIPPET_WORD_CLOUD()
	------------------------
*/
ANT_snippet_word_cloud::ANT_snippet_word_cloud(unsigned long max_length, long length_of_longest_document, ANT_search_engine *engine, ANT_stem *stemmer, long terms) : ANT_snippet(max_length, length_of_longest_document, engine, stemmer)
{
cloud_maker = new ANT_relevance_feedback_blind_kl(engine);
terms_to_generate = terms;
}

/*
	ANT_SNIPPET_WORD_CLOUD()
	------------------------
*/
ANT_snippet_word_cloud::~ANT_snippet_word_cloud()
{
delete cloud_maker;
}

/*
	ANT_SNIPPET_WORD_CLOUD::GET_SNIPPET()
	-------------------------------------
*/
char *ANT_snippet_word_cloud::get_snippet(char *snippet, char *document)
{
long terms_generated;
ANT_memory_index_one_node **top_terms, **current;
char *into, *end;

/*
	Generate the top 100 terms from the KL divergence of the document and the collection
*/
top_terms = cloud_maker->feedback(document, terms_to_generate, &terms_generated);

/*
	now copy as many as you can into the snippet (in order of importance) putting a space between each one
*/
into = snippet;
end = snippet + maximum_snippet_length;

for (current = top_terms; *current != NULL; current++)
	{
	if (into + (*current)->string.length() + 2 < end)	// +1 for the '\0'
		{
		memcpy(into, (*current)->string.string(), (*current)->string.length());
		into += (*current)->string.length();
		*into++ = ' ';
		}
	}

/*
	turn the last space into a string terminator
*/
if (into == snippet)
	*into = '\0';
else
	*(into - 1) = '\0';

/*
	done
*/
return snippet;
}
