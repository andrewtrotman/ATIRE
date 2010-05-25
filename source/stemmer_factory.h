/*
	STEMMER_FACTORY.H
	-----------------
*/
#ifndef STEMMER_FACTORY_H_
#define STEMMER_FACTORY_H_

class ANT_stem;
class ANT_stemmer;
class ANT_search_engine;

/*
	class ANT_STEMMER_FACTORY
	-------------------------
*/
class ANT_stemmer_factory
{
public:
	enum { NONE, S_STRIPPER, PORTER, LOVINS, PAICE_HUSK, OTAGO, DOUBLE_METAPHONE, SOUNDEX };
	enum { WEIGHTED_SIMILARITY = 1, THRESHOLD_SIMILARITY };

public:
	static ANT_stemmer *get_stemmer(long long which_stemmer, ANT_search_engine *engine, long use_wrapper = NONE, double value = 0.0);
	static ANT_stem *get_core_stemmer(long long which_stemmer);
} ;

#endif  /* STEMMER_FACTORY_H_ */
