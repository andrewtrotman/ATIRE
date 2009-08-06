/*
	STEMMER_FACTORY.C
	-----------------
*/
#include "stemmer_none.h"
#include "stemmer_porter.h"
#include "stemmer_lovins.h"
#include "stemmer_paice_husk.h"
#include "stemmer_otago.h"
#include "stemmer_factory.h"
#include "stemmer_term_similarity.h"

/*
	ANT_STEMMER_FACTORY::GET_STEMMER()
	----------------------------------
*/
ANT_stemmer *ANT_stemmer_factory::get_stemmer(long which_stemmer, ANT_search_engine *engine, long use_similarity, double threshold)
{
ANT_stemmer *stemmer = NULL;
switch (which_stemmer)
	{
	case NONE: 			stemmer = new ANT_stemmer_none(engine);			break;
	case S_STRIPPER: 	stemmer = new ANT_stemmer(engine);				break;
	case PORTER: 		stemmer = new ANT_stemmer_porter(engine);		break;
	case LOVINS:		stemmer = new ANT_stemmer_lovins(engine);		break;
	case PAICE_HUSK:	stemmer = new ANT_stemmer_paice_husk(engine);	break;
	case OTAGO:			stemmer = new ANT_stemmer_otago(engine);		break;
	default:            break;
	}

if (use_similarity)
	stemmer = new ANT_stemmer_term_similarity(engine, stemmer, threshold);

return stemmer;
}

