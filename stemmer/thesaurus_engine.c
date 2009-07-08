#include <stdio.h>
#include "stemmer.h"
#include "btree_iterator.h"
#include "search_engine.h"
#include "search_engine_btree_leaf.h"
#include "thesaurus_engine.h"

/*
  I want to compare TF values, so I need this to fill a long * buffer with 
  values for terms.
 */
long long thesaurus_engine::fill_buffer_with_postings(char *term, long *buffer) {
    ANT_search_engine_btree_leaf term_details;
    ANT_compressable_integer *current_document, *end;
    long document;
#ifdef ANT_TOP_K
	ANT_compressable_integer term_frequency;
#else
	ANT_compressable_integer *current_tf;
#endif

    // Get position 
    if (get_postings_details(term, &term_details) == NULL) {
        memset(buffer, 0, sizeof buffer[0] * documents);
        return 0;
    }
    // Get from disk
	if (get_postings(&term_details, postings_buffer) == NULL) {
        memset(buffer, 0, sizeof buffer[0] * documents);
		return 0;
    }
    // Decompress
#ifdef ANT_TOP_K
	factory.decompress(decompress_buffer, postings_buffer, term_details.impacted_length);
	current_document = decompress_buffer;
	end = decompress_buffer + term_details.impacted_length;
	while (current_document < end)
		{
		term_frequency = *current_document++;
		document = -1;
		while (*current_document != 0)
			{
			document += *current_document++;
			buffer[document] = term_frequency;
			}
		current_document++;
		}
#else
	decompress(postings_buffer, &term_details);
	end = posting.docid + term_details.document_frequency;
	document = -1;
	for (current_document = posting.docid, current_tf = posting.tf; current_document < end; current_document++, current_tf++)
		{
		document = *current_document;
		buffer[document] = *current_tf;
		}
#endif

    return term_details.collection_frequency;
}

/*
  Calculates how similar the two buffers are.
*/
double thesaurus_engine::buffer_similarity(long long buffer_a_total, long long buffer_b_total) {
    long long doc;
    double similarity = 0;

    if (buffer_a_total == 0 ||
        buffer_b_total == 0)
        return 0.0;

    for (doc = 0; doc < documents; doc++) {
        similarity += 
            (buffer_a[doc] / (double)buffer_a_total) *
            (buffer_b[doc] / (double)buffer_b_total);
    }
    return similarity;
}

/* 
   TERM_SIMILARITY

   This is in effect a matrix multiplication, between the index and its 
   transpose. Each postings list must be normalised, and the multiplication
   happens such that the matrix left is term x term.

            doc a, doc b             term A, term B              term A, term B
   term A [     0      1 ] \/  doc a [    0       1 ]  == term A [    1       0 ]
   term B [     1      0 ] /\  doc b [    1       0 ]  == term B [    0       1 ]

   (obviously we do this in another way for efficiency)

 */
double thesaurus_engine::term_similarity(char *term1, char *term2) {
    long long row_total1, row_total2;
    
    row_total1 = fill_buffer_with_postings(term1, buffer_a);
    row_total2 = fill_buffer_with_postings(term2, buffer_b);
    
    return buffer_similarity(row_total1, row_total2);
}

/* 
   Stemming exceptions - print these out for now, we can do something with them later.
*/
void thesaurus_engine::stemming_exceptions(ANT_stemmer *stemmer, double threshold) {
    ANT_btree_iterator all_terms(this); // first(NULL) works
    double similarity;
    char *term;
    char *stem;

    for (term = all_terms.first(NULL); term != NULL; term = all_terms.next()) {
        for (stem = stemmer->first(term); stem != NULL; stem = stemmer->next()) {
            // TODO: for efficiency skip if stem == term
            if ((similarity = term_similarity(term, stem)) > 0.5)
                printf("%s is ~= %s by a factor of %f\n", term, stem, similarity);
        }
    }
}
