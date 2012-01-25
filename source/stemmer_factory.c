/*
	STEMMER_FACTORY.C
	-----------------
*/
#include "stem_none.h"
#include "stemmer_none.h"
#include "stemmer_porter.h"
#include "stemmer_krovetz.h"
#include "stemmer_paice_husk.h"
#include "stemmer_otago.h"
#include "stemmer_otago_v2.h"
#include "stemmer_factory.h"
#include "stemmer_snowball.h"
#include "stemmer_term_similarity_weighted.h"
#include "stemmer_term_similarity_threshold.h"
#include "phonetic_iterator_double_metaphone.h"
#include "phonetic_iterator_soundex.h"

/*
	ANT_STEMMER_FACTORY::GET_STEMMER()
	----------------------------------
*/
ANT_stemmer *ANT_stemmer_factory::get_stemmer(long long which_stemmer, ANT_search_engine *engine, long use_wrapper, double value)
{
ANT_stemmer *stemmer = NULL;

switch (which_stemmer)
	{
	case NONE: 			           stemmer = new ANT_stemmer_none(engine);			break;
	case S_STRIPPER: 	           stemmer = new ANT_stemmer(engine);				break;
	case PORTER: 		           stemmer = new ANT_stemmer_porter(engine);		break;
	case OTAGO:			           stemmer = new ANT_stemmer_otago(engine);		break;
	case OTAGO_V2:		           stemmer = new ANT_stemmer_otago_v2(engine);		break;
	case KROVETZ:		           stemmer = new ANT_stemmer_krovetz(engine);		break;
#ifdef ANT_HAS_PAICE_HUSK
	case PAICE_HUSK:	           stemmer = new ANT_stemmer_paice_husk(engine);	break;
#endif
#ifdef ANT_HAS_SNOWBALL
	case SNOWBALL_ARMENIAN:        stemmer = new ANT_stemmer_snowball(engine, "armenian"); break;
	case SNOWBALL_BASQUE:          stemmer = new ANT_stemmer_snowball(engine, "basque"); break;
	case SNOWBALL_CATALAN:         stemmer = new ANT_stemmer_snowball(engine, "catalan"); break;
	case SNOWBALL_DANISH:          stemmer = new ANT_stemmer_snowball(engine, "danish"); break;
	case SNOWBALL_DUTCH:           stemmer = new ANT_stemmer_snowball(engine, "dutch"); break;
	case SNOWBALL_ENGLISH:         stemmer = new ANT_stemmer_snowball(engine, "english"); break;
	case SNOWBALL_FINNISH:         stemmer = new ANT_stemmer_snowball(engine, "finnish"); break;
	case SNOWBALL_FRENCH:          stemmer = new ANT_stemmer_snowball(engine, "french"); break;
	case SNOWBALL_GERMAN:          stemmer = new ANT_stemmer_snowball(engine, "german"); break;
	case SNOWBALL_GERMAN2:         stemmer = new ANT_stemmer_snowball(engine, "german2"); break;
	case SNOWBALL_HUNGARIAN:       stemmer = new ANT_stemmer_snowball(engine, "hungarian"); break;
	case SNOWBALL_ITALIAN:         stemmer = new ANT_stemmer_snowball(engine, "italian"); break;
	case SNOWBALL_KRAAIJ_POHLMANN: stemmer = new ANT_stemmer_snowball(engine, "kraaij_pohlmann"); break;
	case SNOWBALL_LOVINS:          stemmer = new ANT_stemmer_snowball(engine, "lovins"); break;
	case SNOWBALL_NORWEGIAN:       stemmer = new ANT_stemmer_snowball(engine, "norwegian"); break;
	case SNOWBALL_PORTER:          stemmer = new ANT_stemmer_snowball(engine, "porter"); break;
	case SNOWBALL_PORTUGUESE:      stemmer = new ANT_stemmer_snowball(engine, "portuguese"); break;
	case SNOWBALL_ROMANIAN:        stemmer = new ANT_stemmer_snowball(engine, "romanian"); break;
	case SNOWBALL_RUSSIAN:         stemmer = new ANT_stemmer_snowball(engine, "russian"); break;
	case SNOWBALL_SPANISH:         stemmer = new ANT_stemmer_snowball(engine, "spanish"); break;
	case SNOWBALL_SWEDISH:         stemmer = new ANT_stemmer_snowball(engine, "swedish"); break;
	case SNOWBALL_TURKISH:         stemmer = new ANT_stemmer_snowball(engine, "turkish"); break;
#endif
	case DOUBLE_METAPHONE:         stemmer = new ANT_phonetic_iterator_double_metaphone(engine); break;
	case SOUNDEX:                  stemmer = new ANT_phonetic_iterator_soundex(engine); break;
	default:
		break;
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
ANT_stem *ANT_stemmer_factory::get_core_stemmer(long long which_stemmer)
{
ANT_stem *stemmer = NULL;
switch (which_stemmer)
	{
	case NONE: 		               stemmer = new ANT_stem_none;			break;
	case S_STRIPPER:               stemmer = new ANT_stem_s;				break;
	case PORTER: 	               stemmer = new ANT_stem_porter;			break;
	case OTAGO:		               stemmer = new ANT_stem_otago;			break;
	case OTAGO_V2:	               stemmer = new ANT_stem_otago_v2;		break;
	case KROVETZ:	       	       stemmer = new ANT_stem_krovetz;			break;
#ifdef ANT_HAS_PAICE_HUSK
	case PAICE_HUSK:	           stemmer = new ANT_stem_paice_husk;		break;
#endif
#ifdef ANT_HAS_SNOWBALL
	case SNOWBALL_ARMENIAN:        stemmer = new ANT_stem_snowball("armenian"); break;
	case SNOWBALL_BASQUE:          stemmer = new ANT_stem_snowball("basque"); break;
	case SNOWBALL_CATALAN:         stemmer = new ANT_stem_snowball("catalan"); break;
	case SNOWBALL_DANISH:          stemmer = new ANT_stem_snowball("danish"); break;
	case SNOWBALL_DUTCH:           stemmer = new ANT_stem_snowball("dutch"); break;
	case SNOWBALL_ENGLISH:         stemmer = new ANT_stem_snowball("english"); break;
	case SNOWBALL_FINNISH:         stemmer = new ANT_stem_snowball("finnish"); break;
	case SNOWBALL_FRENCH:          stemmer = new ANT_stem_snowball("french"); break;
	case SNOWBALL_GERMAN:          stemmer = new ANT_stem_snowball("german"); break;
	case SNOWBALL_GERMAN2:         stemmer = new ANT_stem_snowball("german2"); break;
	case SNOWBALL_HUNGARIAN:       stemmer = new ANT_stem_snowball("hungarian"); break;
	case SNOWBALL_ITALIAN:         stemmer = new ANT_stem_snowball("italian"); break;
	case SNOWBALL_KRAAIJ_POHLMANN: stemmer = new ANT_stem_snowball("kraaij_pohlmann"); break;
	case SNOWBALL_LOVINS:          stemmer = new ANT_stem_snowball("lovins"); break;
	case SNOWBALL_NORWEGIAN:       stemmer = new ANT_stem_snowball("norwegian"); break;
	case SNOWBALL_PORTER:          stemmer = new ANT_stem_snowball("porter"); break;
	case SNOWBALL_PORTUGUESE:      stemmer = new ANT_stem_snowball("portuguese"); break;
	case SNOWBALL_ROMANIAN:        stemmer = new ANT_stem_snowball("romanian"); break;
	case SNOWBALL_RUSSIAN:         stemmer = new ANT_stem_snowball("russian"); break;
	case SNOWBALL_SPANISH:         stemmer = new ANT_stem_snowball("spanish"); break;
	case SNOWBALL_SWEDISH:         stemmer = new ANT_stem_snowball("swedish"); break;
	case SNOWBALL_TURKISH:         stemmer = new ANT_stem_snowball("turkish"); break;
#endif
	case DOUBLE_METAPHONE:         stemmer = new ANT_phonetic_double_metaphone; break;
	case SOUNDEX: 		           stemmer = new ANT_phonetic_soundex; 	break;

	default:
		break;
	}

return stemmer;
}
