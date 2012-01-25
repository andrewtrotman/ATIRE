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
	enum { WEIGHTED_SIMILARITY = 1, THRESHOLD_SIMILARITY };

	enum 
		{
		NONE, 
		S_STRIPPER, 
		PORTER, 
		PAICE_HUSK, 
		OTAGO, 
		OTAGO_V2, 
		DOUBLE_METAPHONE, 
		SOUNDEX, 
		KROVETZ,
		SNOWBALL_ARMENIAN,
		SNOWBALL_BASQUE,
		SNOWBALL_CATALAN,
		SNOWBALL_DANISH,
		SNOWBALL_DUTCH,
		SNOWBALL_ENGLISH,
		SNOWBALL_FINNISH,
		SNOWBALL_FRENCH,
		SNOWBALL_GERMAN,
		SNOWBALL_GERMAN2,
		SNOWBALL_HUNGARIAN,
		SNOWBALL_ITALIAN,
		SNOWBALL_KRAAIJ_POHLMANN,
		SNOWBALL_LOVINS,
		SNOWBALL_NORWEGIAN,
		SNOWBALL_PORTER,
		SNOWBALL_PORTUGUESE,
		SNOWBALL_ROMANIAN,
		SNOWBALL_RUSSIAN,
		SNOWBALL_SPANISH,
		SNOWBALL_SWEDISH,
		SNOWBALL_TURKISH
		};

public:
	static ANT_stemmer *get_stemmer(long long which_stemmer, ANT_search_engine *engine, long use_wrapper = NONE, double value = 0.0);
	static ANT_stem *get_core_stemmer(long long which_stemmer);
} ;

#endif  /* STEMMER_FACTORY_H_ */
