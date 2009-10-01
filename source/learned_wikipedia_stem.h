/*
	LEARNED_WIKIPEDIA_STEM.H
	------------------------
*/

#ifndef LEARNED_WIKIPEDIA_STEM_H_
#define LEARNED_WIKIPEDIA_STEM_H_

class ANT_learned_wikipedia_stem
{
public:
	ANT_learned_wikipedia_stem() {}
	virtual ~ANT_learned_wikipedia_stem() {}

	virtual size_t stem(const char *term, char *destination);
} ;

#endif  /* LEARNED_WIKIPEDIA_STEM_H_ */
