/*
	RANKING_FUNCTION_IMPACT.C
	-------------------------
*/
#include <math.h>
#include "pragma.h"
#include "ranking_function_impact.h"
#include "search_engine_btree_leaf.h"
#include "compress.h"
#include "search_engine_accumulator.h"

/*
	ANT_RANKING_FUNCTION_IMPACT::RELEVANCE_RANK_TOP_K()
	---------------------------------------------------
*/
void ANT_ranking_function_impact::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long docid, tf;
ANT_compressable_integer *current, *end;

current = impact_ordering;
end = impact_ordering + (term_details->document_frequency >= trim_point ? trim_point : term_details->document_frequency);
while (current < end)
	{
	end += 2;				// account for the impact_order and the terminator
	tf = *current++;		// note that it is too late to prescale and so we only postscale
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		accumulator->add_rsv(docid, postscalar * tf);
		}
	current++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ANT_RANKING_FUNCTION_IMPACT::RELEVANCE_RANK_TOP_K()
	---------------------------------------------------
*/
void ANT_ranking_function_impact::relevance_rank_top_k(ANT_search_engine_result *accumulator, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point)
{
long docid, tf;
ANT_compressable_integer *current, *end;

current = impact_ordering;
end = impact_ordering + (term_details->document_frequency >= trim_point ? trim_point : term_details->document_frequency);
while (current < end)
	{
	end += 2;		// account for the impact_order and the terminator
	tf = *current++;		// do the conversion from integer to float only once.
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		accumulator->add_rsv(docid, tf);
		}
	current++;
	}
}

/*
	ANT_RANKING_FUNCTION_IMPACT::RELEVANCE_RANK_BOOLEAN()
	-----------------------------------------------------
*/
void ANT_ranking_function_impact::relevance_rank_boolean(ANT_bitstring *documents_touched, ANT_search_engine_result *accumulators, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point, double prescalar, double postscalar)
{
long docid, tf;
ANT_compressable_integer *current, *end;

current = impact_ordering;
end = impact_ordering + (term_details->document_frequency >= trim_point ? trim_point : term_details->document_frequency);
while (current < end)
	{
	end += 2;				// account for the impact_order and the terminator
	tf = *current++;		// do the conversion from integer to float only once.
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		accumulators->add_rsv(docid, postscalar * tf);
		documents_touched->unsafe_setbit(docid);
		}
	current++;
	}
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}

/*
	ANT_RANKING_FUNCTION_IMPACT::RELEVANCE_RANK_BOOLEAN()
	-----------------------------------------------------
*/
void ANT_ranking_function_impact::relevance_rank_boolean(ANT_bitstring *documents_touched, ANT_search_engine_result *accumulators, ANT_search_engine_btree_leaf *term_details, ANT_compressable_integer *impact_ordering, long long trim_point)
{
long docid, tf;
ANT_compressable_integer *current, *end;

current = impact_ordering;
end = impact_ordering + (term_details->document_frequency >= trim_point ? trim_point : term_details->document_frequency);
while (current < end)
	{
	end += 2;				// account for the impact_order and the terminator
	tf = *current++;		// do the conversion from integer to float only once.
	docid = -1;
	while (*current != 0)
		{
		docid += *current++;
		accumulators->add_rsv(docid, tf);
		documents_touched->unsafe_setbit(docid);
		}
	current++;
	}
}

/*
	ANT_RANKING_FUNCTION_IMPACT::RANK()
	-----------------------------------
*/
double ANT_ranking_function_impact::rank(ANT_compressable_integer docid, ANT_compressable_integer length, unsigned char term_frequency, long long collection_frequency, long long document_frequency)
{
return (double)term_frequency;
#pragma ANT_PRAGMA_UNUSED_PARAMETER
}
