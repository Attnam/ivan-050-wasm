#include "whandler.h"
#include "graphics.h"
#include "error.h"
#include "bitmap.h"



dynarray<int> globalwindowhandler::KeyBuffer;
bool globalwindowhandler::Initialized = false;
bool (*globalwindowhandler::QuitMessageHandler)() = 0;

#ifdef WIN32
char globalwindowhandler::KeyboardLayoutName[KL_NAMELENGTH];
bool globalwindowhandler::Active = false;

#include <windows.h>
#include <ddraw.h>
#include <mmsystem.h>
#include "ddutil.h"

LRESULT CALLBACK globalwindowhandler::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_SIZE:
			if(wParam == SIZE_MAXHIDE || wParam == SIZE_MINIMIZED)
				Active = false;
			else
			{
				Active = true;

				if(wParam == SIZE_MAXIMIZED)
				{
					graphics::SwitchMode();
					break;
				}
			}

		case WM_MOVE:
		{
			graphics::UpdateBounds();

			if(Initialized && Active)
				graphics::BlitDBToScreen();

			break;
		}

		case WM_PAINT:
		{
			if(Initialized && Active)
				graphics::BlitDBToScreen();

			break;
		}

		case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
					return 0;
			}

			break;
		}

		case WM_CLOSE:
		{
			if(!QuitMessageHandler || QuitMessageHandler())
				PostQuitMessage(0);

			return 0;
		}

		case WM_KEYDOWN:
		{
			ushort Index = KeyBuffer.Search(wParam);

			if(Index == 0xFFFF)
				KeyBuffer.Add(wParam);

			return 0;
		}

		case WM_KEYUP:
		{
			if(wParam == VK_SNAPSHOT)
			{
				DOUBLEBUFFER->Save("Scrshot.bmp");
				return 0;
			}

			ushort Index = KeyBuffer.Search(wParam);

			if(Index != 0xFFFF)
				KeyBuffer.Remove(Index);

			return 0;
		}

		case WM_SYSKEYUP:
			if(wParam == VK_RETURN)
			{
				graphics::SwitchMode();
				return 0;
			}
				
	}

	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int globalwindowhandler::GetKey(bool EmptyBuffer, bool AcceptCommandKeys)
{
	MSG msg;

	if(EmptyBuffer)
	{
		CheckMessages();

		while(KeyBuffer.Length())
			KeyBuffer.Remove(0);
	}

	while(true)
		if(Active && KeyBuffer.Length())
		{
			int Key =  KeyBuffer.Remove(0);

			HKL MappedVirtualKey = LoadKeyboardLayout(KeyboardLayoutName, KLF_SUBSTITUTE_OK | KLF_REPLACELANG | KLF_ACTIVATE );

			ulong ScanCode = MapVirtualKeyEx(Key, 0, MappedVirtualKey);
			ushort ToBeReturned;	
			uchar KeyboardBuffer[256];

			if(Key == VK_LEFT || Key == VK_NUMPAD4) return 0x14B;
			if(Key == VK_HOME || Key == VK_NUMPAD7) return 0x147;
			if(Key == VK_UP || Key == VK_NUMPAD8) return 0x148;
			if(Key == VK_PRIOR || Key == VK_NUMPAD9) return 0x149;	// page up! Believe it, or not!
			if(Key == VK_RIGHT || Key == VK_NUMPAD6) return 0x14D;
			if(Key == VK_NEXT || Key == VK_NUMPAD3) return 0x151;	// page down! Believe it, or not!
			if(Key == VK_DOWN || Key == VK_NUMPAD2) return 0x150;
			if(Key == VK_END || Key == VK_NUMPAD1) return 0x14F;
			if(Key == VK_NUMPAD5) return '.';

			if(!GetKeyboardState(KeyboardBuffer))
				return 'x';

			ToAsciiEx(Key, ScanCode, KeyboardBuffer, &ToBeReturned, 0, MappedVirtualKey);

			if(ToBeReturned != 0 && ToBeReturned != 0xFFFF)
				return ToBeReturned;
			else
				if(AcceptCommandKeys)
					return 0;
		}
		else
			if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
				if(msg.message == WM_QUIT)
					exit(0);
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			else
				WaitMessage();
}

void globalwindowhandler::Init(HINSTANCE hInst, HWND* phWnd, const char* Title, LPCTSTR IconName)
{
	WNDCLASS wc;
	HWND hWnd;

	wc.lpszClassName = Title;
	wc.lpfnWndProc   = (WNDPROC) globalwindowhandler::WndProc;
	wc.style         = CS_OWNDC;
	wc.hInstance     = hInst;
	wc.hIcon         = LoadIcon(hInst, IconName);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;

	if(!RegisterClass( &wc ))
		ABORT("No Window register.");

	hWnd = CreateWindowEx(0, Title, Title, WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, hInst, 0 );

	if(!hWnd)
		ABORT("No Windows.");

	ShowWindow(hWnd, SW_SHOW);
	SetFocus(hWnd);
	UpdateWindow(hWnd);

	*phWnd = hWnd;

	GetKeyboardLayoutName(KeyboardLayoutName);
}

int globalwindowhandler::ReadKey()
{
	while(true)
	{
		CheckMessages();

		if(Active) // Active true if the window is not iconified
			break;
		else
			WaitMessage();
	}

	if(KeyBuffer.Length())
		return GetKey(false, true);
	else
		return 0;
}

void globalwindowhandler::CheckMessages()
{
	MSG msg;

	while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		if(msg.message == WM_QUIT)
			exit(0);
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
}
#else
void globalwindowhandler::Init(const char* Title)
{
  SDL_WM_SetCaption(Title, 0);
  SDL_EnableUNICODE(1);
}

int globalwindowhandler::GetKey(bool EmptyBuffer, bool AcceptCommandKeys)
{
  if(EmptyBuffer)
	{
		CheckMessages();

		while(KeyBuffer.Length())
			KeyBuffer.Remove(0);
	}

	while(true)
		if(KeyBuffer.Length())
		{
			int Key =  KeyBuffer.Remove(0);
			int BackUp = Key;
			/*
			if(Key == VK_LEFT || Key == VK_NUMPAD4) return 0x14B;
			if(Key == VK_HOME || Key == VK_NUMPAD7) return 0x147;
			if(Key == VK_UP || Key == VK_NUMPAD8) return 0x148;
			if(Key == VK_PRIOR || Key == VK_NUMPAD9) return 0x149;	// page up! Believe it, or not!
			if(Key == VK_RIGHT || Key == VK_NUMPAD6) return 0x14D;
			if(Key == VK_NEXT || Key == VK_NUMPAD3) return 0x151;	// page down! Believe it, or not!
			if(Key == VK_DOWN || Key == VK_NUMPAD2) return 0x150;
			if(Key == VK_END || Key == VK_NUMPAD1) return 0x14F;
			if(Key == VK_NUMPAD5) return '.';*/


			return Key;			
		}
		else
		  CheckMessages();
				
}

int globalwindowhandler::ReadKey()
{
  CheckMessages();

  if(KeyBuffer.Length())
    return GetKey(false, true);
  else
    return 0;
}


void globalwindowhandler::CheckMessages()
{
  ushort Index;
  SDL_Event event;
  while( SDL_PollEvent( &event ) )
     {
    switch( event.type ){
      case SDL_KEYDOWN:
	Index = KeyBuffer.Search(event.key.keysym.unicode);

	if(Index == 0xFFFF)
	KeyBuffer.Add(event.key.keysym.unicode);
        break;

      case SDL_KEYUP:
        break;

      default:
        break;
    }
  }

}
#endif
