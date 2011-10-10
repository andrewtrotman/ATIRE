/*
	CANVAS.C
	--------
*/
#include <windows.h>
#include "resources/resource.h"
#include "canvas.h"
#include "bitmapinfor256.h"
#include "pallette.h"
#include "memory_file_line.h"

#define VK_SCROLLLOCK (0x91)

/*
	ANT_CANVAS::ANT_CANVAS()
	------------------------
*/
ANT_canvas::ANT_canvas(HINSTANCE hInstance)
{
this->hInstance = hInstance;
file = new ANT_memory_file_line;
}

/*
	ANT_CANVAS::~ANT_CANVAS()
	-------------------------
*/
ANT_canvas::~ANT_canvas()
{
delete file;
}

/*
	ANT_CANVAS::MAKE_CANVAS()
	-------------------------
*/
void ANT_canvas::make_canvas(void)
{
HGDIOBJ bmp;
BITMAPINFO256 info;

info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
info.bmiHeader.biWidth = WIDTH_IN_PIXELS;
info.bmiHeader.biHeight = -HEIGHT_IN_PIXELS;	// -ve for a top-down DIB
info.bmiHeader.biPlanes = 1;
info.bmiHeader.biBitCount = 8;
info.bmiHeader.biCompression = BI_RGB;			// not compressed
info.bmiHeader.biSizeImage = 0;
info.bmiHeader.biXPelsPerMeter = 0;
info.bmiHeader.biYPelsPerMeter = 0;
info.bmiHeader.biClrUsed = 0;
info.bmiHeader.biClrImportant = 0;
memset(info.bmiColors, 0, sizeof(info.bmiColors));
}

/*
	ANT_CANVAS::LOAD_FILE()
	-----------------------
*/
void ANT_canvas::load_file(void)
{
char chosen_filter[1024];
char chosen_filename[MAX_PATH];
OPENFILENAME parameters;

parameters.lStructSize = sizeof(parameters);
parameters.hwndOwner = window;
parameters.hInstance = hInstance;
parameters.lpstrFilter = "C/C++ files\0*.c;*.cxx;*.cpp\0\0\0";
parameters.lpstrCustomFilter = chosen_filter;
parameters.nMaxCustFilter = sizeof(chosen_filter) - 1;
parameters.nFilterIndex = 1;
parameters.lpstrFile = chosen_filename;
parameters.nMaxFile = sizeof(chosen_filename) - 1;
parameters.lpstrFileTitle = NULL;
parameters.nMaxFileTitle = 0;
parameters.lpstrInitialDir = NULL;
parameters.lpstrTitle = "Open...";
parameters.Flags = OFN_LONGNAMES;
parameters.nFileOffset = 0;
parameters.nFileExtension = 0;
parameters.lpstrDefExt = NULL;
parameters.lCustData = 0;
parameters.lpfnHook = NULL;
parameters.lpTemplateName = 0;

#if (_WIN32_WINNT >= 0x0500)
	parameters.pvReserved = (void *)NULL;
	parameters.dwReserved = 0;
	parameters.FlagsEx = 0;
#endif 
GetOpenFileName(&parameters);
}

/*
	ANT_CANVAS::MENU()
	------------------
*/
void ANT_canvas::menu(WORD clicked)
{
switch (clicked)
	{
	/*
		FILE MENU
	*/
	case ID_FILE_EXIT:
		PostQuitMessage(0);
		break;
	case ID_FILE_OPEN_FILE:
		load_file();
		break;
	}
}

/*
	ANT_CANVAS::WINDOWS_CALLBACK()
	------------------------------
*/
LRESULT ANT_canvas::windows_callback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
HDC hDC; 

switch(message)
	{
	case WM_CREATE:
		make_canvas();
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		return 0;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		return 0;

	case WM_PAINT:
		return 0;

	case WM_TIMER:
		return 0;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
			{
			}
		else
			menu(LOWORD(wParam));
		return 0;
	}
return (DefWindowProc(hwnd,message,wParam,lParam));
}

/*
	WINDOWS_CALLBACK()
	------------------
*/
static LRESULT CALLBACK windows_callback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
ANT_canvas *object = NULL;

if (message == WM_CREATE)
	{
	object = (ANT_canvas *)((CREATESTRUCT *)lParam)->lpCreateParams;
	object->window = hwnd;
	return object->windows_callback(hwnd, message, wParam, lParam);
	}
else if ((object = (ANT_canvas *)GetWindowLongPtr(hwnd, GWLP_USERDATA)) != NULL)
	return object->windows_callback(hwnd, message, wParam, lParam);
else
	return (DefWindowProc(hwnd,message,wParam,lParam));
}

/*
	ANT_CANVAS::CREATE_WINDOW()
	---------------------------
*/
long ANT_canvas::create_window(char *window_title)
{
HWND window;
WNDCLASSEX windowClass;

this->hInstance = hInstance;
windowClass.cbSize = sizeof(WNDCLASSEX);
windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS;
windowClass.lpfnWndProc = ::windows_callback;
windowClass.cbClsExtra = 0;
windowClass.cbWndExtra = 0;
windowClass.hInstance = 0;
windowClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_ATIRE_EDIT));
windowClass.hCursor = NULL;
windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
windowClass.lpszMenuName = NULL;
windowClass.lpszClassName = "ATIRE/Edit";
windowClass.hIconSm = NULL;

RegisterClassEx(&windowClass);

window = CreateWindowEx(NULL,			// extended style
	"ATIRE/Edit",					// class name
	window_title,					// window name
	WS_EX_OVERLAPPEDWINDOW | WS_BORDER| WS_SYSMENU | WS_VISIBLE,
	120, 120,			// x/y coords
	WIDTH_IN_PIXELS,	// width
	2 * HEIGHT_IN_PIXELS,	// height
	NULL,				// handle to parent
	LoadMenu(0, MAKEINTRESOURCE(IDR_MENU)),				// Menu
	0,					// application instance
	this);				// no extra parameter's

SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)this);

UpdateWindow(window);

return 0;
}

