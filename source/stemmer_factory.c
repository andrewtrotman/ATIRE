/*
	STEMMER_FACTORY.C
	-----------------
*/
#include "stem_none.h"
#include "stemmer_none.h"
#include "stemmer_porter.h"
#include "stemmer_lovins.h"
#include "stemmer_paice_husk.h"
#include "stemmer_otago.h"
#include "stemmer_factory.h"
#include "stemmer_term_similarity_weighted.h"
#include "stemmer_term_similarity_threshold.h"

/*
	ANT_STEMMER_FACTORY::GET_STEMMER()
	----------------------------------
*/
ANT_stemmer *ANT_stemmer_factory::get_stemmer(long which_stemmer, ANT_search_engine *engine, long use_wrapper, double value)
{
ANT_stemmer *stemmer = NULL;
switch (which_stemmer)
	{
	case NONE: 			stemmer = new ANT_stemmer_none(engine);			break;
	case S_STRIPPER: 	stemmer = new ANT_stemmer(engine);				break;
	case PORTER: 		stemmer = new ANT_stemmer_porter(engine);		break;
	case OTAGO:			stemmer = new ANT_stemmer_otago(engine);		break;
#ifdef ANT_HAS_LOVINS
	case LOVINS:		stemmer = new ANT_stemmer_lovins(engine);		break;
#endif
#ifdef ANT_HAS_PAICE_HUSK
	case PAICE_HUSK:	stemmer = new ANT_stemmer_paice_husk(engine);	break;
#endif
	default:            break;
	}

switch (use_wrapper)
	{
	case NONE:			break;
	case THRESHOLD_SIMILARITY: 	stemmer = new ANT_stemmer_term_similarity_threshold(engine, stemmer, value);		break;
	case WEIGHTED_SIMILARITY: 	stemmer = new ANT_stemmer_term_similarity_weighted(engine, stemmer, value); 		break;
	default:            break;
	}

return stemmer;
}

/*
	ANT_STEMMER_FACTORY::GET_CORE_STEMMER()
	---------------------------------------
*/
ANT_stem *ANT_stemmer_factory::get_core_stemmer(long which_stemmer)
{
ANT_stem *stemmer = NULL;
switch (which_stemmer)
	{
	case NONE: 			stemmer = new ANT_stem_none;			break;
	case S_STRIPPER: 	stemmer = new ANT_stem_s;				break;
	case PORTER: 		stemmer = new ANT_stem_porter;			break;
	case OTAGO:			stemmer = new ANT_stem_otago;			break;
#ifdef ANT_HAS_LOVINS
	case LOVINS:		stemmer = new ANT_stem_lovins;			break;
#endif
#ifdef ANT_HAS_PAICE_HUSK
	case PAICE_HUSK:	stemmer = new ANT_stem_paice_husk;		break;
#endif
	default:            break;
	}

return stemmer;
}
