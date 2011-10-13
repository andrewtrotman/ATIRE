/*
	CANVAS.C
	--------
*/
#include <windows.h>
#include <shlobj.h>
#include <new>
#include "resources/resource.h"
#include "canvas.h"
#include "memory_file_line.h"
#include "memory_file_line_iterator.h"
#include "point.h"

#define VK_SCROLLLOCK (0x91)

/*
	ANT_CANVAS::ANT_CANVAS()
	------------------------
*/
ANT_canvas::ANT_canvas(HINSTANCE hInstance)
{
this->hInstance = hInstance;
file = new ANT_memory_file_line(this);

ascii_font = (HFONT)GetStockObject(OEM_FIXED_FONT);
unicode_font = CreateFont(10, 10, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
unicode_buffer = new char [unicode_buffer_length = 1024];
}

/*
	ANT_CANVAS::~ANT_CANVAS()
	-------------------------
*/
ANT_canvas::~ANT_canvas()
{
DeleteObject(ascii_font);
DeleteObject(unicode_font);

delete file;
delete [] unicode_buffer;
}

/*
	ANT_CANVAS::LOAD_FILE()
	-----------------------
*/
long long ANT_canvas::load_file(void)
{
long long lines;
IFileDialog *pfd = NULL;
IShellItem *psiResult;
DWORD dwFlags;
PWSTR pszFilePath = NULL;
char chosen_filter[1024];
char chosen_filename[MAX_PATH];
OPENFILENAME parameters;

/*
	Create the Open Dialg object IFileDialog
*/
if ((CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd)) == S_OK))
	{
	/*
		we're on VISTA / Windows 7 or later
	*/
	const COMDLG_FILTERSPEC c_rgSaveTypes[] =
	{
	{L"C/C++ files",		L"*.c;*.cxx;*.cpp;*.h;*.hpp.*.hxx;*.mak;makefile"},
	{L"All Documents (*.*)",		L"*.*"}
	};

	pfd->GetOptions(&dwFlags);
	pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);					// we want to see a file
	pfd->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes);
	pfd->SetFileTypeIndex(1);										// first item in the list is the default
	pfd->SetDefaultExtension(L"doc;docx");
	if (pfd->Show(NULL) == S_OK)
		{
		if (pfd->GetResult(&psiResult) == S_OK)						// get the result object if the user clicks "Open"
			{
			psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

			lines = file->read_file((char *)pszFilePath);

			CoTaskMemFree(pszFilePath);
			psiResult->Release();
			}
		}
	pfd->Release();

	return lines;
	}
else
	{
	/*
		we're on something prior to VISTA (NT through to XP) so use the old-style common control
	*/
	memset(chosen_filename, 0, sizeof(chosen_filename));

	parameters.lStructSize = sizeof(parameters);
	parameters.hwndOwner = window;
	parameters.hInstance = hInstance;
	parameters.lpstrFilter = L"C/C++ files\0*.c;*.cxx;*.cpp;*.h;*.hpp.*.hxx\0\0\0";
	parameters.lpstrCustomFilter = (LPWSTR)chosen_filter;
	parameters.nMaxCustFilter = sizeof(chosen_filter) - 1;
	parameters.nFilterIndex = 1;
	parameters.lpstrFile = (LPWSTR)chosen_filename;
	parameters.nMaxFile = sizeof(chosen_filename) - 1;
	parameters.lpstrFileTitle = NULL;
	parameters.nMaxFileTitle = 0;
	parameters.lpstrInitialDir = NULL;
	parameters.lpstrTitle = L"Open...";
	parameters.Flags = OFN_LONGNAMES;
	parameters.nFileOffset = 0;
	parameters.nFileExtension = 0;
	parameters.lpstrDefExt = NULL;
	parameters.lCustData = 0;
	parameters.lpfnHook = NULL;
	parameters.lpTemplateName = 0;
	#if (_WIN32_WINNT >= 0x0500)
		parameters.pvReserved = NULL;
		parameters.dwReserved = 0;
		parameters.FlagsEx = 0;
	#endif

	if ((GetOpenFileNameW(&parameters)) != 0)
		return file->read_file((char *)parameters.lpstrFile);
	}

return 0;
}

/*
	ANT_CANVAS::MENU()
	------------------
*/
void ANT_canvas::menu(WORD clicked)
{
SCROLLINFO scrollbar;
long long lines;

switch (clicked)
	{
	/*
		FILE MENU
	*/
	case ID_FILE_EXIT:
		PostQuitMessage(0);
		break;
	case ID_FILE_OPEN_FILE:
		lines = load_file();
		set_scroll_position(1, file->get_current_line_number(), file->get_page_size(), file->get_lines_in_file());
		break;
	}
}

/*
	ANT_CANVAS::SET_SCROLL_POSITION()
	---------------------------------
*/
long long ANT_canvas::set_scroll_position(long long zero, long long position, long long page_length, long long end_of_file)
{
SCROLLINFO scrollbar;

SetScrollRange(window, SB_VERT, zero, end_of_file, true);
scrollbar.cbSize = sizeof(scrollbar);
scrollbar.fMask = SIF_RANGE | SIF_TRACKPOS | SIF_PAGE | SIF_POS; 
scrollbar.nMin = zero;
scrollbar.nMax = end_of_file;
scrollbar.nPage = page_length;
scrollbar.nPos = position;
scrollbar.nTrackPos = 0;
SetScrollInfo(window, SB_VERT, &scrollbar, true);
InvalidateRect(window, NULL, true);

return position;
}

/*
	ANT_CANVAS::RENDER_TEXT_SEGMENT()
	---------------------------------
*/
long long ANT_canvas::render_text_segment(ANT_point *where, ANT_rgb *colour, char *string, long long string_length, ANT_point *text_size)
{
TEXTMETRIC text_metrics;
SIZE size;
static const long long RIGHT_MARGIN = 5;
HGDIOBJ hfont;

SelectObject(hDC, ascii_font);
SetTextColor(hDC, RGB(colour->red, colour->green, colour->blue));
TextOutA(hDC, where->x + RIGHT_MARGIN, where->y, string, string_length);
GetTextExtentPointA(hDC, string, string_length, &size);

text_size->x = size.cx;
text_size->y = size.cy;

return string_length;
}

/*
	ANT_CANVAS::RENDER_UTF8_SEGMENT()
	---------------------------------
*/
long long ANT_canvas::render_utf8_segment(ANT_point *where, ANT_rgb *colour, char *string, long long string_length, ANT_point *text_size)
{
TEXTMETRIC text_metrics;
SIZE size;
static const long long RIGHT_MARGIN = 5;
long long result;

while (1)
	{
	if ((result = MultiByteToWideChar(CP_UTF8, 0, string, string_length, (LPWSTR)unicode_buffer, unicode_buffer_length / 2)) != 0)
		break;
	else
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
			result = MultiByteToWideChar(CP_UTF8, 0, string, string_length, NULL, 0);
			if (result == 0)
				return render_text_segment(where, colour, string, string_length, text_size);
			else
				{
				unicode_buffer_length = result + 2;		// +2 for the L'\0'
				delete [] unicode_buffer;
				unicode_buffer = new (std::nothrow) char [unicode_buffer_length];
				}
			}
		else
			return render_text_segment(where, colour, string, string_length, text_size);
	}


SelectObject(hDC, unicode_font);
SetTextColor(hDC, RGB(colour->red, colour->green, colour->blue));

TextOutW(hDC, where->x + RIGHT_MARGIN, where->y, (LPWSTR)unicode_buffer, result);
GetTextExtentPointW(hDC, (LPWSTR)unicode_buffer, result, &size);

text_size->x = size.cx;
text_size->y = size.cy;

return string_length;
}


/*
	ANT_CANVAS::RENDER()
	--------------------
*/
void ANT_canvas::render(void)
{
PAINTSTRUCT paintStruct;
HDC window_dc, dc;
HGDIOBJ hFont;
RECT canvas_size;
HBITMAP bitmap;

/*
	Get the window's DC
*/
dc = GetDC(window);
hDC = CreateCompatibleDC(dc);

/*
	Create a bitmap and put tell the DC to use it.
*/
GetClientRect(window, &canvas_size);
bitmap = CreateCompatibleBitmap(dc, canvas_size.right, canvas_size.bottom);
SelectObject(hDC, bitmap);
BitBlt(hDC, 0, 0, canvas_size.right, canvas_size.bottom, hDC, 0, 0, WHITENESS);

/*
	Set the font
*/
SelectObject(hDC, ascii_font);

/*
	tell ATIRE/Edit to render
*/
file->render();


/*
	Copy the bitmap into the new window
*/
window_dc = BeginPaint(window, &paintStruct);
BitBlt(window_dc, 0, 0, canvas_size.right, canvas_size.bottom, hDC, 0, 0, SRCCOPY);
EndPaint(window, &paintStruct);
ReleaseDC(window, window_dc);

/*
	Clean up
*/
ReleaseDC(window, dc);
DeleteObject(bitmap);
DeleteDC(hDC);
}

/*
	ANT_CANVAS::WINDOWS_CALLBACK()
	------------------------------
*/
LRESULT ANT_canvas::windows_callback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
switch(message)
	{
	case WM_CREATE:
		file->set_window_size(((CREATESTRUCT *)lParam)->cx, ((CREATESTRUCT *)lParam)->cy);
		file->read_file((char *)(L"c:\\ant\\edit\\main.c"));
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

	case WM_VSCROLL:
		{
		long long new_line, new_position;

		switch LOWORD(wParam)
			{
			case SB_LINEUP:
				new_line = file->line_up();
				break;
			case SB_LINEDOWN:
				new_line = file->line_down();
				break;
			case SB_PAGEUP:
				new_line = file->page_up();
				break;
			case SB_PAGEDOWN:
				new_line = file->page_down();
				break;
			case SB_BOTTOM:
				new_line = file->file_end();
				break;
			case SB_TOP:
				new_line = file->file_start();
				break;
			case SB_ENDSCROLL:			// user has finished scrolling, do nothing
				break;
			case SB_THUMBPOSITION:		// user is tracking the scroll bar
			case SB_THUMBTRACK:
				new_position = HIWORD(wParam);
				new_line = file->goto_line(new_position);
				break;
			}
		set_scroll_position(1, file->get_current_line_number(), file->get_page_size(), file->get_lines_in_file());
		InvalidateRect(window, NULL, true);
		return 0;
		}
	case WM_PAINT:
		render();
		return 0;

	case WM_TIMER:
		return 0;

	case WM_MOUSEWHEEL:
		{
		long long lines = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
		long long current;

		if (lines > 0)
			for (current = 0; current < lines; current++)
				{
				file->line_up();
				file->line_up();
				file->line_up();
				}
		else
			for (current = 0; current > lines; current--)
				{
				file->line_down();
				file->line_down();
				file->line_down();
				}

		set_scroll_position(1, file->get_current_line_number(), file->get_page_size(), file->get_lines_in_file());
		InvalidateRect(window, NULL, true);
		return 0;
		}
	case WM_SIZE:
		file->set_window_size(LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)			// ESC key
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
windowClass.hbrBackground = NULL; // (HBRUSH)GetStockObject(WHITE_BRUSH);
windowClass.lpszMenuName = NULL;
windowClass.lpszClassName = L"ATIRE/Edit";
windowClass.hIconSm = NULL;

RegisterClassEx(&windowClass);

window = CreateWindowEx(NULL,			// extended style
	L"ATIRE/Edit",					// class name
	(LPWSTR)window_title,					// window name
	WS_EX_OVERLAPPEDWINDOW | WS_SYSMENU | WS_VISIBLE | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VSCROLL,
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

