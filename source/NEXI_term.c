/*
	NEXI_TERM.C
	-----------
*/
#ifndef __NEXI_TERM_C__
#define __NEXI_TERM_C__

#include <stdio.h>

/*
	class ANT_NEXI_TERM
	-------------------
	This requires some explinatio
*/
class ANT_NEXI_term
{
public:
	ANT_string_pair path;
	ANT_string_pair term;
	static void text_render(void) { printf("TAG:"); path->text_render(); printf("TERM:"); term->text_render(); printf("\n"); }
} ;

#endif __NEXI_TERM_C__
