/*
	CANVAS.C
	--------
*/
#include <windows.h>
#include "resources/resource.h"
#include "canvas.h"
#include "memory_file_line.h"
#include "memory_file_line_iterator.h"
#include "point.h"

#define VK_SCROLLLOCK (0x91)




#define STRICT_TYPED_ITEMIDS
#include <shlobj.h>
#include <objbase.h>      // For COM headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <shlwapi.h>
#include <knownfolders.h> // for KnownFolder APIs/datatypes/function headers
#include <propvarutil.h>  // for PROPVAR-related functions
#include <propkey.h>      // for the Property key APIs/datatypes
#include <propidl.h>      // for the Property System APIs
#include <strsafe.h>      // for StringCchPrintfW
#include <shtypes.h>      // for COMDLG_FILTERSPEC
#include <new>




/*
	ANT_CANVAS::ANT_CANVAS()
	------------------------
*/
ANT_canvas::ANT_canvas(HINSTANCE hInstance)
{
this->hInstance = hInstance;
file = new ANT_memory_file_line(this);
}

/*
	ANT_CANVAS::~ANT_CANVAS()
	-------------------------
*/
ANT_canvas::~ANT_canvas()
{
delete file;
}
#ifdef NEVER
/*
	ANT_CANVAS::LOAD_FILE()
	-----------------------
*/
long long ANT_canvas::load_file(void)
{
char chosen_filter[1024];
char chosen_filename[MAX_PATH];
OPENFILENAME parameters;

memset(chosen_filename, 0, sizeof(chosen_filename));
memset(chosen_filename, 0, sizeof(chosen_filename));

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
	parameters.pvReserved = NULL;
	parameters.dwReserved = 0;
	parameters.FlagsEx = 0;
#endif

if ((GetOpenFileName(&parameters)) != 0)
	return file->read_file(parameters.lpstrFile);
else
	return 0;
}
#else

long long ANT_canvas::load_file(void)
{
extern HRESULT BasicFileOpen();

BasicFileOpen();

return 0;
}

#pragma comment(lib, "comctl32.lib") // For TaskDialog

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define INDEX_WORDDOC 1

const COMDLG_FILTERSPEC c_rgSaveTypes[] =
{
    {L"Word Document (*.doc)",       L"*.doc"},
    {L"Web Page (*.htm; *.html)",    L"*.htm;*.html"},
    {L"Text Document (*.txt)",       L"*.txt"},
    {L"All Documents (*.*)",         L"*.*"}
};

HRESULT BasicFileOpen()
{
IFileDialog *pfd = NULL;
HRESULT hr;

/*
	Create the Open Dialg object IFileDialog
*/
hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
if (SUCCEEDED(hr))
	{
	/*
		we have an object, now use it
	*/
	DWORD dwFlags;

	pfd->GetOptions(&dwFlags);
	pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
	pfd->SetFileTypes(ARRAYSIZE(c_rgSaveTypes), c_rgSaveTypes);
	pfd->SetFileTypeIndex(INDEX_WORDDOC);
	pfd->SetDefaultExtension(L"doc;docx");
	hr = pfd->Show(NULL);

	if (SUCCEEDED(hr))
		{
		// Obtain the result once the user clicks 
		// the 'Open' button.
		// The result is an IShellItem object.
		IShellItem *psiResult;
		hr = pfd->GetResult(&psiResult);
		if (SUCCEEDED(hr))
			{
			// We are just going to print out the 
			// name of the file for sample sake.
			PWSTR pszFilePath = NULL;
			hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
			if (SUCCEEDED(hr))
				{
				TaskDialog(NULL, NULL, L"CommonFileDialogApp", pszFilePath, NULL, TDCBF_OK_BUTTON, TD_INFORMATION_ICON, NULL);
				CoTaskMemFree(pszFilePath);
				}
			psiResult->Release();
			}
		}
	pfd->Release();
	}
return hr;
}

#endif

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

SetTextColor(hDC, RGB(colour->red, colour->green, colour->blue));
TextOut(hDC, where->x, where->y, string, string_length);
GetTextExtentPoint(hDC, string, string_length, &size);

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
hFont = GetStockObject(OEM_FIXED_FONT);
SelectObject(hDC, hFont);

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
DeleteObject(hFont);
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
windowClass.lpszClassName = "ATIRE/Edit";
windowClass.hIconSm = NULL;

RegisterClassEx(&windowClass);

window = CreateWindowEx(NULL,			// extended style
	"ATIRE/Edit",					// class name
	window_title,					// window name
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

