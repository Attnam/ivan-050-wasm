#ifdef USE_SDL
#include "SDL.h"
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
#include "colorbit.h"

void (*graphics::SwitchModeHandler)();

#ifdef USE_SDL
SDL_Surface* graphics::Screen;
#endif

#ifdef __DJGPP__
ulong graphics::BufferSize;
ushort graphics::ScreenSelector = 0;
graphics::vesainfo graphics::VesaInfo;
graphics::modeinfo graphics::ModeInfo;
#endif

bitmap* graphics::DoubleBuffer;
ushort graphics::ResX;
ushort graphics::ResY;
uchar graphics::ColorDepth;
colorizablebitmap* graphics::DefaultFont = 0;

void graphics::Init()
{
  static bool AlreadyInstalled = false;

  if(!AlreadyInstalled)
    {
      AlreadyInstalled = true;

#ifdef USE_SDL
      if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE))
	ABORT("Can't initialize SDL.");
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

void graphics::SetMode(const char* Title, const char* IconName, ushort NewResX, ushort NewResY, bool FullScreen)
{
  if(IconName)
    {
      SDL_Surface* Icon = SDL_LoadBMP(IconName);
      SDL_SetColorKey(Icon, SDL_SRCCOLORKEY, SDL_MapRGB(Icon->format, 255, 255, 255));
      SDL_WM_SetIcon(Icon, NULL);
    }

  ulong Flags = SDL_SWSURFACE;

  if(FullScreen)
    {
      SDL_ShowCursor(SDL_DISABLE);
      Flags |= SDL_FULLSCREEN;
    }

  Screen = SDL_SetVideoMode(NewResX, NewResY, 16, Flags);

  if(!Screen) 
    ABORT("Couldn't set video mode.");

  SDL_WM_SetCaption(Title, 0);
  globalwindowhandler::Init();
  DoubleBuffer = new bitmap(NewResX, NewResY);
  ResX = NewResX;
  ResY = NewResY;
  ColorDepth = 16;
}

void graphics::BlitDBToScreen()
{
  if(SDL_MUSTLOCK(Screen) && SDL_LockSurface(Screen) < 0)
    ABORT("Can't lock screen");

  ushort* SrcPtr = &DoubleBuffer->GetImage()[0][0];
  ushort* DestPtr = static_cast<ushort*>(Screen->pixels);
  ulong ScreenYMove = (Screen->pitch >> 1) - ResX;

  for(ushort y = 0; y < ResY; ++y, DestPtr += ScreenYMove)
    for(ushort x = 0; x < ResX; ++x, ++SrcPtr, ++DestPtr)
      *DestPtr = *SrcPtr;

  if(SDL_MUSTLOCK(Screen))
    SDL_UnlockSurface(Screen);

  SDL_UpdateRect(Screen, 0, 0, ResX, ResY);
}

void graphics::SwitchMode()
{
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

  Screen = SDL_SetVideoMode(ResX, ResY, ColorDepth, Flags);

  if(!Screen) 
    ABORT("Couldn't toggle fullscreen mode.");

  BlitDBToScreen();
}

#endif

void graphics::LoadDefaultFont(const std::string& FileName)
{
  DefaultFont = new colorizablebitmap(FileName);
}

#ifdef __DJGPP__

void graphics::SetMode(const char*, const char*, ushort NewResX, ushort NewResY, bool)
{
  ulong Mode;

  for(Mode = 0; Mode < 0x10000; ++Mode)
    {
      ModeInfo.Retrieve(Mode);
      
      if(ModeInfo.Attribs1 & 0x01
      && ModeInfo.Attribs1 & 0xFF
      && ModeInfo.Width == NewResX
      && ModeInfo.Height == NewResY
      && ModeInfo.BitsPerPixel == 16)
	  break;
    }

  if(Mode == 0x10000)
    ABORT("Resolution %dx%d not supported!", NewResX, NewResY);

  __dpmi_regs Regs;
  Regs.x.ax = 0x4F02;
  Regs.x.bx = Mode | 0x4000;
  __dpmi_int(0x10, &Regs);
  ResX = ModeInfo.Width;
  ResY = ModeInfo.Height;
  BufferSize = ResY * ModeInfo.BytesPerLine;
  delete DoubleBuffer;
  DoubleBuffer = new bitmap(ResX, ResY);
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
  movedata(_my_ds(), ulong(DoubleBuffer->GetImage()[0]), ScreenSelector, 0, BufferSize);
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

