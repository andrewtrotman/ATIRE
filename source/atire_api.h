/*
	ATIRE_API.H
	-----------
*/
#ifndef ATIRE_API_H_
#define ATIRE_API_H_

/*
	class ATIRE_API
	---------------
*/
class ATIRE_API
{
private:
	ANT_NEXI_ant NEXI_parser;				// INEX CO / CAS queries
	ANT_query_boolean boolean_parser;		// full boolean
	long segmentation;						// Chinese segmentation algorithm
	ANT_query parsed_query;
	ANT_ranking_function *ranking_function;	// the ranking function to use (defaults to BM25)

public:
	ATIRE_API();
	virtual ~ATIRE_API();

	/*
		What version are we?
	*/
	char *version(long *version_number)	;

	/*
		Set the chinese segmentation algorithm
	*/
	void set_segmentaiton(long segmentation) { this->segmentation = segmentation; }

	/*
		Parse a NEXI query
	*/
	long parse_NEXI_query(char *query);												// returns an error code (0 = no error, see query.h)

	/*
		Set the ranking function
		for BM25: k1 = p1, b = k2
		for LMD:  u = p1
		for LMJM: l = p1
	*/
	long set_ranking_function(long function, double p1, double p2);

} ;

#endif /* ATIRE_API_H_ */
