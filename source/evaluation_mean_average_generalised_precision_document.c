/*
	EVALUATION_MEAN_AVERAGE_GENERALISED_PRECISION_DOCUMENT.C
	--------------------------------------------------------
*/
#include <stdlib.h>
#include "evaluation_mean_average_generalised_precision_document.h"
#include "search_engine_result_iterator.h"
#include "relevant_subtopic.h"
#include "relevant_document.h"

/*
	ANT_EVALUATION_MEAN_AVERAGE_GENERALISED_PRECISION_DOCUMENT::EVALUATE()
	----------------------------------------------------------------------
	MAgP computed for whole documents
*/
double ANT_evaluation_mean_average_generalised_precision_document::evaluate(ANT_search_engine *search_engine, long topic, long subtopic)
{
ANT_search_engine_result_iterator iterator;
ANT_relevant_subtopic *got;
ANT_relevant_document key, *relevance_data;
long long current;
double precision, doc_precision, doc_recall, doc_f_score, found_and_relevant;
const double beta = 0.25;

if ((got = setup(topic, subtopic)) == NULL)
	return 0;
if (got->number_of_relevant_documents == 0)
	return 0;

key.topic = topic;
key.subtopic = subtopic;

found_and_relevant = precision = 0;
current = 0;
for (key.docid = iterator.first(search_engine); key.docid >= 0 && current < precision_point; key.docid = iterator.next())
	{
	current++;
	if ((relevance_data = (ANT_relevant_document *)bsearch(&key, got->document_list, (size_t)got->number_of_documents, sizeof(*got->document_list), ANT_relevant_document::compare)) != NULL)
		{
		/*
			We have an assessed document
		*/
		if (relevance_data->relevant_characters != 0)
			{
			/*
				And that document is assessed as relevant
			*/
			doc_precision = (double)relevance_data->relevant_characters / (double)relevance_data->document_length;
			doc_recall = 1.0;		// we retrieve the whole document so recall is 1.
			doc_f_score = (1.0 + beta * beta ) * (doc_precision  * doc_recall) / (beta * beta * doc_precision + doc_recall);
			found_and_relevant += doc_f_score;
			precision += (double)found_and_relevant / current;
			}
		}
	}

return precision / (double)got->number_of_relevant_documents;
}
