/*
	LEARNED_WIKIPEDIA_STEM.H
	------------------------
*/

#ifndef __LEARNED_WIKIPEDIA_STEM_H__
#define __LEARNED_WIKIPEDIA_STEM_H__

class ANT_learned_wikipedia_stem
{
public:
	ANT_learned_wikipedia_stem() {}
	virtual ~ANT_learned_wikipedia_stem() {}

	virtual long stem(const char *term, char *destination);
} ;

#endif __LEARNED_WIKIPEDIA_STEM_H__
