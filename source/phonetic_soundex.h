/*
	PHONETIC_SOUNDEX.H
	------------------
*/
#ifndef PHONETIC_SOUNDEX_H_
#define PHONETIC_SOUNDEX_H_

#include "stem.h"

/*
	class ANT_PHONETIC_SOUNDEX
	--------------------------
*/
class ANT_phonetic_soundex : public ANT_stem
{
protected:
	size_t soundex(char *dest, const char *what);

public:
	ANT_phonetic_soundex() {}
	~ANT_phonetic_soundex() {}

	virtual size_t stem(const char *term, char *destination) { return soundex(destination, term); }
	virtual char *name(void) { return "Soundex"; }
} ;

/*
	ANT_PHONETIC_SOUNDEX::SOUNDEX()
	-------------------------------
*/
inline size_t ANT_phonetic_soundex::soundex(char *dest, const char *what)
{
//static const char *sndex_chr = "abcdefghijklmnopqrstuvwxyz";
static const char *soundex_val = "01230129022455012623019202";
char last, current, *into, last_coded;
const char *ch;

strcpy(dest, "0000");
last = soundex_val[(*dest = *what) - 'a'];
into = dest + 1;
last_coded = 0;
for (ch = what + 1; *ch != '\0'; ch++)
	{
	if (ANT_isalpha(*ch))		// ignore non-alphabetics
		{
		current = soundex_val[*ch - 'a'];
		if (current != '0' && current !='9' && current != last)
			if (!(last == '9' && last_coded == current))
				{
				last_coded = *into++ = current;
				if (into >= dest + 4)
					break;
				}
		last = current;
		}
	}
return 1;
}


#endif /* PHONETIC_SOUNDEX_H_ */

