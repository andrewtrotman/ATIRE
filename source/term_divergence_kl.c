/*
	TERM_DIVERGENCE_KL.C
	--------------------
*/
#include <math.h>
#include "term_divergence_kl.h"

/*
	ANT_TERM_DIVERGENCE_KL::DIVERGENCE()
	------------------------------------
	Calculate the Divergence for the current search term.  If the term is not in
	the collection then the divergence is infinate - to get around this we fake
	the existance of every term in the collection (with TF=1)

	kl = sum(p(x) * log (p(x) / q(x))
	where p(x) is the probability in the document and q(x) is the probability in the collection
*/
double ANT_term_divergence_kl::divergence(long long source_tf, long long source_length, long long background_tf, long long background_length)
{
double px, qx;

px = (double)source_tf / (double)source_length;

if (background_tf == 0)
	qx = 1.0 / (double)background_length;
else
	qx = (double)background_tf / (double)background_length;

return px * log (px / qx);
}
