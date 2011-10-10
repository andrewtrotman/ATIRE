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
	void make_canvas(void);
	LRESULT windows_callback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	void load_file(void);
	void menu(WORD clicked);

public:
	ANT_canvas(HINSTANCE hInstance);
	virtual ~ANT_canvas();

	long create_window(char *window_title);
} ;

#endif /* CANVAS_H_ */
