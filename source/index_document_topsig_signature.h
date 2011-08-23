/*
	INDEX_DOCUMENT_TOPSIG_SIGNATURE.H
	---------------------------------
*/
#ifndef INDEX_DOCUMENT_TOPSIG_SIGNATURE_H_
#define INDEX_DOCUMENT_TOPSIG_SIGNATURE_H_

class ANT_index_document_topsig;
class ANT_string_pair;

/*
	class ANT_INDEX_DOCUMENT_TOPSIG_SIGNATURE
	-----------------------------------------
*/
class ANT_index_document_topsig_signature
{
public:
	double *vector;
	long width;
	double density;

public:
	ANT_index_document_topsig_signature(long width, double density);
	~ANT_index_document_topsig_signature();

	void rewind(void);
	unsigned long long add_term(ANT_index_document_topsig *globalstats, ANT_string_pair *term, long long term_frequency, long long document_length, long long collection_length_in_term);
	unsigned long long add_term(ANT_index_document_topsig *globalstats, char *term, long long term_frequency, long long document_length, long long collection_length_in_term);
	double *get_vector(void) { return vector; }
} ;


#endif /* INDEX_DOCUMENT_TOPSIG_SIGNATURE_H_ */
