#include <cstdio>
#include <iostream>
#include <ctime>

#ifdef __DJGPP__
#include <conio.h>
#include <go32.h>
#include <sys/farptr.h>
#endif

#include "hscore.h"
#include "feio.h"
#include "whandler.h"
#include "config.h"
#include "game.h"
#include "igraph.h"
#include "main.h"
#include "femath.h"
#include "proto.h"
#include "script.h"
#include "database.h"
#include "felibdef.h"

const bool ValpuriIsAlive = true;

#ifdef WIN32

int Main(HINSTANCE hInstance, HINSTANCE, HWND* hWnd, LPSTR, int)
{

#else

int Main(int argc, char **argv)
{
  if(argc > 1 && std::string(argv[1]) == "--version")
  {
    std::cout << "Iter Vehemens ad Necem version " << VERSION << std::endl;
    return 0;
  }

#endif

#ifdef VC

  /* You are not expected to understand this. */

  __asm _emit(1 << 0x04)|(1 << 0x07);

#endif

#ifdef __DJGPP__

  /* Saves numlock state and toggles it off */

  char ShiftByteState = _farpeekb(_dos_ds, 0x417);
  _farpokeb(_dos_ds, 0x417, 0);

#endif

  protosystem::GenerateCodeNameMaps();
  femath::SetSeed(time(0));
  game::InitGlobalValueMap();
  databasesystem::Initialize();
  game::InitLuxTable();
  game::InitScript();
  configuration::Load();

#ifdef WIN32
  igraph::Init(hInstance, hWnd);
#else
  igraph::Init();
#endif

#ifndef __DJGPP__
  globalwindowhandler::SetQuitMessageHandler(game::HandleQuitMessage);
#endif

  while(true)
    switch(iosystem::Menu(igraph::GetMenuGraphic(), vector2d(RES.X / 2 - 130, RES.Y / 2 + 20), "\r", "Start Game\rContinue Game\rConfiguration\rHighscores\rQuit\r", MAKE_SHADE_COL(LIGHTGRAY), LIGHTGRAY, std::string("IVAN v. ") + VERSION + "\rBuild " + __DATE__ + "\r"))
      {
      case 0:
	if(game::Init())
	  {
	    game::Run();
	    game::DeInit();
	  }
	break;
      case 1:
	{
	  std::string LoadName = iosystem::WhatToLoadMenu(WHITE, LIGHTGRAY, SAVE_DIR);

	  if(LoadName.length())
	    {
	      LoadName.resize(LoadName.size() - 4);

	      if(game::Init(LoadName))
		{
		  game::Run();
		  game::DeInit();
		}
	    }

	  break;
	}
      case 2:
	configuration::ShowConfigScreen();
	break;
      case 3:
	{
	  highscore HScore;
	  HScore.Draw();
	  break;
	}
      case 4:
	configuration::Save();

#ifdef __DJGPP__

	/* Loads numlock state */

	_farpokeb(_dos_ds, 0x417, ShiftByteState);

#endif

	return 0;
      }
}

