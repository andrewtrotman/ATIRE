/*
	PALLETTE.H
	----------
*/
#ifndef PALLETTE_H_
#define PALLETTE_H_

#include <windows.h>

extern RGBQUAD pallette[];

/*
	class ANT_PALLETTE
	------------------
*/
class ANT_pallette
{
public:
	enum { background, text };

private:
	static RGBQUAD pallette[];

public:
	RGBQUAD get_color(long purpose);
} ;

#endif /* PALLETTE_H_ */
