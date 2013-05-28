/*
	RANKING_FUNCTION_TOPSIG.H
	-------------------------
	This is the base (shared) class stuff for the TopSig ranking functions
	in particular, it computes the length normalisation,
*/
#ifndef RANKING_FUNCTION_TOPSIG_H_
#define RANKING_FUNCTION_TOPSIG_H_

#include "ranking_function.h"

/*
	class ANT_RANKING_FUNCTION_TOPSIG
	---------------------------------
*/
class ANT_ranking_function_topsig : public ANT_ranking_function
{
protected:
	short *document_prior_probability;
	
private:
	void compute_prior_probabilities(long width)
	{
	long current;
	double log_mean_document_length = log((double)mean_document_length);
	double alpha = 0.015 * width;

	document_prior_probability = new short [(size_t)documents_as_integer];
	for (current = 0; current < documents_as_integer; current++)
		document_prior_probability[current] =  (short)(alpha * (log((double)document_lengths[current]) / log_mean_document_length));
	}

public:
	ANT_ranking_function_topsig(ANT_search_engine *engine, long quantize, long long quantization_bits, long width) : ANT_ranking_function(engine, quantize, quantization_bits) { compute_prior_probabilities(width); }
	ANT_ranking_function_topsig(long long documents, ANT_compressable_integer *document_lengths, long long quantization_bits) : ANT_ranking_function(documents, document_lengths, quantization_bits) {}
	~ANT_ranking_function_topsig() { delete [] document_prior_probability; }
} ;

#endif /* RANKING_FUNCTION_TOPSIG_H_ */
