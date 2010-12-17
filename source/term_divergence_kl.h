/*
	TERM_DIVERGENCE_KL.H
	--------------------
*/
#ifndef TERM_DIVERGENCE_KL_H_
#define TERM_DIVERGENCE_KL_H_

#include "term_divergence.h"

/*
	class ANT_TERM_DIVERGENCE_KL
	----------------------------
*/
class ANT_term_divergence_kl : public ANT_term_divergence
{
public:
	virtual double divergence(long long source_tf, long long source_length, long long background_tf, long long background_length);
} ;


#endif /* TERM_DIVERGENCE_KL_H_ */
