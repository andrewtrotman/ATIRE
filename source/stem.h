/*
	STEM.H
	------
*/
#ifndef STEM_H_
#define STEM_H_

/*
	ANT_stem
	--------
*/
class ANT_stem
{
public:
	ANT_stem() {}
	virtual ~ANT_stem() {}
	virtual size_t stem(const char *term, char *destination) = 0;
	virtual char *name(void) = 0;
} ;

#endif /* STEM_H_ */
