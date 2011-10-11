/*
	CANVAS.H
	--------
*/
#ifndef CANVAS_H_
#define CANVAS_H_

#include <windows.h>

class ANT_memory_file_line;

/*
	class ANT_CANVAS
	----------------
*/
class ANT_canvas
{
friend static LRESULT CALLBACK windows_callback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	static const long WIDTH_IN_PIXELS = 480;
	static const long HEIGHT_IN_PIXELS = 240;

private:
	HINSTANCE hInstance;
	HDC bitmap;
	HWND window;

	ANT_memory_file_line *file;

private:
	LRESULT windows_callback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	long long load_file(void);
	long long set_scroll_position(long long zero, long long position, long long page_length, long long end_of_file);
	void menu(WORD clicked);
	void render(void);
	long long set_page_size(long long pixels);

public:
	ANT_canvas(HINSTANCE hInstance);
	virtual ~ANT_canvas();

	long create_window(char *window_title);
} ;

#endif /* CANVAS_H_ */
