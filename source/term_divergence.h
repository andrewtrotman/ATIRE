/*
	TERM_DIVERGENCE.H
	-----------------
*/
#ifndef TERM_DIVERGENCE_H_
#define TERM_DIVERGENCE_H_

/*
	class ANT_TERM_DIVERGENCE
	-------------------------
*/
class ANT_term_divergence
{
public:
	virtual double divergence(long long source_tf, long long source_length, long long background_tf, long long background_length) = 0;
} ;

#endif /* TERM_DIVERGENCE_H_ */
