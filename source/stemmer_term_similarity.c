/*
  STEMMER_TERM_SIMILARITY.C
  -------------------------
*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "str.h"
#include "ctypes.h"
#include "stemmer.h"
#include "btree_iterator.h"
#include "search_engine.h"
#include "search_engine_btree_leaf.h"
#include "search_engine_accumulator.h"
#include "stemmer_term_similarity.h"

/*
  ANT_STEMMER_TERM_SIMILARITY::ANT_STEMMER_TERM_SIMILARITY()
  ----------------------------------------------------------
*/
ANT_stemmer_term_similarity::ANT_stemmer_term_similarity(ANT_search_engine *search_engine, ANT_stemmer *stemmer) : ANT_stemmer(search_engine)
{
this->buffer = new long [(size_t)(search_engine->document_count())];
this->search_engine = search_engine;
this->base_stemmer = stemmer;
}

/*
  ANT_STEMMER_TERM_SIMILARITY::~ANT_STEMMER_TERM_SIMILARITY()
  -----------------------------------------------------------
*/
ANT_stemmer_term_similarity::~ANT_stemmer_term_similarity()
{
delete base_stemmer;
delete [] buffer;
}

/*
	ANT_STEMMER_TERM_SIMILARITY::GET_POSTING_DETAILS()
	--------------------------------------------------
	Necessary, in order to get the right value, as this wraps up the real object, which has
	access to this.
*/
ANT_search_engine_btree_leaf *ANT_stemmer_term_similarity::get_postings_details(ANT_search_engine_btree_leaf *term_details) 
{
return base_stemmer->get_postings_details(term_details);
}

/*
	ANT_STEMMER_TERM_SIMILARITY::FILL_BUFFER_WITH_POSTINGS()
	--------------------------------------------------------
	I want to compare TF values, so I need this to fill a long * buffer with 
	values for terms.

	Returns |tf| * |tf| since I need this for base_term, but can't have it, as I'm doing random access.
*/
long long ANT_stemmer_term_similarity::fill_buffer_with_postings(char *term, long *buffer, long long *document_frequency)
{
ANT_search_engine_btree_leaf term_details;
ANT_compressable_integer *current_document, *end;
long document;
long long tf_length_squared = 0;
ANT_compressable_integer term_frequency;

*document_frequency = 0;
memset(buffer, 0, (size_t)(sizeof (*buffer) * search_engine->document_count()));

if ((current_document = search_engine->get_decompressed_postings(term, &term_details)) == NULL)
	return 0;
*document_frequency = term_details.local_document_frequency;
end = current_document + term_details.impacted_length;
while (current_document < end)
	{
	term_frequency = *current_document++;
	document = -1;
	while (*current_document != 0)
	    {
	    document += *current_document++;
	    buffer[document] = term_frequency;
	    tf_length_squared += term_frequency * term_frequency;
	    }
	current_document++;
	}

return tf_length_squared;
}

/*
	ANT_STEMMER_TERM_SIMILARITY::BUFFER_SIMILARITY()
	------------------------------------------------
	Calculates how similar the two buffers are.
*/
double ANT_stemmer_term_similarity::buffer_similarity(char *b) 
{
ANT_search_engine_btree_leaf term_details;
ANT_compressable_integer *current_document, *end;
double length_b = 0, similarity = 0;
double idf_a, idf_b, mean;
long long document;
ANT_compressable_integer *document_lengths;
ANT_compressable_integer term_frequency;

if (buffer_length_squared == 0)
	return 0.0;

if ((current_document = search_engine->get_decompressed_postings(b, &term_details)) == NULL)
	return 0.0;

end = current_document + term_details.impacted_length;
/* 
	             A . B 
	cos theta = -------
	            |A| |B|

	where A and B are tf.idf scores.
	we can open up the expressions and multiply by 

	idf_a * idf_b  OR idf_a ^ 2 OR idf_b ^ 2

	at the end.


	TF*IDF;
	TF = term_count / |document|
	             
	IDF = log(|collection| / doc_count)
*/

document_lengths = search_engine->get_document_lengths(&mean);

while (current_document < end)
	{
	term_frequency = *current_document++;
	document = -1;
	while (*current_document != 0)
	    {
	    document += *current_document++;
	    length_b += term_frequency * term_frequency;
	    if (buffer[(size_t)document]) 
	        similarity += ((double)buffer[(size_t)document] / (double)document_lengths[(size_t)document]) * ((double)term_frequency / (double)document_lengths[(size_t)document]);
	    }
	current_document++;
	}

idf_a = log((double)search_engine->document_count() / (double)document_frequency);
idf_b = log((double)search_engine->document_count() / (double)term_details.local_document_frequency);

similarity *= idf_a * idf_b;

similarity /= sqrt((double)buffer_length_squared * idf_a * idf_a);
similarity /= sqrt((double)length_b * idf_b * idf_b);


return similarity;
}

/* 
	ANT_STEMMER_TERM_SIMILARITY::TERM_SIMILARITY()
	----------------------------------------------

   This is in effect a matrix multiplication, between the index and its 
   transpose. Each postings list must be normalised, and the multiplication
   happens such that the matrix left is term x term.

            doc a, doc b             term A, term B              term A, term B
   term A [     0      1 ] \/  doc a [    0       1 ]  == term A [    1       0 ]
   term B [     1      0 ] /\  doc b [    1       0 ]  == term B [    0       1 ]

   (obviously we do this in another way for efficiency)
*/
double ANT_stemmer_term_similarity::term_similarity(char *term1, char *term2)
{
long long df;

if (strcmp(term1, term) != 0)
	{
	strncpy(term, term1, MAX_TERM_LENGTH);
	buffer_length_squared = fill_buffer_with_postings(term1, buffer, &df);
	}

return buffer_similarity(term2);
}

/*
	ANT_STEMMER_TERM_SIMILARITY_THRESHOLD::NEXT()
	---------------------------------------------
*/
char *ANT_stemmer_term_similarity::next()
{
return base_stemmer->next();
}

/*
	ANT_STEMMER_TERM_SIMILARITY::FIRST()
	------------------------------------
*/
char *ANT_stemmer_term_similarity::first(char *term)
{
strncpy(this->term, term, MAX_TERM_LENGTH);
buffer_length_squared = fill_buffer_with_postings(term, buffer, &document_frequency);

return base_stemmer->first(term);
}
