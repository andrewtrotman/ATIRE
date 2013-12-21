/*
	EVALUATION_RANK_EFFECTIVENESS.C
	-------------------------------
*/
#include <stdlib.h>
#include "evaluation_rank_effectiveness.h"
#include "search_engine_result_iterator.h"
#include "relevant_subtopic.h"
#include "relevant_document.h"

/*
	ANT_EVALUATION_RANK_EFFECTIVENESS::EVALUATE()
	---------------------------------------------
	The implementation here breaks from the definition given in [1]. In the given definition,
	ranking all the relevant documents before all irrelevant would result in a score of 0.0,
	and vice versa, a score of 1.0. The text and resulting results suggest that this is wrong.
	So we add the "1.0 -" to give the correct scores, as shown in [2].

	[1] Grönqvist L. (2005), Evaluating Latent Semantic Vector Models with Synonym Tests and Document Retrieval, ELECTRA Workshop: Methodologies and Evaluation of Lexical Cohesion Techniques in Real-World Applications Beyond Bag of Words.
	[2] Büttcher S., Clarke C.L.A, Cormack G.V. (2010) Information Retrieval: Implementing and Evaluating Search Engines, p452.
*/
double ANT_evaluation_rank_effectiveness::evaluate(ANT_search_engine *search_engine, long topic, long *valid, long subtopic)
{
ANT_search_engine_result_iterator iterator;
ANT_relevant_subtopic *got;
ANT_relevant_document key, *relevance_data;
long long found_and_nonrelevant, total_nonrelevant, current;
double precision;


if ((got = setup(topic, subtopic)) == NULL)
	{
	*valid = false;
	return 0;
	}

*valid = true;

if ((total_nonrelevant = got->number_of_nonrelevant_documents) == 0)
	return 1;	// topic has no non-relevant documents so they are all relevant so we score a perfect score

key.topic = topic;
key.subtopic = subtopic;

precision = 0;
current = 0;
found_and_nonrelevant = 0;
#ifdef FILENAME_INDEX
for (key.docid = iterator.first(search_engine); key.docid != NULL && current < precision_point; key.docid = iterator.next(), current++)
#else
for (key.docid = iterator.first(search_engine); key.docid >= 0 && current < precision_point; key.docid = iterator.next(), current++)
#endif
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, got->document_list, (size_t)got->number_of_documents, sizeof(*got->document_list), ANT_relevant_document::compare)) != NULL)
		if (relevance_data->relevant_characters == 0)
			found_and_nonrelevant++;
		else
			precision += 1.0 - ((double)found_and_nonrelevant / (double)total_nonrelevant);

return precision / got->number_of_relevant_documents;
}
