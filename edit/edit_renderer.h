/*
	EDIT_RENDERER.H
	---------------
*/
#ifndef EDIT_RENDERER_H_
#define EDIT_RENDERER_H_

#include "point.h"
#include "rgb.h"

/*
	class ANT_EDIT_RENDERER
	----------------------
*/
class ANT_edit_renderer
{
public:
	ANT_edit_renderer() {}
	virtual ~ANT_edit_renderer() {}

public:
	virtual long long render_text_segment(ANT_point *where, ANT_rgb *colour, char *string, long long string_length, ANT_point *text_size) = 0;
	virtual long long render_utf8_segment(ANT_point *where, ANT_rgb *colour, char *string, long long string_length, ANT_point *text_size) = 0;
} ;

#endif /* EDIT_RENDERER_H_ */
