/*
	EVALUATION_MEAN_AVERAGE_GENERALISED_PRECISION_DOCUMENT.H
	--------------------------------------------------------
*/
#ifndef EVALUATION_MEAN_AVERAGE_GENERALISED_PRECISION_DOCUMENT_H_
#define EVALUATION_MEAN_AVERAGE_GENERALISED_PRECISION_DOCUMENT_H_

#include "evaluation.h"

/*
	class ANT_evaluation_mean_average_generalised_precision_document
	----------------------------------------------------------------
*/
class ANT_evaluation_mean_average_generalised_precision_document : public ANT_evaluation
{
public:
	double evaluate(ANT_search_engine *search_engine, long topic, long subtopic = 0);
} ;

#endif /* EVALUATION_MEAN_AVERAGE_GENERALISED_PRECISION_DOCUMENT_H_ */
