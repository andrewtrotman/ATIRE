/*
	RELEVANCE_FEEDBACK_FACTORY.C
	----------------------------
*/
#include <stdio.h>
#include "relevance_feedback_factory.h"
#include "relevance_feedback_blind_kl.h"
#include "relevance_feedback_topsig.h"

/*
	ANT_RELEVANCE_FEEDBACK_FACTORY::GET_FEEDBACKER()
	------------------------------------------------
*/
ANT_relevance_feedback *ANT_relevance_feedback_factory::get_feedbacker(ANT_search_engine *engine, long long which)
{
switch (which)
	{
	case BLIND_KL:
		return new ANT_relevance_feedback_blind_kl(engine);
		break;
	case TOPSIG:
		return new ANT_relevance_feedback_topsig(engine);
		break;
	default:
		return NULL;
	}
}
