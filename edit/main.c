

/*
	MAIN.C
	------
*/

#include <windows.h>
#include "canvas.h"
/*
	WINMAIN()
	---------
*/
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
MSG msg;
long done;
ANT_canvas *client;

client = new ANT_canvas(hInstance);
client->create_window((char *)(L"Edit"));

done = false;
while(!done)
	{
	PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE);		// peek but don't remove the message from the queue (that happens below)
	if (msg.message == WM_QUIT)
		done = true;
	else
		{
		GetMessage(&msg, NULL, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}
	}

GetMessage(&msg, NULL, 0, 0);		// get the WM_QUIT event
return msg.wParam;
}
