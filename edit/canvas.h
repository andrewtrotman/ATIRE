/*
	CANVAS.H
	--------
*/
#ifndef CANVAS_H_
#define CANVAS_H_

#include <windows.h>

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
	static const long TIMER_TEXT_FLASH = 1;
	static const long TIMER_CPU_TICK = 2;
	static const long TIMER_DISPLAY_REFRESH = 4;

private:
	HINSTANCE hInstance;
	HDC bitmap;

public:
	HWND window;

private:
	void make_canvas(void);
	LRESULT windows_callback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	void menu(WORD clicked);

public:
	ANT_canvas(HINSTANCE hInstance);
	virtual ~ANT_canvas() {}

	long create_window(char *window_title);
} ;

#endif /* CANVAS_H_ */
