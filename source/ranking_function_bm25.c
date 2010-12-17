/*
	RANKING_FUNCTION_BM25.C
	-----------------------
*/
#include <math.h>
#include "pragma.h"
#include "ranking_function_bm25.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

/*
	ANT_RANKING_FUNCTION_BM25::INIT()
	---------------------------------
*/
void ANT_ranking_function_BM25::init(double k1, double b)
{
const double one_minus_b = 1.0 - b;
long current;

this->k1 = k1;
this->b = b;

/*
	Precompute the length normalisation prior probability part of BM25
*/
document_prior_probability = new float [(size_t)documents_as_integer];
for (current = 0; current < documents_as_integer; current++)
	document_prior_probability[current] =  (float)(k1 * (one_minus_b + b * (document_lengths[current] / mean_document_length)));
}

/*
	ANT_RANKING_FUNCTION_BM25::~ANT_RANKING_FUNCTION_BM25()
	-------------------------------------------------------
*/
ANT_ranking_function_BM25::~ANT_ranking_function_BM25()
{
delete [] document_prior_probability;
}

/*
	ANT_RANKING_FUNCTION_BM25::RELEVANCE_RANK_TOP_K()
	-------------------------------------------------
*/
void ANT_ranking_function_BM25::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
const double k1 = this->k1;
const double k1_plus_1 = k1 + 1.0;
long docid;
double top_row, tf, idf;
ANT_compressable_integer *current, *end;

/*
	          N
	IDF = log -
	          n

	This variant of IDF is better than log((N - n + 0.5) / (n + 0.5)) on the 70 INEX 2008 Wikipedia topics
*/
idf = log((double)documents / (double)term_details->document_frequency);

/*
	               tf(td) * (k1 + 1)
	rsv = ----------------------------------- * IDF
	                                  len(d)
	      tf(td) + k1 * (1 - b + b * --------)
                                    av_len_d

	In this implementation we ignore k3 and the number of times the term occurs in the query.
*/
/*
	Alright, so, we have the trim_point at this point.  The trim point is an optimising factor.  It is the
	number of postings we should process before early-terminating.  As the postings are stored in impact
	order (typically from highest to lowest term frequency), the first posting is most probably the most
	relevant document and so on.  Oh, but as there will be multiple postings with the same impact score
	and we don't know which is "best", the early termination actually only happens at the end of processing
	one complete impact value's postings list.

	So how do we compute where to end?  The impacted list is of length:document frequencies plus the impacts 
	plus the terminators.  So we can compute the ending point from document frequency if we add to that pointer
	for each impact and each zero.  Now that we can compare document frequency, we can compare to the trim_point
	measured in postings and early terminate after we have processed that "batch" of postings.
*/
current = impact_ordering;
end = impact_ordering + (term_details->document_frequency >= trim_point ? trim_point : term_details->document_frequency);
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++;
	top_row = prescalar * tf * k1_plus_1;
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		accumulator->add_rsv(docid, postscalar * idf * (top_row / (prescalar * tf + document_prior_probability[docid])));
		}
	current++;		// skip over the zero
	}
}

/*
	ANT_RANKING_FUNCTION_BM25::RELEVANCE_RANK_BOOLEAN()
	---------------------------------------------------
	This is an exact copy of the relevance_rank_top_k() function except that it also flips the
	sits in the documents_touched bitstring.
*/
void ANT_ranking_function_BM25::relevance_rank_boolean(ANT_bitstring *documents_touched, ANT_search_engine_result *accumulators, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
const double k1 = this->k1;
const double k1_plus_1 = k1 + 1.0;
long docid;
double top_row, tf, idf;
ANT_compressable_integer *current, *end;

idf = log((double)documents / (double)term_details->document_frequency);
current = impact_ordering;
end = impact_ordering + (term_details->document_frequency >= trim_point ? trim_point : term_details->document_frequency);
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++;
	top_row = prescalar * tf * k1_plus_1;
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		accumulators->add_rsv(docid, postscalar * idf * (top_row / (prescalar * tf + document_prior_probability[docid])));
		documents_touched->unsafe_setbit(docid);
		}
	current++;		// skip over the zero
	}
}

/*
	ANT_RANKING_FUNCTION_BM25::RANK()
	---------------------------------
*/
double ANT_ranking_function_BM25::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency)
{
const double k1_plus_1 = k1 + 1.0;
double idf, tf, rsv;

tf = (double)term_frequency;
idf = log((double)documents / (double)document_frequency);
rsv = idf * ((tf * k1_plus_1) / (tf + document_prior_probability[docid]));
return rsv;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
