/*
	SEARCH_ENGINE_READABILITY.C
	---------------------------
*/
#include <math.h>
#include "search_engine_readability.h"
#include "memory.h"
#include "search_engine_btree_leaf.h"
#include "search_engine_accumulator.h"
#include "compress_variable_byte.h"

/*
	ANT_SEARCH_ENGINE_READABILITY::ANT_SEARCH_ENGINE_READABILITY()
	--------------------------------------------------------------
*/
ANT_search_engine_readability::ANT_search_engine_readability(ANT_memory *memory) : ANT_search_engine(memory)
{
long long current_readability;
ANT_search_engine_btree_leaf collection_details;
ANT_compress_variable_byte variable_byte;

document_readability = (long *)memory->malloc(documents * sizeof(*document_readability));

/*
	read and decompress the readability vector
*/
get_postings_details("~dalechall", &collection_details);
get_postings(&collection_details, postings_buffer);
variable_byte.decompress(posting.docid, postings_buffer, collection_details.document_frequency);

hardest_document = 0;

for (current_readability = 0; current_readability < collection_details.document_frequency; current_readability++)
	{
	document_readability[current_readability] = posting.docid[current_readability];
	if (document_readability[current_readability] > hardest_document)
		hardest_document = document_readability[current_readability];
	}

memory->realign();
}

/*
	ANT_SEARCH_ENGINE_READABILITY::RELEVANCE_RANK()
	-----------------------------------------------
*/
void ANT_search_engine_readability::relevance_rank(ANT_search_engine_btree_leaf *term_details, ANT_search_engine_posting *postings)
{
const double k1 = 0.9;
const double b = 0.4;
const double k1_plus_1 = k1 + 1.0;
const double one_minus_b = 1.0 - b;
long docid, which;
double tf, idf, bm25;
docid = -1;

idf = log((double)(documents) / (double)term_details->document_frequency);

for (which = 0; which < term_details->document_frequency; which++)
	{
	docid = postings->docid[which];
	tf = postings->tf[which];
	
	bm25 = idf * ((tf * k1_plus_1) / (tf + k1 * (one_minus_b + b * (document_lengths[docid] / mean_document_length))));
	
	/*
		use some combination of bm25 and readability score
	*/
	accumulator[docid].add_rsv((0.5 * bm25) + (0.5 * (20000 - document_readability[docid]) / 1000));
//	accumulator[docid].add_rsv(hardest_document - document_readability[docid]);
	}
}
