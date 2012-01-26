/*
	STEM_SNOWBALL.H
	---------------
*/
#ifndef STEM_SNOWBALL_H_
#define STEM_SNOWBALL_H_

#include "stem.h"
#include "libstemmer.h"

/*
	class ANT_STEM_SNOWBALL
	-----------------------
*/
class ANT_stem_snowball : public ANT_stem
{
private:
	char stemmer_name[32];	// long enough to hold the longest snowball stemmer's name ("snowball Kraaij Pohlmann")
	struct sb_stemmer *snowball;

public:
	ANT_stem_snowball(char *algorithm, char *character_encoding = NULL) { snowball = sb_stemmer_new(algorithm, character_encoding); }
	virtual ~ANT_stem_snowball() { sb_stemmer_delete(snowball); }
	virtual size_t stem(const char *term, char *destination);
	virtual char *name(void) { return stemmer_name; }
} ;

#endif /* STEM_SNOWBALL_H_ */
