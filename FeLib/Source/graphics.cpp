/*
 *
 *  Iter Vehemens ad Necem (IVAN)
 *  Copyright (C) Timo Kiviluoto
 *  Released under the GNU General
 *  Public License
 *
 *  See LICENSING which should be included
 *  along with this file for more details
 *
 */

#ifdef USE_SDL
#include "SDL.h"
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef __DJGPP__
#include <dpmi.h>
#include <conio.h>
#include <go32.h>
#endif

#include "graphics.h"
#include "bitmap.h"
#include "whandler.h"
#include "error.h"
#include "rawbit.h"

void (*graphics::SwitchModeHandler)();

#ifdef USE_SDL
#if SDL_MAJOR_VERSION == 1
SDL_Surface* graphics::Screen;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
SDL_Surface* graphics::TempSurface;
#endif
#else
SDL_Window* graphics::Window;
SDL_Renderer* graphics::Renderer;
SDL_Texture* graphics::Texture;
#endif
#endif

#ifdef __DJGPP__
ulong graphics::BufferSize;
ushort graphics::ScreenSelector = 0;
graphics::vesainfo graphics::VesaInfo;
graphics::modeinfo graphics::ModeInfo;
#endif

bitmap* graphics::DoubleBuffer;
v2 graphics::Res;
int graphics::ColorDepth;
rawbitmap* graphics::DefaultFont = 0;

void graphics::Init()
{
  static truth AlreadyInstalled = false;

  if(!AlreadyInstalled)
  {
    AlreadyInstalled = true;

#ifdef __EMSCRIPTEN__
  SDL_SetHint(SDL_HINT_EMSCRIPTEN_ASYNCIFY, "1");
#endif

#ifdef USE_SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE))
      ABORT("Can't initialize SDL.");
#if SDL_MAJOR_VERSION == 2
  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
#endif
#endif

#ifdef __DJGPP__
    VesaInfo.Retrieve();
#endif

    atexit(graphics::DeInit);
  }
}

void graphics::DeInit()
{
  delete DefaultFont;
  DefaultFont = 0;

#ifdef USE_SDL
#if SDL_MAJOR_VERSION == 1
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  SDL_FreeSurface(TempSurface);
#endif
#endif
  SDL_Quit();
#endif

#ifdef __DJGPP__
  if(ScreenSelector)
  {
    __dpmi_free_ldt_descriptor(ScreenSelector);
    ScreenSelector = 0;
    textmode(0x3);
  }
#endif
}

#ifdef USE_SDL

void graphics::SetMode(const char* Title, const char* IconName,
		       v2 NewRes, truth FullScreen)
{
  if(IconName)
  {
    SDL_Surface* Icon = SDL_LoadBMP(IconName);
#if SDL_MAJOR_VERSION == 1
    SDL_SetColorKey(Icon, SDL_SRCCOLORKEY,
		    SDL_MapRGB(Icon->format, 255, 255, 255));
    SDL_WM_SetIcon(Icon, NULL);
#else
    SDL_SetColorKey(Icon, SDL_TRUE,
		    SDL_MapRGB(Icon->format, 255, 255, 255));
    SDL_SetWindowIcon(Window, Icon);
#endif
  }

  ulong Flags = SDL_SWSURFACE;

  if(FullScreen)
  {
    SDL_ShowCursor(SDL_DISABLE);
#if SDL_MAJOR_VERSION == 1
    Flags |= SDL_FULLSCREEN;
#else
    Flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif
  }

#if SDL_MAJOR_VERSION == 1
  Screen = SDL_SetVideoMode(NewRes.X, NewRes.Y, 16, Flags);

  if(!Screen)
    ABORT("Couldn't set video mode.");

  SDL_WM_SetCaption(Title, 0);
#else
  Window = SDL_CreateWindow(Title,
			  SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          NewRes.X, NewRes.Y, Flags);
  if(!Window)
    ABORT("Couldn't set video mode.");

  Renderer = SDL_CreateRenderer(Window, -1, 0);
  if(!Renderer)
    ABORT("Couldn't set renderer mode.");

  Texture = SDL_CreateTexture(Renderer,
         SDL_PIXELFORMAT_RGB565,
         SDL_TEXTUREACCESS_STREAMING,
	 NewRes.X, NewRes.Y);
#endif

  globalwindowhandler::Init();
  DoubleBuffer = new bitmap(NewRes);
  Res = NewRes;
  ColorDepth = 16;

#if SDL_MAJOR_VERSION == 1
#if SDL_BYTEORDER == SDL_BIG_ENDIAN

  Uint32 rmask, gmask, bmask;
  rmask = 0xF800;
  gmask = 0x7E0;
  bmask = 0x1F;

  TempSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, Res.X, Res.Y, 16,
				     rmask, gmask, bmask, 0);

  if(!TempSurface)
      ABORT("CreateRGBSurface failed: %s\n", SDL_GetError());

#endif
#endif
}

#if SDL_BYTEORDER == SDL_BIG_ENDIAN

void graphics::BlitDBToScreen()
{
#if SDL_MAJOR_VERSION == 1
  SDL_LockSurface(TempSurface);
  packcol16* SrcPtr = DoubleBuffer->GetImage()[0];
  packcol16* DestPtr = static_cast<packcol16*>(TempSurface->pixels);
  ulong ScreenYMove = (TempSurface->pitch >> 1);
  ulong LineSize = Res.X << 1;

  for(int y = 0; y < Res.Y; ++y, SrcPtr += Res.X, DestPtr += ScreenYMove)
    memcpy(DestPtr, SrcPtr, LineSize);

  SDL_UnlockSurface(TempSurface);
  SDL_Surface* S = SDL_DisplayFormat(TempSurface);
  SDL_BlitSurface(S, NULL, Screen, NULL);
  SDL_FreeSurface(S);
  SDL_UpdateRect(Screen, 0, 0, Res.X, Res.Y);
#else
  SDL_UpdateTexture(sdlTexture, NULL, myPixels, 640 * sizeof (Uint32));
#endif
}

#else

void graphics::BlitDBToScreen()
{
#if SDL_MAJOR_VERSION == 1
  if(SDL_MUSTLOCK(Screen) && SDL_LockSurface(Screen) < 0)
    ABORT("Can't lock screen");

  packcol16* SrcPtr = DoubleBuffer->GetImage()[0];
  packcol16* DestPtr = static_cast<packcol16*>(Screen->pixels);
  ulong ScreenYMove = (Screen->pitch >> 1);
  ulong LineSize = Res.X << 1;

  for(int y = 0; y < Res.Y; ++y, SrcPtr += Res.X, DestPtr += ScreenYMove)
    memcpy(DestPtr, SrcPtr, LineSize);

  if(SDL_MUSTLOCK(Screen))
    SDL_UnlockSurface(Screen);

  SDL_UpdateRect(Screen, 0, 0, Res.X, Res.Y);
#else
  packcol16* SrcPtr = DoubleBuffer->GetImage()[0];
  SDL_UpdateTexture(Texture, NULL, SrcPtr, Res.X * sizeof(packcol16));
  SDL_RenderClear(Renderer);
  SDL_RenderCopy(Renderer, Texture, NULL, NULL);
  SDL_RenderPresent(Renderer);
#endif
}

#endif

void graphics::SwitchMode()
{
#if SDL_MAJOR_VERSION == 1
  ulong Flags;

  if(Screen->flags & SDL_FULLSCREEN)
  {
    SDL_ShowCursor(SDL_ENABLE);
    Flags = SDL_SWSURFACE;
  }
  else
  {
    SDL_ShowCursor(SDL_DISABLE);
    Flags = SDL_SWSURFACE|SDL_FULLSCREEN;
  }

  if(SwitchModeHandler)
    SwitchModeHandler();

  Screen = SDL_SetVideoMode(Res.X, Res.Y, ColorDepth, Flags);

  if(!Screen)
    ABORT("Couldn't toggle fullscreen mode.");

  BlitDBToScreen();
#else
   ulong Flags = SDL_GetWindowFlags(Window);
   if (Flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
      SDL_SetWindowFullscreen(Window, 0);
   } else {
      SDL_SetWindowFullscreen(Window, SDL_WINDOW_FULLSCREEN_DESKTOP);
   }
  BlitDBToScreen();
#endif
}

#endif

void graphics::LoadDefaultFont(const festring& FileName)
{
  DefaultFont = new rawbitmap(FileName);
}

#ifdef __DJGPP__

void graphics::SetMode(const char*, const char*, v2 NewRes, truth)
{
  ulong Mode;

  for(Mode = 0; Mode < 0x10000; ++Mode)
  {
    ModeInfo.Retrieve(Mode);

    if(ModeInfo.Attribs1 & 0x01
       && ModeInfo.Attribs1 & 0xFF
       && ModeInfo.Width == NewRes.X
       && ModeInfo.Height == NewRes.Y
       && ModeInfo.BitsPerPixel == 16)
      break;
  }

  if(Mode == 0x10000)
    ABORT("Resolution %dx%d not supported!", NewRes.X, NewRes.Y);

  __dpmi_regs Regs;
  Regs.x.ax = 0x4F02;
  Regs.x.bx = Mode | 0x4000;
  __dpmi_int(0x10, &Regs);
  Res.X = ModeInfo.Width;
  Res.Y = ModeInfo.Height;
  BufferSize = Res.Y * ModeInfo.BytesPerLine;
  delete DoubleBuffer;
  DoubleBuffer = new bitmap(Res);
  __dpmi_meminfo MemoryInfo;
  MemoryInfo.size = BufferSize;
  MemoryInfo.address = ModeInfo.PhysicalLFBAddress;
  __dpmi_physical_address_mapping(&MemoryInfo);
  __dpmi_lock_linear_region(&MemoryInfo);
  ScreenSelector = __dpmi_allocate_ldt_descriptors(1);
  __dpmi_set_segment_base_address(ScreenSelector, MemoryInfo.address);
  __dpmi_set_segment_limit(ScreenSelector, BufferSize - 1);
}

void graphics::BlitDBToScreen()
{
  movedata(_my_ds(), ulong(DoubleBuffer->GetImage()[0]),
	   ScreenSelector, 0, BufferSize);
}

void graphics::vesainfo::Retrieve()
{
  Signature = 0x32454256;
  dosmemput(this, sizeof(vesainfo), __tb);
  __dpmi_regs Regs;
  Regs.x.ax = 0x4F00;
  Regs.x.di =  __tb       & 0x000F;
  Regs.x.es = (__tb >> 4) & 0xFFFF;
  __dpmi_int(0x10, &Regs);
  dosmemget(__tb, sizeof(vesainfo), this);
}

void graphics::modeinfo::Retrieve(ushort Mode)
{
  __dpmi_regs Regs;
  Regs.x.ax = 0x4F01;
  Regs.x.cx = Mode;
  Regs.x.di =  __tb       & 0x000F;
  Regs.x.es = (__tb >> 4) & 0xFFFF;
  dosmemput(this, sizeof(modeinfo), __tb);
  __dpmi_int(0x10, &Regs);
  dosmemget(__tb, sizeof(modeinfo), this);
}

#endif
