#ifdef __DJGPP__
#include <conio.h>
#include "graphics.h"
#endif

#ifdef WIN32
#include <windows.h>
#else
#include <iostream>
#endif

#ifdef USE_SDL
#include "SDL.h"
#endif

#include "stdlib.h"
#include "error.h"

int Main(int, char**);

int main(int argc, char* argv[])
{
  globalerrorhandler::Install();

  try
    {
      return Main(argc, argv);
    }
  catch(...)
    {
#ifdef WIN32
      ShowWindow(GetActiveWindow(), SW_HIDE);
      char Buffer[256];
      strcpy(Buffer, "Fatal Error: Unknown exception thrown.");
      strcat(Buffer, globalerrorhandler::GetBugMsg());
      MessageBox(NULL, Buffer, "Program aborted!", MB_OK|MB_ICONEXCLAMATION);
#endif
#ifdef LINUX
      std::cout << "Fatal Error: Unknown exception thrown." << globalerrorhandler::GetBugMsg() << std::endl;
#endif
#ifdef __DJGPP__
      graphics::DeInit();
      std::cout << "Fatal Error: Unknown exception thrown." << globalerrorhandler::GetBugMsg() << std::endl;
#endif
      exit(3);
    }

  exit(0);
}
