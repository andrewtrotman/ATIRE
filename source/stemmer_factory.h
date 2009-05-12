/*
	STEMMER_FACTORY.H
	-----------------
*/
#ifndef __STEMMER_FACTORY_H__
#define __STEMMER_FACTORY_H__

class ANT_stemmer;
class ANT_search_engine;

/*
	class ANT_STEMMER_FACTORY
	-------------------------
*/
class ANT_stemmer_factory
{
public:
	enum { NONE, S_STRIPPER, PORTER, LOVINS, PAICE_HUSK, OTAGO };

public:
	static ANT_stemmer *get_stemmer(long which_stemmer, ANT_search_engine *engine);
} ;

#endif __STEMMER_FACTORY_H__
