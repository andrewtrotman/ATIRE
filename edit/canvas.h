/*
	CANVAS.H
	--------
*/
#ifndef CANVAS_H_
#define CANVAS_H_

#include <windows.h>
#include "edit_renderer.h"

class ANT_memory_file_line;

/*
	class ANT_CANVAS
	----------------
*/
class ANT_canvas : ANT_edit_renderer
{
friend static LRESULT CALLBACK windows_callback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	static const long WIDTH_IN_PIXELS = 480;
	static const long HEIGHT_IN_PIXELS = 240;

private:
	HFONT ascii_font;
	HFONT unicode_font;
	HINSTANCE hInstance;
	HDC hDC;
	HWND window;

	ANT_memory_file_line *file;
	char *unicode_buffer;
	long long unicode_buffer_length;

private:
	LRESULT windows_callback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	long long load_file(void);
	long long set_scroll_position(long long zero, long long position, long long page_length, long long end_of_file);
	void menu(WORD clicked);
	void render(void);

public:
	ANT_canvas(HINSTANCE hInstance);
	virtual ~ANT_canvas();

	long create_window(char *window_title);
	virtual long long render_utf8_segment(ANT_point *where, ANT_rgb *colour, char *string, long long string_length, ANT_point *text_size);
	virtual long long render_text_segment(ANT_point *where, ANT_rgb *colour, char *string, long long string_length, ANT_point *text_size);
} ;

#endif /* CANVAS_H_ */
