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

/*
	ANT_STEMMER_FACTORY::GET_STEMMER()
	----------------------------------
*/
ANT_stemmer *ANT_stemmer_factory::get_stemmer(long which_stemmer, ANT_search_engine *engine)
{
switch (which_stemmer)
	{
	case NONE: 			return new ANT_stemmer_none(engine);		break;
	case S_STRIPPER: 	return new ANT_stemmer(engine);				break;
	case PORTER: 		return new ANT_stemmer_porter(engine);		break;
	case LOVINS:		return new ANT_stemmer_lovins(engine);		break;
	case PAICE_HUSK:	return new ANT_stemmer_paice_husk(engine);	break;
	case OTAGO:			return new ANT_stemmer_otago(engine);		break;
	default:
		return NULL;
	}
}

